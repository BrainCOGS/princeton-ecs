/**
  Fast methods for converting data structures from OpenCV to Matlab.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef imageIMAGE_H
#define imageIMAGE_H

#include <opencv2\core.hpp>
#include "quickSelect.h"


template<typename Pixel>
struct MatToMatlab32
{
  void operator()(cv::Mat& image, float*& dataPtr)
  {
    // Loop over each pixel in the image 
    for (int iRow = 0; iRow < image.rows; ++iRow) {
      Pixel*        pixRow        = image.ptr<Pixel>(iRow);
      for (int iCol = 0; iCol < image.cols; ++iCol) {
        dataPtr[iCol*image.rows]  = static_cast<Pixel>( pixRow[iCol] );
      } // end loop over columns
      
      // Write next row
      ++dataPtr;
    } // end loop over rows

    // Set write pointer to the end of the written data
    dataPtr        += image.rows * (image.cols - 1);
  }
};


#endif //imageIMAGE_H
