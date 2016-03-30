/**
  Computes filtered image using a selection mask to select pixels to include. NaN-valued pixels are ignored.

  Usage syntax:
    filtered  = ecs.adaptiveSumFilter(image, weight, isSelected);

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include "lib/imageFilter.h"
#include "lib/quickSelect.h"



template<typename Pixel>
class AdaptiveSumFilter2D : public ImageFilter2D<Pixel> {
protected:
  const double*       weight;
  double              sumPixels;
  double              sumWeight;
  int                 maskOffset;

public:
  AdaptiveSumFilter2D(const int imageWidth, const int imageHeight, const mxArray* matWeight)
    : ImageFilter2D<Pixel>(imageWidth, imageHeight, static_cast<int>(mxGetN(matWeight)), static_cast<int>(mxGetM(matWeight)))
    , weight        (mxGetPr(matWeight))
  {
    maskOffset    = this->maskWidth * this->maskHeight;
  }

  virtual void clear()
  {
    sumPixels     = 0;
    sumWeight     = 0;
  }
  
  virtual void add(const Pixel& pixelValue, const int weightPixel, const int /*sourcePixel*/, const int targetPixel)
  {
    const double      tgtWeight   = weight[weightPixel + targetPixel*maskOffset];
    if (tgtWeight == tgtWeight && pixelValue == pixelValue) {
      sumPixels  += tgtWeight * pixelValue;
      sumWeight  += tgtWeight;
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
  if (nlhs != 1 || nrhs < 2 || nrhs > 3) {
    mexEvalString("help ecs.adaptiveSumFilter");
    mexErrMsgIdAndTxt ( "adaptiveSumFilter:usage", "Incorrect number of inputs/outputs provided." );
  }


  // Parse input
  const mxArray*              image         =            prhs[0];
  const mxArray*              weight        =            prhs[1];
  const mxArray*              selection     = nrhs > 2 ? prhs[2] : 0;

  if (mxGetNumberOfDimensions(image) != 2)
    mexErrMsgIdAndTxt("adaptiveSumFilter:image", "Only 2D images are supported for now.");
  if (mxIsComplex(image))
    mexErrMsgIdAndTxt("adaptiveSumFilter:image", "Only real valued images are supported for now.");

  if (!mxIsDouble(weight))
    mexErrMsgIdAndTxt("adaptiveSumFilter:weight", "weight must be a double matrix.");
  if (mxGetM(weight) % 2 == 0)
    mexErrMsgIdAndTxt("adaptiveSumFilter:weight", "weight must have an odd number of rows.");
  if (mxGetN(weight) % 2 == 0)
    mexErrMsgIdAndTxt("adaptiveSumFilter:weight", "weight must have an odd number of columns.");
  if (mxGetNumberOfDimensions(weight) != 3)
    mexErrMsgIdAndTxt("adaptiveSumFilter:weight", "weight must be a 3D array.");
  if (mxGetDimensions(weight)[2] != mxGetNumberOfElements(image))
    mexErrMsgIdAndTxt("adaptiveSumFilter:weight", "weight must have depth equal to the number of image pixels.");

  const bool*                 isSelected    = 0;
  if (selection) {
    if (!mxIsLogical(selection))
      mexErrMsgIdAndTxt("adaptiveSumFilter:selection", "selection must be a logical matrix.");
    if (mxGetNumberOfElements(selection) != mxGetNumberOfElements(image))
      mexErrMsgIdAndTxt("adaptiveSumFilter:selection", "selection must have the same number of elements as image.");
    isSelected                = (const bool*) mxGetData(selection);
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
  applyFilter<AdaptiveSumFilter2D>(image, mxGetData(filtered), isSelected, weight);
}

