/**
  Fast methods for converting data structures from OpenCV to Matlab.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef CVTOMATLAB_H
#define CVTOMATLAB_H

#include <mex.h>
#include <opencv2/core.hpp>
#include "quickSelect.h"


template<typename Pixel, typename Data>
struct MatToMatlab
{
  void operator()(const cv::Mat& image, const int dataType, void* data, float offset = 0, const bool* masked = 0, Data emptyValue = 0)
  {
    Data*             dataPtr   = (Data*) data;

    // Loop over each pixel in the image 
    for (int iRow = 0; iRow < image.rows; ++iRow) {
      const Pixel*    pixRow    = image.ptr<Pixel>(iRow);
      for (int iCol = 0, iPix = 0; iCol < image.cols; ++iCol, iPix += image.rows) {
        dataPtr[iPix] = ( masked && masked[iPix] )
                        ? emptyValue
                        : static_cast<Data>( pixRow[iCol] ) - offset
                        ;
      } // end loop over columns
      
      // Write next row
      ++dataPtr;
      if (masked)     ++masked;
    } // end loop over rows
  }

  void operator()(const std::vector<cv::Mat>& imgStack, const int dataType, void* data, float offset = 0, const bool* masked = 0, Data emptyValue = 0)
  {
    Data*             dataPtr   = (Data*) data;

    // Loop over each frame
    for (int iFrame = 0; iFrame < imgStack.size(); ++iFrame) {
      operator()(imgStack[iFrame], dataType, dataPtr, offset, masked, emptyValue);
      dataPtr        += imgStack[iFrame].rows * imgStack[iFrame].cols;
    } // end loop over frames
  }
};



template<typename Pixel, typename Data>
struct MatlabToCVMatHelper
{
  void operator()(cv::Mat& image, const Data* dataPtr)
  {
    // Loop over each pixel in the image 
    for (int iRow = 0; iRow < image.rows; ++iRow) {
      Pixel*          pixRow      = image.ptr<Pixel>(iRow);
      for (int iCol = 0; iCol < image.cols; ++iCol) {
        pixRow[iCol]  = static_cast<Pixel>( dataPtr[iCol * image.rows] );
      } // end loop over columns
      
      // Read from next row
      ++dataPtr;
    } // end loop over rows
  }
};

template<typename Pixel, typename Data>
struct MatlabToCVMat
{
  void operator()(cv::Mat& image, const int dataType, const void* dataPtr, const size_t numRows, const size_t numCols, const size_t iFrame = 0)
  {
    image.create(static_cast<int>(numRows), static_cast<int>(numCols), dataType);

    const Data*         data      = (const Data*) dataPtr;
    cvTypeCall<MatlabToCVMatHelper, Data>(image, data + numRows*numCols*iFrame, iFrame);
  }

  void operator()(std::vector<cv::Mat>& imgStack, const int dataType, const mxArray* input)
  {
    // Compute number of frames from dimensions >= 3
    const size_t*             inputSize       = mxGetDimensions(input);
    size_t                    numFrames       = 1;
    for (size_t iDim = 2, maxDims = mxGetNumberOfDimensions(input); iDim < maxDims; ++iDim)
      numFrames              *= inputSize[iDim];
    imgStack.resize(numFrames);

    // Loop through and process each frame
    const Data*               inputData       = (const Data*) mxGetData(input);
    for (size_t iFrame = 0; iFrame < numFrames; ++iFrame) {
      imgStack[iFrame].create(static_cast<int>(inputSize[0]), static_cast<int>(inputSize[1]), dataType);
      cvTypeCall<MatlabToCVMatHelper, Data>(imgStack[iFrame], inputData);

      // Set write pointer to the end of the read data
      inputData              += inputSize[0] * inputSize[1];
    }
  }
};



#endif //CVTOMATLAB_H
