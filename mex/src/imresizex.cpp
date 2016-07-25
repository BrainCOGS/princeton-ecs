/**
  Resizes an image. NaN-valued pixels are ignored.

  Usage syntax:
    filtered  = cv.imresizex(image, xScale, yScale, nanMask);

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include <opencv2/core.hpp>
#include "lib/matUtils.h"
#include "lib/imageCondenser.h"



template<template<typename,typename> class Functor, typename SrcPixel, typename TgtPixel>
void processFrames(const size_t numFrames, const int nFramePixels, const void* srcData, TgtPixel* target, const CondenserInfo2D* info, const TgtPixel offset = 0, const bool* masked = 0, const TgtPixel emptyValue = 0)
{
  Functor<SrcPixel, TgtPixel>     processor;
  const SrcPixel*                 source  = (const SrcPixel*) srcData;

  for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
    processor(source, target, info, offset, masked, emptyValue);
    source += nFramePixels;
  }
}


///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nlhs != 1 || nrhs < 3 || nrhs > 4) {
    mexEvalString("help cv.imresizex");
    mexErrMsgIdAndTxt ( "imresizex:usage", "Incorrect number of inputs/outputs provided." );
  }


  // Parse input
  const mxArray*              source          =               prhs[0];
  const double                xScale          =   mxGetScalar(prhs[1]);
  const double                yScale          =   mxGetScalar(prhs[2]);
  const mxArray*              nanMask         = ( nrhs >  3 ? prhs[3] : 0 );
  const bool*                 mask            = 0;

  if (nanMask) {
    if (!mxIsLogical(nanMask))
      mexErrMsgIdAndTxt( "imresizex:load", "nanMask must be a logical matrix.");
    mask                      = (const bool*) mxGetData(nanMask);
  }


  const size_t*               srcDim          = mxGetDimensions(source);
  const size_t                numFrames       = ( mxGetNumberOfDimensions(source) < 3 ? 1 : srcDim[2] );
  const int                   srcWidth        = srcDim[1];
  const int                   srcHeight       = srcDim[0];
  const int                   imgWidth        = cvRound(srcWidth  * xScale);
  const int                   imgHeight       = cvRound(srcHeight * yScale);
  CondenserInfo2D             condenser(srcWidth, srcHeight, imgWidth, imgHeight);


  //---------------------------------------------------------------------------
  // Create output structure
  size_t                      dimension[]     = {size_t(imgHeight), size_t(imgWidth), numFrames};
  plhs[0]                     = mxCreateNumericArray(3, dimension, mxSINGLE_CLASS, mxREAL);
  const int                   nFramePixels    = imgHeight * imgWidth;
  const void*                 srcData         = mxGetData(source);
  float*                      imgData         = (float*) mxGetData(plhs[0]);

  // Call the stack processor
  switch (mxGetClassID(source)) {
  case mxDOUBLE_CLASS:        processFrames<ImageCondenser2D, double            >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxSINGLE_CLASS:        processFrames<ImageCondenser2D, float             >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxCHAR_CLASS:
  case mxINT8_CLASS:          processFrames<ImageCondenser2D, char              >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxUINT8_CLASS:         processFrames<ImageCondenser2D, unsigned char     >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxINT16_CLASS:         processFrames<ImageCondenser2D, short             >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxUINT16_CLASS:        processFrames<ImageCondenser2D, unsigned short    >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxINT32_CLASS:         processFrames<ImageCondenser2D, int               >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxUINT32_CLASS:        processFrames<ImageCondenser2D, unsigned int      >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxINT64_CLASS:         processFrames<ImageCondenser2D, long long         >(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  case mxUINT64_CLASS:        processFrames<ImageCondenser2D, unsigned long long>(numFrames, nFramePixels, srcData, imgData, &condenser, 0.f, mask, (float) mxGetNaN());    break;
  default:                    mexErrMsgIdAndTxt( "imresizex:arguments", "Unsupported type of image." );
  }
  
  

  
  
  
  
  
  
  
  
}
