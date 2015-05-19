/**
  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef CVUTILS_H
#define CVUTILS_H

#include <opencv2\core.hpp>


/**
  Display an equalized image.
*/
void imshoweq(const std::string& winname, const cv::Mat& image, const int waitMS = 1);

/**
  Display a scaled image, with pixels transformed as \alpha \times x + \beta.
*/
void imshowsc(const std::string& winname, const cv::Mat& image, const double alpha, const double beta, const int waitMS = 1);

/**
  Display an image scaled so that minValue corresponds to black and maxValue to white.
*/
void imshowrange(const std::string& winname, const cv::Mat& image, const double minValue, const double maxValue, const int waitMS = 1);


/**
  Convenience function to return the min and max data type range for a given
  cv::Mat bit depth. The return value is the range (max - min).
*/
double cvBitRange(const int bitDepth, double& minValue, double& maxValue);


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


#endif //CVUTILS_H
