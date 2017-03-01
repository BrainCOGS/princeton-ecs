/**
  Very fast warping with locally linear assumptions.

  Usage syntax:
    interpolated = cv.barycentricMeshWarp(source, xSample, ySample, xTarget, yTarget);

  *Sample should be locations in pixel units of the original measurements.
  *Target are the locations in pixel units to which the original image should be warped,
  specified as a function of time.
*/


#include <mex.h>
#include <vector>
#include <algorithm>
#include <cmath>


typedef double (*TruncatorFcn)(double);


//=============================================================================
template<typename Number>
void matrixMultiply(Number* out, const Number* A, const Number* B, const int nCols = 1)
{
  for (int iCol = 0; iCol < nCols; ++iCol, B += 2) {
    *out  = A[0]*B[0] + A[2]*B[1];                      ++out;
    *out  = A[1]*B[0] + A[3]*B[1];                      ++out;
  }
}
template<typename Number>
void matrixMultiply(Number* out, const Number* A, const Number* B, const int nCols, const Number offset, const Number scale)
{
  for (int iCol = 0; iCol < nCols; ++iCol, B += 2) {
    Number  B0  = offset + scale*B[0];
    Number  B1  = offset + scale*B[1];
    *out    = A[0]*B0 + A[2]*B1;                        ++out;
    *out    = A[1]*B0 + A[3]*B1;                        ++out;
  }
}

template<typename Number>
void vectorAddTo(Number* out, const Number* A, const int nCols = 1)
{
  for (int iCol = 0; iCol < nCols; ++iCol) {
    *out += A[0];                                       ++out;
    *out += A[1];                                       ++out;
  }
}
template<typename Number>
void vectorAddTo(Number* out, const Number* A, const int nCols, const Number offset, const Number scale)
{
  for (int iCol = 0; iCol < nCols; ++iCol) {
    *out += offset + scale*( A[0] );                    ++out;
    *out += offset + scale*( A[1] );                    ++out;
  }
}


//=============================================================================
template<typename Pixel>
void barycentricMeshWarp( const mxArray* source, mxArray* target, const mxArray* xSample, const mxArray* ySample
                        , const mxArray* xTarget, const mxArray* yTarget, const bool perFrameX, const bool perFrameY
                        )
{
  // Get pointers to data
  const Pixel*          src             = (const Pixel*)  mxGetData(source);
  const double*         xCenter         = (const double*) mxGetData(xSample);   // note that these are all 1-based indexing!
  const double*         yCenter         = (const double*) mxGetData(ySample);   // note that these are all 1-based indexing!
  const Pixel*          xLoc            = (const Pixel*)  mxGetData(xTarget);   // note that these are all 1-based indexing!
  const Pixel*          yLoc            = (const Pixel*)  mxGetData(yTarget);   // note that these are all 1-based indexing!
  Pixel*                tgt             = (Pixel*)        mxGetData(target);

  const mwSize*         nDims           = mxGetDimensions(source);
  const mwSize          nRows           = nDims[0];
  const mwSize          nCols           = nDims[1];
  const mwSize          nFrames         = mxGetNumberOfDimensions(source) > 2 ? nDims[2] : 1;
  const mwSize          nPixels         = nRows * nCols;
  const mwSize          nPatchX         = mxGetNumberOfElements(xSample);
  const mwSize          nPatchY         = mxGetNumberOfElements(ySample);
  const mwSize          nPatches        = nPatchX * nPatchY;
  const mwSize          lastPatchX      = nPatchX - 1;
  const mwSize          lastPatchY      = nPatchY - 1;


  /*
    We want to resample values measured on an irregular point cloud (the motion warped measurement)
    so that they lie on a regular grid (the target motion-corrected image). Since we define the
    coordinate system using pixel indices, the set of possible sample points are simply given by
    all integers within a given source patch. However since we divide each patch up into a lower-
    left and an upper-right triangle, the inverted orientation of the upper-right triangle axes 
    (relative to the target pixel coordinate system) means that we should use the integer ceiling
    assuming that we're using the integer floor for the lower-left triangle.
  */
  TruncatorFcn          truncator[]     = {std::floor, std::ceil};


  // Preallocated scratch space to save on reallocation
  std::vector<double>   srcGrid, relOrig;
  std::vector<int>      tgtGrid;
  
  // Set pixels w/o info to NaN
  const Pixel           noData          = static_cast<Pixel>( mxGetNaN() );
  std::fill(tgt, tgt + nPixels*nFrames, noData);



  //...........................................................................

  // Loop over frames
  for (mwSize iFrame = 0; iFrame < nFrames; ++iFrame) 
  {
    for (mwSize iX = 0; iX < lastPatchX; ++iX) {
      for (mwSize iY = 0; iY < lastPatchY; ++iY) {

        // Scale factors from patch to pixels
        int             iTri            = 0;
        const double    nSources[]      = { xCenter[iX+1] - xCenter[iX], yCenter[iY+1] - yCenter[iY] };
        
        // Loop over lower-left vs. upper-right triangular decompositions of this patch
        for (int iDir = 1; iDir >= -1; iDir -= 2) 
        {
          TruncatorFcn  fTrunc          = truncator[iDir < 0];
          
          // Location of the source triangle in the coordinate system of the target image
          const double  oWarped[]       = { xLoc[iY+iTri      + nPatchY*(iX+iTri)     ] - 1
                                          , yLoc[iY+iTri      + nPatchY*(iX+iTri)     ] - 1
                                          };
          const double  shape[]         = { xLoc[iY+iTri      + nPatchY*(iX+iTri+iDir)] - 1 - oWarped[0]
                                          , yLoc[iY+iTri      + nPatchY*(iX+iTri+iDir)] - 1 - oWarped[1]
                                          , xLoc[iY+iTri+iDir + nPatchY*(iX+iTri     )] - 1 - oWarped[0]
                                          , yLoc[iY+iTri+iDir + nPatchY*(iX+iTri     )] - 1 - oWarped[1]
                                          };


          // Invert shape matrix for transformations into barycentric coordinates
          const double  detShape        = 1.0 / ( shape[0]*shape[3] - shape[1]*shape[2] );
          const double  invShape[]      = {  shape[3] * detShape
                                          , -shape[1] * detShape
                                          , -shape[2] * detShape
                                          ,  shape[0] * detShape
                                          };

          // Relative origin of target query grid (pixels in output image)
          // Here we use the closest grid point to the origin of the source triangle
          const int     oGrid[]         = { static_cast<int>(fTrunc(oWarped[0]))
                                          , static_cast<int>(fTrunc(oWarped[1]))
                                          };
          const double  oGridRel[]      = { oGrid[0] - oWarped[0], oGrid[1] - oWarped[1] };
          
          // Origin of the query grid in barycentric coordinates
          double        bcGridOrig[2];
          matrixMultiply(bcGridOrig, invShape, oGridRel, 1);

          // Determine the bounding box of the source triangle relative to the origin (oGrid)
          double        nMin[2], nMax[2];
          nMin[0]       = 0;            nMin[1]       = 0;
          nMax[0]       = 0;            nMax[1]       = 0;
          for (int iSide = 0; iSide < 3; iSide += 2) {        // two sides of the triangle, i.e. columns of shape
            for (int iCoord = 0; iCoord < 2; ++iCoord) {      // x,y
              // Pixel location at or just outside of this vertex
              double    vertexBound     = static_cast<int>(fTrunc( oWarped[iCoord] + shape[iSide + iCoord] )) 
                                        - oGrid[iCoord]
                                        ;
              nMin[iCoord]              = std::min(nMin[iCoord], vertexBound);
              nMax[iCoord]              = std::max(nMax[iCoord], vertexBound);
            }
          }
          
          
          /*
            In the target coordinate system the query grid consists of all integer-valued
            points. We use a brute force approach of generating all possible query grid 
            points, transforming them into barycentric coordinates, and then keeping those 
            that are within the triangle.
          */
          srcGrid.clear();
          tgtGrid.clear();
          for (int iGrid = nMin[0]; iGrid <= nMax[0]; ++iGrid) {
            for (int jGrid = nMin[1]; jGrid <= nMax[1]; ++jGrid) {

              double    bcTest[2];
              double    rGrid[]         = {iGrid, jGrid};
              matrixMultiply(bcTest, invShape, rGrid);
              vectorAddTo(bcTest, bcGridOrig);

              if (bcTest[0] >= 0 && bcTest[1] >= 0 && bcTest[0] + bcTest[1] <= 1) {
                // Scaled so that barycentric coordinates are for the nearest 3 pixels
                // (as opposed to corners of a patch)
                srcGrid.push_back(bcTest[0] * nSources[0]);
                srcGrid.push_back(bcTest[1] * nSources[1]);

                // Index of output pixel in target coordinate system
                tgtGrid.push_back(oGrid[0] + iGrid);
                tgtGrid.push_back(oGrid[1] + jGrid);
              }

            }
          }


          /*
            To specify a value at the target point, we use barycentric interpolation where
            the triangle that contains the target point should use nearest available source 
            pixels. Here we make the locally linear assumption that the original measurement 
            is distributed on a regular grid, where the axes of this grid is along the two 
            sides of the patch-based triangle. In other words the pixel-based triangle T' is 
            assumed to be a simple scaling of the patch triangle T:
                   T' = T S 
                   S  = [ 1/Nx    0  ]
                        [   0   1/Ny ]
            where Nx and Ny are the number of pixels within the x and y directions of the
            original (motion-warped) image.

            We derive the following to convert from patch-based to pixel-based barycentric
            coordinates. First we define points in the target coordinate system:
                  r   : location of the target point, i.e. where we want to obtain the
                        interpolated value
                  r0  : origin of the patch-based triangle
                  r0' : origin of the pixel-based triangle

            The triangle shape is used to translate this to barycentric coordinates b:
                 r  = r0 + T b
                 b' = inv(T') (r - r0')
                    = inv(S) inv(T) ( r0 - r0' + T b )
                    = inv(S) b + inv(S) inv(T) ( r0 - r0' )

            Lastly r0' are given by integer/N steps along the patch shape axes.
          */
          const size_t  nPoints         = srcGrid.size() / 2;
          const double  scaledInv[]     = { invShape[0] * nSources[0]
                                          , invShape[1] * nSources[1]
                                          , invShape[2] * nSources[0]
                                          , invShape[3] * nSources[1]
                                          };

          for (size_t iPt = 0, iData = 0; iPt < nPoints; ++iPt, iData += 2) 
          {
            // Ignore points where the target pixel is out of range
            const size_t  jData         = iData + 1;
            if (tgtGrid[iData] < 0 || tgtGrid[iData] >= nCols || tgtGrid[jData] < 0 || tgtGrid[jData] >= nRows)
              continue;


            // Compute barycentric coordinates in the lower left pixel-based triangle 
            int         src1            = static_cast<int>(std::floor( srcGrid[iData] ));
            int         src2            = static_cast<int>(std::floor( srcGrid[jData] ));
            double      step1           = src1 / nSources[0];
            double      step2           = src2 / nSources[1];
            double      rOffset[]       = { tgtGrid[iData] - oWarped[0], tgtGrid[jData] - oWarped[1] };
            double      relOrig[]       = { rOffset[0] - step1*shape[0] - step2*shape[2]
                                          , rOffset[1] - step1*shape[1] - step2*shape[3]
                                          };
            double      bcCoords[3];
            matrixMultiply(bcCoords, scaledInv, relOrig);


            /*
              If the sum of the barycentric coordinates exceeds 1, that means that instead of the
              lower-left triangle the query point is in the upper-right triangle formed by the
              nearest 4 pixels. In this case we have to invert various measurements.
            */
            int         jDir            = iDir;
            if (bcCoords[0] + bcCoords[1] > 1) {
              jDir     *= -1;
              src1      = static_cast<int>(std::ceil( srcGrid[iData] ));
              src2      = static_cast<int>(std::ceil( srcGrid[jData] ));
              step1     = src1 / nSources[0];
              step2     = src2 / nSources[1];

              relOrig[0]= step1*shape[0] + step2*shape[2] - rOffset[0];
              relOrig[1]= step1*shape[1] + step2*shape[3] - rOffset[1];
              matrixMultiply(bcCoords, scaledInv, relOrig);
            }


            // Location of source pixels at the origin and sides of the triangle
            const int   rowOrig         = static_cast<int>( yCenter[iY+iTri] - 1 ) + iDir*src2;
            const int   colOrig         = static_cast<int>( xCenter[iX+iTri] - 1 ) + iDir*src1;
            if (rowOrig < 0 || rowOrig >= nRows || colOrig < 0 || colOrig >= nCols)
              continue;

            const int   rowSide         = rowOrig + jDir;
            const int   colSide         = colOrig + jDir;

            /*
              Read source pixels when available, otherwise ignore them
              FIXME:  Here I've just dropped the unavailable coordinates by setting their
              HACK    coefficients to zero and renormalizing the weights. It probably makes more
                      sense to switch to linear interpolation assuming that there are >= 2 valid
                      points remaining, or to use the one remaining value if it comes to that.
            */
            Pixel       srcValue[]      = { colSide < 0 || colSide >= nCols ? noData : src[rowOrig + nRows*colSide]
                                          , rowSide < 0 || rowSide >= nRows ? noData : src[rowSide + nRows*colOrig]
                                          ,                                            src[rowOrig + nRows*colOrig]
                                          };
            double      bcNorm          = 0;

            bcCoords[2] = 1 - bcCoords[0] - bcCoords[1];
            for (int iVal = 0; iVal < 3; ++iVal) {
              if (mxIsNaN(srcValue[iVal])) {
                srcValue[iVal]          = 0;
                bcCoords[iVal]          = 0;
              }
              else      bcNorm         += bcCoords[iVal];
            }


            // The interpolated value is a weighted sum of measurement values at the source pixels
            if (bcNorm > 0) {
              tgt[ tgtGrid[jData] + nRows*tgtGrid[iData] ]
                        = ( bcCoords[0] * srcValue[0]
                          + bcCoords[1] * srcValue[1]
                          + bcCoords[2] * srcValue[2]
                          ) / bcNorm
                          ;
            }
          } // end loop over grid points to sample



          // Increment triangle origin offset
          iTri          = iTri + 1;
        } // end loop over triangles
      } // end loop over patches in y
    } // end loop over patches in x


    //.........................................................................
    // Advance pointers to the next frame
    src                += nPixels;
    tgt                += nPixels;
    xLoc               += nPatches;
    yLoc               += nPatches;
    if (perFrameX)
      xCenter          += nPatchX;
    if (perFrameY)
      yCenter          += nPatchY;
  } // end loop over frames

}



//=============================================================================
bool checkTypeAndSizes(const mxArray* source, const mxArray* centers, const mxArray* shifts, const mwSize dim, const char* label)
{
  if (mxGetClassID(source) != mxGetClassID(shifts))
    mexErrMsgIdAndTxt( "barycentricMeshWarp:input", "Data type of source image and %sTarget must be the same.", label );
  if (!mxIsDouble(centers))
    mexErrMsgIdAndTxt( "barycentricMeshWarp:input", "%sSample must be of data type double.", label );

  const mwSize*         nSrcDims        = mxGetDimensions(source);
  const mwSize*         nTgtDims        = mxGetDimensions(shifts);
  const mwSize          nSrcFrames      = mxGetNumberOfDimensions(source) > 2 ? nSrcDims[2] : 1;
  const mwSize          nTgtFrames      = mxGetNumberOfDimensions(shifts) > 2 ? nTgtDims[2] : 1;

  if (nTgtFrames < nSrcFrames)
    mexErrMsgIdAndTxt( "barycentricMeshWarp:input", "%sTarget must have at least as many frames (3rd dimension) as source movie.", label );

  if (mxGetM(centers) == 1 || mxGetN(centers) == 1) {         // 1-dimensional list specified
    if (mxGetNumberOfElements(centers) != nTgtDims[dim])
      mexErrMsgIdAndTxt( "barycentricMeshWarp:input", "%sSample must have the same number of elements as dimension %d of %sTarget.", label, dim+1, label );
    return false;
  }

  if (mxGetM(centers) != nTgtDims[dim] || mxGetN(centers) != nSrcFrames)
    mexErrMsgIdAndTxt( "barycentricMeshWarp:input", "%sSample must have %d rows (dimension %d of %sTarget) and %d columns (source movie frames).", label, dim+1, label, nSrcFrames );
  return true;
}


//=============================================================================
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  //----- Parse arguments
  if (nrhs < 5) {
    mexEvalString("help cv.barycentricMeshWarp");
    mexErrMsgIdAndTxt( "barycentricMeshWarp:usage", "Incorrect number of inputs/outputs provided." );
  }

  const mxArray*        source          = prhs[0];
  const mxArray*        xSample         = prhs[1];
  const mxArray*        ySample         = prhs[2];
  const mxArray*        xTarget         = prhs[3];
  const mxArray*        yTarget         = prhs[4];

  const bool            perFrameX       = checkTypeAndSizes(source, xSample, xTarget, 1, "x");
  const bool            perFrameY       = checkTypeAndSizes(source, ySample, yTarget, 0, "y");
  if (mxGetNumberOfElements(xTarget) != mxGetNumberOfElements(yTarget))
    mexErrMsgIdAndTxt( "barycentricMeshWarp:input", "xTarget and yTarget must have the same number of elements." );

  
  const mwSize*         srcDims         = mxGetDimensions(source);
  const mwSize          nDims           = mxGetNumberOfDimensions(source);
  std::vector<size_t>   dimensions;
  dimensions.assign(srcDims, srcDims + nDims);
  plhs[0]               = mxCreateUninitNumericArray( nDims, dimensions.data(), mxGetClassID(source), mxREAL );


  switch (mxGetClassID(source)) {
  case mxSINGLE_CLASS :   barycentricMeshWarp<float         >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxCHAR_CLASS   :   barycentricMeshWarp<char          >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxDOUBLE_CLASS :   barycentricMeshWarp<double        >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxINT8_CLASS   :   barycentricMeshWarp<char          >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxUINT8_CLASS  :   barycentricMeshWarp<unsigned char >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxINT16_CLASS  :   barycentricMeshWarp<short         >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxUINT16_CLASS :   barycentricMeshWarp<unsigned short>(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxINT32_CLASS  :   barycentricMeshWarp<int           >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxUINT32_CLASS :   barycentricMeshWarp<unsigned int  >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxINT64_CLASS  :   barycentricMeshWarp<int64_t       >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;
  case mxUINT64_CLASS :   barycentricMeshWarp<uint64_t      >(source, plhs[0], xSample, ySample, xTarget, yTarget, perFrameX, perFrameY);   break;

  default:
    mexErrMsgIdAndTxt("barycentricMeshWarp:arguments", "Unsupported type of source image.");
  }
}

