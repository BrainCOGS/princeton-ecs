/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

/****************************************************************************************\
    A part of the file implements TIFF reader on base of libtiff library
    (see otherlibs/_graphics/readme.txt for copyright notice)
\****************************************************************************************/

#include "precomp.hpp"
#include "grfmt_tiff.hpp"
#include <opencv2/imgproc.hpp>
#include "utils.hpp"



namespace cv
{
static const char fmtSignTiffII[] = "II\x2a\x00";

static const char fmtSignTiffMM[] = "MM\x00\x2a";

#include "libtiff/tiff.h"
#include "libtiff/tiffio.h"

static int grfmt_tiff_err_handler_init = 0;
static void GrFmtSilentTIFFErrorHandler( const char*, const char*, va_list ) {}

TiffDecoder::TiffDecoder()
{
    m_tif = 0;
    if( !grfmt_tiff_err_handler_init )
    {
        grfmt_tiff_err_handler_init = 1;

        TIFFSetErrorHandler( GrFmtSilentTIFFErrorHandler );
        TIFFSetWarningHandler( GrFmtSilentTIFFErrorHandler );
    }
    m_hdr = false;
}


void TiffDecoder::close()
{
    if( m_tif )
    {
        TIFF* tif = (TIFF*)m_tif;
        TIFFClose( tif );
        m_tif = 0;
    }
}

TiffDecoder::~TiffDecoder()
{
    close();
}

bool TiffDecoder::setSource( const String& filename )
{
    m_filename = filename;
    m_buf.release();
    return true;
}

size_t TiffDecoder::signatureLength() const
{
    return 4;
}

bool TiffDecoder::checkSignature( const String& signature ) const
{
    return signature.size() >= 4 &&
        (memcmp(signature.c_str(), fmtSignTiffII, 4) == 0 ||
        memcmp(signature.c_str(), fmtSignTiffMM, 4) == 0);
}

int TiffDecoder::normalizeChannelsNumber(int channels) const
{
    return channels > 4 ? 4 : channels;
}

bool TiffDecoder::readHeader()
{
    bool result = false;

    TIFF* tif = static_cast<TIFF*>(m_tif);
    if (!m_tif)
    {
        // TIFFOpen() mode flags are different to fopen().  A 'b' in mode "rb" has no effect when reading.
        // http://www.remotesensing.org/libtiff/man/TIFFOpen.3tiff.html
        tif = TIFFOpen(m_filename.c_str(), "r");
    }

    if( tif )
    {
        uint32 wdth = 0, hght = 0;
        uint16 photometric = 0;
        m_tif = tif;

        if( TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &wdth ) &&
            TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &hght ) &&
            TIFFGetField( tif, TIFFTAG_PHOTOMETRIC, &photometric ))
        {
            uint16 bpp=8, ncn = photometric > 1 ? 3 : 1;
            m_format = SAMPLEFORMAT_VOID;
            TIFFGetField( tif, TIFFTAG_BITSPERSAMPLE, &bpp );
            TIFFGetField( tif, TIFFTAG_SAMPLESPERPIXEL, &ncn );
            
            // SAK -- HACK to (more) properly read out signed vs. unsigned images,
            // because of course ImageJ cannot do something as simple as to save
            // TIFFTAG_SAMPLEFORMAT (nor preserve existing header info).
            if (!TIFFGetField( tif, TIFFTAG_SAMPLEFORMAT, &m_format )) {
              char*   desc;
              if (TIFFGetField( tif, TIFFTAG_IMAGEDESCRIPTION, &desc )) {
                for (int iDesc = 0; desc[iDesc]; ++iDesc)
                  if (desc[iDesc]=='\n' && desc[iDesc+1]=='c' && desc[iDesc+2]=='0' && desc[iDesc+3]=='=' && desc[iDesc+4]=='-') {
                    m_format = SAMPLEFORMAT_INT;
                    break;
                  }
              }
            }

            m_width = wdth;
            m_height = hght;
            if((bpp == 32 && ncn == 3) || photometric == PHOTOMETRIC_LOGLUV)
            {
                m_type = CV_32FC3;
                m_hdr = true;
                return true;
            }
            m_hdr = false;

            if( bpp > 8 &&
               ((photometric != 2 && photometric != 1) ||
                (ncn != 1 && ncn != 3 && ncn != 4)))
                bpp = 8;

            int wanted_channels = normalizeChannelsNumber(ncn);
            switch(bpp)
            {
                case 8:
                    m_type = CV_MAKETYPE(CV_8U, photometric > 1 ? wanted_channels : 1);
                    break;
                case 16:
                    m_type = CV_MAKETYPE(CV_16U, photometric > 1 ? wanted_channels : 1);
                    break;

                case 32:
                    m_type = CV_MAKETYPE(CV_32F, photometric > 1 ? 3 : 1);
                    break;
                case 64:
                    m_type = CV_MAKETYPE(CV_64F, photometric > 1 ? 3 : 1);
                    break;

                default:
                    result = false;
            }
            result = true;
        }
    }

    if( !result )
        close();

    return result;
}

bool TiffDecoder::nextPage()
{
    // Prepare the next page, if any.
    return m_tif &&
           TIFFReadDirectory(static_cast<TIFF*>(m_tif)) &&
           readHeader();
}

bool  TiffDecoder::readData( Mat& img )
{
    if(m_hdr && img.type() == CV_32FC3)
    {
        return readHdrData(img);
    }
    bool result = false;
    bool color = img.channels() > 1;
    uchar* data = img.ptr();

    if( img.depth() != CV_8U && img.depth() != CV_16U && img.depth() != CV_32F && img.depth() != CV_64F )
        return false;

    if( m_tif && m_width && m_height )
    {
        TIFF* tif = (TIFF*)m_tif;
        uint32 tile_width0 = m_width, tile_height0 = 0;
        int x, y, i;
        int is_tiled = TIFFIsTiled(tif);
        uint16 photometric;
        TIFFGetField( tif, TIFFTAG_PHOTOMETRIC, &photometric );
        uint16 bpp = 8, ncn = photometric > 1 ? 3 : 1;
        TIFFGetField( tif, TIFFTAG_BITSPERSAMPLE, &bpp );
        TIFFGetField( tif, TIFFTAG_SAMPLESPERPIXEL, &ncn );
        const int bitsPerByte = 8;
        int dst_bpp = (int)(img.elemSize1() * bitsPerByte);
        int wanted_channels = normalizeChannelsNumber(img.channels());

        if(dst_bpp == 8)
        {
            char errmsg[1024];
            if(!TIFFRGBAImageOK( tif, errmsg ))
            {
                close();
                return false;
            }
        }

        if( (!is_tiled) ||
            (is_tiled &&
            TIFFGetField( tif, TIFFTAG_TILEWIDTH, &tile_width0 ) &&
            TIFFGetField( tif, TIFFTAG_TILELENGTH, &tile_height0 )))
        {
            if(!is_tiled)
                TIFFGetField( tif, TIFFTAG_ROWSPERSTRIP, &tile_height0 );

            if( tile_width0 <= 0 )
                tile_width0 = m_width;

            if( tile_height0 <= 0 )
                tile_height0 = m_height;

            AutoBuffer<uchar> _buffer( size_t(8) * tile_height0*tile_width0);
            uchar* buffer = _buffer;
            ushort* buffer16 = (ushort*)buffer;
            float* buffer32 = (float*)buffer;
            double* buffer64 = (double*)buffer;
            int tileidx = 0;

            for( y = 0; y < m_height; y += tile_height0, data += img.step*tile_height0 )
            {
                int tile_height = tile_height0;

                if( y + tile_height > m_height )
                    tile_height = m_height - y;

                for( x = 0; x < m_width; x += tile_width0, tileidx++ )
                {
                    int tile_width = tile_width0, ok;

                    if( x + tile_width > m_width )
                        tile_width = m_width - x;

                    switch(dst_bpp)
                    {
                        case 8:
                        {
                            uchar * bstart = buffer;
                            if( !is_tiled )
                                ok = TIFFReadRGBAStrip( tif, y, (uint32*)buffer );
                            else
                            {
                                ok = TIFFReadRGBATile( tif, x, y, (uint32*)buffer );
                                //Tiles fill the buffer from the bottom up
                                bstart += (tile_height0 - tile_height) * tile_width0 * 4;
                            }
                            if( !ok )
                            {
                                close();
                                return false;
                            }

                            for( i = 0; i < tile_height; i++ )
                                if( color )
                                {
                                    if (wanted_channels == 4)
                                    {
                                        icvCvt_BGRA2RGBA_8u_C4R( bstart + i*tile_width0*4, 0,
                                                             data + x*4 + img.step*(tile_height - i - 1), 0,
                                                             cvSize(tile_width,1) );
                                    }
                                    else
                                    {
                                        icvCvt_BGRA2BGR_8u_C4C3R( bstart + i*tile_width0*4, 0,
                                                             data + x*3 + img.step*(tile_height - i - 1), 0,
                                                             cvSize(tile_width,1), 2 );
                                    }
                                }
                                else
                                    icvCvt_BGRA2Gray_8u_C4C1R( bstart + i*tile_width0*4, 0,
                                                              data + x + img.step*(tile_height - i - 1), 0,
                                                              cvSize(tile_width,1), 2 );
                            break;
                        }

                        case 16:
                        {
                            if( !is_tiled )
                                ok = (int)TIFFReadEncodedStrip( tif, tileidx, (uint32*)buffer, (tsize_t)-1 ) >= 0;
                            else
                                ok = (int)TIFFReadEncodedTile( tif, tileidx, (uint32*)buffer, (tsize_t)-1 ) >= 0;

                            if( !ok )
                            {
                                close();
                                return false;
                            }

                            for( i = 0; i < tile_height; i++ )
                            {
                                if( color )
                                {
                                    if( ncn == 1 )
                                    {
                                        icvCvt_Gray2BGR_16u_C1C3R(buffer16 + i*tile_width0*ncn, 0,
                                                                  (ushort*)(data + img.step*i) + x*3, 0,
                                                                  cvSize(tile_width,1) );
                                    }
                                    else if( ncn == 3 )
                                    {
                                        icvCvt_RGB2BGR_16u_C3R(buffer16 + i*tile_width0*ncn, 0,
                                                               (ushort*)(data + img.step*i) + x*3, 0,
                                                               cvSize(tile_width,1) );
                                    }
                                    else if (ncn == 4)
                                    {
                                        if (wanted_channels == 4)
                                        {
                                            icvCvt_BGRA2RGBA_16u_C4R(buffer16 + i*tile_width0*ncn, 0,
                                                (ushort*)(data + img.step*i) + x * 4, 0,
                                                cvSize(tile_width, 1));
                                        }
                                        else
                                        {
                                            icvCvt_BGRA2BGR_16u_C4C3R(buffer16 + i*tile_width0*ncn, 0,
                                                (ushort*)(data + img.step*i) + x * 3, 0,
                                                cvSize(tile_width, 1), 2);
                                        }
                                    }
                                    else
                                    {
                                        icvCvt_BGRA2BGR_16u_C4C3R(buffer16 + i*tile_width0*ncn, 0,
                                                               (ushort*)(data + img.step*i) + x*3, 0,
                                                               cvSize(tile_width,1), 2 );
                                    }
                                }
                                else
                                {
                                    if( ncn == 1 )
                                    {
                                        memcpy((ushort*)(data + img.step*i)+x,
                                               buffer16 + i*tile_width0*ncn,
                                               tile_width*sizeof(buffer16[0]));
                                    }
                                    else
                                    {
                                        icvCvt_BGRA2Gray_16u_CnC1R(buffer16 + i*tile_width0*ncn, 0,
                                                               (ushort*)(data + img.step*i) + x, 0,
                                                               cvSize(tile_width,1), ncn, 2 );
                                    }
                                }
                            }
                            break;
                        }

                        case 32:
                        case 64:
                        {
                            if( !is_tiled )
                                ok = (int)TIFFReadEncodedStrip( tif, tileidx, buffer, (tsize_t)-1 ) >= 0;
                            else
                                ok = (int)TIFFReadEncodedTile( tif, tileidx, buffer, (tsize_t)-1 ) >= 0;

                            if( !ok || ncn != 1 )
                            {
                                close();
                                return false;
                            }

                            for( i = 0; i < tile_height; i++ )
                            {
                                if(dst_bpp == 32)
                                {
                                    memcpy((float*)(data + img.step*i)+x,
                                           buffer32 + i*tile_width0*ncn,
                                           tile_width*sizeof(buffer32[0]));
                                }
                                else
                                {
                                    memcpy((double*)(data + img.step*i)+x,
                                         buffer64 + i*tile_width0*ncn,
                                         tile_width*sizeof(buffer64[0]));
                                }
                            }

                            break;
                        }
                        default:
                        {
                            close();
                            return false;
                        }
                    }
                }
            }

            result = true;
        }
    }

    return result;
}

bool TiffDecoder::readHdrData(Mat& img)
{
    int rows_per_strip = 0, photometric = 0;
    if(!m_tif)
    {
        return false;
    }
    TIFF *tif = static_cast<TIFF*>(m_tif);
    TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip);
    TIFFGetField( tif, TIFFTAG_PHOTOMETRIC, &photometric );
    TIFFSetField(tif, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_FLOAT);
    int size = 3 * m_width * m_height * sizeof (float);
    tstrip_t strip_size = 3 * m_width * rows_per_strip;
    float *ptr = img.ptr<float>();
    for (tstrip_t i = 0; i < TIFFNumberOfStrips(tif); i++, ptr += strip_size)
    {
        TIFFReadEncodedStrip(tif, i, ptr, size);
        size -= strip_size * sizeof(float);
    }
    close();
    if(photometric == PHOTOMETRIC_LOGLUV)
    {
        cvtColor(img, img, COLOR_XYZ2BGR);
    }
    else
    {
        cvtColor(img, img, COLOR_RGB2BGR);
    }
    return true;
}

}
