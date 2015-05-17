/**
  Functors for manipulating images.

  These functions are speed optimized at the cost of generality. If you 
  don't know what they're doing, you should probably not be using them.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef MANIPULATEIMAGE_H
#define MANIPULATEIMAGE_H

#include <opencv2\core.hpp>


/**
  Shifts the source image by (deltaRows, deltaCols) and stores the output
  in target. Out-of-range values in target are set to emptyValue.
*/
template<typename Pixel>
struct CopyShiftedImage32
{
  void operator()(cv::Mat& target, const cv::Mat& source, const int deltaRows, const int deltaCols, const float emptyValue)
  {
    // Sanity check for output size and type
    CV_DbgAssert(  (target.rows       == source.rows)
                && (target.cols       == source.cols)
                && (source.type()     == CV_32F)
                );

    // Loop over each pixel in the image stack
    for (int tRow = 0; tRow < target.rows; ++tRow) {
      Pixel*          tgtRow      = target.ptr<Pixel >(tRow);

      // Special case if the shifted row does not exist in source
      const int       sRow        = tRow - deltaRows;
      if (sRow < 0 || sRow >= source.rows) {
        for (int tCol = 0; tCol < target.cols; ++tCol)
          tgtRow[tCol]            = Pixel(emptyValue);
      }

      else {
        const float*  srcRow      = source.ptr<float>(sRow);
        int           tCol        = 0;
        int           sCol        = -deltaCols;

        // Source column out of range
        for (; sCol < 0 && tCol < target.cols; ++tCol, ++sCol)
          tgtRow[tCol]            = Pixel(emptyValue);

        // Source column in range
        for (; sCol < source.cols && tCol < target.cols; ++tCol, ++sCol)
          tgtRow[tCol]            = Pixel(srcRow[sCol]);

        // Source column out of range
        for (; tCol < target.cols; ++tCol)
          tgtRow[tCol]            = Pixel(emptyValue);
      }
    } // end loop over rows
  }
};


#endif //MANIPULATEIMAGE_H
