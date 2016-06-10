/**
  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef CONVERSIONUTILS_H
#define CONVERSIONUTILS_H



/**
  Arithmetic on different bit depth images.
*/
template<typename Pixel>
struct AddImage32
{
  void operator()(const cv::Mat& source, cv::Mat& target, const double scale = 1)
  {
    // Sanity check for output size and type
    CV_DbgAssert(  (target.rows       == source.rows)
                && (target.cols       == source.cols)
                && (target.type()     == CV_32F)
                );

    // Loop over each pixel in the image stack
    for (int row = 0; row < target.rows; ++row) {
      float*          tgtRow      = target.ptr<float>(row);
      const Pixel*    srcRow      = source.ptr<Pixel>(row);

      for (int col = 0; col < target.cols; ++col)
        tgtRow[col]  += static_cast<float>(scale * srcRow[col]);
    } // end loop over rows
  }
};



#endif //CONVERSIONUTILS_H
