/**
  Functors for computing the median of a given stack of images. 
  Note that for speed when repeating the same computation, these 
  functions assume that the temporary storage structures have been
  preallocated (properly) by the user. If you don't know what 
  they're doing, you should probably not be using them.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef MEDIANIMAGE_H
#define MEDIANIMAGE_H

#include <opencv2\core.hpp>
#include "quickSelect.h"


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
          traceTemp[iFrame]     = pixRow[iFrame][iCol];

        // Store the computed median
        medRow[iCol]            = quickSelect(traceTemp);
      } // end loop over columns
    } // end loop over rows
  }
};


#endif //MEDIANIMAGE_H
