/**
  Computes a motion corrected version of the given movie using cv::matchTemplate().

  Usage syntax:
    [xShifts, yShifts]  = cvMotionCorrect( inputPath, outputPath, maxShift, maxIter                 ...
                                         , [minShiftChange = 0], [restrictInterp = false]           ...
                                         , [methodInterp = -1], [methodCorr = 5], [emptyValue = 0]  ...
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
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <libtiff\tiffio.h>
#include <mex.h>
#include "lib\matUtils.h"
#include "lib\medianImage.h"
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
  if (nrhs < 4 || nrhs > 9 || nlhs > 2) {
    mexErrMsgIdAndTxt ( "cvMotionCorrect:usage"
                      , "Usage:\n   [xShifts, yShifts] = cvMotionCorrect(inputPath, outputPath, maxShift, maxIter, [minShiftChange = 0], [restrictInterp = false], [methodInterp = 1], [methodCorr = 5], [emptyValue = 0])"
                      );
  }
  if (!mxIsChar(prhs[0]))     mexErrMsgIdAndTxt("cvMotionCorrect:arguments", "inputPath must be a string.");
  if (!mxIsChar(prhs[1]))     mexErrMsgIdAndTxt("cvMotionCorrect:arguments", "inputPath must be a string.");


  // Parse input
  char*                       inputPath       = mxArrayToString(prhs[0]);
  char*                       outputPath      = mxArrayToString(prhs[1]);
  const int                   maxShift        = int( mxGetScalar(prhs[2]) );
  const int                   maxIter         = int( mxGetScalar(prhs[3]) );
  const double                minShiftChange  = ( nrhs > 4 ? mxGetScalar(prhs[4])          : 0.     );
  const bool                  restrictInterp  = ( nrhs > 5 ? mxGetScalar(prhs[5]) > 0      : false  );
  const int                   methodInterp    = ( nrhs > 6 ? int( mxGetScalar(prhs[6]) )   : cv::InterpolationFlags::INTER_LINEAR     );
  const int                   methodCorr      = ( nrhs > 7 ? int( mxGetScalar(prhs[7]) )   : cv::TemplateMatchModes::TM_CCOEFF_NORMED );
  const float                 emptyValue      = ( nrhs > 8 ? float( mxGetScalar(prhs[8]) ) : 0.f    );


  //---------------------------------------------------------------------------

  // Load image with stored bit depth
  std::vector<cv::Mat>        imgOrig;
  if (!cv::imreadmulti(inputPath, imgOrig, cv::ImreadModes::IMREAD_ANYDEPTH | cv::ImreadModes::IMREAD_ANYCOLOR))
    mexErrMsgIdAndTxt( "cvMotionCorrect:load", "Failed to load input image." );
  if (imgOrig.empty())
    mexErrMsgIdAndTxt( "cvMotionCorrect:load", "Input image has no frames." );
  if (imgOrig[0].cols * imgOrig[0].rows < 3)
    mexErrMsgIdAndTxt( "cvMotionCorrect:load", "Input image too small, must have at least 3 pixels." );

  // Create output structures
  const size_t                numFrames       = imgOrig.size();
  double*                     xShifts         = 0;
  double*                     yShifts         = 0;
  if (nlhs > 0)             { plhs[0]         = mxCreateDoubleMatrix(numFrames, maxIter, mxREAL);    xShifts = mxGetPr(plhs[0]); }
  if (nlhs > 1)             { plhs[1]         = mxCreateDoubleMatrix(numFrames, maxIter, mxREAL);    yShifts = mxGetPr(plhs[1]); }


  //---------------------------------------------------------------------------

  // The template size restricts the maximum allowable shift
  const int                   firstRefRow     = min(maxShift, (imgOrig[0].rows - 1)/2);
  const int                   firstRefCol     = min(maxShift, (imgOrig[0].cols - 1)/2);

  // Preallocate temporary storage for computations
  cv::Mat                     frmClone  (imgOrig[0].rows                , imgOrig[0].cols                , imgOrig[0].type());
  cv::Mat                     frmTemp   (imgOrig[0].rows                , imgOrig[0].cols                , CV_32F);
  cv::Mat                     imgRef    (imgOrig[0].rows - 2*firstRefRow, imgOrig[0].cols - 2*firstRefCol, CV_32F);
  cv::Mat                     metric    (2*firstRefRow + 1              , 2*firstRefCol + 1              , CV_32F);
  std::vector<float>          traceTemp(numFrames);

  // Translation matrix, for use with sub-pixel registration
  cv::Mat                     translator(2, 3, CV_32F);
  float*                      xTrans          = translator.ptr<float>(0);
  float*                      yTrans          = translator.ptr<float>(1);
  if (methodInterp >= 0) {
    xTrans[0]                 = 1;
    xTrans[1]                 = 0;
    yTrans[0]                 = 0;
    yTrans[1]                 = 1;
  }

  //cv::imshow("Original", imgOrig[0]);  cv::waitKey(1);

  for (int iter = 0; iter < maxIter; ++iter) 
  {
    // Compute median image
    cvCall<Median32VecMat>(imgOrig, imgRef, traceTemp, firstRefRow, firstRefCol);
    //cv::imshow("Template", imgRef);  cv::waitKey(1);

    // Loop through frames and correct each one
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
      imgOrig[iFrame].convertTo(frmTemp, CV_32F);
      //cv::imshow("Image", frmTemp);  cv::waitKey(1);

      // Obtain metric values for all possible shifts and find the optimum
      cv::Point               optimum;
      cv::matchTemplate(frmTemp, imgRef, metric, methodCorr);
      if (methodCorr == cv::TemplateMatchModes::TM_SQDIFF || methodCorr == cv::TemplateMatchModes::TM_SQDIFF_NORMED)
            cv::minMaxLoc(metric, NULL, NULL, &optimum, NULL    );
      else  cv::minMaxLoc(metric, NULL, NULL, NULL    , &optimum);

      // If interpolation is desired, use a gaussian peak fit to resolve it
      if (  (methodInterp >= 0)
        &&  (!restrictInterp || (iter > 0 && optimum.x < 2 && optimum.y < 2))
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
        xTrans[2]             = optimum.x + float( ( ln10 - ln12 ) / ( 2 * ln10 - 4 * ln11 + 2 * ln12 ) );
        yTrans[2]             = optimum.y + float( ( ln01 - ln21 ) / ( 2 * ln01 - 4 * ln11 + 2 * ln21 ) );

        // Unfortunately we have to clone into a Mat object of the same type
        // because affine transforms cannot work in-situ (nor across types)
        imgOrig[iFrame].copyTo(frmClone);
        cv::warpAffine(frmClone, imgOrig[iFrame], translator, imgOrig[iFrame].size(), methodInterp);

        // Store the applied shifts
        if (xShifts) {
          *xShifts            = xTrans[2];    ++xShifts;
          *yShifts            = yTrans[2];    ++yShifts;
        }
      }

      // In case of no sub-pixel interpolation, perform a simple (and fast) pixel shift
      else {
        cvCall<CopyShiftedImage32>(imgOrig[iFrame], frmTemp, optimum.y, optimum.x, emptyValue);

        // Store the applied shifts
        if (xShifts) {
          *xShifts            = optimum.x;    ++xShifts;
          *yShifts            = optimum.y;    ++yShifts;
        }
      }
    } // end loop over frames
  } // end loop over iterations


  //---------------------------------------------------------------------------
  // Output to disk
  TIFF*                       header          = TIFFOpen(inputPath, "f");
  TIFFClose(header);

  //---------------------------------------------------------------------------
  // Memory cleanup
  mxFree(inputPath);
  mxFree(outputPath);
}

