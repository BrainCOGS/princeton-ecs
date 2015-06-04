/**
  Tools to read/write images (single frame or stack), including populating header 
  fields with those copied from another TIFF file.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef TIFFSTACK_H
#define TIFFSTACK_H

#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include "grfmt_tiff.hpp"

/**
  Mostly copied from loadsave.cpp imreadmulti_().
*/
template<typename Functor>
bool  processStack(const char* filename, int flags, Functor& processor)
{
  /// Search for the relevant decoder to handle the imagery
  cv::TiffDecoder               decoder;

  /// set the filename in the driver
  decoder.setSource(filename);

  // read the header to make sure it succeeds
  if (!decoder.readHeader())    return false;


  // grab the decoded type
  int                           type    = decoder.type();
  if (flags != cv::IMREAD_UNCHANGED)
  {
    if ((flags & CV_LOAD_IMAGE_ANYDEPTH) == 0)
      type = CV_MAKETYPE(CV_8U, CV_MAT_CN(type));

    if ((flags & CV_LOAD_IMAGE_COLOR) != 0 ||
      ((flags & CV_LOAD_IMAGE_ANYCOLOR) != 0 && CV_MAT_CN(type) > 1))
      type = CV_MAKETYPE(CV_MAT_DEPTH(type), 3);
    else
      type = CV_MAKETYPE(CV_MAT_DEPTH(type), 1);
  }


  cv::Mat                       mat(decoder.height(), decoder.width(), type);
    const int                   origFlags = mat.flags;
  while (true)
  {
    if (decoder.height() != mat.rows || decoder.width() != mat.cols)
      cvError(CV_BadImageSize, "processStack", "Image stack must consist of frames of the same size.", "tiffStack.h", 52);
    
    // Restore flags to reuse preallocated space
    mat.flags                   = origFlags;

    // read the image data
    if (!decoder.readData(mat)) break;

    // SAK: HACK for reinterpreting signed data, by forcing the 
    //      type to be signed instead
    if (decoder.format() == 2) //SAMPLEFORMAT_INT
    {
      if (type == CV_8U)
        mat.flags   = (mat.flags & ~type) | CV_MAT_DEPTH(CV_8S);
      else if (type == CV_16U)
        mat.flags   = (mat.flags & ~type) | CV_MAT_DEPTH(CV_16S);
    }

    processor(mat);
    if (!decoder.nextPage())    break;
  }

  return true;
}


//bool  stackWrite( const std::string&      outputFile
//                , cv::InputArray          img
//                , const int               flags   = 0
//                );


#endif //TIFFSTACK_H
