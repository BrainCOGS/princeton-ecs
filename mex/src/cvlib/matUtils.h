#ifndef CVUTILS_H
#define CVUTILS_H

#include <mexopencv.hpp>


/**
  Convenience function to get the bit depth of an image or stack of images
  (assumed identical in encoding type).
*/


/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image>
void cvCall(Image& image)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image);    break;
  case CV_8S :    TFunctor<schar >()(image);    break;
  case CV_16U:    TFunctor<ushort>()(image);    break;
  case CV_16S:    TFunctor<short >()(image);    break;
  case CV_32S:    TFunctor<int   >()(image);    break;
  case CV_32F:    TFunctor<float >()(image);    break;
  case CV_64F:    TFunctor<double>()(image);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1>
void cvCall(Image& image, Arg1& arg1)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1);    break;
  case CV_16S:    TFunctor<short >()(image, arg1);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1);    break;
  case CV_32F:    TFunctor<float >()(image, arg1);    break;
  case CV_64F:    TFunctor<double>()(image, arg1);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
void cvCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9, Arg10& arg10)
{
  if (image.channels() != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (image.depth()) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", image.depth());     break;
  }
}


#endif //CVUTILS_H
