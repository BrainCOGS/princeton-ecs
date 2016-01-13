#include <cmath>
#include <climits>
#include <mex.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "matUtils.h"


/**
  Display an equalized image.
*/
void imshoweq(const std::string& winname, const cv::Mat& image, const double offset, const int waitMS)
{
  static cv::Ptr<cv::CLAHE>   clahe   = cv::createCLAHE();
  static cv::Mat              imgTemp;
  static cv::Mat              imgDisplay;
  image.convertTo(imgTemp, CV_16U, 1, offset);
  clahe->apply(imgTemp, imgDisplay);
  
  cv::imshow(winname, imgDisplay);
  if (waitMS >= 0)
    cv::waitKey(waitMS);
}

/**
  Display a scaled image, with pixels transformed as \alpha \times x + \beta.
*/
void imshowsc(const std::string& winname, const cv::Mat& image, const double alpha, const double beta, const int waitMS)
{
  cv::Mat         imgDisplay;
  image.convertTo(imgDisplay, CV_8U, alpha, beta);
  cv::imshow(winname, imgDisplay);

  if (waitMS >= 0)
    cv::waitKey(waitMS);
}

/**
  Display an image scaled so that minValue corresponds to black and maxValue to white.
*/
void imshowrange(const std::string& winname, const cv::Mat& image, const double minValue, const double maxValue, const int waitMS)
{
  double                scale   = 255. / (maxValue - minValue);
  if (scale != scale)   scale   = 1;
  
  cv::Mat               imgDisplay;
  image.convertTo(imgDisplay, CV_8U, scale, -scale*minValue);
  cv::imshow(winname, imgDisplay);

  if (waitMS >= 0)
    cv::waitKey(waitMS);
}

/**
  Convenience function to return the min and max data type range for a given
  cv::Mat bit depth. The return value is the range (max - min).
*/
double cvBitRange(const int bitDepth, double& minValue, double& maxValue)
{
  switch (bitDepth) {
  case CV_8U :    minValue  = std::numeric_limits<uchar >::min();
                  maxValue  = std::numeric_limits<uchar >::max();
                  break;                                
  case CV_8S :    minValue  = std::numeric_limits<schar >::min();
                  maxValue  = std::numeric_limits<schar >::max();
                  break;                                
  case CV_16U:    minValue  = std::numeric_limits<ushort>::min();
                  maxValue  = std::numeric_limits<ushort>::max();
                  break;                                
  case CV_16S:    minValue  = std::numeric_limits<short >::min();
                  maxValue  = std::numeric_limits<short >::max();
                  break;                                
  case CV_32S:    minValue  = std::numeric_limits<int   >::min();
                  maxValue  = std::numeric_limits<int   >::max();
                  break;                                
  case CV_32F:    minValue  = std::numeric_limits<float >::min();
                  maxValue  = std::numeric_limits<float >::max();
                  break;                                
  case CV_64F:    minValue  = std::numeric_limits<double>::min();
                  maxValue  = std::numeric_limits<double>::max();
                  break;
  default:        mexErrMsgIdAndTxt("cvCall:bitdepth", "Unsupported bit depth %d.", bitDepth);    break;
  }

  return (maxValue - minValue);
}
double cvBitRange(const int bitDepth)
{
  double  minValue, maxValue;
  return cvBitRange(bitDepth, minValue, maxValue);
}


/**
  Convenience function to get the bit depth of an image or stack of images
  (assumed identical in encoding type).
*/
int cvBitDepth(const std::vector<cv::Mat>& image)
{
  if (image.empty())
    return CV_8U;
    //mexErrMsgIdAndTxt("cvBitDepth:empty", "There are no images in the given stack.");
  return cvBitDepth(image[0]);
}

int cvBitDepth(const cv::Mat& image)
{
  if (image.empty())
    return CV_8U;
    //mexErrMsgIdAndTxt("cvBitDepth:empty", "Empty image structure encountered.");
  return image.depth();
}

/**
  Convenience function to get the data type of an image or stack of images
  (assumed identical in encoding type).
*/
int cvType(const std::vector<cv::Mat>& image)
{
  if (image.empty())
    return CV_8U;
    //mexErrMsgIdAndTxt("cvType:empty", "There are no images in the given stack.");
  return cvType(image[0]);
}

int cvType(const cv::Mat& image)
{
  if (image.empty())
    return CV_8U;
    //mexErrMsgIdAndTxt("cvType:empty", "Empty image structure encountered.");
  return image.type();
}

/**
  Convenience function to get the number of channels of an image or stack of 
  images (assumed identical in encoding type).
*/
int cvNumChannels(const std::vector<cv::Mat>& image)
{
  if (image.empty())
    return 1;
    //mexErrMsgIdAndTxt("cvNumChannels:empty", "There are no images in the given stack.");
  return cvNumChannels(image[0]);
}

int cvNumChannels(const cv::Mat& image)
{
  if (image.empty())
    return 1;
    //mexErrMsgIdAndTxt("cvNumChannels:empty", "Empty image structure encountered.");
  return image.channels();
}
