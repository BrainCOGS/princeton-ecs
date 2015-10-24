/**
  These functions are speed optimized at the cost of generality. If you 
  don't know what they're doing, you should probably not be using them.

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#ifndef IMAGECONDENSER_H
#define IMAGECONDENSER_H

#include <vector>
#include <algorithm>
#include <mex.h>
#include <opencv2/core.hpp>


/// Class for mapping a larger source image to a smaller target image.
class CondenserInfo2D {
protected:
  static void computeContribution(std::vector<double>* weight, std::vector<int>& start, std::vector<int>& bound, const int numTarget, const int numSource, const double tolerance = 1e-15)
  {
    const double      binWidth    = 1.0 * numSource / numTarget;
    for (int tgt = 0; tgt < numTarget; ++tgt)
    {
      double          binStart    = tgt * binWidth;
      double          binEdge     = binStart + binWidth;
      start[tgt]      = static_cast<int>(binStart + tolerance);       // floor
      bound[tgt]      = static_cast<int>(binEdge  - tolerance + 1);   // ceil
      if (bound[tgt] > numSource)     // numerical safety
        bound[tgt]    = numSource;

      weight[tgt].reserve(bound[tgt] - start[tgt]);
      for (int pix = start[tgt]; pix < bound[tgt]; ++pix) {
        const double  pixUp       = ( pix < bound[tgt]-1 )
                                  ? pix + 1
                                  : binEdge
                                  ;
        weight[tgt].push_back(pixUp - binStart);
        binStart      = pixUp;
      }
    }
  }

public:
  const int                       sourceWidth ;
  const int                       sourceHeight;
  const int                       targetWidth ;
  const int                       targetHeight;

  std::vector<double>*            rowWeight;
  std::vector<double>*            colWeight;
  std::vector<int>                rowStart ;
  std::vector<int>                colStart ;
  std::vector<int>                rowBound ;
  std::vector<int>                colBound ;


public:
  CondenserInfo2D(const int sourceWidth, const int sourceHeight, const int targetWidth, const int targetHeight)
    : sourceWidth ( sourceWidth                         )
    , sourceHeight( sourceHeight                        )
    , targetWidth ( targetWidth                         )
    , targetHeight( targetHeight                        )
    , rowWeight   (new std::vector<double>[targetHeight])
    , colWeight   (new std::vector<double>[targetWidth ])
    , rowStart    (targetHeight                         )
    , colStart    (targetWidth                          )
    , rowBound    (targetHeight                         )
    , colBound    (targetWidth                          )
  {
    // Precache common quantities for speed
    computeContribution(rowWeight, rowStart, rowBound, targetHeight, sourceHeight);
    computeContribution(colWeight, colStart, colBound, targetWidth , sourceWidth );
  }

  ~CondenserInfo2D()
  {
    delete[] rowWeight;
    delete[] colWeight;
  }
};


template<typename SrcPixel, typename TgtPixel>
class ImageCondenser2D 
{
protected:
  static void accumulateMean(double& mean, double& weight, const double x, const double w)
  {
    if (w <= 0 || x != x)
      return;

    weight             += w;
    mean               += (x - mean) * w / weight;
  }

public:
  void operator()(const cv::Mat& cvSource, TgtPixel* target, const CondenserInfo2D* info, const TgtPixel offset = 0, const bool* masked = 0, const TgtPixel emptyValue = 0)
  {
    CV_Assert( cvSource.isContinuous() );

    // Loop through target pixels
    for (int tgtCol = 0, tgtPix = 0; tgtCol < info->targetWidth; ++tgtCol) {
      const SrcPixel*             source      = cvSource.ptr<SrcPixel>(0, info->colStart[tgtCol]);
      for (int tgtRow = 0; tgtRow < info->targetHeight; ++tgtRow, ++tgtPix) {
        double                    mean        = 0;
        double                    sumWeight   = 0;

        const double*             wRow        = info->rowWeight[tgtRow].data();
        for (int srcRow = info->rowStart[tgtRow]; srcRow < info->rowBound[tgtRow]; ++srcRow) {
          const SrcPixel*         pix         = source + info->sourceWidth * srcRow;
          const double*           wCol        = info->colWeight[tgtCol].data();
          for (int srcCol = info->colStart[tgtCol]; srcCol < info->colBound[tgtCol]; ++srcCol, ++pix) {
            if (!masked || !masked[srcRow + info->sourceHeight*srcCol])
              accumulateMean(mean, sumWeight, *pix, (*wRow) * (*wCol));
            ++wCol;
          }
          ++wRow;
        }

        if (sumWeight)
          target[tgtPix]          = cv::saturate_cast<TgtPixel>( mean + offset );
        else    target[tgtPix]    = emptyValue;
      } // end loop over rows
    } // end loop over columns
  }
  
  
  void operator()(const SrcPixel* source, TgtPixel* target, const CondenserInfo2D* info, const TgtPixel offset = 0, const bool* masked = 0, const TgtPixel emptyValue = 0)
  {
    // Loop through target pixels
    for (int tgtCol = 0, tgtPix = 0; tgtCol < info->targetWidth; ++tgtCol) {
      const SrcPixel*             sourceCol   = source + info->sourceHeight * info->colStart[tgtCol];
      for (int tgtRow = 0; tgtRow < info->targetHeight; ++tgtRow, ++tgtPix) {
        double                    mean        = 0;
        double                    sumWeight   = 0;

        const double*             wCol        = info->colWeight[tgtCol].data();
        for (int srcCol = info->colStart[tgtCol]; srcCol < info->colBound[tgtCol]; ++srcCol) {
          const SrcPixel*         pix         = sourceCol + info->rowStart[tgtRow];
          const double*           wRow        = info->rowWeight[tgtRow].data();
          for (int srcRow = info->rowStart[tgtRow]; srcRow < info->rowBound[tgtRow]; ++srcRow, ++pix) {
            if (!masked || !masked[srcRow + info->sourceHeight*srcCol])
              accumulateMean(mean, sumWeight, *pix, (*wRow) * (*wCol));
            ++wRow;
          }
          ++wCol;
        }

        if (sumWeight)
          target[tgtPix]          = cv::saturate_cast<TgtPixel>( mean + offset );
        else    target[tgtPix]    = emptyValue;
      } // end loop over rows
    } // end loop over columns
  }
};


#endif //IMAGECONDENSER_H
