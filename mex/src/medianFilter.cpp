/**
  Computes a median filtered image using a mask to select pixels to include. NaN-valued pixels are ignored.

  Usage syntax:
    filtered  = ecs.medianFilter(image, mask);

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include "lib/imageFilter.h"
#include "lib/quickSelect.h"



template<typename Pixel>
class MedianFilter2D : public ImageFilter2D<Pixel> {
protected:
  const bool*           mask;
  std::vector<Pixel>    pixelValues;

public:
  MedianFilter2D(const int imageWidth, const int imageHeight, const mxArray* matMask)
    : ImageFilter2D (imageWidth, imageHeight, static_cast<int>(mxGetN(matMask)), static_cast<int>(mxGetM(matMask)))
    , mask          ((const bool*) mxGetData(matMask))
  {
    pixelValues.reserve(maskHeight * maskWidth);
  }

  virtual void clear()
  {
    pixelValues.clear();
  }
  
  virtual void add(const Pixel& pixelValue, const int maskPixel)
  {
    if (mask[maskPixel] && pixelValue == pixelValue)
      pixelValues.push_back(pixelValue);
  }
  
  virtual Pixel compute()
  {
    return static_cast<Pixel>( quickSelect(pixelValues) );
  }
};



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs != 2 || nlhs != 1) {
    mexEvalString("help ecs.medianFilter");
    mexErrMsgIdAndTxt ( "medianFilter:usage", "Incorrect number of inputs/outputs provided." );
  }


  // Parse input
  const mxArray*              image         = prhs[0];
  const mxArray*              mask          = prhs[1];

  if (mxGetNumberOfDimensions(image) != 2)
    mexErrMsgIdAndTxt("medianFilter:image", "Only 2D images are supported for now.");
  if (mxIsComplex(image))
    mexErrMsgIdAndTxt("medianFilter:image", "Only real valued images are supported for now.");

  if (!mxIsLogical(mask))
    mexErrMsgIdAndTxt("medianFilter:mask", "mask must be boolean.");
  if (mxGetM(mask) % 2 == 0)
    mexErrMsgIdAndTxt("medianFilter:mask", "mask must have an odd number of rows.");
  if (mxGetN(mask) % 2 == 0)
    mexErrMsgIdAndTxt("medianFilter:mask", "mask must have an odd number of columns.");


  //---------------------------------------------------------------------------

  // Create output structure
  mxArray*                    filtered      = mxCreateNumericArray( mxGetNumberOfDimensions(image)
                                                                  , mxGetDimensions(image)
                                                                  , mxGetClassID(image)
                                                                  , mxREAL
                                                                  );
  plhs[0]                     = filtered;

  // Create filter class and process
  applyFilter<MedianFilter2D>(image, mxGetData(filtered), mask);
}

