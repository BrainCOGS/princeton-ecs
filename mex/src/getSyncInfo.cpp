/**
  Parses the TIFF file header to extract ScanImage specific synchronization information.

  Usage syntax:
      [acquisition, epoch, frameTime, dataTime, data] = getSyncInfo(inputFile, dataType)

  Author:   Sue Ann Koay (koay@princeton.edu)
*/


#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <mex.h>
#include <tiffio.h>


static const char       ACQ_NAME[]    = "acquisitionNumbers";
static const char       TIME_NAME[]   = "frameTimestamps_sec";
static const char       EPOCH_NAME[]  = "epoch";
static const char       DATA_NAME[]   = "I2CData";
static const size_t     N_ACQNAME     = std::strlen(ACQ_NAME);
static const size_t     N_TIMENAME    = std::strlen(TIME_NAME);
static const size_t     N_EPOCHNAME   = std::strlen(EPOCH_NAME);
static const size_t     N_DATANAME    = std::strlen(DATA_NAME);
static const int        N_TIMESTAMP   = 6;
static const double     INVALID       = mxGetNaN();



template<typename Number>
bool readScalarField(char*& desc, const char* fieldName, const size_t nameLength, std::vector<Number>& data)
{
  for (; *desc; ++desc) {
    // Locate field name
    if (std::strncmp(desc, fieldName, nameLength)) {
      while (*desc && *desc != '\n')  ++desc;      // skip until end of line
    }
    else {            // match
      for (desc += nameLength; *desc != '='; ++desc)
        if (!*desc || *desc == '\n')
          return false;

      // Parse results as double precision
      ++desc;
      data.push_back(std::strtod(desc, &desc));
      while (*desc && *desc != '\n')  ++desc;      // skip until end of line
      return true;
    }
  }
  return false;
}

template<typename Number>
bool readVectorField(char*& desc, const char* fieldName, const size_t nameLength, std::vector<Number>& data)
{
  for (; *desc; ++desc) {
    // Locate field name
    if (std::strncmp(desc, fieldName, nameLength)) {
      while (*desc && *desc != '\n')  ++desc;      // skip until end of line
    }
    else {            // match
      for (desc += nameLength; *desc != '['; ++desc)
        if (!*desc || *desc == '\n')
          return false;

      // Parse results as double precision
      for (++desc; *desc != ']';)
        data.push_back(std::strtod(desc, &desc));

      while (*desc && *desc != '\n')  ++desc;      // skip until end of line
      return true;
    }
  }
  return false;
}


//=============================================================================
template<typename Number, int NumBytes>
void parseInfo(const char* inputFile, mxArray*& matAcquisition, mxArray*& matEpoch, mxArray*& matFrameTime, mxArray*& matDataTime, mxArray*& matData, const mxClassID dataClass)
{
  //----- Preallocate scratch space
  std::vector<double>   acquisition, epoch;
  std::vector<double>   frameTime, dataTime;
  std::vector<Number>   data;
  unsigned char         temp[NumBytes];


  //----- Loop through directory headers
  TIFF*                 img           = TIFFOpen(inputFile, "r");
  if (img == NULL)      mexErrMsgIdAndTxt("getSyncInfo:input", "Failed to load input file '%s'.", inputFile);


  size_t                nFrames       = 0;
  size_t                nData         = 0;
  do {
    // Read image description tag
    char*               desc          = NULL;
    if (!TIFFGetField(img, TIFFTAG_IMAGEDESCRIPTION, &desc))
      mexErrMsgIdAndTxt("getSyncInfo:header", "Failed to read image description tag for frame %d of '%s'.", nFrames+1, inputFile);


    // Locate acqusition number assuming that it is meaningfully recorded only for the first frame
    if (nFrames < 1 && !readScalarField(desc, ACQ_NAME, N_ACQNAME, acquisition))
      mexErrMsgIdAndTxt("getSyncInfo:header", "Failed to parse %s for frame %d of '%s'.", ACQ_NAME, nFrames+1, inputFile);

    // Locate frame timestamp
    if (!readScalarField(desc, TIME_NAME, N_TIMENAME, frameTime))
      mexErrMsgIdAndTxt("getSyncInfo:header", "Failed to parse %s for frame %d of '%s'.", TIME_NAME, nFrames+1, inputFile);

    // Locate wall clock time
    if (!readVectorField(desc, EPOCH_NAME, N_EPOCHNAME, epoch))
      mexErrMsgIdAndTxt("getSyncInfo:header", "Failed to parse %s for frame %d of '%s'.", EPOCH_NAME, nFrames+1, inputFile);


    // Locate I2C field entry
    for (++desc; *desc; ++desc) {
      if (std::strncmp(desc, DATA_NAME, N_DATANAME)) {
        while (*desc && *desc != '\n')  ++desc;      // skip until end of line
      }
      else {            // match
        desc           += N_DATANAME;
        for (; *desc != '{'; ++desc)
          if (!*desc || *desc == '\n')
            mexErrMsgIdAndTxt("getSyncInfo:header", "Failed to parse %s for frame %d of '%s'.", DATA_NAME, nFrames+1, inputFile);

        // Retain first entry
        bool            noData        = false;
        for (++desc; *desc != '{'; ++desc)
          if (!*desc || *desc == '\n') {
            noData      = true;
            break;
          }
        if (noData) {
          dataTime.push_back(INVALID);
          break;
        }

        // Parse time stamp as double precision
        ++desc;
        dataTime.push_back(std::strtod(desc, &desc));

        // Parse data packet
        for (; *desc != '['; ++desc) {
          if (!*desc || *desc == '\n')
            mexErrMsgIdAndTxt("getSyncInfo:header", "Failed to parse %s for frame %d of '%s'.", DATA_NAME, nFrames+1, inputFile);
        }

        int             count         = 0;
        for (++desc; *desc != ']'; ++desc, ++count) {
          if (!*desc || *desc == '\n')
            mexErrMsgIdAndTxt("getSyncInfo:header", "Failed to parse %s for frame %d of '%s'.", DATA_NAME, nFrames+1, inputFile);

          for (int iByte = 0; iByte < NumBytes; ++iByte) {
            const char* current       = desc;
            temp[iByte] = static_cast<unsigned char>(std::strtoul(current, &desc, 10));
            if (desc == current) {
              if (iByte < 1)  break;
              mexErrMsgIdAndTxt("getSyncInfo:header", "Failed to parse %s entry %d for frame %d of '%s'.", DATA_NAME, count, nFrames+1, inputFile);
            }
          }
          data.push_back(*reinterpret_cast<Number*>(temp));
        }

        if (nData < 1)  nData         = count;
        else if (nData != count)
          mexErrMsgIdAndTxt("getSyncInfo:header", "Inconsistent number of %s entries %d (expected %d) read for frame %d of '%s'.", DATA_NAME, count, nData, nFrames+1, inputFile);

        break;
      }
    }
    
    ++nFrames;
  } while (TIFFReadDirectory(img));



  //----- Copy output to Matlab
  matAcquisition        = mxCreateDoubleScalar(acquisition.size() ? acquisition[0] : 0.);
  matEpoch              = mxCreateDoubleMatrix (1    , N_TIMESTAMP, mxREAL);
  matFrameTime          = mxCreateDoubleMatrix (1    , nFrames    , mxREAL);
  matDataTime           = mxCreateDoubleMatrix (1    , nFrames    , mxREAL);
  matData               = mxCreateNumericMatrix(nData, nFrames    , dataClass, mxREAL);

  double*               outEpoch      = mxGetPr(matEpoch);
  double*               outFrameTime  = mxGetPr(matFrameTime);
  double*               outDataTime   = mxGetPr(matDataTime );
  Number*               outData       = (Number*) mxGetData(matData);

  for (int iFrame = 0, iSource = 0; iFrame < nFrames; ++iFrame, ++outFrameTime, ++outDataTime) {
    *outFrameTime       = frameTime[iFrame];
    *outDataTime        = dataTime [iFrame];
    if (mxIsNaN(*outDataTime)) {
      for (int iData = 0; iData < nData; ++iData, ++outData)
        *outData        = 0;
    } else {
      for (int iData = 0; iData < nData; ++iData, ++iSource, ++outData)
        *outData        = data[iSource];
    }
  }


  if (epoch.size() != N_TIMESTAMP * nFrames)
    mexErrMsgIdAndTxt("getSyncInfo:header", "Incorrect number of elements %d (expected %d) per %s vector in '%s'.", epoch.size()/nFrames, N_TIMESTAMP, EPOCH_NAME, inputFile);
  for (size_t iData = 0; iData < N_TIMESTAMP; ++iData, ++outEpoch)
    *outEpoch           = epoch[iData];
}



//=============================================================================
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  //----- Parse arguments
  if (nlhs != 5 || nrhs != 2) {
    mexEvalString("help ecs.getSyncInfo");
    mexErrMsgIdAndTxt ( "getSyncInfo:usage", "Incorrect number of inputs/outputs provided." );
  }

  const mxArray*        matInput      = prhs[0];
  const mxArray*        matType       = prhs[1];
  if (!mxIsChar(matInput))
    mexErrMsgIdAndTxt("getSyncInfo:arguments", "inputFile must be a character array.");
  if (!mxIsChar(matType))
    mexErrMsgIdAndTxt("getSyncInfo:arguments", "dataType must be a character array.");


  std::vector<char>     inputFile(mxGetNumberOfElements(matInput) + 1);
  mxGetString(matInput, inputFile.data(), static_cast<mwSize>(inputFile.size()));

  std::vector<char>     dataType(mxGetNumberOfElements(matType) + 1);
  mxGetString(matType, dataType.data(), static_cast<mwSize>(dataType.size()));


  if      (strcmp(dataType.data(), "int8"  ) == 0)   parseInfo<char          , 1>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxINT8_CLASS  );
  else if (strcmp(dataType.data(), "uint8" ) == 0)   parseInfo<unsigned char , 1>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxUINT8_CLASS );  
  else if (strcmp(dataType.data(), "int16" ) == 0)   parseInfo<short         , 2>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxINT16_CLASS );  
  else if (strcmp(dataType.data(), "uint16") == 0)   parseInfo<unsigned short, 2>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxUINT16_CLASS);  
  else if (strcmp(dataType.data(), "int32" ) == 0)   parseInfo<int           , 4>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxINT32_CLASS );  
  else if (strcmp(dataType.data(), "uint32") == 0)   parseInfo<unsigned int  , 4>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxUINT32_CLASS);  
  else if (strcmp(dataType.data(), "int64" ) == 0)   parseInfo<int64_t       , 8>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxINT64_CLASS );  
  else if (strcmp(dataType.data(), "uint64") == 0)   parseInfo<uint64_t      , 8>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxUINT64_CLASS);  
  else if (strcmp(dataType.data(), "single") == 0)   parseInfo<float         , 4>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxSINGLE_CLASS);
  else if (strcmp(dataType.data(), "double") == 0)   parseInfo<double        , 8>(inputFile.data(), plhs[0], plhs[1], plhs[2], plhs[3], plhs[4], mxDOUBLE_CLASS);  
  else    mexErrMsgIdAndTxt("getSyncInfo:dataType", "Unsupported dataType '%s'.", dataType.data());
}

