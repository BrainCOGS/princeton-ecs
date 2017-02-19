/**
  Gets information about an image stack with minimal amount of frame reading.

  The data structure returned by this function can be passed to subsequent
  functions like cv.imreadx() to actually load the frames, obtain motion
  correction shifts, and so forth.

  Usage syntax:
    info = imfinfox( inputPath, [lazy = false] );
  where inputPath can be either a string (single file), or a cellstring for 
  multi-file stacks.

  Note that if lazy = false, this function checks that all images in the stack 
  have the same width and height. An exception will be thrown if discrepancies 
  are encountered.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <vector>
#include <limits>
#include <algorithm>
#include <cctype>
#include <mex.h>
#include <tiffio.h>

#undef max



static const char       CHANNELS_NAME[] = "scanimage.SI.hChannels.channelSave";
static const size_t     N_CHANNELSNAME  = std::strlen(CHANNELS_NAME);



template<typename Number>
bool readVectorField(char*& desc, const char* fieldName, const size_t nameLength, std::vector<Number>& data)
{
  for (; *desc; ++desc) {
    // Locate field name
    if (std::strncmp(desc, fieldName, nameLength)) {
      while (*desc && *desc != '\n')          ++desc;     // skip until end of line
    }
    else {            // match
      for (desc += nameLength; *desc != '['; ++desc) {
        if (!*desc || *desc == '\n')
          return false;
        if (std::isdigit(*desc)) {
          --desc;
          break;
        }
      }

      // Parse results as base-10 integers
      for (++desc; *desc != ']' && *desc != '\n' && *desc;) {
        data.push_back(std::strtol(desc, &desc, 10));
        while (*desc == ';' || *desc == ',')  ++desc;     // skip item specifiers
      }

      while (*desc && *desc != '\n')          ++desc;     // skip until end of line
      return true;
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs < 1 || nrhs > 3) {
    mexEvalString("help ecs.imfinfox");
    mexErrMsgIdAndTxt ( "imfinfox:usage", "Incorrect number of inputs/outputs provided." );
  }

  // Handle single vs. multiple input files
  const mxArray*              input           = prhs[0];
  const bool                  lazy            = nrhs > 1 ? mxGetScalar(prhs[1]) > 0 : false;
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
  std::vector<int>            srcChannels;
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
    if (!TIFFGetField(img, TIFFTAG_SAMPLEFORMAT, &sampleFormat)) {
      double                  minValue;
      if (!TIFFGetField(img, TIFFTAG_SMINSAMPLEVALUE, &minValue)) {
        if (iIn < 1)
          mexWarnMsgIdAndTxt("imfinfox:header", "Failed to read sample format for '%s', guessing based on number of bits per sample.", inputPath[iIn]);
        minValue              = 0;
      }
      else if (iIn < 1)       mexWarnMsgIdAndTxt("imfinfox:header", "Failed to read sample format for '%s', deducing from minimum sample value instead.", inputPath[iIn]);
      if (bitsPerSample < 32)
        sampleFormat          = ( minValue < 0 ? SAMPLEFORMAT_INT : SAMPLEFORMAT_UINT );
      else  sampleFormat      = SAMPLEFORMAT_IEEEFP;
    }

    // Special case for ScanImage files: Parse image description tag
    char*                     desc          = NULL;
    std::vector<int>          channels;
    if (TIFFGetField(img, TIFFTAG_IMAGEDESCRIPTION, &desc)) {
      // Locate list of saved channels
      readVectorField(desc, CHANNELS_NAME, N_CHANNELSNAME, channels);
    }


    // Check for consistency across stack
    if (iIn > 0) {
      if (srcWidth  != width        )   mexErrMsgIdAndTxt("imfinfox:stack", "Image width for '%s' is inconsistent with other file(s)."    , inputPath[iIn]);
      if (srcHeight != height       )   mexErrMsgIdAndTxt("imfinfox:stack", "Image height for '%s' is inconsistent with other file(s)."   , inputPath[iIn]);
      if (srcBits   != bitsPerSample)   mexErrMsgIdAndTxt("imfinfox:stack", "Bits per sample for '%s' is inconsistent with other file(s).", inputPath[iIn]);
      if (srcFormat != sampleFormat )   mexErrMsgIdAndTxt("imfinfox:stack", "Sample format for '%s' is inconsistent with other file(s)."  , inputPath[iIn]);
      if (srcChannels.size() != channels.size())
        mexErrMsgIdAndTxt("imfinfox:stack", "Number of channels for '%s' is inconsistent with other file(s).", inputPath[iIn]);
      for (size_t iChannel = 0; iChannel < channels.size(); ++iChannel)
        if (srcChannels[iChannel] != channels[iChannel])
          mexErrMsgIdAndTxt("imfinfox:stack", "Channel %d for '%s' is inconsistent with other file(s).", channels[iChannel], inputPath[iIn]);
    }
    else {
      srcWidth                = width;
      srcHeight               = height;
      srcBits                 = bitsPerSample;
      srcFormat               = sampleFormat;
      srcChannels             = channels;
    }


    if (lazy) break;

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

  mxArray*                    matChannels     = mxCreateDoubleMatrix(1, srcChannels.size(), mxREAL);
  double*                     channels        = (double*) mxGetData(matChannels);
  for (size_t iChannel = 0; iChannel < srcChannels.size(); ++iChannel)
    channels[iChannel]        = srcChannels[iChannel];


  static const char*          OUT_FIELDS[]    = { "width"
                                                , "height"
                                                , "bitsPerSample"
                                                , "sampleFormat"
                                                , "frames"
                                                , "filePaths"
                                                , "fileFrames"
                                                , "channels"
                                                };
  plhs[0]                     = mxCreateStructMatrix(1, 1, 8, OUT_FIELDS);
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
  mxSetFieldByNumber(plhs[0], 0, 7, matChannels);
}
