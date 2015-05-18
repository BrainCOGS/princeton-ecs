#include <cmath>
#include <mex.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "matUtils.h"


/**
  Display an equalized image.
*/
void imshoweq(const std::string& winname, const cv::Mat& image, const int waitMS)
{
  //double          minValue, maxValue;
  //cv::minMaxLoc(image, &minValue, &maxValue);
  double          minValue  = -200;
  double          maxValue  = 600;

  cv::Mat         imgDisplay;
  const double    scale   = 255. / std::max(1., maxValue - minValue);
  image.convertTo(imgDisplay, CV_8U, scale, -scale*minValue);

  //cv::equalizeHist(imgDisplay, imgDisplay);
  cv::imshow(winname, imgDisplay);

  if (waitMS >= 0)
    cv::waitKey(waitMS);
}



/**
  Convenience function to get the bit depth of an image or stack of images
  (assumed identical in encoding type).
*/
int cvBitDepth(const std::vector<cv::Mat>& image)
{
  if (image.empty())
    mexErrMsgIdAndTxt("cvBitDepth:empty", "There are no images in the given stack.");
  return cvBitDepth(image[0]);
}

int cvBitDepth(const cv::Mat& image)
{
  if (image.empty())
    mexErrMsgIdAndTxt("cvBitDepth:empty", "Empty image structure encountered.");
  return image.depth();
}

/**
  Convenience function to get the data type of an image or stack of images
  (assumed identical in encoding type).
*/
int cvType(const std::vector<cv::Mat>& image)
{
  if (image.empty())
    mexErrMsgIdAndTxt("cvType:empty", "There are no images in the given stack.");
  return cvType(image[0]);
}

int cvType(const cv::Mat& image)
{
  if (image.empty())
    mexErrMsgIdAndTxt("cvType:empty", "Empty image structure encountered.");
  return image.type();
}

/**
  Convenience function to get the number of channels of an image or stack of 
  images (assumed identical in encoding type).
*/
int cvNumChannels(const std::vector<cv::Mat>& image)
{
  if (image.empty())
    mexErrMsgIdAndTxt("cvNumChannels:empty", "There are no images in the given stack.");
  return cvNumChannels(image[0]);
}

int cvNumChannels(const cv::Mat& image)
{
  if (image.empty())
    mexErrMsgIdAndTxt("cvNumChannels:empty", "Empty image structure encountered.");
  return image.channels();
}
