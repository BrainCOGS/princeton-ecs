/**
  Computes a median filtered image using a selection mask to select pixels to include. 

  Usage syntax:
    filtered  = ecs.absMinFilter(image, weight, [refValue = 0], [masked = []], [isSelected = []], [minWeight = 0], [emptyValue = nan]);

  NaN-valued pixels are ignored.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include "lib/imageFilter.h"
#include "lib/quickSelect.h"



template<typename Pixel>
class AbsMinFilter2D : public ImageFilter2D<Pixel> {
protected:
  const double*       weight;
  const bool*         masked;
  const double        refValue;
  Pixel               minValue;
  int                 numCompared;
  const Pixel         emptyValue;

public:
  AbsMinFilter2D( const int imageWidth, const int imageHeight
                , const mxArray* matWeight, const double refValue, const bool* masked
                , const double minWeight, const Pixel emptyValue
                )
    : ImageFilter2D (imageWidth, imageHeight, static_cast<int>(mxGetN(matWeight)), static_cast<int>(mxGetM(matWeight)))
    , weight        (mxGetPr(matWeight))
    , refValue      (refValue)
    , masked        (masked)
    , emptyValue    (emptyValue)
  {
  }

  virtual void clear()
  {
    minValue      = emptyValue;
    numCompared   = 0;
  }
  
  virtual void add(const Pixel& pixelValue, const int weightPixel, const int sourcePixel, const int /*targetPixel*/)
  {
    if  (   weight[weightPixel] == weight[weightPixel]
        &&  pixelValue          == pixelValue
        &&  (!masked || !masked[sourcePixel])
        ) {
      if  ( ( numCompared < 1 )
         || ( std::abs(pixelValue - refValue) < std::abs(minValue - refValue) )
          )
        minValue  = pixelValue;
      ++numCompared;
    }
  }
  
  virtual Pixel compute()
  {
    return minValue;
  }
};



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nlhs != 1 || nrhs < 2 || nrhs > 7) {
    mexEvalString("help ecs.absMinFilter");
    mexErrMsgIdAndTxt ( "absMinFilter:usage", "Incorrect number of inputs/outputs provided." );
  }


  // Parse input
  const mxArray*              image         =                                   prhs[0];
  const mxArray*              weight        =                                   prhs[1];
  const double                refValue      = nrhs > 2            ? mxGetScalar(prhs[2]) : 0;
  const mxArray*              masked        = nrhs > 3 && !mxIsEmpty(prhs[3]) ? prhs[3]  : 0;
  const mxArray*              selection     = nrhs > 4 && !mxIsEmpty(prhs[4]) ? prhs[4]  : 0;
  const double                minWeight     = nrhs > 5            ? mxGetScalar(prhs[5]) : 0;
  const double                emptyValue    = nrhs > 6            ? mxGetScalar(prhs[6]) : mxGetNaN();

  if (mxGetNumberOfDimensions(image) != 2)
    mexErrMsgIdAndTxt("absMinFilter:image", "Only 2D images are supported for now.");
  if (mxIsComplex(image))
    mexErrMsgIdAndTxt("absMinFilter:image", "Only real valued images are supported for now.");

  if (!mxIsDouble(weight))
    mexErrMsgIdAndTxt("absMinFilter:weight", "weight must be a double matrix.");
  if (mxGetM(weight) % 2 == 0)
    mexErrMsgIdAndTxt("absMinFilter:weight", "weight must have an odd number of rows.");
  if (mxGetN(weight) % 2 == 0)
    mexErrMsgIdAndTxt("absMinFilter:weight", "weight must have an odd number of columns.");

  const bool*                 isSelected    = 0;
  if (selection) {
    if (!mxIsLogical(selection))
      mexErrMsgIdAndTxt("absMinFilter:selection", "selection must be a logical matrix.");
    if (mxGetNumberOfElements(selection) != mxGetNumberOfElements(image))
      mexErrMsgIdAndTxt("absMinFilter:selection", "selection must have the same number of elements as image.");
    isSelected                = (const bool*) mxGetData(selection);
  }

  const bool*                 isMasked      = 0;
  if (masked) {
    if (!mxIsLogical(masked))
      mexErrMsgIdAndTxt("absMinFilter:masked", "masked must be a logical matrix.");
    if (mxGetNumberOfElements(masked) != mxGetNumberOfElements(image))
      mexErrMsgIdAndTxt("absMinFilter:masked", "masked must have the same number of elements as image.");
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
  applyFilter<AbsMinFilter2D>(image, mxGetData(filtered), isSelected, weight, refValue, isMasked, minWeight, emptyValue);
}

