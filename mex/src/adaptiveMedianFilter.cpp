/**
  Computes a median filtered image using a mask to select pixels to include. NaN-valued pixels are ignored.

  Usage syntax:
    filtered  = ecs.adaptiveMedianFilter(image, category, numCategories, targetFracPixels, [isSelected = []], [emptyValue = nan]);

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <mex.h>
#include "lib/imageFilter.h"
#include "lib/quickSelect.h"



template<typename Pixel>
class AdaptiveMedianFilter2D : public ImageFilter2D<Pixel> {
protected:
  const int             numCategories;
  const double          targetFracPixels;
  const int*            category;
  std::vector<Pixel>*   categoryValues;
  std::vector<size_t>   numPixels;
  std::vector<Pixel>    pixelValues;
  const Pixel           emptyValue;

public:
  AdaptiveMedianFilter2D( const int imageWidth, const int imageHeight
                        , const mxArray* matCategory, const int numCategories
                        , const double targetFracPixels
                        , const Pixel emptyValue
                        )
    : ImageFilter2D   (imageWidth, imageHeight, static_cast<int>(mxGetN(matCategory)), static_cast<int>(mxGetM(matCategory)))
    , numCategories   (numCategories)
    , targetFracPixels(targetFracPixels)
    , category        ((const int*) mxGetData(matCategory))
    , categoryValues  (new std::vector<Pixel>[numCategories])
    , numPixels       (numCategories)
    , emptyValue      (emptyValue)
  {
    for (size_t iCat = 0; iCat < numCategories; ++iCat)
      categoryValues[iCat].reserve(maskHeight * maskWidth);
  }

  ~AdaptiveMedianFilter2D()
  {
    delete[] categoryValues;
  }

  virtual void clear()
  {
    for (size_t iCat = 0; iCat < numCategories; ++iCat) {
      categoryValues[iCat].clear();
      numPixels[iCat] = 0;
    }
  }
  
  virtual void add(const Pixel& pixelValue, const int maskPixel, const int /*sourcePixel*/, const int /*targetPixel*/)
  {
    if (category[maskPixel] < numCategories) {
      ++numPixels[category[maskPixel]];
      if (pixelValue == pixelValue)
        categoryValues[category[maskPixel]].push_back(pixelValue);
    }
  }
  
  virtual Pixel compute()
  {
    pixelValues.clear();
    for (size_t iCat = 0; iCat < numCategories; ++iCat) {
      for (size_t iPix = 0; iPix < categoryValues[iCat].size(); ++iPix)
        pixelValues.push_back(categoryValues[iCat][iPix]);
    
      if (pixelValues.size() > targetFracPixels * numPixels[iCat])
        return static_cast<Pixel>( quickSelect(pixelValues) );
    }

    return emptyValue;
  }
};



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nlhs != 1 || nrhs < 4 || nrhs > 6) {
      mexEvalString("help ecs.adaptiveMedianFilter");
    mexErrMsgIdAndTxt ( "adaptiveMedianFilter:usage", "Incorrect number of inputs/outputs provided." );
  }


  // Parse input
  const mxArray*              image             = prhs[0];
  const mxArray*              mask              = prhs[1];
  const int                   numCategories     = static_cast<int>( mxGetScalar(prhs[2]) );
  const double                targetFracPixels  = mxGetScalar(prhs[3]);
  const mxArray*              selection         = nrhs > 4 ? prhs[4] : 0;
  const double                emptyValue        = nrhs > 5 ? mxGetScalar(prhs[5]) : mxGetNaN();


  if (mxGetNumberOfDimensions(image) != 2)
    mexErrMsgIdAndTxt("adaptiveMedianFilter:image", "Only 2D images are supported for now.");
  if (mxIsComplex(image))
    mexErrMsgIdAndTxt("adaptiveMedianFilter:image", "Only real valued images are supported for now.");

  if (mxGetClassID(mask) != mxINT32_CLASS)
    mexErrMsgIdAndTxt("adaptiveMedianFilter:mask", "mask must be int32.");
  if (mxGetM(mask) % 2 == 0)
    mexErrMsgIdAndTxt("adaptiveMedianFilter:mask", "mask must have an odd number of rows.");
  if (mxGetN(mask) % 2 == 0)
    mexErrMsgIdAndTxt("adaptiveMedianFilter:mask", "mask must have an odd number of columns.");

  const bool*                 isSelected    = 0;
  if (selection) {
    if (!mxIsLogical(selection))
      mexErrMsgIdAndTxt("weightedSumFilter:selection", "selection must be a logical matrix.");
    if (mxGetNumberOfElements(selection) != mxGetNumberOfElements(image))
      mexErrMsgIdAndTxt("weightedSumFilter:selection", "selection must have the same number of elements as image.");
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
  applyFilter<AdaptiveMedianFilter2D>(image, mxGetData(filtered), isSelected, mask, numCategories, targetFracPixels, emptyValue);
}

