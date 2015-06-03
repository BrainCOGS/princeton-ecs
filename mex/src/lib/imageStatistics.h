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

#include <opencv2\core.hpp>
#include "quickSelect.h"
#include "utilities.h"


/**
  Median across stack.
*/
template<typename Pixel>
struct MedianVecMat32
{
  void operator()(const std::vector<cv::Mat>& stack, cv::Mat& median, std::vector<float>& traceTemp, int firstRow, int firstCol)
  {
    // Sanity check for output size and type
    CV_DbgAssert(   !stack.empty()
                && (median.rows       <= stack[0].rows - firstRow)
                && (median.cols       <= stack[0].cols - firstCol)
                && (median.type()     == CV_32F)
                && (traceTemp.size()  == stack.size())
                );

    // Temporary storage
    const size_t    numFrames   = stack.size();
    std::vector<const Pixel*>   pixRow(numFrames);


    // Loop over each pixel in the image stack
    for (int iRow = 0; iRow < median.rows; ++iRow) {
      // Pre-cache row pointers
      for (size_t iFrame = 0; iFrame < numFrames; ++iFrame)
        pixRow[iFrame]          = stack[iFrame].ptr<Pixel>(firstRow + iRow) + firstCol;

      float*        medRow      = median.ptr<float>(iRow);
      for (int iCol = 0; iCol < median.cols; ++iCol) {

        // Copy the stack of pixels into temporary storage
        for (size_t iFrame = 0; iFrame < numFrames; ++iFrame)
          traceTemp[iFrame]     = static_cast<float>( pixRow[iFrame][iCol] );

        // Store the computed median
        medRow[iCol]            = quickSelect(traceTemp);
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

#endif //IMAGESTATISTICS_H
