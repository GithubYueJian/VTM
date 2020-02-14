/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2019, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief Implementation of InterpolationFilter class
 */

// ====================================================================================================================
// Includes
// ====================================================================================================================

#include "Rom.h"
#include "InterpolationFilter.h"

#include "ChromaFormat.h"

#if JVET_J0090_MEMORY_BANDWITH_MEASURE
CacheModel* InterpolationFilter::m_cacheModel;
#endif
//! \ingroup CommonLib
//! \{

// ====================================================================================================================
// Tables
// ====================================================================================================================
const TFilterCoeff InterpolationFilter::m_lumaFilter4x4[LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_LUMA] =
{
  {  0, 0,   0, 64,  0,   0,  0,  0 },
  {  0, 1,  -3, 63,  4,  -2,  1,  0 },
  {  0, 1,  -5, 62,  8,  -3,  1,  0 },
  {  0, 2,  -8, 60, 13,  -4,  1,  0 },
  {  0, 3, -10, 58, 17,  -5,  1,  0 }, //1/4
  {  0, 3, -11, 52, 26,  -8,  2,  0 },
  {  0, 2,  -9, 47, 31, -10,  3,  0 },
  {  0, 3, -11, 45, 34, -10,  3,  0 },
  {  0, 3, -11, 40, 40, -11,  3,  0 }, //1/2
  {  0, 3, -10, 34, 45, -11,  3,  0 },
  {  0, 3, -10, 31, 47,  -9,  2,  0 },
  {  0, 2,  -8, 26, 52, -11,  3,  0 },
  {  0, 1,  -5, 17, 58, -10,  3,  0 }, //3/4
  {  0, 1,  -4, 13, 60,  -8,  2,  0 },
  {  0, 1,  -3,  8, 62,  -5,  1,  0 },
  {  0, 1,  -2,  4, 63,  -3,  1,  0 }
};

const TFilterCoeff InterpolationFilter::m_lumaFilter[LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_LUMA] =
{
  {  0, 0,   0, 64,  0,   0,  0,  0 },
  {  0, 1,  -3, 63,  4,  -2,  1,  0 },
  { -1, 2,  -5, 62,  8,  -3,  1,  0 },
  { -1, 3,  -8, 60, 13,  -4,  1,  0 },
  { -1, 4, -10, 58, 17,  -5,  1,  0 },
  { -1, 4, -11, 52, 26,  -8,  3, -1 },
  { -1, 3,  -9, 47, 31, -10,  4, -1 },
  { -1, 4, -11, 45, 34, -10,  4, -1 },
  { -1, 4, -11, 40, 40, -11,  4, -1 },
  { -1, 4, -10, 34, 45, -11,  4, -1 },
  { -1, 4, -10, 31, 47,  -9,  3, -1 },
  { -1, 3,  -8, 26, 52, -11,  4, -1 },
  {  0, 1,  -5, 17, 58, -10,  4, -1 },
  {  0, 1,  -4, 13, 60,  -8,  3, -1 },
  {  0, 1,  -3,  8, 62,  -5,  2, -1 },
  {  0, 1,  -2,  4, 63,  -3,  1,  0 }
};

#if JVET_P0088_P0353_RPR_FILTERS
// 1.5x
const TFilterCoeff InterpolationFilter::m_lumaFilterRPR1[LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_LUMA] =
{
  { -1, -5, 17, 42, 17, -5, -1,  0 },
  {  0, -5, 15, 41, 19, -5, -1,  0 },
  {  0, -5, 13, 40, 21, -4, -1,  0 },
  {  0, -5, 11, 39, 24, -4, -2,  1 },
  {  0, -5,  9, 38, 26, -3, -2,  1 },
  {  0, -5,  7, 38, 28, -2, -3,  1 },
  {  1, -5,  5, 36, 30, -1, -3,  1 },
  {  1, -4,  3, 35, 32,  0, -4,  1 },
  {  1, -4,  2, 33, 33,  2, -4,  1 },
  {  1, -4,  0, 32, 35,  3, -4,  1 },
  {  1, -3, -1, 30, 36,  5, -5,  1 },
  {  1, -3, -2, 28, 38,  7, -5,  0 },
  {  1, -2, -3, 26, 38,  9, -5,  0 },
  {  1, -2, -4, 24, 39, 11, -5,  0 },
  {  0, -1, -4, 21, 40, 13, -5,  0 },
  {  0, -1, -5, 19, 41, 15, -5,  0 }
};

// 2x
const TFilterCoeff InterpolationFilter::m_lumaFilterRPR2[LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_LUMA] =
{
  { -4,  2, 20, 28, 20,  2, -4,  0 },
  { -4,  0, 19, 29, 21,  5, -4, -2 },
  { -4, -1, 18, 29, 22,  6, -4, -2 },
  { -4, -1, 16, 29, 23,  7, -4, -2 },
  { -4, -1, 16, 28, 24,  7, -4, -2 },
  { -4, -1, 14, 28, 25,  8, -4, -2 },
  { -3, -3, 14, 27, 26,  9, -3, -3 },
  { -3, -1, 12, 28, 25, 10, -4, -3 },
  { -3, -3, 11, 27, 27, 11, -3, -3 },
  { -3, -4, 10, 25, 28, 12, -1, -3 },
  { -3, -3,  9, 26, 27, 14, -3, -3 },
  { -2, -4,  8, 25, 28, 14, -1, -4 },
  { -2, -4,  7, 24, 28, 16, -1, -4 },
  { -2, -4,  7, 23, 29, 16, -1, -4 },
  { -2, -4,  6, 22, 29, 18, -1, -4 },
  { -2, -4,  5, 21, 29, 19,  0, -4 }
};
#endif

const TFilterCoeff InterpolationFilter::m_lumaAltHpelIFilter[NTAPS_LUMA] = {  0, 3, 9, 20, 20, 9, 3, 0 };
const TFilterCoeff InterpolationFilter::m_chromaFilter[CHROMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_CHROMA] =
{
  {  0, 64,  0,  0 },
  { -1, 63,  2,  0 },
  { -2, 62,  4,  0 },
  { -2, 60,  7, -1 },
  { -2, 58, 10, -2 },
  { -3, 57, 12, -2 },
  { -4, 56, 14, -2 },
  { -4, 55, 15, -2 },
  { -4, 54, 16, -2 },
  { -5, 53, 18, -2 },
  { -6, 52, 20, -2 },
  { -6, 49, 24, -3 },
  { -6, 46, 28, -4 },
  { -5, 44, 29, -4 },
  { -4, 42, 30, -4 },
  { -4, 39, 33, -4 },
  { -4, 36, 36, -4 },
  { -4, 33, 39, -4 },
  { -4, 30, 42, -4 },
  { -4, 29, 44, -5 },
  { -4, 28, 46, -6 },
  { -3, 24, 49, -6 },
  { -2, 20, 52, -6 },
  { -2, 18, 53, -5 },
  { -2, 16, 54, -4 },
  { -2, 15, 55, -4 },
  { -2, 14, 56, -4 },
  { -2, 12, 57, -3 },
  { -2, 10, 58, -2 },
  { -1,  7, 60, -2 },
  {  0,  4, 62, -2 },
  {  0,  2, 63, -1 },
};

#if JVET_P0088_P0353_RPR_FILTERS
//1.5x
const TFilterCoeff InterpolationFilter::m_chromaFilterRPR1[CHROMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_CHROMA] =
{
  { 12, 40, 12,  0 },
  { 11, 40, 13,  0 },
  { 10, 40, 15, -1 },
  {  9, 40, 16, -1 },
  {  8, 40, 17, -1 },
  {  8, 39, 18, -1 },
  {  7, 39, 19, -1 },
  {  6, 38, 21, -1 },
  {  5, 38, 22, -1 },
  {  4, 38, 23, -1 },
  {  4, 37, 24, -1 },
  {  3, 36, 25,  0 },
  {  3, 35, 26,  0 },
  {  2, 34, 28,  0 },
  {  2, 33, 29,  0 },
  {  1, 33, 30,  0 },
  {  1, 31, 31,  1 },
  {  0, 30, 33,  1 },
  {  0, 29, 33,  2 },
  {  0, 28, 34,  2 },
  {  0, 26, 35,  3 },
  {  0, 25, 36,  3 },
  { -1, 24, 37,  4 },
  { -1, 23, 38,  4 },
  { -1, 22, 38,  5 },
  { -1, 21, 38,  6 },
  { -1, 19, 39,  7 },
  { -1, 18, 39,  8 },
  { -1, 17, 40,  8 },
  { -1, 16, 40,  9 },
  { -1, 15, 40, 10 },
  {  0, 13, 40, 11 },
};

//2x
const TFilterCoeff InterpolationFilter::m_chromaFilterRPR2[CHROMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_CHROMA] =
{
  { 17, 30, 17,  0 },
  { 17, 30, 18, -1 },
  { 16, 30, 18,  0 },
  { 16, 30, 18,  0 },
  { 15, 30, 18,  1 },
  { 14, 30, 18,  2 },
  { 13, 29, 19,  3 },
  { 13, 29, 19,  3 },
  { 12, 29, 20,  3 },
  { 11, 28, 21,  4 },
  { 10, 28, 22,  4 },
  { 10, 27, 22,  5 },
  {  9, 27, 23,  5 },
  {  9, 26, 24,  5 },
  {  8, 26, 24,  6 },
  {  7, 26, 25,  6 },
  {  7, 25, 25,  7 },
  {  6, 25, 26,  7 },
  {  6, 24, 26,  8 },
  {  5, 24, 26,  9 },
  {  5, 23, 27,  9 },
  {  5, 22, 27, 10 },
  {  4, 22, 28, 10 },
  {  4, 21, 28, 11 },
  {  3, 20, 29, 12 },
  {  3, 19, 29, 13 },
  {  3, 19, 29, 13 },
  {  2, 18, 30, 14 },
  {  1, 18, 30, 15 },
  {  0, 18, 30, 16 },
  {  0, 18, 30, 16 },
  { -1, 18, 30, 17 }
};
#endif

const TFilterCoeff InterpolationFilter::m_bilinearFilter[LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_BILINEAR] =
{
  { 64,  0, },
  { 60,  4, },
  { 56,  8, },
  { 52, 12, },
  { 48, 16, },
  { 44, 20, },
  { 40, 24, },
  { 36, 28, },
  { 32, 32, },
  { 28, 36, },
  { 24, 40, },
  { 20, 44, },
  { 16, 48, },
  { 12, 52, },
  { 8, 56, },
  { 4, 60, },
};

const TFilterCoeff InterpolationFilter::m_bilinearFilterPrec4[LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS][NTAPS_BILINEAR] =
{
  { 16,  0, },
  { 15,  1, },
  { 14,  2, },
  { 13, 3, },
  { 12, 4, },
  { 11, 5, },
  { 10, 6, },
  { 9, 7, },
  { 8, 8, },
  { 7, 9, },
  { 6, 10, },
  { 5, 11, },
  { 4, 12, },
  { 3, 13, },
  { 2, 14, },
  { 1, 15, }
};
// ====================================================================================================================
// Private member functions
// ====================================================================================================================

InterpolationFilter::InterpolationFilter()
{
  m_filterHor[0][0][0] = filter<8, false, false, false>;
  m_filterHor[0][0][1] = filter<8, false, false, true>;
  m_filterHor[0][1][0] = filter<8, false, true, false>;
  m_filterHor[0][1][1] = filter<8, false, true, true>;

  m_filterHor[1][0][0] = filter<4, false, false, false>;
  m_filterHor[1][0][1] = filter<4, false, false, true>;
  m_filterHor[1][1][0] = filter<4, false, true, false>;
  m_filterHor[1][1][1] = filter<4, false, true, true>;

  m_filterHor[2][0][0] = filter<2, false, false, false>;
  m_filterHor[2][0][1] = filter<2, false, false, true>;
  m_filterHor[2][1][0] = filter<2, false, true, false>;
  m_filterHor[2][1][1] = filter<2, false, true, true>;

  m_filterVer[0][0][0] = filter<8, true, false, false>;
  m_filterVer[0][0][1] = filter<8, true, false, true>;
  m_filterVer[0][1][0] = filter<8, true, true, false>;
  m_filterVer[0][1][1] = filter<8, true, true, true>;

  m_filterVer[1][0][0] = filter<4, true, false, false>;
  m_filterVer[1][0][1] = filter<4, true, false, true>;
  m_filterVer[1][1][0] = filter<4, true, true, false>;
  m_filterVer[1][1][1] = filter<4, true, true, true>;

  m_filterVer[2][0][0] = filter<2, true, false, false>;
  m_filterVer[2][0][1] = filter<2, true, false, true>;
  m_filterVer[2][1][0] = filter<2, true, true, false>;
  m_filterVer[2][1][1] = filter<2, true, true, true>;

  m_filterCopy[0][0]   = filterCopy<false, false>;
  m_filterCopy[0][1]   = filterCopy<false, true>;
  m_filterCopy[1][0]   = filterCopy<true, false>;
  m_filterCopy[1][1]   = filterCopy<true, true>;

  m_weightedTriangleBlk = xWeightedTriangleBlk;
}


/**
 * \brief Apply unit FIR filter to a block of samples
 *
 * \param bitDepth   bitDepth of samples
 * \param src        Pointer to source samples
 * \param srcStride  Stride of source samples
 * \param dst        Pointer to destination samples
 * \param dstStride  Stride of destination samples
 * \param width      Width of block
 * \param height     Height of block
 * \param isFirst    Flag indicating whether it is the first filtering operation
 * \param isLast     Flag indicating whether it is the last filtering operation
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// !!! NOTE !!!
//
//  This is the scalar version of the function.
//  If you change the functionality here, consider to switch off the SIMD implementation of this function.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<bool isFirst, bool isLast>
void InterpolationFilter::filterCopy( const ClpRng& clpRng, const Pel *src, int srcStride, Pel *dst, int dstStride, int width, int height, bool biMCForDMVR)
{
  int row, col;

  if ( isFirst == isLast )
  {
    for (row = 0; row < height; row++)
    {
      for (col = 0; col < width; col++)
      {
        dst[col] = src[col];
        JVET_J0090_CACHE_ACCESS( &src[col], __FILE__, __LINE__ );
      }

      src += srcStride;
      dst += dstStride;
    }
  }
  else if ( isFirst )
  {
    const int shift = std::max<int>(2, (IF_INTERNAL_PREC - clpRng.bd));

    if (biMCForDMVR)
    {
      int shift10BitOut, offset;
      if ((clpRng.bd - IF_INTERNAL_PREC_BILINEAR) > 0)
      {
        shift10BitOut = (clpRng.bd - IF_INTERNAL_PREC_BILINEAR);
        offset = (1 << (shift10BitOut - 1));
        for (row = 0; row < height; row++)
        {
          for (col = 0; col < width; col++)
          {
            dst[col] = (src[col] + offset) >> shift10BitOut;
          }
          src += srcStride;
          dst += dstStride;
        }
      }
      else
      {
        shift10BitOut = (IF_INTERNAL_PREC_BILINEAR - clpRng.bd);
        for (row = 0; row < height; row++)
        {
          for (col = 0; col < width; col++)
          {
            dst[col] = src[col] << shift10BitOut;
          }
          src += srcStride;
          dst += dstStride;
        }
      }
    }
    else
    for (row = 0; row < height; row++)
    {
      for (col = 0; col < width; col++)
      {
        Pel val = leftShift_round(src[col], shift);
        dst[col] = val - (Pel)IF_INTERNAL_OFFS;
        JVET_J0090_CACHE_ACCESS( &src[col], __FILE__, __LINE__ );
      }

      src += srcStride;
      dst += dstStride;
    }
  }
  else
  {
    const int shift = std::max<int>(2, (IF_INTERNAL_PREC - clpRng.bd));

    if (biMCForDMVR)
    {
      int shift10BitOut, offset;
      if ((clpRng.bd - IF_INTERNAL_PREC_BILINEAR) > 0)
      {
        shift10BitOut = (clpRng.bd - IF_INTERNAL_PREC_BILINEAR);
        offset = (1 << (shift10BitOut - 1));
        for (row = 0; row < height; row++)
        {
          for (col = 0; col < width; col++)
          {
            dst[col] = (src[col] + offset) >> shift10BitOut;
          }
          src += srcStride;
          dst += dstStride;
        }
      }
      else
      {
        shift10BitOut = (IF_INTERNAL_PREC_BILINEAR - clpRng.bd);
        for (row = 0; row < height; row++)
        {
          for (col = 0; col < width; col++)
          {
            dst[col] = src[col] << shift10BitOut;
          }
          src += srcStride;
          dst += dstStride;
        }
      }
    }
    else
    for (row = 0; row < height; row++)
    {
      for (col = 0; col < width; col++)
      {
        Pel val = src[ col ];
        val = rightShift_round((val + IF_INTERNAL_OFFS), shift);

        dst[col] = ClipPel( val, clpRng );
        JVET_J0090_CACHE_ACCESS( &src[col], __FILE__, __LINE__ );
      }

      src += srcStride;
      dst += dstStride;
    }
  }
}

/**
 * \brief Apply FIR filter to a block of samples
 *
 * \tparam N          Number of taps
 * \tparam isVertical Flag indicating filtering along vertical direction
 * \tparam isFirst    Flag indicating whether it is the first filtering operation
 * \tparam isLast     Flag indicating whether it is the last filtering operation
 * \param  bitDepth   Bit depth of samples
 * \param  src        Pointer to source samples
 * \param  srcStride  Stride of source samples
 * \param  dst        Pointer to destination samples
 * \param  dstStride  Stride of destination samples
 * \param  width      Width of block
 * \param  height     Height of block
 * \param  coeff      Pointer to filter taps
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// !!! NOTE !!!
//
//  This is the scalar version of the function.
//  If you change the functionality here, consider to switch off the SIMD implementation of this function.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<int N, bool isVertical, bool isFirst, bool isLast>
void InterpolationFilter::filter(const ClpRng& clpRng, Pel const *src, int srcStride, Pel *dst, int dstStride, int width, int height, TFilterCoeff const *coeff, bool biMCForDMVR)
{
  int row, col;

  Pel c[8];
  c[0] = coeff[0];
  c[1] = coeff[1];
  if ( N >= 4 )
  {
    c[2] = coeff[2];
    c[3] = coeff[3];
  }
  if ( N >= 6 )
  {
    c[4] = coeff[4];
    c[5] = coeff[5];
  }
  if ( N == 8 )
  {
    c[6] = coeff[6];
    c[7] = coeff[7];
  }

  int cStride = ( isVertical ) ? srcStride : 1;
  src -= ( N/2 - 1 ) * cStride;

  int offset;
  int headRoom = std::max<int>(2, (IF_INTERNAL_PREC - clpRng.bd));
  int shift    = IF_FILTER_PREC;
  // with the current settings (IF_INTERNAL_PREC = 14 and IF_FILTER_PREC = 6), though headroom can be
  // negative for bit depths greater than 14, shift will remain non-negative for bit depths of 8->20
  CHECK(shift < 0, "Negative shift");

  if ( isLast )
  {
    shift += (isFirst) ? 0 : headRoom;
    offset = 1 << (shift - 1);
    offset += (isFirst) ? 0 : IF_INTERNAL_OFFS << IF_FILTER_PREC;
  }
  else
  {
    shift -= (isFirst) ? headRoom : 0;
    offset = (isFirst) ? -IF_INTERNAL_OFFS << shift : 0;
  }

  if (biMCForDMVR)
  {
    if( isFirst )
    {
      shift = IF_FILTER_PREC_BILINEAR - (IF_INTERNAL_PREC_BILINEAR - clpRng.bd);
      offset = 1 << (shift - 1);
    }
    else
    {
      shift = 4;
      offset = 1 << (shift - 1);
    }
  }
  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      int sum;

      sum  = src[ col + 0 * cStride] * c[0];
      sum += src[ col + 1 * cStride] * c[1];
      JVET_J0090_CACHE_ACCESS( &src[ col + 0 * cStride], __FILE__, __LINE__ );
      JVET_J0090_CACHE_ACCESS( &src[ col + 1 * cStride], __FILE__, __LINE__ );
      if ( N >= 4 )
      {
        sum += src[ col + 2 * cStride] * c[2];
        sum += src[ col + 3 * cStride] * c[3];
        JVET_J0090_CACHE_ACCESS( &src[ col + 2 * cStride], __FILE__, __LINE__ );
        JVET_J0090_CACHE_ACCESS( &src[ col + 3 * cStride], __FILE__, __LINE__ );
      }
      if ( N >= 6 )
      {
        sum += src[ col + 4 * cStride] * c[4];
        sum += src[ col + 5 * cStride] * c[5];
        JVET_J0090_CACHE_ACCESS( &src[ col + 4 * cStride], __FILE__, __LINE__ );
        JVET_J0090_CACHE_ACCESS( &src[ col + 5 * cStride], __FILE__, __LINE__ );
      }
      if ( N == 8 )
      {
        sum += src[ col + 6 * cStride] * c[6];
        sum += src[ col + 7 * cStride] * c[7];
        JVET_J0090_CACHE_ACCESS( &src[ col + 6 * cStride], __FILE__, __LINE__ );
        JVET_J0090_CACHE_ACCESS( &src[ col + 7 * cStride], __FILE__, __LINE__ );
      }

      Pel val = ( sum + offset ) >> shift;
      if ( isLast )
      {
        val = ClipPel( val, clpRng );
      }
      dst[col] = val;
    }

    src += srcStride;
    dst += dstStride;
  }
}

/**
 * \brief Filter a block of samples (horizontal)
 *
 * \tparam N          Number of taps
 * \param  bitDepth   Bit depth of samples
 * \param  src        Pointer to source samples
 * \param  srcStride  Stride of source samples
 * \param  dst        Pointer to destination samples
 * \param  dstStride  Stride of destination samples
 * \param  width      Width of block
 * \param  height     Height of block
 * \param  isLast     Flag indicating whether it is the last filtering operation
 * \param  coeff      Pointer to filter taps
 */
template<int N>
void InterpolationFilter::filterHor(const ClpRng& clpRng, Pel const *src, int srcStride, Pel *dst, int dstStride, int width, int height, bool isLast, TFilterCoeff const *coeff, bool biMCForDMVR)
{
//#if ENABLE_SIMD_OPT_MCIF
  if( N == 8 )
  {
    m_filterHor[0][1][isLast](clpRng, src, srcStride, dst, dstStride, width, height, coeff, biMCForDMVR);
  }
  else if( N == 4 )
  {
    m_filterHor[1][1][isLast](clpRng, src, srcStride, dst, dstStride, width, height, coeff, biMCForDMVR);
  }
  else if( N == 2 )
  {
    m_filterHor[2][1][isLast](clpRng, src, srcStride, dst, dstStride, width, height, coeff, biMCForDMVR);
  }
  else
  {
    THROW( "Invalid tap number" );
  }
}

/**
 * \brief Filter a block of samples (vertical)
 *
 * \tparam N          Number of taps
 * \param  bitDepth   Bit depth
 * \param  src        Pointer to source samples
 * \param  srcStride  Stride of source samples
 * \param  dst        Pointer to destination samples
 * \param  dstStride  Stride of destination samples
 * \param  width      Width of block
 * \param  height     Height of block
 * \param  isFirst    Flag indicating whether it is the first filtering operation
 * \param  isLast     Flag indicating whether it is the last filtering operation
 * \param  coeff      Pointer to filter taps
 */
template<int N>
void InterpolationFilter::filterVer(const ClpRng& clpRng, Pel const *src, int srcStride, Pel *dst, int dstStride, int width, int height, bool isFirst, bool isLast, TFilterCoeff const *coeff, bool biMCForDMVR)
{
//#if ENABLE_SIMD_OPT_MCIF
  if( N == 8 )
  {
    m_filterVer[0][isFirst][isLast]( clpRng, src, srcStride, dst, dstStride, width, height, coeff, biMCForDMVR);
  }
  else if( N == 4 )
  {
    m_filterVer[1][isFirst][isLast]( clpRng, src, srcStride, dst, dstStride, width, height, coeff, biMCForDMVR);
  }
  else if( N == 2 )
  {
    m_filterVer[2][isFirst][isLast]( clpRng, src, srcStride, dst, dstStride, width, height, coeff, biMCForDMVR);
  }
  else{
    THROW( "Invalid tap number" );
  }
}

// ====================================================================================================================
// Public member functions
// ====================================================================================================================

/**
 * \brief Filter a block of Luma/Chroma samples (horizontal)
 *
 * \param  compID     Chroma component ID
 * \param  src        Pointer to source samples
 * \param  srcStride  Stride of source samples
 * \param  dst        Pointer to destination samples
 * \param  dstStride  Stride of destination samples
 * \param  width      Width of block
 * \param  height     Height of block
 * \param  frac       Fractional sample offset
 * \param  isLast     Flag indicating whether it is the last filtering operation
 * \param  fmt        Chroma format
 * \param  bitDepth   Bit depth
 */
void InterpolationFilter::filterHor(const ComponentID compID, Pel const *src, int srcStride, Pel *dst, int dstStride, int width, int height, int frac, bool isLast, const ChromaFormat fmt, const ClpRng& clpRng, int nFilterIdx, bool biMCForDMVR, bool useAltHpelIf)
{
#if JVET_P0088_P0353_RPR_FILTERS
  if( frac == 0 && nFilterIdx < 2 )
#else
  if( frac == 0 )
#endif
  {
    m_filterCopy[true][isLast]( clpRng, src, srcStride, dst, dstStride, width, height, biMCForDMVR );
  }
  else if( isLuma( compID ) )
  {
    CHECK( frac < 0 || frac >= LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS, "Invalid fraction" );
    if( nFilterIdx == 1 )
    {
      filterHor<NTAPS_BILINEAR>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_bilinearFilterPrec4[frac], biMCForDMVR );
    }
    else if( nFilterIdx == 2 )
    {
      filterHor<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_lumaFilter4x4[frac], biMCForDMVR );
    }
#if JVET_P0088_P0353_RPR_FILTERS
    else if( nFilterIdx == 3 )
    {
      filterHor<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_lumaFilterRPR1[frac], biMCForDMVR );
    }
    else if( nFilterIdx == 4 )
    {
      filterHor<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_lumaFilterRPR2[frac], biMCForDMVR );
    }
#endif
    else if( frac == 8 && useAltHpelIf )
    {
      filterHor<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_lumaAltHpelIFilter, biMCForDMVR );
    }
    else if( ( width == 4 && height == 4 ) || ( width == 4 && height == ( 4 + NTAPS_LUMA - 1 ) ) )
    {
      filterHor<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_lumaFilter4x4[frac], biMCForDMVR );
    }
    else
    {
      filterHor<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_lumaFilter[frac], biMCForDMVR );

    }
  }
  else
  {
    const uint32_t csx = getComponentScaleX( compID, fmt );
    CHECK( frac < 0 || csx >= 2 || ( frac << ( 1 - csx ) ) >= CHROMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS, "Invalid fraction" );
#if JVET_P0088_P0353_RPR_FILTERS
    if( nFilterIdx == 3 )
    {
      filterHor<NTAPS_CHROMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_chromaFilterRPR1[frac << ( 1 - csx )], biMCForDMVR );
    }
    else if( nFilterIdx == 4 )
    {
      filterHor<NTAPS_CHROMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_chromaFilterRPR2[frac << ( 1 - csx )], biMCForDMVR );
    }
    else
    {
      filterHor<NTAPS_CHROMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_chromaFilter[frac << ( 1 - csx )], biMCForDMVR );
    }
#else
    filterHor<NTAPS_CHROMA>( clpRng, src, srcStride, dst, dstStride, width, height, isLast, m_chromaFilter[frac << ( 1 - csx )], biMCForDMVR );
#endif
  }
}


/**
 * \brief Filter a block of Luma/Chroma samples (vertical)
 *
 * \param  compID     Colour component ID
 * \param  src        Pointer to source samples
 * \param  srcStride  Stride of source samples
 * \param  dst        Pointer to destination samples
 * \param  dstStride  Stride of destination samples
 * \param  width      Width of block
 * \param  height     Height of block
 * \param  frac       Fractional sample offset
 * \param  isFirst    Flag indicating whether it is the first filtering operation
 * \param  isLast     Flag indicating whether it is the last filtering operation
 * \param  fmt        Chroma format
 * \param  bitDepth   Bit depth
 */
void InterpolationFilter::filterVer(const ComponentID compID, Pel const *src, int srcStride, Pel *dst, int dstStride, int width, int height, int frac, bool isFirst, bool isLast, const ChromaFormat fmt, const ClpRng& clpRng, int nFilterIdx, bool biMCForDMVR, bool useAltHpelIf)
{
#if JVET_P0088_P0353_RPR_FILTERS
  if( frac == 0 && nFilterIdx < 2 )
#else
  if( frac == 0 )
#endif
  {
    m_filterCopy[isFirst][isLast]( clpRng, src, srcStride, dst, dstStride, width, height, biMCForDMVR );
  }
  else if( isLuma( compID ) )
  {
    CHECK( frac < 0 || frac >= LUMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS, "Invalid fraction" );
    if( nFilterIdx == 1 )
    {
      filterVer<NTAPS_BILINEAR>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_bilinearFilterPrec4[frac], biMCForDMVR );
    }
    else if( nFilterIdx == 2 )
    {
      filterVer<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_lumaFilter4x4[frac], biMCForDMVR );
    }
#if JVET_P0088_P0353_RPR_FILTERS
    else if( nFilterIdx == 3 )
    {
      filterVer<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_lumaFilterRPR1[frac], biMCForDMVR );
    }
    else if( nFilterIdx == 4 )
    {
      filterVer<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_lumaFilterRPR2[frac], biMCForDMVR );
    }
#endif
    else if( frac == 8 && useAltHpelIf )
    {
      filterVer<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_lumaAltHpelIFilter, biMCForDMVR );
    }
    else if( width == 4 && height == 4 )
    {
      filterVer<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_lumaFilter4x4[frac], biMCForDMVR );
    }
    else
    {
      filterVer<NTAPS_LUMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_lumaFilter[frac], biMCForDMVR );
    }
  }
  else
  {
    const uint32_t csy = getComponentScaleY( compID, fmt );
    CHECK( frac < 0 || csy >= 2 || ( frac << ( 1 - csy ) ) >= CHROMA_INTERPOLATION_FILTER_SUB_SAMPLE_POSITIONS, "Invalid fraction" );
#if JVET_P0088_P0353_RPR_FILTERS
    if( nFilterIdx == 3 )
    {
      filterVer<NTAPS_CHROMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_chromaFilterRPR1[frac << ( 1 - csy )], biMCForDMVR );
    }
    else if( nFilterIdx == 4 )
    {
      filterVer<NTAPS_CHROMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_chromaFilterRPR2[frac << ( 1 - csy )], biMCForDMVR );
    }
    else
    {
      filterVer<NTAPS_CHROMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_chromaFilter[frac << ( 1 - csy )], biMCForDMVR );
    }
#else
    filterVer<NTAPS_CHROMA>( clpRng, src, srcStride, dst, dstStride, width, height, isFirst, isLast, m_chromaFilter[frac << ( 1 - csy )], biMCForDMVR );
#endif
  }
}

void InterpolationFilter::xWeightedTriangleBlk( const PredictionUnit &pu, const uint32_t width, const uint32_t height, const ComponentID compIdx, const bool splitDir, PelUnitBuf& predDst, PelUnitBuf& predSrc0, PelUnitBuf& predSrc1 )
{
  Pel*    dst        = predDst .get(compIdx).buf;
  Pel*    src0       = predSrc0.get(compIdx).buf;
  Pel*    src1       = predSrc1.get(compIdx).buf;
  int32_t strideDst  = predDst .get(compIdx).stride  - width;
  int32_t strideSrc0 = predSrc0.get(compIdx).stride  - width;
  int32_t strideSrc1 = predSrc1.get(compIdx).stride  - width;

  const char    log2WeightBase    = 3;
  const ClpRng  clipRng           = pu.cu->slice->clpRngs().comp[compIdx];
  const int32_t clipbd            = clipRng.bd;
  const int32_t shiftDefault      = std::max<int>(2, (IF_INTERNAL_PREC - clipbd));
  const int32_t offsetDefault     = (1<<(shiftDefault-1)) + IF_INTERNAL_OFFS;
  const int32_t shiftWeighted     = std::max<int>(2, (IF_INTERNAL_PREC - clipbd)) + log2WeightBase;
  const int32_t offsetWeighted    = (1 << (shiftWeighted - 1)) + (IF_INTERNAL_OFFS << log2WeightBase);
#if JVET_P0530_TPM_WEIGHT_ALIGN
  int32_t stepX = 1 << getComponentScaleX(compIdx, pu.chromaFormat);
  int32_t stepY = 1 << getComponentScaleY(compIdx, pu.chromaFormat);

  int32_t widthY = width << getComponentScaleX(compIdx, pu.chromaFormat);
  int32_t heightY = height << getComponentScaleY(compIdx, pu.chromaFormat);

  int32_t ratioWH = (widthY > heightY) ? (widthY / heightY) : 1;
  int32_t ratioHW = (widthY > heightY) ? 1 : (heightY / widthY);

  int32_t weightedLength = 7;
  int32_t weightedStartPos = (splitDir == 0) ? (0 - (weightedLength >> 1) * ratioWH) : (widthY - ((weightedLength + 1) >> 1) * ratioWH);
#else
  const int32_t ratioWH           = (width > height) ? (width / height) : 1;
  const int32_t ratioHW           = (width > height) ? 1 : (height / width);

  const bool    longWeight        = (compIdx == COMPONENT_Y);
  const int32_t weightedLength    = longWeight ? 7 : 3;
        int32_t weightedStartPos  = ( splitDir == 0 ) ? ( 0 - (weightedLength >> 1) * ratioWH ) : ( width - ((weightedLength + 1) >> 1) * ratioWH );
#endif       
        int32_t weightedEndPos    = weightedStartPos + weightedLength * ratioWH - 1;
        int32_t weightedPosoffset = ( splitDir == 0 ) ? ratioWH : -ratioWH;

        Pel     tmpPelWeighted;
        int32_t weightIdx;
        int32_t x, y, tmpX, tmpY, tmpWeightedStart, tmpWeightedEnd;
#if JVET_P0530_TPM_WEIGHT_ALIGN
  for (y = 0; y < heightY; y += ratioHW)
  {
    if (y % stepY != 0)
    {
      weightedStartPos += weightedPosoffset;
      weightedEndPos += weightedPosoffset;
      continue;
    }
    for (tmpY = ratioHW; tmpY > 0; tmpY -= stepY)
    {
      for (x = 0; x < weightedStartPos; x += stepX)
      {
#else
  for( y = 0; y < height; y+= ratioHW )
  {
    for( tmpY = ratioHW; tmpY > 0; tmpY-- )
    {
      for( x = 0; x < weightedStartPos; x++ )
      {
#endif
        *dst++ = ClipPel( rightShift( (splitDir == 0 ? *src1 : *src0) + offsetDefault, shiftDefault), clipRng );
        src0++;
        src1++;
      }

      tmpWeightedStart = std::max((int32_t)0, weightedStartPos);
#if JVET_P0530_TPM_WEIGHT_ALIGN
      tmpWeightedEnd = std::min(weightedEndPos, (int32_t)(widthY - 1));
#else
      tmpWeightedEnd   = std::min(weightedEndPos, (int32_t)(width - 1));
#endif
      weightIdx        = 1;
      if( weightedStartPos < 0 )
      {
        weightIdx     += abs(weightedStartPos) / ratioWH;
      }
      for( x = tmpWeightedStart; x <= tmpWeightedEnd; x+= ratioWH )
      {
#if JVET_P0530_TPM_WEIGHT_ALIGN
        if (x % stepX != 0)
        {
          weightIdx++;
          continue;
        }

        for (tmpX = ratioWH; tmpX > 0; tmpX -= stepX)
        {
          tmpPelWeighted = Clip3(1, 7, weightIdx);
#else
        for( tmpX = ratioWH; tmpX > 0; tmpX-- )
        {
          tmpPelWeighted = Clip3( 1, 7, longWeight ? weightIdx : (weightIdx * 2));
#endif
          tmpPelWeighted = splitDir ? ( 8 - tmpPelWeighted ) : tmpPelWeighted;
          *dst++         = ClipPel( rightShift( (tmpPelWeighted*(*src0++) + ((8 - tmpPelWeighted) * (*src1++)) + offsetWeighted), shiftWeighted ), clipRng );
        }
        weightIdx ++;
      }

#if JVET_P0530_TPM_WEIGHT_ALIGN
      int32_t start = ((weightedEndPos + 1) % stepX != 0) ? (weightedEndPos + 2) : (weightedEndPos + 1);
      for (x = start; x < widthY; x += stepX)
      {
#else
      for( x = weightedEndPos + 1; x < width; x++ )
      {
#endif
        *dst++ = ClipPel( rightShift( (splitDir == 0 ? *src0 : *src1) + offsetDefault, shiftDefault ), clipRng );
        src0++;
        src1++;
      }

      dst  += strideDst;
      src0 += strideSrc0;
      src1 += strideSrc1;
    }
    weightedStartPos += weightedPosoffset;
    weightedEndPos   += weightedPosoffset;
  }
}

void InterpolationFilter::weightedTriangleBlk(const PredictionUnit &pu, const uint32_t width, const uint32_t height, const ComponentID compIdx, const bool splitDir, PelUnitBuf& predDst, PelUnitBuf& predSrc0, PelUnitBuf& predSrc1)
{
  m_weightedTriangleBlk(pu, width, height, compIdx, splitDir, predDst, predSrc0, predSrc1);
}

/**
 * \brief turn on SIMD fuc
 *
 * \param bEn   enabled of SIMD function for interpolation
 */
void InterpolationFilter::initInterpolationFilter( bool enable )
{
#if ENABLE_SIMD_OPT_MCIF
#ifdef TARGET_SIMD_X86
  if ( enable )
  {
    initInterpolationFilterX86();
  }
#endif
#endif
}

//! \}
