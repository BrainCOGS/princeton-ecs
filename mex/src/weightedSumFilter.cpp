/**
  Computes a median filtered image using a selection mask to select pixels to include. 

  Usage syntax:
    filtered  = ecs.weightedSumFilter(image, weight, [masked = []], [isSelected = []], [minWeight = 0], [emptyValue = nan]);

  NaN-valued pixels are ignored.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include "lib/imageFilter.h"
#include "lib/quickSelect.h"



template<typename Pixel>
class WeightedSumFilter2D : public ImageFilter2D<Pixel> {
protected:
  const double*       weight;
  const bool*         masked;
  double              sumPixels;
  double              sumWeight;
  const double        minWeight;
  const Pixel         emptyValue;

public:
  WeightedSumFilter2D ( const int imageWidth, const int imageHeight
                      , const mxArray* matWeight, const bool* masked
                      , const double minWeight, const Pixel emptyValue
                      )
    : ImageFilter2D (imageWidth, imageHeight, static_cast<int>(mxGetN(matWeight)), static_cast<int>(mxGetM(matWeight)))
    , weight        (mxGetPr(matWeight))
    , masked        (masked)
    , minWeight     (minWeight)
    , emptyValue    (emptyValue)
  {
  }

  virtual void clear()
  {
    sumPixels = 0;
    sumWeight = 0;
  }
  
  virtual void add(const Pixel& pixelValue, const int weightPixel, const int sourcePixel, const int /*targetPixel*/)
  {
    if  (   weight[weightPixel] == weight[weightPixel]
        &&  pixelValue          == pixelValue
        &&  (!masked || !masked[sourcePixel])
        ) {
      sumPixels  += weight[weightPixel] * pixelValue;
      sumWeight  += weight[weightPixel];
    }
  }
  
  virtual Pixel compute()
  {
    return static_cast<Pixel>( sumWeight > minWeight ? sumPixels / sumWeight : emptyValue );
  }
};



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nlhs != 1 || nrhs < 2 || nrhs > 6) {
    mexEvalString("help ecs.weightedSumFilter");
    mexErrMsgIdAndTxt ( "weightedSumFilter:usage", "Incorrect number of inputs/outputs provided." );
  }


  // Parse input
  const mxArray*              image         =                                   prhs[0];
  const mxArray*              weight        =                                   prhs[1];
  const mxArray*              masked        = nrhs > 2 && !mxIsEmpty(prhs[2]) ? prhs[2]  : 0;
  const mxArray*              selection     = nrhs > 3 && !mxIsEmpty(prhs[3]) ? prhs[3]  : 0;
  const double                minWeight     = nrhs > 4            ? mxGetScalar(prhs[4]) : 0;
  const double                emptyValue    = nrhs > 5            ? mxGetScalar(prhs[5]) : mxGetNaN();

  if (mxGetNumberOfDimensions(image) != 2)
    mexErrMsgIdAndTxt("weightedSumFilter:image", "Only 2D images are supported for now.");
  if (mxIsComplex(image))
    mexErrMsgIdAndTxt("weightedSumFilter:image", "Only real valued images are supported for now.");

  if (!mxIsDouble(weight))
    mexErrMsgIdAndTxt("weightedSumFilter:weight", "weight must be a double matrix.");
  if (mxGetM(weight) % 2 == 0)
    mexErrMsgIdAndTxt("weightedSumFilter:weight", "weight must have an odd number of rows.");
  if (mxGetN(weight) % 2 == 0)
    mexErrMsgIdAndTxt("weightedSumFilter:weight", "weight must have an odd number of columns.");

  const bool*                 isSelected    = 0;
  if (selection) {
    if (!mxIsLogical(selection))
      mexErrMsgIdAndTxt("weightedSumFilter:selection", "selection must be a logical matrix.");
    if (mxGetNumberOfElements(selection) != mxGetNumberOfElements(image))
      mexErrMsgIdAndTxt("weightedSumFilter:selection", "selection must have the same number of elements as image.");
    isSelected                = (const bool*) mxGetData(selection);
  }

  const bool*                 isMasked      = 0;
  if (masked) {
    if (!mxIsLogical(masked))
      mexErrMsgIdAndTxt("weightedSumFilter:masked", "masked must be a logical matrix.");
    if (mxGetNumberOfElements(masked) != mxGetNumberOfElements(image))
      mexErrMsgIdAndTxt("weightedSumFilter:masked", "masked must have the same number of elements as image.");
    isMasked                  = (const bool*) mxGetData(masked);
  }


  //---------------------------------------------------------------------------

  // Create output structure
  mxArray*                    filtered      = mxCreateNumericArray( mxGetNumberOfDimensions(image)
                                                                  , mxGetDimensions(image)
                                                                  , mxGetClassID(image)
                                                                  , mxREAL
                                                                  );
  plhs[0]                     = filtered;

  // Create filter class and process
  applyFilter<WeightedSumFilter2D>(image, mxGetData(filtered), isSelected, weight, isMasked, minWeight, emptyValue);
}

