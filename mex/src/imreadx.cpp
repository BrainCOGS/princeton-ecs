/**
  Loads the given image stack into memory, applying row/column shifts (rigid translation) to each frame.
 
  If sub-pixel registration is requested, cv::warpAffine() is used.
  Usage syntax:
    [image, stats, median] = imreadx( inputPath, xShift, yShift, [xScale = 1], [yScale = 1] ...
                                    , [blackTolerance = nan], [subtractZero = false]        ...
                                    , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]  ...
                                    , [methodResize = cve.InterpolationFlags.INTER_AREA]    ...
                                    );

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <libtiff/tiffio.h>
#include <mex.h>
#include "lib/matUtils.h"
#include "lib/tiffStack.h"
#include "lib/cvToMatlab.h"
#include "lib/manipulateImage.h"
#include "lib/imageStatistics.h"


#define _DO_NOT_EXPORT
#if defined(_DO_NOT_EXPORT)
#define DllExport  
#else
#define DllExport __declspec(dllexport)
#endif



/**
*/
template<typename Pixel>
class ImageProcessor
{
public:
  ImageProcessor() 
    : nFramePixels  (0)
    , emptyNSigmas  (5)
    , emptyProb     (-999)
    , offset        (0)
    , maxZeroValue  (std::numeric_limits<double>::infinity())
    , numFrames     (0)
    , translator    (2, 3, CV_32F)
    , xTrans        (translator.ptr<float>(0))
    , yTrans        (translator.ptr<float>(1))
    , emptyValue    ( mxGetNaN() )
  {
    xTrans[0]                   = 1;
    xTrans[1]                   = 0;
    yTrans[0]                   = 0;
    yTrans[1]                   = 1;
  }

  void operator()(const cv::Mat& image)
  {
    const cv::Mat*              source  = 0;
    cv::Mat*                    target  = &frmTemp;
    ++numFrames;

    //---------------------------------------------------------------------------
    //  Black frame detection
    //---------------------------------------------------------------------------

    if (emptyProb > 0) {
      // Use the first frame to estimate the black level and variance
      bool                      isEmpty = false;
      if (numFrames == 1) {
        cvCall<AccumulateMatStatistics>(image, statistics);

        // Account for multiple samples when computing the fraction of pixels that are
        // expected to fall below the zero + noise threshold
        emptyProb               = std::pow(emptyProb, nFramePixels);
        offset                  = statistics.getMean();
        maxZeroValue            = offset + emptyNSigmas * statistics.getRMS();
        isEmpty                 = true;   // by definition
      }
      else {
        static const double     negInf  = -std::numeric_limits<double>::infinity();
        int                     numZeros;
        cvCall<CountPixelsInRange>(image, negInf, maxZeroValue, numZeros);
        if (numZeros >= emptyProb * image.rows * image.cols)
          isEmpty               = true;
      }

      // Special case where the entire frame should be set to NaN
      if (isEmpty) {
        static const float      nan     = mxGetNaN();
        for (int iPix = 0; iPix < nFramePixels; ++iPix, ++imgData)
          *imgData              = nan;

        // Move on to next shift if they have been provided
        if (xShift) {
          ++xShift;
          ++yShift;
        }
        return;                 // Skip all further processing
      }
    }


    //---------------------------------------------------------------------------
    //  Translation
    //---------------------------------------------------------------------------

    if (xShift) {
      // Unfortunately we have to copy the frame before warping, because otherwise
      // the operations are performed with the input and not output precision
      image.convertTo(frmClone, CV_32F);

      // Perform an affine transformation i.e. sub-pixel shift via interpolation
      if (methodInterp >= 0)
      {
        xTrans[2]               = float( *xShift );
        yTrans[2]               = float( *yShift );
        cv::warpAffine( frmClone, frmTemp, translator, image.size()
                      , methodInterp, cv::BorderTypes::BORDER_CONSTANT, emptyValue
                      );
      }

      // Perform a simple pixel shift
      else {
        frmTemp.create(image.size(), CV_32F);
        cvCall<CopyShiftedImage32>(frmTemp, frmClone, *xShift, *yShift, emptyValue[0]);
      }

      // Move on to next shift
      ++xShift;
      ++yShift;

      // Swap source and scratch space for next operation
      source                  = &frmTemp;
      target                  = &frmClone;
    }
    else {
      // Use original source for the next operation
      source                  = &image;
      target                  = &frmTemp;
    } // translation


    //---------------------------------------------------------------------------
    //  Resize
    //---------------------------------------------------------------------------
    if (methodResize >= 0) {
      cv::resize(*source, *target, cv::Size(), xScale, yScale, methodResize);

      // Set source for the next operation
      source                  = target;
    } // resize

   
    //---------------------------------------------------------------------------
    //  Copy to Matlab
    //---------------------------------------------------------------------------
    cvCall<MatToMatlab32>(*source, imgData, offset);
  }


public:
  cv::Mat           frmClone;
  cv::Mat           frmTemp ;
  Pixel*            imgData;
  double*           xShift;
  double*           yShift;
  double            xScale;
  double            yScale;
  bool              subtractZero;
  int               methodInterp;
  int               methodResize;

  int               nFramePixels;
  double            emptyNSigmas;
  double            emptyProb;
  
  SampleStatistics  statistics;
  double            offset;
  double            maxZeroValue;

protected:
  int               numFrames;
  cv::Mat           translator;
  float*            xTrans;
  float*            yTrans;
  const cv::Scalar  emptyValue;
};



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs < 1 || nrhs > 9 || nlhs > 3) {
    mexEvalString("help cv.imreadx");
    mexErrMsgIdAndTxt ( "imreadx:usage", "Incorrect number of inputs/outputs provided." );
  }

  // Handle single vs. multiple input files
  const mxArray*              input           = prhs[0];
  std::vector<char*>          inputPath;
  if (mxIsCell(input)) {
    inputPath.resize(mxGetNumberOfElements(input));
    for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) {
      inputPath[iIn]          = mxArrayToString(mxGetCell(input, iIn));
      if (!inputPath[iIn])    mexErrMsgIdAndTxt("imreadx:arguments", "Non-string item encountered in inputPath array.");
    }
  }
  else if (!mxIsChar(prhs[0]))
    mexErrMsgIdAndTxt("imreadx:arguments", "inputPath must be a string or array of strings.");
  else
    inputPath.push_back( mxArrayToString(input) );


  // Parse input
  ImageProcessor<float>       processor;
  processor.xShift            = ( nrhs > 1 && !mxIsEmpty(prhs[1]) ) ? mxGetPr(prhs[1])          : 0     ;
  processor.yShift            = ( nrhs > 2 && !mxIsEmpty(prhs[2]) ) ? mxGetPr(prhs[2])          : 0     ;
  processor.xScale            = ( nrhs > 3 && !mxIsEmpty(prhs[3]) ) ? mxGetScalar(prhs[3])      : -999  ;
  processor.yScale            = ( nrhs > 4 && !mxIsEmpty(prhs[4]) ) ? mxGetScalar(prhs[4])      : -999  ;
  processor.emptyProb         = ( nrhs > 5 ?                          mxGetScalar(prhs[5])      : -999  );
  processor.subtractZero      = ( nrhs > 6 ?                         (mxGetScalar(prhs[6]) > 0) : false );
  processor.methodInterp      = ( nrhs > 7 ? int( mxGetScalar(prhs[7]) ) : cv::InterpolationFlags::INTER_LINEAR );
  processor.methodResize      = ( nrhs > 8 ? int( mxGetScalar(prhs[8]) ) : cv::InterpolationFlags::INTER_AREA   );
  
  // Sanity checks
  if ((processor.xShift == 0) != (processor.yShift == 0))
    mexErrMsgIdAndTxt( "imreadx:load", "If xShift is provided, yShift must be provided as well, and vice versa.");
  if ((processor.xScale <= 0) != (processor.yScale <= 0))
    mexErrMsgIdAndTxt( "imreadx:load", "If xScale is provided, yScale must be provided as well, and vice versa.");
  if (processor.xScale <= 0)
    processor.methodResize    = -1;

  // Suppress warnings as they block execution until the user clicks the dialog box
  TIFFErrorHandler            errHandler      = TIFFSetWarningHandler(NULL);


  //---------------------------------------------------------------------------
  // Get parameters of image stack
  uint32                      imgWidth        = 0;
  uint32                      imgHeight       = 0;
  int                         numFrames       = 0;
  for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) 
  {
    TIFF*                     tif             = TIFFOpen(inputPath[iIn], "r");
    if (!tif)                 mexErrMsgIdAndTxt( "imreadx:load", "Failed to load input image from %s.", inputPath[iIn] );

    uint32                    width, height;
    if (  !TIFFGetField( tif, TIFFTAG_IMAGEWIDTH , &width  )
      ||  !TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &height )
       )
      mexErrMsgIdAndTxt( "imreadx:load", "Invalid image header, could not obtain width and height from %s.", inputPath[iIn] );

    if (iIn < 1) {
      imgWidth                = width;
      imgHeight               = height;
    }
    else if (width != imgWidth || height != imgHeight)
      mexErrMsgIdAndTxt( "imreadx:load", "Inconsistent image width (%d vs. %d) or height (%d vs. %d) in %s.", inputPath[iIn], width, imgWidth, height, imgHeight );

    do { ++numFrames; } while (TIFFReadDirectory(tif));
    TIFFClose(tif);
  }

  // Adjust for scaling if provided
  if (processor.methodResize >= 0) {
    imgWidth                  = cvRound(imgWidth  * processor.xScale);
    imgHeight                 = cvRound(imgHeight * processor.yScale);
  }

  //---------------------------------------------------------------------------
  // Create output structure
  size_t                      dimension[]     = {imgHeight, imgWidth, numFrames};
  plhs[0]                     = mxCreateNumericArray(3, dimension, mxSINGLE_CLASS, mxREAL);
  processor.nFramePixels      = imgHeight * imgWidth;
  processor.imgData           = (float*) mxGetData(plhs[0]);

  // Check that we have enough frame shifts
  if (processor.xShift) {
    if (mxGetNumberOfElements(prhs[1]) != numFrames)
      mexErrMsgIdAndTxt( "imreadx:load", "Number of xShift (%d) is not equal to the number of frames (%d) in this image stack.", mxGetNumberOfElements(prhs[1]), numFrames);
    if (mxGetNumberOfElements(prhs[2]) != numFrames)
      mexErrMsgIdAndTxt( "imreadx:load", "Number of yShift (%d) is not equal to the number of frames (%d) in this image stack.", mxGetNumberOfElements(prhs[2]), numFrames);
  }

  //---------------------------------------------------------------------------
  // Call the stack processor
  for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) 
    processStack(inputPath[iIn], cv::ImreadModes::IMREAD_UNCHANGED, processor);

  // Return statistics structure if so requested
  if (nlhs > 1) {
    static const char*        STAT_FIELDS[]   = { "zeroLevel"
                                                , "zeroNoise"
                                                , "zeroThreshold"
                                                };
    plhs[1]                   = mxCreateStructMatrix(1, 1, 3, STAT_FIELDS);
    mxSetField(plhs[1], 0, "zeroLevel"    , mxCreateDoubleScalar(processor.statistics.getMean()));
    mxSetField(plhs[1], 0, "zeroNoise"    , mxCreateDoubleScalar(processor.statistics.getRMS()));
    mxSetField(plhs[1], 0, "zeroThreshold", mxCreateDoubleScalar(processor.maxZeroValue));
  }

  // Compute median if so requested
  if (nlhs > 2) {
    std::vector<float>        traceTemp(numFrames);
    plhs[2]                   = mxCreateNumericMatrix(imgHeight, imgWidth, mxSINGLE_CLASS, mxREAL);
    float*                    medData         = (float*) mxGetData(plhs[2]);

    // Loop over each pixel in the image stack
    const float*              pixCol          = processor.imgData;
    for (size_t iCol = 0; iCol < imgWidth; ++iCol) {
      for (size_t iRow = 0; iRow < imgHeight; ++iRow) {
        // Copy the stack of pixels into temporary storage
        int                   nTrace          = 0;
        for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
          const float         pixValue        = pixCol[iFrame * processor.nFramePixels];
          if (!mxIsNaN(pixValue)) {
            traceTemp[nTrace] = pixValue;
            ++nTrace;
          }
        }
        ++pixCol;

        // Store the computed median
        *medData              = quickSelect(traceTemp, nTrace);
        ++medData;
      } // end loop over columns
    } // end loop over rows
  }


  //---------------------------------------------------------------------------
  // Memory cleanup
  for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) 
    mxFree(inputPath[iIn]);

  // Restore default error handler
  TIFFSetWarningHandler(errHandler);
}
