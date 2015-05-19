/**
  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <libtiff/tiffio.h>
#include <mex.h>


#define _DO_NOT_EXPORT
#if defined(_DO_NOT_EXPORT)
#define DllExport  
#else
#define DllExport __declspec(dllexport)
#endif



///////////////////////////////////////////////////////////////////////////
// Main entry point to a MEX function
///////////////////////////////////////////////////////////////////////////


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{  
  // Check inputs to mex function
  if (nrhs != 1 || nlhs != 1) {
    mexErrMsgIdAndTxt ( "debugImage:usage"
                      , "Usage:\n   image = debugImage(inputPath)"
                      );
  }
  if (!mxIsChar(prhs[0]))     mexErrMsgIdAndTxt("cvMotionCorrect:arguments", "inputPath must be a string.");


  // Parse input
  char*                       inputPath       = mxArrayToString(prhs[0]);

  //---------------------------------------------------------------------------

  TIFF*                       tif             = TIFFOpen(inputPath, "r");
  if (!tif)
    mexErrMsgIdAndTxt("debugImage:input", "Failed to load image file %s.", inputPath);

  // Output
  uint32                      imageWidth, imageHeight;
  if ( !TIFFGetField(tif, TIFFTAG_IMAGEWIDTH , &imageWidth )
    || !TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageHeight)
     )
    mexErrMsgIdAndTxt("debugImage:input", "Invalid header for %s, no size information available.", inputPath);

  uint32                      rowsPerStrip;
  short                       bytesPerSample;
	TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP , &rowsPerStrip);
	TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bytesPerSample);
  bytesPerSample             /= 8;

  short                       fillOrder;
  if (!TIFFGetField(tif, TIFFTAG_FILLORDER, &fillOrder))
    fillOrder                 = -1;


  plhs[0]                     = mxCreateDoubleMatrix(imageWidth, imageHeight, mxREAL);  // transposed!
  double*                     imgData         = mxGetPr(plhs[0]);

  const int                   isTiled         = TIFFIsTiled(tif);
  const tmsize_t              stripSize       = TIFFStripSize(tif);
  const tstrip_t              numStrips       = TIFFNumberOfStrips(tif);
  const tmsize_t              scanline        = TIFFScanlineSize(tif);
  tdata_t                     buf             = _TIFFmalloc(stripSize);
  //unsigned short*             rawData         = (unsigned short*) buf;
  short*                      rawData         = (short*) buf;
  //unsigned char*              rawData         = (unsigned char*) buf;
	//_TIFFmemset(buf, 0, TIFFStripSize(tif));

  
  tmsize_t                    imgIndex        = 0;
  for (tstrip_t iStrip = 0; iStrip < numStrips; ++iStrip) {
    tmsize_t                  stripLength     = TIFFReadEncodedStrip(tif, iStrip, rawData, (tmsize_t) -1);
    //TIFFSwabArrayOfShort((uint16*) rawData, stripLength/bytesPerSample);
    for (tmsize_t iPix = 0; iPix < stripLength/bytesPerSample; ++iPix, ++imgIndex)
      imgData[imgIndex]       = rawData[iPix];
  }

 // for (uint32 row = 0; row < imageHeight; row += rowsPerStrip)
	//{
 //   uint32                    strip           = TIFFComputeStrip(tif, row, 0);
 //   tmsize_t                  stripLength     = TIFFReadEncodedStrip(tif, strip, buf, (tmsize_t) -1);

 //   for (tmsize_t iPix = 0; iPix < stripLength/bytesPerSample; ++iPix, ++imgIndex)
 //     imgData[imgIndex]       = rawData[iPix];
	//}



  //---------------------------------------------------------------------------
  // Memory cleanup
  _TIFFfree(buf);
  TIFFClose(tif);
  mxFree(inputPath);
}

