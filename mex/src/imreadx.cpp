/**
  Loads the given image stack into memory, applying row/column shifts (rigid translation) to each frame.
 
  If sub-pixel registration is requested, cv::warpAffine() is used.
  Usage syntax:
    image = imreadx( inputPath, xShift, yShift, xScale, yScale, [methodInterp = cve.InterpolationFlags.INTER_LINEAR], [methodResize = cve.InterpolationFlags.INTER_AREA] );

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <libtiff/tiffio.h>
#include <mex.h>
#include "lib\matUtils.h"
#include "lib\tiffStack.h"
#include "lib\cvToMatlab.h"
#include "lib\manipulateImage.h"


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
    : translator(2, 3, CV_32F)
    , xTrans    (translator.ptr<float>(0))
    , yTrans    (translator.ptr<float>(1))
    , emptyValue( mxGetNaN() )
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
    cvCall<MatToMatlab32>(*source, imgData);
  }


public:
  cv::Mat           frmClone;
  cv::Mat           frmTemp ;
  Pixel*            imgData;
  double*           xShift;
  double*           yShift;
  double            xScale;
  double            yScale;
  int               methodInterp;
  int               methodResize;

protected:
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
  if (nrhs < 1 || nrhs > 7 || nlhs > 1) {
    mexEvalString("help cv.imreadx");
    mexErrMsgIdAndTxt ( "imreadx:usage", "Incorrect number of inputs/outputs provided." );
  }
  if (!mxIsChar(prhs[0]))     mexErrMsgIdAndTxt("imreadx:arguments", "inputPath must be a string.");


  // Parse input
  ImageProcessor<float>       processor;
  char*                       inputPath       = mxArrayToString(prhs[0]);
  processor.xShift            = ( nrhs > 1 && !mxIsEmpty(prhs[1]) ) ? mxGetPr(prhs[1])      : 0     ;
  processor.yShift            = ( nrhs > 2 && !mxIsEmpty(prhs[2]) ) ? mxGetPr(prhs[2])      : 0     ;
  processor.xScale            = ( nrhs > 3 && !mxIsEmpty(prhs[3]) ) ? mxGetScalar(prhs[3])  : -999  ;
  processor.yScale            = ( nrhs > 4 && !mxIsEmpty(prhs[4]) ) ? mxGetScalar(prhs[4])  : -999  ;
  processor.methodInterp      = ( nrhs > 5 ? int( mxGetScalar(prhs[5]) ) : cv::InterpolationFlags::INTER_LINEAR );
  processor.methodResize      = ( nrhs > 6 ? int( mxGetScalar(prhs[6]) ) : cv::InterpolationFlags::INTER_AREA   );
  
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
  TIFF*                       tif             = TIFFOpen(inputPath, "r");
  if (!tif)                   mexErrMsgIdAndTxt( "imreadx:load", "Failed to load input image." );

  uint32                      imgWidth, imgHeight;
  if (  !TIFFGetField( tif, TIFFTAG_IMAGEWIDTH , &imgWidth  )
    ||  !TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &imgHeight )
     )
    mexErrMsgIdAndTxt( "imreadx:load", "Invalid image header, could not obtain width and height." );

  int                         numFrames       = 0;
  do { ++numFrames; } while (TIFFReadDirectory(tif));
  TIFFClose(tif);

  // Adjust for scaling if provided
  if (processor.methodResize >= 0) {
    imgWidth                  = cvRound(imgWidth  * processor.xScale);
    imgHeight                 = cvRound(imgHeight * processor.yScale);
  }

  //---------------------------------------------------------------------------
  // Create output structure
  size_t                      dimension[]     = {imgHeight, imgWidth, numFrames};
  plhs[0]                     = mxCreateNumericArray(3, dimension, mxSINGLE_CLASS, mxREAL);
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
  processStack(inputPath, cv::ImreadModes::IMREAD_UNCHANGED, processor);

  //---------------------------------------------------------------------------
  // Memory cleanup
  mxFree(inputPath);

  // Restore default error handler
  TIFFSetWarningHandler(errHandler);
}
