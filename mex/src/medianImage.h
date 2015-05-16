/**
  Computes the median of a given stack of images.
*/


#ifndef CVMEDIANIMAGE_H
#define CVMEDIANIMAGE_H

#include <mexopencv.hpp>
#include "cvUtils.h"


template<typename Pixel>
struct MedianVecMat
{
  void operator()(std::vector<cv::Mat>        imgOrig;
  if (!cv::imreadmulti(inputPath, imgOrig, cv::ImreadModes::IMREAD_ANYDEPTH | cv::ImreadModes::IMREAD_ANYCOLOR))
    mexErrMsgIdAndTxt( "cvMotionCorrect:load", "Failed to load input image." );
  if (imgOrig.empty())
    mexErrMsgIdAndTxt( "cvMotionCorrect:load", "Input image has no frames." );


  // Preallocate temporary storage for computations
  cv::Mat                     frmTemp(imgOrig[0].rows, imgOrig[0].cols, CV_32F);
  cv::Mat                     imgRef (imgOrig[0].rows, imgOrig[0].cols, CV_32F);
  std::vector<double>         traceTemp(imgOrig.size());

  const size_t                numFrames     = imgOrig.size();
  for (int iter = 0; iter < maxIter; ++iter) 
  {
    // Compute median image
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
      for (int iRow = 0; iRow < imgOrig[iFrame].row; ++iRow) {
        
        p = imgOrig[iFrame].ptr<uchar>(i);
        for (int iCol = 0; iCol < imgOrig[iFrame].col; ++iCol) {

        }
      }
    }

    //
    imgOrig[0].convertTo(frmTemp, CV_32F);
  } // end 
  //int d = imgOrig.depth();
  //int c =  imgOrig.channels();
  //bool bad = (imgOrig.data == NULL);
  
  //cv::Mat   frame();

  ////---------------------------------------------------------------------------

  //// Convert mxArray inputs into OpenCV types
  //cv::Ptr<cv::Mat> templateImgCV = ocvMxArrayToImage_uint8(prhs[0], true);
  //cv::Ptr<cv::Mat> imgCV         = ocvMxArrayToImage_uint8(prhs[1], true);
  //  
  //// Allocate output matrix
  //int outRows = imgCV->rows - templateImgCV->rows + 1;
  //int outCols = imgCV->cols - templateImgCV->cols + 1;
  //  
  //cv::Mat outCV((int)outRows, (int)outCols, CV_32FC1);
  //  
  //// Run the OpenCV template matching routine
  //cv::matchTemplate(*imgCV, *templateImgCV, outCV, CV_TM_CCOEFF_NORMED );
  //  
  //// Put the data back into the output MATLAB array
  //plhs[0] = ocvMxArrayFromImage_single(outCV);

  //---------------------------------------------------------------------------
  // Memory cleanup

  mxFree(inputPath);
  mxFree(outputPath);
 
  //---------------------------------------------------------------------------
  // Returned values
}


#endif //CVMEDIANIMAGE_H
