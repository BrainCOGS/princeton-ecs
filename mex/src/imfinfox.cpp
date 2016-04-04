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


#include <vector>
#include <limits>
#include <algorithm>
#include <mex.h>
#include <tiffio.h>

#undef max



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs < 1 || nrhs > 2 || nlhs != 1) {
    mexEvalString("help ecs.imfinfox");
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
  const size_t                maxNumFrames  = ( nrhs > 1 && !mxIsEmpty(prhs[1]) && mxIsFinite(mxGetScalar(prhs[1])) ) 
                                            ? static_cast<size_t>(mxGetScalar(prhs[1]))
                                            : std::numeric_limits<size_t>::max()
                                            ;


  //---------------------------------------------------------------------------
  // Get parameters of image stack
  size_t                      srcWidth        = 0;
  size_t                      srcHeight       = 0;
  size_t                      srcBits         = 0;
  size_t                      srcFormat       = 0;
  size_t                      totalFrames     = 0;
  mxArray*                    matNumFrames    = mxCreateDoubleMatrix(1, inputPath.size(), mxREAL);
  double*                     numFrames       = mxGetPr(matNumFrames);
  for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) 
  {
    TIFF*                     img             = TIFFOpen(inputPath[iIn], "r");
    if (img == NULL)          mexErrMsgIdAndTxt("imfinfox:input", "Failed to load input file '%s'.", inputPath[iIn]);

    // Read info from TIFF header
    uint32                    width, height;
    if (!TIFFGetField(img, TIFFTAG_IMAGEWIDTH, &width))
      mexErrMsgIdAndTxt("imfinfox:header", "Failed to image width for '%s'.", inputPath[iIn]);
    if (!TIFFGetField(img, TIFFTAG_IMAGELENGTH, &height))
      mexErrMsgIdAndTxt("imfinfox:header", "Failed to image height for '%s'.", inputPath[iIn]);

    uint16                    bitsPerSample, sampleFormat;
    if (!TIFFGetField(img, TIFFTAG_BITSPERSAMPLE, &bitsPerSample))
      mexErrMsgIdAndTxt("imfinfox:header", "Failed to read bits per sample for '%s'.", inputPath[iIn]);
    if (!TIFFGetField(img, TIFFTAG_SAMPLEFORMAT, &sampleFormat))
      mexErrMsgIdAndTxt("imfinfox:header", "Failed to read sample format for '%s'.", inputPath[iIn]);

    // Check for consistency across stack
    if (iIn > 0) {
      if (srcWidth  != width        )   mexErrMsgIdAndTxt("imfinfox:stack", "Image width for '%s' is inconsistent with other file(s)."    , inputPath[iIn]);
      if (srcHeight != height       )   mexErrMsgIdAndTxt("imfinfox:stack", "Image height for '%s' is inconsistent with other file(s)."   , inputPath[iIn]);
      if (srcBits   != bitsPerSample)   mexErrMsgIdAndTxt("imfinfox:stack", "Bits per sample for '%s' is inconsistent with other file(s).", inputPath[iIn]);
      if (srcFormat != sampleFormat )   mexErrMsgIdAndTxt("imfinfox:stack", "Sample format for '%s' is inconsistent with other file(s)."  , inputPath[iIn]);
    }
    else {
      srcWidth                = width;
      srcHeight               = height;
      srcBits                 = bitsPerSample;
      srcFormat               = sampleFormat;
    }


    do {
      ++( numFrames[iIn] );
    } while (TIFFReadDirectory(img));

    totalFrames              += numFrames[iIn];
    if (totalFrames >= maxNumFrames)          break;
  }

  //---------------------------------------------------------------------------
  // Create output structure
  mxArray*                    matInput        = mxCreateCellMatrix(1, inputPath.size());
  for (size_t iIn = 0; iIn < inputPath.size(); ++iIn)
    mxSetCell(matInput, iIn, mxCreateString(inputPath[iIn]));

  static const char*          OUT_FIELDS[]    = { "width"
                                                , "height"
                                                , "bitsPerSample"
                                                , "sampleFormat"
                                                , "frames"
                                                , "filePaths"
                                                , "fileFrames"
                                                };
  plhs[0]                     = mxCreateStructMatrix(1, 1, 7, OUT_FIELDS);
  mxSetFieldByNumber(plhs[0], 0, 0, mxCreateDoubleScalar(srcWidth));
  mxSetFieldByNumber(plhs[0], 0, 1, mxCreateDoubleScalar(srcHeight));
  mxSetFieldByNumber(plhs[0], 0, 2, mxCreateDoubleScalar(srcBits));
  switch (srcFormat) {
  case SAMPLEFORMAT_UINT:           mxSetFieldByNumber(plhs[0], 0, 3, mxCreateString("UInt"));            break;
  case SAMPLEFORMAT_INT:            mxSetFieldByNumber(plhs[0], 0, 3, mxCreateString("Int"));             break;
  case SAMPLEFORMAT_IEEEFP:         mxSetFieldByNumber(plhs[0], 0, 3, mxCreateString("IEEEFP"));          break;
  case SAMPLEFORMAT_VOID:           mxSetFieldByNumber(plhs[0], 0, 3, mxCreateString("Void"));            break;
  case SAMPLEFORMAT_COMPLEXINT:     mxSetFieldByNumber(plhs[0], 0, 3, mxCreateString("ComplexInt"));      break;
  case SAMPLEFORMAT_COMPLEXIEEEFP:  mxSetFieldByNumber(plhs[0], 0, 3, mxCreateString("ComplexIEEEFP"));   break;
  default:                          mxSetFieldByNumber(plhs[0], 0, 3, mxCreateDoubleScalar(srcFormat));   break;
  }
  mxSetFieldByNumber(plhs[0], 0, 4, mxCreateDoubleScalar(std::min(totalFrames, maxNumFrames)));
  mxSetFieldByNumber(plhs[0], 0, 5, matInput);
  mxSetFieldByNumber(plhs[0], 0, 6, matNumFrames);
}
