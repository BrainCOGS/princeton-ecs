/**
  Computes a motion corrected version of the given movie using cv::matchTemplate().
  Usage syntax:

    [xShifts, yShifts]  = cvMotionCorrect( inputPath, outputPath, maxShift, maxIter       ...
                                         , [minShiftChange = 0], [restrictInterp = false] ...
                                         , [methodInterp = 1], [methodCorr = 5]           ...
                                         );

  The output is stored in the given outputPath, **overwriting** if the file already exists.
  Nothing is returned to the Matlab environment.

  The memory usage of this function is numRows * numCols * (numZ + 1), i.e. one
  frame more than the input image stack. The median image is used as the template
  to which frames are aligned, except for a border of maxShift pixels in size which
  is omitted since it is possible for motion correction to crop up to that much of
  the frame.
*/


#include <mexopencv.hpp>
#include <opencv2\imgcodecs.hpp>
#include "quickSelect.h"
#include "cvUtils.h"


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
  if (nrhs < 4 || nrhs > 8 || nlhs > 2) {
    mexErrMsgIdAndTxt ( "cvMotionCorrect:usage"
                      , "Usage:\n   [xShifts, yShifts] = cvMotionCorrect(inputPath, outputPath, maxShift, maxIter, [minShiftChange = 0], [restrictInterp = false], [methodInterp = 1], [methodCorr = 5])"
                      );
  }
  if (!mxIsChar(prhs[0]))     mexErrMsgIdAndTxt("cvMotionCorrect:arguments", "inputPath must be a string.");
  if (!mxIsChar(prhs[1]))     mexErrMsgIdAndTxt("cvMotionCorrect:arguments", "inputPath must be a string.");

  
  char*                       inputPath       = mxArrayToString(prhs[0]);
  char*                       outputPath      = mxArrayToString(prhs[1]);
  const double                maxShift        = mxGetScalar(prhs[2]);
  const int                   maxIter         = mxGetScalar(prhs[3]);
  const double                minShiftChange  = nrhs > 4 ? mxGetScalar(prhs[4]) : 0.;
  const bool                  restrictInterp  = nrhs > 5 ? mxGetScalar(prhs[5]) : false;
  const int                   methodInterp    = nrhs > 6 ? mxGetScalar(prhs[6]) : 1;
  const int                   methodCorr      = nrhs > 7 ? mxGetScalar(prhs[7]) : CV_TM_CCOEFF_NORMED;

  //---------------------------------------------------------------------------

  // Load image with stored bit depth
  std::vector<cv::Mat>        imgOrig;
  if (!cv::imreadmulti(inputPath, imgOrig, cv::ImreadModes::IMREAD_ANYDEPTH | cv::ImreadModes::IMREAD_ANYCOLOR))
    mexErrMsgIdAndTxt( "cvMotionCorrect:load", "Failed to load input image." );
  if (imgOrig.empty())
    mexErrMsgIdAndTxt( "cvMotionCorrect:load", "Input image has no frames." );


  // Preallocate temporary storage for computations
  cv::Mat                     frmTemp(imgOrig[0].rows, imgOrig[0].cols, CV_32F);
  cv::Mat                     imgRef (imgOrig[0].rows, imgOrig[0].cols, CV_32F);
  std::vector<double>         traceTemp(imgOrig.size());

  const size_t                numFrames     = imgOrig.size();
  for (int iter = 0; iter < maxIter; ++iter) 
  {
    // Compute median image
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
      for (int iRow = 0; iRow < imgOrig[iFrame].row; ++iRow) {
        
        p = imgOrig[iFrame].ptr<uchar>(i);
        for (int iCol = 0; iCol < imgOrig[iFrame].col; ++iCol) {

        }
      }
    }

    //
    imgOrig[0].convertTo(frmTemp, CV_32F);
  } // end 
  //int d = imgOrig.depth();
  //int c =  imgOrig.channels();
  //bool bad = (imgOrig.data == NULL);
  
  //cv::Mat   frame();

  ////---------------------------------------------------------------------------

  //// Convert mxArray inputs into OpenCV types
  //cv::Ptr<cv::Mat> templateImgCV = ocvMxArrayToImage_uint8(prhs[0], true);
  //cv::Ptr<cv::Mat> imgCV         = ocvMxArrayToImage_uint8(prhs[1], true);
  //  
  //// Allocate output matrix
  //int outRows = imgCV->rows - templateImgCV->rows + 1;
  //int outCols = imgCV->cols - templateImgCV->cols + 1;
  //  
  //cv::Mat outCV((int)outRows, (int)outCols, CV_32FC1);
  //  
  //// Run the OpenCV template matching routine
  //cv::matchTemplate(*imgCV, *templateImgCV, outCV, CV_TM_CCOEFF_NORMED );
  //  
  //// Put the data back into the output MATLAB array
  //plhs[0] = ocvMxArrayFromImage_single(outCV);

  //---------------------------------------------------------------------------
  // Memory cleanup

  mxFree(inputPath);
  mxFree(outputPath);
 
  //---------------------------------------------------------------------------
  // Returned values
}

