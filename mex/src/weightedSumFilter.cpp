/**
  Computes a median filtered image using a mask to select pixels to include. NaN-valued pixels are ignored.

  Usage syntax:
    filtered  = ecs.weightedSumFilter(image, weight);

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include "lib/imageFilter.h"
#include "lib/quickSelect.h"



template<typename Pixel>
class WeightedSumFilter2D : public ImageFilter2D<Pixel> {
protected:
  const double*       weight;
  double              sumPixels;
  double              sumWeight;

public:
  WeightedSumFilter2D(const int imageWidth, const int imageHeight, const mxArray* matWeight)
    : ImageFilter2D (imageWidth, imageHeight, static_cast<int>(mxGetN(matWeight)), static_cast<int>(mxGetM(matWeight)))
    , weight        (mxGetPr(matWeight))
  {
  }

  virtual void clear()
  {
    sumPixels = 0;
    sumWeight = 0;
  }
  
  virtual void add(const Pixel& pixelValue, const int weightPixel)
  {
    if (weight[weightPixel] == weight[weightPixel] && pixelValue == pixelValue) {
      sumPixels  += weight[weightPixel] * pixelValue;
      sumWeight  += weight[weightPixel];
    }
  }
  
  virtual Pixel compute()
  {
    return static_cast<Pixel>( sumPixels / sumWeight );
  }
};



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs != 2 || nlhs != 1) {
    mexEvalString("help ecs.weightedSumFilter");
    mexErrMsgIdAndTxt ( "weightedSumFilter:usage", "Incorrect number of inputs/outputs provided." );
  }


  // Parse input
  const mxArray*              image         = prhs[0];
  const mxArray*              weight        = prhs[1];

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


  //---------------------------------------------------------------------------

  // Create output structure
  mxArray*                    filtered      = mxCreateNumericArray( mxGetNumberOfDimensions(image)
                                                                  , mxGetDimensions(image)
                                                                  , mxGetClassID(image)
                                                                  , mxREAL
                                                                  );
  plhs[0]                     = filtered;

  // Create filter class and process
  applyFilter<WeightedSumFilter2D>(image, mxGetData(filtered), weight);
}

