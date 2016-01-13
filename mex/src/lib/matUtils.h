/**
  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef MATUTILS_H
#define MATUTILS_H

#include <opencv2/core.hpp>


/**
  Display an equalized image.
*/
void imshoweq(const std::string& winname, const cv::Mat& image, const double offset = 0, const int waitMS = -9);

/**
  Display a scaled image, with pixels transformed as \alpha \times x + \beta.
*/
void imshowsc(const std::string& winname, const cv::Mat& image, const double alpha, const double beta, const int waitMS = -9);

/**
  Display an image scaled so that minValue corresponds to black and maxValue to white.
*/
void imshowrange(const std::string& winname, const cv::Mat& image, const double minValue, const double maxValue, const int waitMS = -9);


/**
  Convenience function to return the min and max data type range for a given
  cv::Mat bit depth. The return value is the range (max - min).
*/
double cvBitRange(const int bitDepth, double& minValue, double& maxValue);
double cvBitRange(const int bitDepth);


/**
  Convenience function to get the bit depth of an image or stack of images
  (assumed identical in encoding type).
*/
int cvBitDepth(const std::vector<cv::Mat>&);
int cvBitDepth(const cv::Mat&);

/**
  Convenience function to get the data type of an image or stack of images
  (assumed identical in encoding type).
*/
int cvType(const std::vector<cv::Mat>&);
int cvType(const cv::Mat&);

/**
  Convenience function to get the number of channels of an image or stack of 
  images (assumed identical in encoding type).
*/
int cvNumChannels(const std::vector<cv::Mat>&);
int cvNumChannels(const cv::Mat&);


//=============================================================================

/**
  Convenience function to deduce the data type given the bit depth of an image.
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image>
void cvCall(Image& image)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image);    break;
  case CV_8S :    TFunctor<schar >()(image);    break;
  case CV_16U:    TFunctor<ushort>()(image);    break;
  case CV_16S:    TFunctor<short >()(image);    break;
  case CV_32S:    TFunctor<int   >()(image);    break;
  case CV_32F:    TFunctor<float >()(image);    break;
  case CV_64F:    TFunctor<double>()(image);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1);    break;
  case CV_16S:    TFunctor<short >()(image, arg1);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1);    break;
  case CV_32F:    TFunctor<float >()(image, arg1);    break;
  case CV_64F:    TFunctor<double>()(image, arg1);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
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
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_8S :    TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_16U:    TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_16S:    TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_32S:    TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_32F:    TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_64F:    TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

//=============================================================================

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image>
void cvTypeCall(Image& image)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image);    break;
  case CV_8S :    TFunctor<schar , Type>()(image);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image);    break;
  case CV_16S:    TFunctor<short , Type>()(image);    break;
  case CV_32S:    TFunctor<int   , Type>()(image);    break;
  case CV_32F:    TFunctor<float , Type>()(image);    break;
  case CV_64F:    TFunctor<double, Type>()(image);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1>
void cvTypeCall(Image& image, Arg1& arg1)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3, arg4);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3, arg4);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3, arg4);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3, arg4);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3, arg4);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3, arg4);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3, arg4);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9, Arg10& arg10)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Type, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11>
void cvTypeCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9, Arg10& arg10, Arg11& arg11)
{
  if (cvNumChannels(image) != 1)
    mexErrMsgIdAndTxt("cvTypeCall:format", "Only grayscale images are supported.");

  switch (cvBitDepth(image)) {
  case CV_8U :    TFunctor<uchar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);    break;
  case CV_8S :    TFunctor<schar , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);    break;
  case CV_16U:    TFunctor<ushort, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);    break;
  case CV_16S:    TFunctor<short , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);    break;
  case CV_32S:    TFunctor<int   , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);    break;
  case CV_32F:    TFunctor<float , Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);    break;
  case CV_64F:    TFunctor<double, Type>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);    break;
  default:        mexErrMsgIdAndTxt("cvTypeCall:bitdepth", "Unsupported bit depth %d.", cvBitDepth(image));     break;
  }
}

//=============================================================================

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2, Arg3& arg3)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2, arg3);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2, arg3);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2, arg3);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2, arg3);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2, arg3);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2, arg3);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2, arg3);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2, arg3);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2, arg3);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2, arg3);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2, arg3);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2, arg3);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2, arg3, arg4);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2, arg3, arg4);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2, arg3, arg4);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2, arg3, arg4);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2, arg3, arg4);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2, arg3, arg4);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2, arg3, arg4);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2, arg3, arg4);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2, arg3, arg4);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2, arg3, arg4);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2, arg3, arg4);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2, arg3, arg4);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2, arg3, arg4, arg5);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2, arg3, arg4, arg5);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2, arg3, arg4, arg5);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}

/**
  TFunctor should be a template class with operator() defined; the latter will
  be called with image as an argument.
*/
template<template<typename, typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
void cvMatlabCall(Image& image, const mxClassID classID, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9, Arg10& arg10)
{
  int                     dataType;
  switch (classID) {
  case mxSINGLE_CLASS :   cvTypeCall<TFunctor, float >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxLOGICAL_CLASS:   cvTypeCall<TFunctor, bool  >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxCHAR_CLASS   :   cvTypeCall<TFunctor, char  >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxDOUBLE_CLASS :   cvTypeCall<TFunctor, double>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxINT8_CLASS   :   cvTypeCall<TFunctor, schar >(image, dataType = CV_8S , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxUINT8_CLASS  :   cvTypeCall<TFunctor, uchar >(image, dataType = CV_8U , arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxINT16_CLASS  :   cvTypeCall<TFunctor, short >(image, dataType = CV_16S, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxUINT16_CLASS :   cvTypeCall<TFunctor, ushort>(image, dataType = CV_16U, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxINT32_CLASS  :   cvTypeCall<TFunctor, int   >(image, dataType = CV_32S, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxUINT32_CLASS :   cvTypeCall<TFunctor, uint  >(image, dataType = CV_32F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxINT64_CLASS  :   cvTypeCall<TFunctor, int64 >(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxUINT64_CLASS :   cvTypeCall<TFunctor, uint64>(image, dataType = CV_64F, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  default:                mexErrMsgIdAndTxt("cvMatlabCall:classID", "Unsupported class ID %d.", classID);                    break;
  }
}



//=============================================================================

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1>
void matlabCall(Image& image, Arg1& arg1)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2, arg3);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2, arg3);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2, arg3);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2, arg3);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2, arg3);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2, arg3);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2, arg3);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2, arg3);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2, arg3);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2, arg3);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2, arg3);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2, arg3);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2, arg3, arg4);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2, arg3, arg4);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2, arg3, arg4);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2, arg3, arg4);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2, arg3, arg4);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2, arg3, arg4);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2, arg3, arg4);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2, arg3, arg4);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2, arg3, arg4);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2, arg3, arg4);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2, arg3, arg4);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2, arg3, arg4);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2, arg3, arg4, arg5);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2, arg3, arg4, arg5);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2, arg3, arg4, arg5, arg6);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}

/**
TFunctor should be a template class with operator() defined; the latter will
be called with image as an argument.
*/
template<template<typename> class TFunctor, typename Image, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
void matlabCall(Image& image, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4, Arg5& arg5, Arg6& arg6, Arg7& arg7, Arg8& arg8, Arg9& arg9, Arg10& arg10)
{
  switch (mxGetClassID(image)) {
  case mxSINGLE_CLASS :   TFunctor<float >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxLOGICAL_CLASS:   TFunctor<bool  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxCHAR_CLASS   :   TFunctor<char  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxDOUBLE_CLASS :   TFunctor<double>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxINT8_CLASS   :   TFunctor<schar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxUINT8_CLASS  :   TFunctor<uchar >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxINT16_CLASS  :   TFunctor<short >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxUINT16_CLASS :   TFunctor<ushort>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxINT32_CLASS  :   TFunctor<int   >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxUINT32_CLASS :   TFunctor<uint  >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxINT64_CLASS  :   TFunctor<int64 >()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  case mxUINT64_CLASS :   TFunctor<uint64>()(image, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);    break;
  default:                mexErrMsgIdAndTxt("matlabCall:mxGetClassID(image)", "Unsupported class ID %d.", mxGetClassID(image));                    break;
  }
}


#endif //MATUTILS_H
