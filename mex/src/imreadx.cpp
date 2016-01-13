/**
  Loads the given image stack into memory, applying row/column shifts (rigid translation) to each frame.

  Usage syntax:
    [image, stats, median] = imreadx( inputPath, [xShift = []], [yShift = []]               ...
                                    , [xScale = 1], [yScale = 1], [maxNumFrames = inf]      ...
                                    , [blackTolerance = nan], [subtractZero = false]        ...
                                    , [methodInterp = cve.InterpolationFlags.INTER_LINEAR]  ...
                                    , [methodResize = cve.InterpolationFlags.INTER_AREA]    ...
                                    , [nanMask = []]                                        ...
                                    );

  maxNumFrames = nan can be used to return only the statistics structure, which saves on memory load in 
  case the image stack is very large. Note that the median image cannot be computed in this case because
  it requires the full stack to be in memory.
 
  If sub-pixel registration is requested, cv::warpAffine() is used.

  Todo:     Binned median.
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



/**
*/
template<typename Pixel>
class ImageProcessor : public cv::MatFunction
{
public:
  ImageProcessor() 
    : imgData       (0)
    , xShift        (0)
    , yShift        (0)
    , xScale        (0)
    , yScale        (0)
    , maxNumFrames  (std::numeric_limits<int>::max())
    , subtractZero  (false)
    , methodInterp  (0)
    , methodResize  (0)
    , nanMask       (0)
    , nFramePixels  (0)
    , frameOffset   (0)
    , emptyNSigmas  (5)
    , emptyProb     (-999)
    , stackStats    (0)
    , offset        (0)
    , maxZeroValue  (std::numeric_limits<double>::infinity())
    , numFrames     (0)
    , translator    (2, 3, CV_32F)
    , xTrans        (translator.ptr<float>(0))
    , yTrans        (translator.ptr<float>(1))
    , emptyPix      ( static_cast<Pixel>(mxGetNaN()) )
    , emptyValue    ( mxGetNaN() )
    , condenser     (0)
  {
    xTrans[0]                   = 1;
    xTrans[1]                   = 0;
    yTrans[0]                   = 0;
    yTrans[1]                   = 1;
  }

  bool operator()(cv::Mat& image)
  {
    if (numFrames >= maxNumFrames)
      return false;

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
        offset                  = static_cast<Pixel>( statistics.getMean() );
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
        if (frameOffset > 0) {  // Only required in case of storing data
          static const Pixel    nan     = static_cast<Pixel>( mxGetNaN() );
          for (int iPix = 0; iPix < nFramePixels; ++iPix, ++imgData)
            *imgData            = nan;
        }

        // Move on to next shift if they have been provided
        if (xShift) {
          ++xShift;
          ++yShift;
        }
        return true;            // Skip all further processing
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
        cvCall<CopyShiftedImage32>(frmTemp, frmClone, *xShift, *yShift, emptyPix);
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
    //  One-shot masking and resize
    //---------------------------------------------------------------------------

    // For area interpolation, can directly write to output since this is the last operation
    if (methodResize == cv::InterpolationFlags::INTER_AREA)
      cvTypeCall<ImageCondenser2D, Pixel>(*source, imgData, condenser, offset, nanMask, emptyPix);


    //-------------------------------------------------------------------------
    //  Apply mask only
    //-------------------------------------------------------------------------

    else if (methodResize <= 0)
      cvMatlabCall<MatToMatlab>(*source, imgClass, imgData, offset, nanMask, emptyPix);


    //---------------------------------------------------------------------------
    //  Mask and resize (but does not ignore nan)
    //---------------------------------------------------------------------------

    // No support for other type of resizing methods; use default functions regardless of ability to ignore nan
    else {
      if (nanMask) {
        target->convertTo(frmClone, CV_32F);
        cvCall<MaskPixels>(frmClone, nanMask, emptyPix);
        target                = &frmClone;
      } // apply mask

      cv::resize(*source, *target, cv::Size(), xScale, yScale, methodResize);

      // Set source for the next operation
      source                  = target;


      // Copy to Matlab
      cvMatlabCall<MatToMatlab>(*source, imgClass, imgData, offset);
    } // resize



    //---------------------------------------------------------------------------
    //  Compute image statistics
    //---------------------------------------------------------------------------

    if (stackStats)
      stackStats->add(imgData);


    //---------------------------------------------------------------------------
    //  Advance write pointer
    //---------------------------------------------------------------------------

    imgData                  += frameOffset;

    return true;
  }


public:
  cv::Mat             frmClone;
  cv::Mat             frmTemp ;
  mxClassID           imgClass;
  Pixel*              imgData;
  double*             xShift;
  double*             yShift;
  double              xScale;
  double              yScale;
  int                 maxNumFrames;
  bool                subtractZero;
  int                 methodInterp;
  int                 methodResize;
  const bool*         nanMask;
  CondenserInfo2D*    condenser;

  int                 nFramePixels;
  int                 frameOffset;
  double              emptyNSigmas;
  double              emptyProb;
  
  ImageStatistics*    stackStats;
  SampleStatistics    statistics;
  Pixel               offset;
  double              maxZeroValue;

protected:
  int                 numFrames;
  cv::Mat             translator;
  float*              xTrans;
  float*              yTrans;
  Pixel               emptyPix;
  const cv::Scalar    emptyValue;
};


//_________________________________________________________________________
void checkNumShifts(const mxArray* matShifts, double*& ptrShifts, const int firstFrame, const int numFrames, const char* name)
{
  const int           numRows     = mxGetM(matShifts);
  const int           numCols     = mxGetN(matShifts);
  const int           numShifts   = mxGetNumberOfElements(matShifts);

  if (numCols > 1 || numRows > 1) {
    if (numRows - firstFrame < numFrames)
      mexErrMsgIdAndTxt( "imreadx:shifts", "Number of %s rows (%d) is less than the number of frames (%d) in this image stack.", name, numShifts, numFrames);
    else {
      ptrShifts      += firstFrame + (numCols - 1) * numRows;
      if (numCols > 1)
        mexWarnMsgIdAndTxt( "imreadx:shifts", "Multiple columns (%d) of %s provided. Will use last column for corrections.", numCols, name);
    }
  }

  else if (numRows - firstFrame < numFrames)
    mexErrMsgIdAndTxt( "imreadx:shifts", "Number of %s (%d) is less than the number of frames (%d) in this image stack.", name, numShifts, numFrames);

  else
    ptrShifts        += firstFrame;
}


///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs < 1 || nrhs > 11 || nlhs > 3) {
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
    mexErrMsgIdAndTxt("imreadx:arguments", "inputPath must be a string or cell array of strings.");
  else
    inputPath.push_back( mxArrayToString(input) );

  
  // Parse input
  const bool                  computeStats            = ( nlhs > 1 );
  const bool                  computeMedian           = ( nlhs > 2 );
  ImageProcessor<float>       processor;
                              processor.xShift        = ( nrhs >  1 && !mxIsEmpty(prhs[1]) ) ?     mxGetPr(prhs[1])           : 0     ;
                              processor.yShift        = ( nrhs >  2 && !mxIsEmpty(prhs[2]) ) ?     mxGetPr(prhs[2])           : 0     ;
                              processor.xScale        = ( nrhs >  3 && !mxIsEmpty(prhs[3]) ) ?     mxGetScalar(prhs[3])       : -999  ;
                              processor.yScale        = ( nrhs >  4 && !mxIsEmpty(prhs[4]) ) ?     mxGetScalar(prhs[4])       : -999  ;
  const bool                  storeStack              = ( nrhs <= 5 || !mxIsNaN(mxGetScalar(prhs[5])) );
                              processor.emptyProb     = ( nrhs >  6 ?                              mxGetScalar(prhs[6])       : -999  );
                              processor.subtractZero  = ( nrhs >  7 ?                             (mxGetScalar(prhs[7]) > 0)  : false );
  const mxArray*              nanMask                 = ( nrhs >  8 ?                                          prhs[8]        : 0     );
                              processor.methodInterp  = ( nrhs >  9 ? int( mxGetScalar(prhs[ 9]) ) : cv::InterpolationFlags::INTER_LINEAR );
                              processor.methodResize  = ( nrhs > 10 ? int( mxGetScalar(prhs[10]) ) : cv::InterpolationFlags::INTER_AREA   );

  int                         firstFrame              = 0;
  if (nrhs > 5) {
    switch (mxGetNumberOfElements(prhs[5])) {
    case 0:
      break;
    case 1:
      if (mxIsFinite(mxGetScalar(prhs[5])))
        processor.maxNumFrames= cv::saturate_cast<int>(mxGetScalar(prhs[5]));
      break;
    case 2:
      {
        const double*         frameRange              = mxGetPr(prhs[5]);
        firstFrame            = cv::saturate_cast<int>(frameRange[0]) - 1;
        processor.maxNumFrames= cv::saturate_cast<int>(frameRange[1]) - firstFrame;
      }
      break;
    default:
      mexErrMsgIdAndTxt( "imreadx:arguments", "maxNumFrames must be a scalar or a range [min,max].");
    }
  }



  // Sanity checks
  if ((processor.xShift == 0) != (processor.yShift == 0))
    mexErrMsgIdAndTxt( "imreadx:load", "If xShift is provided, yShift must be provided as well, and vice versa.");
  if ((processor.xScale <= 0) != (processor.yScale <= 0))
    mexErrMsgIdAndTxt( "imreadx:load", "If xScale is provided, yScale must be provided as well, and vice versa.");
  if (processor.xScale <= 0 || (processor.xScale == 1 && processor.yScale == 1))
    processor.methodResize    = -1;
  if (computeMedian && !storeStack)
    mexErrMsgIdAndTxt( "imreadx:arguments", "Median cannot be computed unless image stack is loaded into memory (maxNumFrames > 0).");

  if (nanMask) {
    if (!mxIsLogical(nanMask))
      mexErrMsgIdAndTxt( "imreadx:load", "nanMask must be a logical matrix.");
    processor.nanMask         = (const bool*) mxGetData(nanMask);
  }


  //---------------------------------------------------------------------------
  // Get parameters of image stack
  int                         srcWidth        = 0;
  int                         srcHeight       = 0;
  int                         srcBits         = 0;
  int                         numFrames       = 0;
  for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) {
    numFrames                += cv::imfinfo(inputPath[iIn], srcWidth, srcHeight, srcBits, iIn > 0);
    if (numFrames >= processor.maxNumFrames)  break;
  }
  if (numFrames < processor.maxNumFrames)
    processor.maxNumFrames    = numFrames;                // in case there are not enough available
  else if (numFrames > processor.maxNumFrames)
    numFrames                 = processor.maxNumFrames;   // user request to stop at a certain number

  // Ensure proper size of masks
  if (nanMask && (mxGetM(nanMask) != srcHeight || mxGetN(nanMask) != srcWidth))
    mexErrMsgIdAndTxt( "imreadx:load", "Incorrect size of nanMask, must be equal to original image size (width = %d, height = %d).", srcWidth, srcHeight);


  // Adjust for scaling if provided
  int                         imgWidth        = srcWidth ;
  int                         imgHeight       = srcHeight;
  if (processor.methodResize >= 0) {
    imgWidth                  = cvRound(imgWidth  * processor.xScale);
    imgHeight                 = cvRound(imgHeight * processor.yScale);
    processor.condenser       = new CondenserInfo2D(srcWidth, srcHeight, imgWidth, imgHeight);
  }
  processor.nFramePixels      = imgHeight * imgWidth;

  //---------------------------------------------------------------------------
  // Create output structure
  if (storeStack) {
    size_t                    dimension[]     = {imgHeight, imgWidth, numFrames};
    plhs[0]                   = mxCreateNumericArray(3, dimension, mxSINGLE_CLASS, mxREAL);
    processor.frameOffset     = processor.nFramePixels;
  } else {
    plhs[0]                   = mxCreateNumericMatrix(imgHeight, imgWidth, mxSINGLE_CLASS, mxREAL);
    processor.frameOffset     = 0;
  }
  processor.imgClass          = mxGetClassID(plhs[0]);
  processor.imgData           = (float*) mxGetData(plhs[0]);


  mxArray*                    imgMin          = 0;
  mxArray*                    imgMax          = 0;
  mxArray*                    imgMean         = 0;
  mxArray*                    imgStd          = 0;
  if (computeStats) {
    imgMin                    = mxCreateDoubleMatrix(imgHeight, imgWidth, mxREAL);
    imgMax                    = mxCreateDoubleMatrix(imgHeight, imgWidth, mxREAL);
    imgMean                   = mxCreateDoubleMatrix(imgHeight, imgWidth, mxREAL);
    imgStd                    = mxCreateDoubleMatrix(imgHeight, imgWidth, mxREAL);
    processor.stackStats      = new ImageStatistics( processor.nFramePixels, mxGetPr(imgMean), mxGetPr(imgMin), mxGetPr(imgMax) );
  }


  // Check that we have enough frame shifts
  if (processor.xShift) {
    checkNumShifts(prhs[1], processor.xShift, firstFrame, numFrames, "xShift");
    checkNumShifts(prhs[2], processor.yShift, firstFrame, numFrames, "yShift");
  }

  //---------------------------------------------------------------------------
  // Call the stack processor
  if (inputPath.size() == 1) {
    cv::imreadmulti(inputPath[0], &processor, cv::ImreadModes::IMREAD_UNCHANGED, firstFrame);
  }
  else {
    mexPrintf("       ");
    for (size_t iIn = 0; iIn < inputPath.size(); ++iIn) {
      mexPrintf("\b\b\b\b\b\b\b%3d/%-3d", iIn+1, inputPath.size());
      mexEvalString("drawnow");
      if (!cv::imreadmulti(inputPath[iIn], &processor, cv::ImreadModes::IMREAD_UNCHANGED, firstFrame))
        break;
      firstFrame              = 0;
    }
    mexPrintf("\b\b\b\b\b\b\b%s", "");
    mexEvalString("drawnow");
  }


  // Return statistics structure if so requested
  if (computeStats) {
    processor.stackStats->getRMS(mxGetPr(imgStd));

    static const char*        STAT_FIELDS[]   = { "zeroLevel"
                                                , "zeroNoise"
                                                , "zeroThreshold"
                                                , "min"
                                                , "max"
                                                , "mean"
                                                , "std"
                                                };
    plhs[1]                   = mxCreateStructMatrix(1, 1, 7, STAT_FIELDS);
    mxSetField(plhs[1], 0, "zeroLevel"    , mxCreateDoubleScalar(processor.statistics.getMean()));
    mxSetField(plhs[1], 0, "zeroNoise"    , mxCreateDoubleScalar(processor.statistics.getRMS()));
    mxSetField(plhs[1], 0, "zeroThreshold", mxCreateDoubleScalar(processor.maxZeroValue));
    mxSetField(plhs[1], 0, "min"          , imgMin );
    mxSetField(plhs[1], 0, "max"          , imgMax );
    mxSetField(plhs[1], 0, "mean"         , imgMean);
    mxSetField(plhs[1], 0, "std"          , imgStd );
  }

  // Compute median if so requested
  if (computeMedian) {
    std::vector<float>        traceTemp(numFrames);
    plhs[2]                   = mxCreateNumericMatrix(imgHeight, imgWidth, mxSINGLE_CLASS, mxREAL);
    float*                    medData         = (float*) mxGetData(plhs[2]);

    // Loop over each pixel in the image stack
    const float*              pixCol          = (float*) mxGetData(plhs[0]);
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

  if (processor.condenser)
    delete processor.condenser;
  if (processor.stackStats)
    delete processor.stackStats;
}
