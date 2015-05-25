/**
  Computes a motion corrected version of the given movie using cv::matchTemplate().

  Usage syntax:
    [xShifts, yShifts, numIterations]
        = cv.motionCorrect( inputPath, maxShift, maxIter                            ...
                          , [displayProgress = false], [stopBelowShift = 0]         ...
                          , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]    ...
                          , [methodCorr = cve.TemplateMatchModes.TM_CCOEFF_NORMED]  ...
                          , [emptyValue = mean]                                     ...
                          );

  The output is stored in the given outputPath, **overwriting** if the file already exists.
  The history of x and y shifts can be returned to Matlab,

  The memory usage of this function is numRows * numCols * (numZ + 1), i.e. one
  frame more than the input image stack. The median image is used as the template
  to which frames are aligned, except for a border of maxShift pixels in size which
  is omitted since it is possible for motion correction to crop up to that much of
  the frame.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <cmath>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <mex.h>
#include "lib/matUtils.h"
#include "lib/medianImage.h"
#include "lib/manipulateImage.h"


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
  if (nrhs < 3 || nrhs > 8 || nlhs > 3) {
    mexEvalString("help cv.motionCorrect");
    mexErrMsgIdAndTxt ( "motionCorrect:usage", "Incorrect number of inputs/outputs provided." );
  }
  if (!mxIsChar(prhs[0]))     mexErrMsgIdAndTxt("motionCorrect:arguments", "inputPath must be a string.");


  // Parse input
  char*                       inputPath       = mxArrayToString(prhs[0]);
  const int                   maxShift        = int( mxGetScalar(prhs[1]) );
  const int                   maxIter         = int( mxGetScalar(prhs[2]) );
  const bool                  displayProgress = ( nrhs > 3 ? mxGetScalar(prhs[3]) > 0     : false  );
  const double                stopBelowShift  = ( nrhs > 4 ? mxGetScalar(prhs[4])         : 0.     );
  const int                   methodInterp    = ( nrhs > 5 ? int( mxGetScalar(prhs[5]) )  : cv::InterpolationFlags::INTER_LINEAR     );
  const int                   methodCorr      = ( nrhs > 6 ? int( mxGetScalar(prhs[6]) )  : cv::TemplateMatchModes::TM_CCOEFF_NORMED );
  const double                usrEmptyValue   = ( nrhs > 7 ?      mxGetScalar(prhs[7])    : 0.     );
  const bool                  emptyIsMean     = ( nrhs < 8 );
  const bool                  subPixelReg     = ( methodInterp >= 0 );


  //---------------------------------------------------------------------------

  // Load image with stored bit depth
  std::vector<cv::Mat>        imgStack;
  if (!cv::imreadmulti(inputPath, imgStack, cv::ImreadModes::IMREAD_UNCHANGED))
    mexErrMsgIdAndTxt( "motionCorrect:load", "Failed to load input image." );
  if (imgStack.empty())
    mexErrMsgIdAndTxt( "motionCorrect:load", "Input image has no frames." );
  if (imgStack[0].cols * imgStack[0].rows < 3)
    mexErrMsgIdAndTxt( "motionCorrect:load", "Input image too small, must have at least 3 pixels." );

  // Create output structures
  const size_t                numFrames       = imgStack.size();
  double*                     xShifts         = 0;
  double*                     yShifts         = 0;
  if (nlhs > 0)             { plhs[0]         = mxCreateDoubleMatrix(numFrames, maxIter, mxREAL);   xShifts = mxGetPr(plhs[0]); }
  else                      { xShifts         = new double[numFrames];              std::fill(xShifts, xShifts+numFrames, 0.);  }
  if (nlhs > 1)             { plhs[1]         = mxCreateDoubleMatrix(numFrames, maxIter, mxREAL);   yShifts = mxGetPr(plhs[1]); }
  else                      { yShifts         = new double[numFrames];              std::fill(xShifts, xShifts+numFrames, 0.);  }

  
  //---------------------------------------------------------------------------

  // The template size restricts the maximum allowable shift
  const int                   firstRefRow     = std::min(maxShift, (imgStack[0].rows - 1)/2);
  const int                   firstRefCol     = std::min(maxShift, (imgStack[0].cols - 1)/2);

  // Preallocate temporary storage for computations
  cv::Mat                     frmTemp   (imgStack[0].rows                , imgStack[0].cols                , CV_32F);
  cv::Mat                     imgRef    (imgStack[0].rows - 2*firstRefRow, imgStack[0].cols - 2*firstRefCol, CV_32F);
  cv::Mat                     metric    (2*firstRefRow + 1               , 2*firstRefCol + 1               , CV_32F);
  std::vector<float>          traceTemp(numFrames);
  std::vector<cv::Mat>        imgShifted(numFrames);

  // Obtain some global statistics to be used for data scaling and display
  int                         scratchType     = imgStack[0].depth();
  if (scratchType == CV_8U || scratchType == CV_8S)
    scratchType              += 2;
  const bool                  isIntegral      = ( scratchType < CV_32F );   // N.B. This depends on the ordering of OpenCV constants, which hopefully are unlikely to change

  double                      minValue        =  1e308;
  double                      maxValue        = -1e308;
  double                      meanValue       = 0;
  if (isIntegral || displayProgress)
  {
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
      double                  frmMin, frmMax;
      cv::minMaxLoc(imgStack[iFrame], &frmMin, &frmMax);
      minValue                = std::min(minValue, frmMin);
      maxValue                = std::max(maxValue, frmMax);
      if (emptyIsMean && !displayProgress)
        meanValue            += cv::mean(imgStack[iFrame])[0];
    } // end loop over frames
    if (maxValue <= minValue)
      mexErrMsgIdAndTxt( "motionCorrect:image", "Invalid range [%.3g, %.3g] of pixel values in image stack; the image cannot be completely uniform for motion correction.", minValue, maxValue);
  }
  meanValue                  /= numFrames;


  // For integer format data, precompute data range so that the temporary shifted 
  // images can be scaled to use the full range; this helps with precision issues
  // when sub-pixel registration is used. Also note that the minimum bit depth
  // used is 16, since it is likely that an 8 bit representation will lead to
  // severe truncation of sub-pixel shifts.

  double                      pixScale, pixShift;
  if (isIntegral) {
    pixScale                  = cvBitRange(scratchType) / (maxValue - minValue);
    pixShift                  = -pixScale* minValue;
  } else {
    pixScale                  = 1;
    pixShift                  = 0;
  }
  for (size_t iFrame = 0; iFrame < numFrames; ++iFrame)
    imgStack[iFrame].convertTo(imgShifted[iFrame], scratchType, pixScale, pixShift);

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


  // For display of images, predetermine a grayscale range
  double                      showMin, showMax;
  if (displayProgress) {
    double                    variance        = 0;
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
      cv::Scalar              mean, stddev;
      cv::meanStdDev(imgStack[iFrame], mean, stddev);
      meanValue              += mean[0];
      variance               += stddev[0] * stddev[0];
    } // end loop over frames
    meanValue                /= numFrames;

    const double              stddev          = sqrt(variance /= numFrames);
    showMin                   = pixScale * std::max(minValue, meanValue - 3*stddev) + pixShift;
    showMax                   = pixScale * std::min(maxValue, meanValue + 4*stddev) + pixShift;
  }
  
  const cv::Scalar            emptyValue( emptyIsMean ? pixScale * meanValue + pixShift : usrEmptyValue );

  
  //---------------------------------------------------------------------------

  int                         iteration       = 0;
  for (int iShift = 0; iteration < maxIter; ++iteration)
  {
    // Relative index at which the previous shifts were stored
    const int                 iPrevX          = ( nlhs < 1 || iteration < 1 ? 0 : numFrames );
    const int                 iPrevY          = ( nlhs < 2 || iteration < 1 ? 0 : numFrames );

    // Compute median image
    cvCall<MedianVecMat32>(imgShifted, imgRef, traceTemp, firstRefRow, firstRefCol);
    if (displayProgress)      imshowrange("Template", imgRef, showMin, showMax);

    // Loop through frames and correct each one
    double                    maxRelShift     = -1e308;
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame, ++iShift) {
      imgStack[iFrame].convertTo(frmTemp, CV_32F, pixScale, pixShift);
      //if (displayProgress)    imshowrange("Image", frmTemp, showMin, showMax);

      // Obtain metric values for all possible shifts and find the optimum
      cv::Point               optimum;
      cv::matchTemplate(frmTemp, imgRef, metric, methodCorr);
      if (methodCorr == cv::TemplateMatchModes::TM_SQDIFF || methodCorr == cv::TemplateMatchModes::TM_SQDIFF_NORMED)
            cv::minMaxLoc(metric, NULL, NULL, &optimum, NULL    );
      else  cv::minMaxLoc(metric, NULL, NULL, NULL    , &optimum);


      // If interpolation is desired, use a gaussian peak fit to resolve it
      double                  colShift, rowShift;
      if (   subPixelReg
        &&  (optimum.x > 0)
        &&  (optimum.x < metric.cols-1)
        &&  (optimum.y > 0)
        &&  (optimum.y < metric.rows-1)
         ) {
        
        // The following are the three rows centered at the optimum
        const float*          row0            = metric.ptr<float>(optimum.y - 1);
        const float*          row1            = metric.ptr<float>(optimum.y    );
        const float*          row2            = metric.ptr<float>(optimum.y + 1);

        // Precompute the log value once and for all
        const double          ln10            = log(row1[optimum.x - 1]);
        const double          ln11            = log(row1[optimum.x    ]);
        const double          ln12            = log(row1[optimum.x + 1]);
        const double          ln01            = log(row0[optimum.x    ]);
        const double          ln21            = log(row2[optimum.x    ]);

        // 1D Gaussian interpolation in each direction
        double                xPeak           = ( ln10 - ln12 ) / ( 2 * ln10 - 4 * ln11 + 2 * ln12 );
        double                yPeak           = ( ln01 - ln21 ) / ( 2 * ln01 - 4 * ln11 + 2 * ln21 );
        if (xPeak != xPeak)   xPeak           = 0;
        if (yPeak != yPeak)   yPeak           = 0;
        xTrans[2]             = colShift      = -( optimum.x - firstRefCol + xPeak );
        yTrans[2]             = rowShift      = -( optimum.y - firstRefRow + yPeak );

        // Perform an affine transformation i.e. sub-pixel shift via interpolation
        cv::warpAffine( frmTemp, imgShifted[iFrame], translator, frmTemp.size()
                      , methodInterp, cv::BorderTypes::BORDER_CONSTANT, emptyValue
                      );
      }

      // In case of no sub-pixel interpolation, perform a simple (and fast) pixel shift
      else {
        // Remember that the template is offset so shifts are relative to that
        colShift              = -( optimum.x - firstRefCol );
        rowShift              = -( optimum.y - firstRefRow );
        cvCall<CopyShiftedImage32>(imgShifted[iFrame], frmTemp, colShift, rowShift, emptyValue[0]);
      }

      // Record history of shifts
      maxRelShift             = std::max(maxRelShift, std::fabs(colShift - xShifts[iShift - iPrevX]));
      maxRelShift             = std::max(maxRelShift, std::fabs(rowShift - yShifts[iShift - iPrevY]));
      xShifts[iShift]         = colShift;
      yShifts[iShift]         = rowShift;
       
      if (displayProgress)    imshowrange("Corrected", imgShifted[iFrame], showMin, showMax);
    } // end loop over frames


    // Stop if the maximum shift relative to the previous iteration is small enough
    if (maxRelShift < stopBelowShift)           break;
    if (nlhs < 1)             iShift            = 0;
  } // end loop over iterations


  //---------------------------------------------------------------------------
  // Output
  if (nlhs > 2)               plhs[2]           = mxCreateDoubleScalar(iteration);


  //---------------------------------------------------------------------------
  // Memory cleanup
  mxFree(inputPath);

  if (nlhs < 1)               delete[] xShifts;
  if (nlhs < 2)               delete[] yShifts;
}

