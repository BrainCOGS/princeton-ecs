/**
  Tools to write TIFF images (single frame or stack), including populating header 
  fields with those copied from another TIFF file.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef TIFFSTACK_H
#define TIFFSTACK_H

#include <string>
#include <opencv2\core.hpp>
#include <libtiff\tiffio.h>


bool  tiffwrite ( const std::string&    outputFile
                , cv::InputArray img
                , const int             flags   = 0
                );


#endif //TIFFSTACK_H
