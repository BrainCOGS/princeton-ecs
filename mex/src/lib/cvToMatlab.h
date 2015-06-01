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
  void operator()(const cv::Mat& image, float*& dataPtr, float offset)
  {
    // Loop over each pixel in the image 
    for (int iRow = 0; iRow < image.rows; ++iRow) {
      const Pixel*    pixRow      = image.ptr<Pixel>(iRow);
      for (int iCol = 0; iCol < image.cols; ++iCol) {
        dataPtr[iCol*image.rows]  = static_cast<float>( pixRow[iCol] ) - offset;
      } // end loop over columns
      
      // Write next row
      ++dataPtr;
    } // end loop over rows

    // Set write pointer to the end of the written data
    dataPtr          += image.rows * (image.cols - 1);
  }
};


/**
  Nice names for commonly used enums.
*/
//enum TemplateMatchModes {
//    TM_SQDIFF        = 0, //!< \f[R(x,y)= \sum _{x',y'} (T(x',y')-I(x+x',y+y'))^2\f]
//    TM_SQDIFF_NORMED = 1, //!< \f[R(x,y)= \frac{\sum_{x',y'} (T(x',y')-I(x+x',y+y'))^2}{\sqrt{\sum_{x',y'}T(x',y')^2 \cdot \sum_{x',y'} I(x+x',y+y')^2}}\f]
//    TM_CCORR         = 2, //!< \f[R(x,y)= \sum _{x',y'} (T(x',y')  \cdot I(x+x',y+y'))\f]
//    TM_CCORR_NORMED  = 3, //!< \f[R(x,y)= \frac{\sum_{x',y'} (T(x',y') \cdot I(x+x',y+y'))}{\sqrt{\sum_{x',y'}T(x',y')^2 \cdot \sum_{x',y'} I(x+x',y+y')^2}}\f]
//    TM_CCOEFF        = 4, //!< \f[R(x,y)= \sum _{x',y'} (T'(x',y')  \cdot I'(x+x',y+y'))\f]
//                          //!< where
//                          //!< \f[\begin{array}{l} T'(x',y')=T(x',y') - 1/(w  \cdot h)  \cdot \sum _{x'',y''} T(x'',y'') \\ I'(x+x',y+y')=I(x+x',y+y') - 1/(w  \cdot h)  \cdot \sum _{x'',y''} I(x+x'',y+y'') \end{array}\f]
//    TM_CCOEFF_NORMED = 5  //!< \f[R(x,y)= \frac{ \sum_{x',y'} (T'(x',y') \cdot I'(x+x',y+y')) }{ \sqrt{\sum_{x',y'}T'(x',y')^2 \cdot \sum_{x',y'} I'(x+x',y+y')^2} }\f]
//};



#endif //imageIMAGE_H
