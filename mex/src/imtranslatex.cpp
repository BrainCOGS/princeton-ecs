/**
  Applies row/column shifts (rigid translation) to each frame.

  Usage syntax:
    image = imtranslatex( original, xShift, yShift                                ...
                        , [xScale = 1], [yScale = 1], [nanMask = []]              ...
                        , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]    ...
                        );

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "lib/matUtils.h"
#include "lib/cvToMatlab.h"
#include "lib/imageCondenser.h"



/**
*/
template<typename Pixel>
class TranslateImage 
{
public:
  TranslateImage()
    : translator(2, 3, CV_32F)
    , xTrans    (translator.ptr<float>(0))
    , yTrans    (translator.ptr<float>(1))
    , offset    (0)
  {
    xTrans[0]   = 1;
    xTrans[1]   = 0;
    yTrans[0]   = 0;
    yTrans[1]   = 1;
  }

  void operator() ( const mxArray* matSource, void* targetPtr, const size_t tgtOffset
                  , const double* xShift, const double* yShift, const int methodInterp
                  , const bool* nanMask, const CondenserInfo2D* condenser, double emptyValue = mxGetNaN()
                  )
  {
    Pixel*            target    = (      Pixel*)  targetPtr;
    const Pixel*      source    = (const Pixel*)  mxGetData(matSource);
    const int         srcType   = mxGetClassID(matSource);
    const size_t*     dimension = mxGetDimensions(matSource);
    const size_t      srcOffset = dimension[0] * dimension[1];
    const size_t      numFrames = (mxGetNumberOfDimensions(matSource) > 2 ? dimension[2] : 1);

    // Setup temporary output
    MatlabToCVMatHelper<double,Pixel>   dataCopier;
    frmOrig.create(dimension[0], dimension[1], CV_64F);


    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame)
    {
      // Copy frame to high precision buffer
      dataCopier(frmOrig, source);

      // Perform an affine transformation i.e. sub-pixel shift via interpolation
      xTrans[2]       = float( *xShift );
      yTrans[2]       = float( *yShift );
      cv::warpAffine( frmOrig, frmShifted, translator, frmOrig.size()
                    , methodInterp, cv::BorderTypes::BORDER_CONSTANT, emptyValue
                    );

      // Move on to next shift
      ++xShift;
      ++yShift;

      // For area interpolation, can directly write to output since this is the last operation
      cvTypeCall<ImageCondenser2D, Pixel>(frmShifted, target, condenser, offset, nanMask, emptyValue);
      target         += tgtOffset;
      source         += srcOffset;
    }
  }

protected:
  cv::Mat             translator;
  float*              xTrans;
  float*              yTrans;
  cv::Mat             frmOrig;
  cv::Mat             frmShifted ;
  Pixel               offset;
};



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs < 3 || nrhs > 7 || nlhs > 3) {
    mexEvalString("help cv.imtranslatex");
    mexErrMsgIdAndTxt ( "imtranslatex:usage", "Incorrect number of inputs/outputs provided." );
  }

  // Parse input
  const mxArray*              input           = prhs[0];
  const double*               xShift          = mxGetPr(prhs[1]);
  const double*               yShift          = mxGetPr(prhs[2]);
  const double                xScale          = ( nrhs >  3 && !mxIsEmpty(prhs[3]) ) ?     mxGetScalar(prhs[3])       : 1;
  const double                yScale          = ( nrhs >  4 && !mxIsEmpty(prhs[4]) ) ?     mxGetScalar(prhs[4])       : 1;
  const mxArray*              nanMask         =   nrhs >  5 ?                                          prhs[5]        : 0;
  const int                   methodInterp    =   nrhs >  6 ? int( mxGetScalar(prhs[6]) ) : cv::InterpolationFlags::INTER_LINEAR;

  // Sanity checks
  if (nanMask && !mxIsLogical(nanMask))
    mexErrMsgIdAndTxt( "imtranslatex:load", "nanMask must be a logical matrix.");


  //---------------------------------------------------------------------------

  // Create output structure
  const size_t*               dimension       = mxGetDimensions(input);
  const size_t                srcHeight       = dimension[0];
  const size_t                srcWidth        = dimension[1];
  const size_t                numFrames       = (mxGetNumberOfDimensions(input) > 2 ? dimension[2] : 1);

  // Ensure proper size of masks
  if (nanMask && (mxGetM(nanMask) != srcHeight || mxGetN(nanMask) != srcWidth))
    mexErrMsgIdAndTxt( "imtranslatex:load", "Incorrect size of nanMask, must be equal to original image size (width = %d, height = %d).", srcWidth, srcHeight);

  // Adjust for scaling 
  int                         imgWidth        = cvRound(srcWidth  * xScale);
  int                         imgHeight       = cvRound(srcHeight * yScale);
  CondenserInfo2D             condenser(srcWidth, srcHeight, imgWidth, imgHeight);
  CondenserInfo2D*            condenserInfo   = &condenser;

  // Call workhorse function
  size_t                      outDimension[]  = {imgHeight, imgWidth, numFrames};
  plhs[0]                     = mxCreateNumericArray(3, outDimension, mxGetClassID(input), mxREAL);
  void*                       target          = mxGetData(plhs[0]);
  const bool*                 masked          = ( nanMask ? (const bool*) mxGetData(nanMask) : 0 );
  const size_t                tgtPixels       = imgHeight * imgWidth;
  matlabCall<TranslateImage>(input, target, tgtPixels, xShift, yShift, methodInterp, masked, condenserInfo);
}
