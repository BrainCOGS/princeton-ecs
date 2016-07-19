/**
  Functors for computing statistics of a given stack of images. 
  Note that for speed when repeating the same computation, these 
  functions assume that the temporary storage structures have been
  preallocated (properly) by the user. If you don't know what 
  they're doing, you should probably not be using them.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef IMAGESTATISTICS_H
#define IMAGESTATISTICS_H

#include <opencv2/core.hpp>
#include "quickSelect.h"
#include "utilities.h"


/**
  Median across stack.
*/
template<typename Pixel>
struct MedianVecMat32
{
  void operator() ( const std::vector<cv::Mat>& stack, cv::Mat& median, std::vector<float>& traceTemp
                  , int firstRow = 0, int firstCol = 0, const std::vector<bool>* omit = 0
                  )
  {
    // Sanity check for output size and type
    CV_DbgAssert(   !stack.empty()
                && (median.rows       <= stack[0].rows - firstRow)
                && (median.cols       <= stack[0].cols - firstCol)
                && (median.type()     == CV_32F)
                && (traceTemp.size()  >= stack.size())
                );

    // Temporary storage
    const size_t    numFrames   = stack.size();
    std::vector<const Pixel*>   pixRow(numFrames);


    // Loop over each pixel in the image stack
    for (int iRow = 0; iRow < median.rows; ++iRow) {
      // Pre-cache row pointers
      for (size_t iFrame = 0; iFrame < numFrames; ++iFrame)
        if (!omit || !(*omit)[iFrame])
          pixRow[iFrame]        = stack[iFrame].ptr<Pixel>(firstRow + iRow) + firstCol;

      float*        medRow      = median.ptr<float>(iRow);
      for (int iCol = 0; iCol < median.cols; ++iCol) {

        // Copy the stack of pixels into temporary storage
        size_t      nTraces     = 0;
        for (size_t iFrame = 0; iFrame < numFrames; ++iFrame)
          if (!omit || !(*omit)[iFrame]) {
            traceTemp[nTraces]  = static_cast<float>( pixRow[iFrame][iCol] );
            if (traceTemp[nTraces] == traceTemp[nTraces])
              ++nTraces;        // only keep valid elements (not NaN)
          }

        // Store the computed median
        medRow[iCol]            = quickSelect(traceTemp, nTraces);
      } // end loop over columns
    } // end loop over rows
  }
};


/**
  Accumulate mean and variance given an input image.
*/
template<typename Pixel>
struct AccumulateMatStatistics
{
  void operator()(const cv::Mat& image, SampleStatistics& statistics)
  {
    // Loop over each pixel in the image stack
    for (int iRow = 0; iRow < image.rows; ++iRow) {
      const Pixel*    row     = image.ptr<Pixel>(iRow);
      for (int iCol = 0; iCol < image.cols; ++iCol) {
        statistics.add(row[iCol]);
      } // end loop over columns
    } // end loop over rows
  }

  void operator()(const std::vector<cv::Mat>& imgStack, SampleStatistics& statistics, const std::vector<double>* scale = 0)
  {
    for (size_t iFrame = 0; iFrame < imgStack.size(); ++iFrame) {
      operator()(imgStack[iFrame] * (scale ? (*scale)[iFrame] : 1.), statistics);
    } // end loop over frames
  }
};


/**
  Count number of pixels within the given range (inclusive).
*/
template<typename Pixel>
struct CountPixelsInRange
{
  void operator()(const cv::Mat& image, double minValue, double maxValue, int& count)
  {
    count             = 0;

    // Loop over each pixel in the image stack
    for (int iRow = 0; iRow < image.rows; ++iRow) {
      const Pixel*    row     = image.ptr<Pixel>(iRow);
      for (int iCol = 0; iCol < image.cols; ++iCol) {
        double        value   = static_cast<double>(row[iCol]);
        if (value >= minValue && value <= maxValue)
          ++count;
      } // end loop over columns
    } // end loop over rows
  }
};


/**
  Black a.k.a. empty frame detection
*/
template<typename Pixel>
struct DetectEmptyFrames
{
  void operator()(const std::vector<cv::Mat>& stack, std::vector<bool>& isEmpty, double emptyProb, double*& zeroValue, double emptyNSigmas = 5)
  {
    // Special case for empty stacks
    isEmpty.resize(stack.size());
    if (stack.empty())        return;


    // Short circuit in case the zero level has been precomputed
    double                    maxZeroValue  = -1e308;
    if (zeroValue && mxIsFinite(*zeroValue)) {
      maxZeroValue            = *zeroValue;
    }

    else {
      // Use the first frame to estimate the black level and variance
      SampleStatistics        statistics;
      cvCall<AccumulateMatStatistics>(stack[0], statistics);

      // Account for multiple samples when computing the fraction of pixels that are
      // expected to fall below the zero + noise threshold
      isEmpty[0]              = true;       // by definition
      const Pixel             offset        = static_cast<Pixel>( statistics.getMean() );
      maxZeroValue            = offset + emptyNSigmas * statistics.getRMS();
      if (zeroValue)         *zeroValue     = maxZeroValue;
    }


    // Iterate through frames and decide if each one is completely black
    const int                 nFramePixels  = stack[0].rows * stack[0].cols;
    emptyProb                 = std::pow(emptyProb, nFramePixels);
    for (size_t iFrame = 0; iFrame < stack.size(); ++iFrame)
    {
      static const double     negInf        = -std::numeric_limits<double>::infinity();
      int                     numZeros;
      cvCall<CountPixelsInRange>(stack[iFrame], negInf, maxZeroValue, numZeros);
      if (numZeros >= emptyProb * nFramePixels)
        isEmpty[iFrame]       = true;
    }
  }
};


#endif //IMAGESTATISTICS_H
