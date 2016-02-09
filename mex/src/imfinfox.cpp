/**
  Gets information about an image stack with minimal amount of frame reading.

  The data structure returned by this function can be passed to subsequent
  functions like cv.imreadx() to actually load the frames, obtain motion
  correction shifts, and so forth.

  Usage syntax:
    info = imfinfox( inputPath );
  where inputPath can be either a string (single file), or a cellstring for 
  multi-file stacks.

  Note that this function checks that all images in the stack have the same
  width and height. An exception will be thrown if discrepancies are 
  encountered.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "lib/matUtils.h"
#include "lib/cvToMatlab.h"
#include "lib/manipulateImage.h"
#include "lib/imageStatistics.h"
#include "lib/imageCondenser.h"



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs < 1 || nrhs > 2 || nlhs != 1) {
    mexEvalString("help cv.imfinfox");
    mexErrMsgIdAndTxt ( "imfinfox:usage", "Incorrect number of inputs/outputs provided." );
  }

  // Handle single vs. multiple input files
  const mxArray*              input           = prhs[0];
  std::vector<char*>          inputPath;
  if (mxIsCell(input)) {
    inputPath.resize(mxGetNumberOfElements(input));
    for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) {
      inputPath[iIn]          = mxArrayToString(mxGetCell(input, iIn));
      if (!inputPath[iIn])    mexErrMsgIdAndTxt("imfinfox:arguments", "Non-string item encountered in inputPath array.");
    }
  }
  else if (!mxIsChar(prhs[0]))
    mexErrMsgIdAndTxt("imfinfox:arguments", "inputPath must be a string or cell array of strings.");
  else
    inputPath.push_back( mxArrayToString(input) );

  
  // Parse input
  const int                   maxNumFrames  = ( nrhs > 1 && !mxIsEmpty(prhs[1]) && mxIsFinite(mxGetScalar(prhs[1])) ) 
                                            ? cv::saturate_cast<int>(mxGetScalar(prhs[1]))
                                            : std::numeric_limits<int>::max()
                                            ;


  //---------------------------------------------------------------------------
  // Get parameters of image stack
  int                         srcWidth        = 0;
  int                         srcHeight       = 0;
  int                         srcBits         = 0;
  int                         numFrames       = 0;
  for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) {
    numFrames                += cv::imfinfo(inputPath[iIn], srcWidth, srcHeight, srcBits, iIn > 0);
    if (numFrames >= maxNumFrames)            break;
  }

  //---------------------------------------------------------------------------
  // Create output structure
  static const char*          OUT_FIELDS[]    = { "width"
                                                , "height"
                                                , "bitsPerSample"
                                                , "frames"
                                                };
  plhs[0]                     = mxCreateStructMatrix(1, 1, 4, OUT_FIELDS);
  mxSetFieldByNumber(plhs[0], 0, 0, mxCreateDoubleScalar(srcWidth));
  mxSetFieldByNumber(plhs[0], 0, 1, mxCreateDoubleScalar(srcHeight));
  mxSetFieldByNumber(plhs[0], 0, 2, mxCreateDoubleScalar(srcBits));
  mxSetFieldByNumber(plhs[0], 0, 3, mxCreateDoubleScalar(std::min(numFrames, maxNumFrames)));
}
