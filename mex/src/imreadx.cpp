/**
  Loads the given image stack into memory, applying row/column shifts (rigid translation) to each frame.
 
  If sub-pixel registration is requested, cv::warpAffine() is used.
  Usage syntax:
    image = imreadx( inputPath, dx, dy, [methodInterp = 1] );

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <mex.h>
#include "lib\matUtils.h"
#include "lib\cvToMatlab.h"
#include "lib\manipulateImage.h"


#define _DO_NOT_EXPORT
#if defined(_DO_NOT_EXPORT)
#define DllExport  
#else
#define DllExport __declspec(dllexport)
#endif




///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs < 3 || nrhs > 4 || nlhs > 1) {
    mexEvalString("help cv.imreadx");
    mexErrMsgIdAndTxt ( "imreadx:usage", "Incorrect number of inputs/outputs provided." );
  }
  if (!mxIsChar(prhs[0]))     mexErrMsgIdAndTxt("imreadx:arguments", "inputPath must be a string.");


  // Parse input
  char*                       inputPath       = mxArrayToString(prhs[0]);
  const double*               xShift          = mxGetPr(prhs[1]);
  const double*               yShift          = mxGetPr(prhs[2]);
  const int                   methodInterp    = ( nrhs > 3 ? int( mxGetScalar(prhs[3]) ) : cv::InterpolationFlags::INTER_LINEAR );
  const bool                  subPixelReg     = ( methodInterp >= 0 );


  //---------------------------------------------------------------------------

  // Load image with stored bit depth
  std::vector<cv::Mat>        imgStack;
  if (!cv::imreadmulti(inputPath, imgStack, cv::ImreadModes::IMREAD_ANYDEPTH | cv::ImreadModes::IMREAD_ANYCOLOR))
    mexErrMsgIdAndTxt( "imreadx:load", "Failed to load input image." );
  if (imgStack.empty())
    mexErrMsgIdAndTxt( "imreadx:load", "Input image has no frames." );
  if (imgStack[0].depth() > CV_32F)
    mexErrMsgIdAndTxt( "imreadx:bitdepth", "64-bit (double) precision data not supported yet." );

  // Create output structure
  const size_t                numFrames       = imgStack.size();
  size_t                      dimension[]     = {imgStack[0].rows, imgStack[0].cols, numFrames};
  plhs[0]                     = mxCreateNumericArray(3, dimension, mxSINGLE_CLASS, mxREAL);
  float*                      imgData         = (float*) mxGetData(plhs[0]);

  // Check that we have enough frame shifts
  if (mxGetNumberOfElements(prhs[1]) != numFrames)
    mexErrMsgIdAndTxt( "imreadx:load", "Number of dx shifts (%d) is not equal to the number of frames (%d) in this image stack.", mxGetNumberOfElements(prhs[1]), numFrames);
  if (mxGetNumberOfElements(prhs[2]) != numFrames)
    mexErrMsgIdAndTxt( "imreadx:load", "Number of dy shifts (%d) is not equal to the number of frames (%d) in this image stack.", mxGetNumberOfElements(prhs[1]), numFrames);

  // Translation matrix, for use with sub-pixel registration
  cv::Mat                     translator(2, 3, CV_32F);
  float*                      xTrans          = translator.ptr<float>(0);
  float*                      yTrans          = translator.ptr<float>(1);
  if (subPixelReg) {
    xTrans[0]                 = 1;
    xTrans[1]                 = 0;
    yTrans[0]                 = 0;
    yTrans[1]                 = 1;
  }

  // Temporary storage for computations
  cv::Mat                     frmClone  (imgStack[0].rows, imgStack[0].cols, CV_32F);
  cv::Mat                     frmTemp   (imgStack[0].rows, imgStack[0].cols, CV_32F);
  const cv::Scalar            emptyValue      ( mxGetNaN() );

  //---------------------------------------------------------------------------

  // Perform an affine transformation i.e. sub-pixel shift via interpolation
  if (subPixelReg)
  {
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
      // Unfortunately we have to copy the frame before warping, because otherwise
      // the operations are performed with the input and not output precision
      imgStack[iFrame].convertTo(frmClone, frmClone.type());

      xTrans[2]               = float( xShift[iFrame] );
      yTrans[2]               = float( yShift[iFrame] );
      cv::warpAffine( frmClone, frmTemp, translator, imgStack[iFrame].size()
                    , methodInterp, cv::BorderTypes::BORDER_CONSTANT, emptyValue
                    );
      cvCall<MatToMatlab32>(frmTemp, imgData);
    } // end loop over frames
  }

  // Perform a simple pixel shift
  else {
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
      cvCall<CopyShiftedImage32>(frmTemp, imgStack[iFrame], xShift[iFrame], yShift[iFrame], emptyValue[0]);
      cvCall<MatToMatlab32>(frmTemp, imgData);
    }
  }

  //---------------------------------------------------------------------------
  // Memory cleanup
  mxFree(inputPath);
}

