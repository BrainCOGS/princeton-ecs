/**
  Functors for applying filters to images.

  These functions are speed optimized at the cost of generality. If you 
  don't know what they're doing, you should probably not be using them.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <vector>
#include <algorithm>
#include <mex.h>


/// Base class for image filters.
template<typename Pixel>
class ImageFilter2D {
public:
  const int           imageWidth ;
  const int           imageHeight;
  const int           maskWidth  ;
  const int           maskHeight ;
  const int           halfWidth  ;
  const int           halfHeight ;
  std::vector<int>    rowBound   ;    // for mask
  std::vector<int>    colBound   ;    // for mask
  std::vector<int>    firstRow   ;    // relative to image row
  std::vector<int>    firstCol   ;    // relative to image col

public:
  ImageFilter2D(const int imageWidth, const int imageHeight, const int maskWidth, const int maskHeight)
    : imageWidth  (imageWidth )
    , imageHeight (imageHeight)
    , maskWidth   (maskWidth  )
    , maskHeight  (maskHeight )
    , halfWidth   (maskWidth  / 2)
    , halfHeight  (maskHeight / 2)
    , rowBound    (imageHeight)
    , colBound    (imageWidth )
    , firstRow    (imageHeight)
    , firstCol    (imageWidth )
  {
    for (int row = 0; row < imageHeight; ++row) {
      firstRow[row]   = std::max(-halfHeight , -row           );
      rowBound[row]   = std::min(halfHeight+1, imageHeight-row) - firstRow[row];
    }

    for (int col = 0; col < imageWidth; ++col) {
      firstCol[col]   = std::max(-halfWidth  , -col           );
      colBound[col]   = std::min(halfWidth+1 , imageWidth-col ) - firstCol[col];
    }
  }
  virtual ~ImageFilter2D() { }

  virtual void clear() = 0;
  virtual void add(const Pixel& pixelValue, const int maskPixel, const int sourcePixel, const int targetPixel) = 0;
  virtual Pixel compute() = 0;



  int maskFirstPixel (int row, int col) const { return halfHeight+firstRow[row] + maskHeight * (halfWidth+firstCol[col]); }
  int maskOffset     (int row)          const { return maskHeight - rowBound[row]; }

  int imageFirstPixel(int row, int col) const { return row+firstRow[row] + imageHeight * (col+firstCol[col]); }
  int imageOffset    (int row)          const { return imageHeight - rowBound[row]; }


  /**
    Applies the given filtering function to the source image and stores the output in the target image.
  */
  void operator()(void* targetImage, const void* sourceImage, const bool* isSelected)
  {
    Pixel*                target      = (Pixel*)        targetImage;
    const Pixel*          source      = (const Pixel*)  sourceImage;

    for (int tgtCol = 0, tgtPix = 0; tgtCol < imageWidth; ++tgtCol) {
      for (int tgtRow = 0; tgtRow < imageHeight; ++tgtRow, ++tgtPix) {
        if (isSelected && !isSelected[tgtPix]) {
          target[tgtPix]  = source[tgtPix];
          continue;
        }

        const int         mskOffset   = maskOffset (tgtRow);
        const int         srcOffset   = imageOffset(tgtRow);
        int               mskPix      = maskFirstPixel (tgtRow, tgtCol);
        int               srcPix      = imageFirstPixel(tgtRow, tgtCol);

        clear();
        for (int mskCol = 0; mskCol < colBound[tgtCol]; ++mskCol) {
          for (int mskRow = 0; mskRow < rowBound[tgtRow]; ++mskRow) {
            add(source[srcPix], mskPix, srcPix, tgtPix);
            ++srcPix;
            ++mskPix;
          }
          mskPix         += mskOffset;
          srcPix         += srcOffset;
        }
        target[tgtPix]    = compute();
      } // end loop over rows
    } // end loop over columns
  }
};


//=================================================================================================

// Create filter class and process
template<template<typename> class Filter, typename Arg1>
void applyFilter(const mxArray* image, void* outImage, const bool* isSelected, const Arg1& arg1)
{
  const int                   imageWidth    = static_cast<int>( mxGetN(image) );
  const int                   imageHeight   = static_cast<int>( mxGetM(image) );
  void*                       inImage       = mxGetData(image);

  switch (mxGetClassID(image)) {
  case mxDOUBLE_CLASS:    Filter<double             >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxSINGLE_CLASS:    Filter<float              >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxCHAR_CLASS:
  case mxINT8_CLASS:      Filter<char               >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxUINT8_CLASS:     Filter<unsigned char      >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxINT16_CLASS:     Filter<short              >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxUINT16_CLASS:    Filter<unsigned short     >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxINT32_CLASS:     Filter<int                >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxUINT32_CLASS:    Filter<unsigned int       >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxINT64_CLASS:     Filter<long long          >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  case mxUINT64_CLASS:    Filter<unsigned long long >(imageWidth, imageHeight, arg1)( outImage, inImage, isSelected );    break;
  default:                mexErrMsgIdAndTxt( "applyFilter:arguments", "Unsupported type of image." );
  }
}

// Create filter class and process
template<template<typename> class Filter, typename Arg1, typename Arg2>
void applyFilter(const mxArray* image, void* outImage, const bool* isSelected, const Arg1& arg1, const Arg2& arg2)
{
  const int                   imageWidth    = static_cast<int>( mxGetN(image) );
  const int                   imageHeight   = static_cast<int>( mxGetM(image) );
  void*                       inImage       = mxGetData(image);

  switch (mxGetClassID(image)) {
  case mxDOUBLE_CLASS:    Filter<double             >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxSINGLE_CLASS:    Filter<float              >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxCHAR_CLASS:
  case mxINT8_CLASS:      Filter<char               >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxUINT8_CLASS:     Filter<unsigned char      >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxINT16_CLASS:     Filter<short              >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxUINT16_CLASS:    Filter<unsigned short     >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxINT32_CLASS:     Filter<int                >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxUINT32_CLASS:    Filter<unsigned int       >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxINT64_CLASS:     Filter<long long          >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  case mxUINT64_CLASS:    Filter<unsigned long long >(imageWidth, imageHeight, arg1, arg2)( outImage, inImage, isSelected );    break;
  default:                mexErrMsgIdAndTxt( "applyFilter:arguments", "Unsupported type of image." );
  }
}

// Create filter class and process
template<template<typename> class Filter, typename Arg1, typename Arg2, typename Arg3>
void applyFilter(const mxArray* image, void* outImage, const bool* isSelected, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
{
  const int                   imageWidth    = static_cast<int>( mxGetN(image) );
  const int                   imageHeight   = static_cast<int>( mxGetM(image) );
  void*                       inImage       = mxGetData(image);

  switch (mxGetClassID(image)) {
  case mxDOUBLE_CLASS:    Filter<double             >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxSINGLE_CLASS:    Filter<float              >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxCHAR_CLASS:
  case mxINT8_CLASS:      Filter<char               >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxUINT8_CLASS:     Filter<unsigned char      >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxINT16_CLASS:     Filter<short              >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxUINT16_CLASS:    Filter<unsigned short     >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxINT32_CLASS:     Filter<int                >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxUINT32_CLASS:    Filter<unsigned int       >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxINT64_CLASS:     Filter<long long          >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  case mxUINT64_CLASS:    Filter<unsigned long long >(imageWidth, imageHeight, arg1, arg2, arg3)( outImage, inImage, isSelected );    break;
  default:                mexErrMsgIdAndTxt( "applyFilter:arguments", "Unsupported type of image." );
  }
}

// Create filter class and process
template<template<typename> class Filter, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
void applyFilter(const mxArray* image, void* outImage, const bool* isSelected, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4)
{
  const int                   imageWidth    = static_cast<int>( mxGetN(image) );
  const int                   imageHeight   = static_cast<int>( mxGetM(image) );
  void*                       inImage       = mxGetData(image);

  switch (mxGetClassID(image)) {
  case mxDOUBLE_CLASS:    Filter<double             >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxSINGLE_CLASS:    Filter<float              >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxCHAR_CLASS:
  case mxINT8_CLASS:      Filter<char               >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxUINT8_CLASS:     Filter<unsigned char      >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxINT16_CLASS:     Filter<short              >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxUINT16_CLASS:    Filter<unsigned short     >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxINT32_CLASS:     Filter<int                >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxUINT32_CLASS:    Filter<unsigned int       >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxINT64_CLASS:     Filter<long long          >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  case mxUINT64_CLASS:    Filter<unsigned long long >(imageWidth, imageHeight, arg1, arg2, arg3, arg4)( outImage, inImage, isSelected );    break;
  default:                mexErrMsgIdAndTxt( "applyFilter:arguments", "Unsupported type of image." );
  }
}

// Create filter class and process
template<template<typename> class Filter, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
void applyFilter(const mxArray* image, void* outImage, const bool* isSelected, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5)
{
  const int                   imageWidth    = static_cast<int>( mxGetN(image) );
  const int                   imageHeight   = static_cast<int>( mxGetM(image) );
  void*                       inImage       = mxGetData(image);

  switch (mxGetClassID(image)) {
  case mxDOUBLE_CLASS:    Filter<double             >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxSINGLE_CLASS:    Filter<float              >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxCHAR_CLASS:
  case mxINT8_CLASS:      Filter<char               >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxUINT8_CLASS:     Filter<unsigned char      >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxINT16_CLASS:     Filter<short              >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxUINT16_CLASS:    Filter<unsigned short     >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxINT32_CLASS:     Filter<int                >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxUINT32_CLASS:    Filter<unsigned int       >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxINT64_CLASS:     Filter<long long          >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  case mxUINT64_CLASS:    Filter<unsigned long long >(imageWidth, imageHeight, arg1, arg2, arg3, arg4, arg5)( outImage, inImage, isSelected );    break;
  default:                mexErrMsgIdAndTxt( "applyFilter:arguments", "Unsupported type of image." );
  }
}


#endif //IMAGEFILTER_H
