/*************************************************************************
 *
 *  $RCSfile: convertiso2022jp.c,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: rt $ $Date: 2004-06-17 11:41:18 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef INCLUDED_RTL_TEXTENC_CONVERTISO2022JP_H
#include "convertiso2022jp.h"
#endif

#ifndef INCLUDED_RTL_TEXTENC_CONTEXT_H
#include "context.h"
#endif
#ifndef INCLUDED_RTL_TEXTENC_CONVERTER_H
#include "converter.h"
#endif
#ifndef INCLUDED_RTL_TEXTENC_TENCHELP_H
#include "tenchelp.h"
#endif
#ifndef INCLUDED_RTL_TEXTENC_UNICHARS_H
#include "unichars.h"
#endif

#ifndef _RTL_ALLOC_H_
#include "rtl/alloc.h"
#endif
#ifndef _RTL_TEXTCVT_H
#include "rtl/textcvt.h"
#endif
#ifndef _SAL_TYPES_H_
#include "sal/types.h"
#endif

typedef enum /* order is important: */
{
    IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII,
    IMPL_ISO_2022_JP_TO_UNICODE_STATE_JIS_ROMAN,
    IMPL_ISO_2022_JP_TO_UNICODE_STATE_0208,
    IMPL_ISO_2022_JP_TO_UNICODE_STATE_0208_2,
    IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC,
    IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC_LPAREN,
    IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC_DOLLAR
} ImplIso2022JpToUnicodeState;

typedef struct
{
    ImplIso2022JpToUnicodeState m_eState;
    sal_uInt32 m_nRow;
} ImplIso2022JpToUnicodeContext;

typedef enum
{
    IMPL_UNICODE_TO_ISO_2022_JP_SET_NONE,
    IMPL_UNICODE_TO_ISO_2022_JP_SET_ASCII,
    IMPL_UNICODE_TO_ISO_2022_JP_SET_0208
} ImplUnicodeToIso2022JpSet;

typedef struct
{
    sal_Unicode m_nHighSurrogate;
    ImplUnicodeToIso2022JpSet m_eSet;
} ImplUnicodeToIso2022JpContext;

void * ImplCreateIso2022JpToUnicodeContext(void)
{
    void * pContext
        = rtl_allocateMemory(sizeof (ImplIso2022JpToUnicodeContext));
    ((ImplIso2022JpToUnicodeContext *) pContext)->m_eState
        = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII;
    return pContext;
}

void ImplResetIso2022JpToUnicodeContext(void * pContext)
{
    if (pContext)
        ((ImplIso2022JpToUnicodeContext *) pContext)->m_eState
            = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII;
}

sal_Size ImplConvertIso2022JpToUnicode(ImplTextConverterData const * pData,
                                       void * pContext,
                                       sal_Char const * pSrcBuf,
                                       sal_Size nSrcBytes,
                                       sal_Unicode * pDestBuf,
                                       sal_Size nDestChars,
                                       sal_uInt32 nFlags,
                                       sal_uInt32 * pInfo,
                                       sal_Size * pSrcCvtBytes)
{
    ImplDBCSToUniLeadTab const * pJisX0208Data
        = ((ImplIso2022JpConverterData const *) pData)->
              m_pJisX0208ToUnicodeData;
    ImplIso2022JpToUnicodeState eState
        = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII;
    sal_uInt32 nRow = 0;
    sal_uInt32 nInfo = 0;
    sal_Size nConverted = 0;
    sal_Unicode * pDestBufPtr = pDestBuf;
    sal_Unicode * pDestBufEnd = pDestBuf + nDestChars;

    if (pContext)
    {
        eState = ((ImplIso2022JpToUnicodeContext *) pContext)->m_eState;
        nRow = ((ImplIso2022JpToUnicodeContext *) pContext)->m_nRow;
    }

    for (; nConverted < nSrcBytes; ++nConverted)
    {
        sal_Bool bUndefined = sal_True;
        sal_uInt32 nChar = *((sal_uChar const *) pSrcBuf)++;
        switch (eState)
        {
        case IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII:
            if (nChar == 0x1B) /* ESC */
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC;
            else if (nChar < 0x80)
                if (pDestBufPtr != pDestBufEnd)
                    *pDestBufPtr++ = (sal_Unicode) nChar;
                else
                    goto no_output;
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_ISO_2022_JP_TO_UNICODE_STATE_JIS_ROMAN:
            if (nChar == 0x1B) /* ESC */
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC;
            else if (nChar < 0x80)
                if (pDestBufPtr != pDestBufEnd)
                {
                    switch (nChar)
                    {
                    case 0x5C: /* \ */
                        nChar = 0xA5; /* YEN SIGN */
                        break;

                    case 0x7E: /* ~ */
                        nChar = 0xAF; /* MACRON */
                        break;
                    }
                    *pDestBufPtr++ = (sal_Unicode) nChar;
                }
                else
                    goto no_output;
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_ISO_2022_JP_TO_UNICODE_STATE_0208:
            if (nChar == 0x1B) /* ESC */
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC;
            else if (nChar >= 0x21 && nChar <= 0x7E)
            {
                nRow = nChar;
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_0208_2;
            }
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_ISO_2022_JP_TO_UNICODE_STATE_0208_2:
            if (nChar >= 0x21 && nChar <= 0x7E)
            {
                sal_uInt16 nUnicode = 0;
                sal_uInt32 nFirst = pJisX0208Data[nRow].mnTrailStart;
                if (nChar >= nFirst
                    && nChar <= pJisX0208Data[nRow].mnTrailEnd)
                    nUnicode = pJisX0208Data[nRow].
                                   mpToUniTrailTab[nChar - nFirst];
                if (nUnicode != 0)
                    if (pDestBufPtr != pDestBufEnd)
                    {
                        *pDestBufPtr++ = (sal_Unicode) nUnicode;
                        eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_0208;
                    }
                    else
                        goto no_output;
                else
                    goto bad_input;
            }
            else
            {
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC:
            switch (nChar)
            {
            case 0x24: /* $ */
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC_DOLLAR;
                break;

            case 0x28: /* ( */
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC_LPAREN;
                break;

            default:
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC_LPAREN:
            switch (nChar)
            {
            case 0x42: /* A */
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII;
                break;

            case 0x4A: /* J */
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_JIS_ROMAN;
                break;

            default:
                bUndefined = sal_False;
                goto bad_input;
            }
            break;

        case IMPL_ISO_2022_JP_TO_UNICODE_STATE_ESC_DOLLAR:
            switch (nChar)
            {
            case 0x40: /* @ */
            case 0x42: /* B */
                eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_0208;
                break;

            default:
                bUndefined = sal_False;
                goto bad_input;
            }
            break;
        }
        continue;

    bad_input:
        switch (ImplHandleBadInputTextToUnicodeConversion(
                    bUndefined, sal_True, 0, nFlags, &pDestBufPtr, pDestBufEnd,
                    &nInfo))
        {
        case IMPL_BAD_INPUT_STOP:
            eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII;
            break;

        case IMPL_BAD_INPUT_CONTINUE:
            eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII;
            continue;

        case IMPL_BAD_INPUT_NO_OUTPUT:
            goto no_output;
        }
        break;

    no_output:
        --pSrcBuf;
        nInfo |= RTL_TEXTTOUNICODE_INFO_DESTBUFFERTOSMALL;
        break;
    }

    if (eState > IMPL_ISO_2022_JP_TO_UNICODE_STATE_0208
        && (nInfo & (RTL_TEXTTOUNICODE_INFO_ERROR
                         | RTL_TEXTTOUNICODE_INFO_DESTBUFFERTOSMALL))
               == 0)
        if ((nFlags & RTL_TEXTTOUNICODE_FLAGS_FLUSH) == 0)
            nInfo |= RTL_TEXTTOUNICODE_INFO_SRCBUFFERTOSMALL;
    else
        switch (ImplHandleBadInputTextToUnicodeConversion(
                    sal_False, sal_True, 0, nFlags, &pDestBufPtr, pDestBufEnd,
                    &nInfo))
        {
        case IMPL_BAD_INPUT_STOP:
        case IMPL_BAD_INPUT_CONTINUE:
            eState = IMPL_ISO_2022_JP_TO_UNICODE_STATE_ASCII;
            break;

        case IMPL_BAD_INPUT_NO_OUTPUT:
            nInfo |= RTL_TEXTTOUNICODE_INFO_DESTBUFFERTOSMALL;
            break;
        }

    if (pContext)
    {
        ((ImplIso2022JpToUnicodeContext *) pContext)->m_eState = eState;
        ((ImplIso2022JpToUnicodeContext *) pContext)->m_nRow = nRow;
    }
    if (pInfo)
        *pInfo = nInfo;
    if (pSrcCvtBytes)
        *pSrcCvtBytes = nConverted;

    return pDestBufPtr - pDestBuf;
}

void * ImplCreateUnicodeToIso2022JpContext(void)
{
    void * pContext
        = rtl_allocateMemory(sizeof (ImplUnicodeToIso2022JpContext));
    ((ImplUnicodeToIso2022JpContext *) pContext)->m_nHighSurrogate = 0;
    ((ImplUnicodeToIso2022JpContext *) pContext)->m_eSet
        = IMPL_UNICODE_TO_ISO_2022_JP_SET_NONE;
    return pContext;
}

void ImplResetUnicodeToIso2022JpContext(void * pContext)
{
    if (pContext)
    {
        ((ImplUnicodeToIso2022JpContext *) pContext)->m_nHighSurrogate = 0;
        ((ImplUnicodeToIso2022JpContext *) pContext)->m_eSet
            = IMPL_UNICODE_TO_ISO_2022_JP_SET_NONE;
    }
}

sal_Size ImplConvertUnicodeToIso2022Jp(ImplTextConverterData const * pData,
                                       void * pContext,
                                       sal_Unicode const * pSrcBuf,
                                       sal_Size nSrcChars,
                                       sal_Char * pDestBuf,
                                       sal_Size nDestBytes,
                                       sal_uInt32 nFlags,
                                       sal_uInt32 * pInfo,
                                       sal_Size * pSrcCvtChars)
{
    ImplUniToDBCSHighTab const * pJisX0208Data
        = ((ImplIso2022JpConverterData const *) pData)->
              m_pUnicodeToJisX0208Data;
    sal_Unicode nHighSurrogate = 0;
    ImplUnicodeToIso2022JpSet eSet = IMPL_UNICODE_TO_ISO_2022_JP_SET_NONE;
    sal_uInt32 nInfo = 0;
    sal_Size nConverted = 0;
    sal_Char * pDestBufPtr = pDestBuf;
    sal_Char * pDestBufEnd = pDestBuf + nDestBytes;
    sal_Bool bWritten;

    if (pContext)
    {
        nHighSurrogate
            = ((ImplUnicodeToIso2022JpContext *) pContext)->m_nHighSurrogate;
        eSet = ((ImplUnicodeToIso2022JpContext *) pContext)->m_eSet;
    }

    for (; nConverted < nSrcChars; ++nConverted)
    {
        sal_Bool bUndefined = sal_True;
        sal_uInt32 nChar = *pSrcBuf++;
        if (nHighSurrogate == 0)
        {
            if (ImplIsHighSurrogate(nChar))
            {
                nHighSurrogate = (sal_Unicode) nChar;
                continue;
            }
        }
        else if (ImplIsLowSurrogate(nChar))
            nChar = ImplCombineSurrogates(nHighSurrogate, nChar);
        else
        {
            bUndefined = sal_False;
            goto bad_input;
        }

        if (ImplIsLowSurrogate(nChar) || ImplIsNoncharacter(nChar))
        {
            bUndefined = sal_False;
            goto bad_input;
        }

        if (nChar == 0x0A || nChar == 0x0D) /* LF, CR */
        {
            if (eSet == IMPL_UNICODE_TO_ISO_2022_JP_SET_0208)
                if (pDestBufEnd - pDestBufPtr >= 3)
                {
                    *pDestBufPtr++ = 0x1B; /* ESC */
                    *pDestBufPtr++ = 0x28; /* ( */
                    *pDestBufPtr++ = 0x42; /* B */
                }
                else
                    goto no_output;
            eSet = IMPL_UNICODE_TO_ISO_2022_JP_SET_NONE;
            if (pDestBufPtr != pDestBufEnd)
                *pDestBufPtr++ = (sal_Char) nChar;
            else
                goto no_output;
        }
        else if (nChar == 0x1B)
            goto bad_input;
        else if (nChar < 0x80)
        {
            if (eSet != IMPL_UNICODE_TO_ISO_2022_JP_SET_ASCII)
                if (pDestBufEnd - pDestBufPtr >= 3)
                {
                    *pDestBufPtr++ = 0x1B; /* ESC */
                    *pDestBufPtr++ = 0x28; /* ( */
                    *pDestBufPtr++ = 0x42; /* B */
                    eSet = IMPL_UNICODE_TO_ISO_2022_JP_SET_ASCII;
                }
                else
                    goto no_output;
            if (pDestBufPtr != pDestBufEnd)
                *pDestBufPtr++ = (sal_Char) nChar;
            else
                goto no_output;
        }
        else
        {
            sal_uInt16 nBytes = 0;
            sal_uInt32 nIndex1 = nChar >> 8;
            if (nIndex1 < 0x100)
            {
                sal_uInt32 nIndex2 = nChar & 0xFF;
                sal_uInt32 nFirst = pJisX0208Data[nIndex1].mnLowStart;
                if (nIndex2 >= nFirst
                    && nIndex2 <= pJisX0208Data[nIndex1].mnLowEnd)
                {
                    nBytes = pJisX0208Data[nIndex1].
                                 mpToUniTrailTab[nIndex2 - nFirst];
                    if (nBytes == 0)
                        /* For some reason, the tables in tcvtjp4.tab do not
                           include these two conversions: */
                        switch (nChar)
                        {
                        case 0xA5: /* YEN SIGN */
                            nBytes = 0x216F;
                            break;

                        case 0xAF: /* MACRON */
                            nBytes = 0x2131;
                            break;
                        }
                }
            }
            if (nBytes != 0)
            {
                if (eSet != IMPL_UNICODE_TO_ISO_2022_JP_SET_0208)
                    if (pDestBufEnd - pDestBufPtr >= 3)
                    {
                        *pDestBufPtr++ = 0x1B; /* ESC */
                        *pDestBufPtr++ = 0x24; /* $ */
                        *pDestBufPtr++ = 0x42; /* B */
                        eSet = IMPL_UNICODE_TO_ISO_2022_JP_SET_0208;
                    }
                    else
                        goto no_output;
                if (pDestBufEnd - pDestBufPtr >= 2)
                {
                    *pDestBufPtr++ = (sal_Char) (nBytes >> 8);
                    *pDestBufPtr++ = (sal_Char) (nBytes & 0xFF);
                }
                else
                    goto no_output;
            }
            else
                goto bad_input;
        }
        nHighSurrogate = 0;
        continue;

    bad_input:
        switch (ImplHandleBadInputUnicodeToTextConversion(
                    bUndefined,
                    nChar,
                    nFlags,
                    &pDestBufPtr,
                    pDestBufEnd,
                    &nInfo,
                    "\x1B(B",
                    eSet == IMPL_UNICODE_TO_ISO_2022_JP_SET_ASCII ? 0 : 3,
                    &bWritten))
        {
        case IMPL_BAD_INPUT_STOP:
            nHighSurrogate = 0;
            break;

        case IMPL_BAD_INPUT_CONTINUE:
            if (bWritten)
                eSet = IMPL_UNICODE_TO_ISO_2022_JP_SET_ASCII;
            nHighSurrogate = 0;
            continue;

        case IMPL_BAD_INPUT_NO_OUTPUT:
            goto no_output;
        }
        break;

    no_output:
        --pSrcBuf;
        nInfo |= RTL_UNICODETOTEXT_INFO_DESTBUFFERTOSMALL;
        break;
    }

    if ((nInfo & (RTL_UNICODETOTEXT_INFO_ERROR
                      | RTL_UNICODETOTEXT_INFO_DESTBUFFERTOSMALL))
            == 0)
    {
        sal_Bool bFlush = sal_True;
        if (nHighSurrogate != 0)
            if ((nFlags & RTL_UNICODETOTEXT_FLAGS_FLUSH) != 0)
                nInfo |= RTL_UNICODETOTEXT_INFO_SRCBUFFERTOSMALL;
            else
                switch (ImplHandleBadInputUnicodeToTextConversion(
                            sal_False,
                            0,
                            nFlags,
                            &pDestBufPtr,
                            pDestBufEnd,
                            &nInfo,
                            "\x1B(B",
                            eSet == IMPL_UNICODE_TO_ISO_2022_JP_SET_ASCII ?
                                0 : 3,
                            &bWritten))
                {
                case IMPL_BAD_INPUT_STOP:
                    nHighSurrogate = 0;
                    bFlush = sal_False;
                    break;

                case IMPL_BAD_INPUT_CONTINUE:
                    if (bWritten)
                        eSet = IMPL_UNICODE_TO_ISO_2022_JP_SET_ASCII;
                    nHighSurrogate = 0;
                    break;

                case IMPL_BAD_INPUT_NO_OUTPUT:
                    nInfo |= RTL_UNICODETOTEXT_INFO_DESTBUFFERTOSMALL;
                    break;
                }
        if (bFlush
            && eSet == IMPL_UNICODE_TO_ISO_2022_JP_SET_0208
            && (nFlags & RTL_UNICODETOTEXT_FLAGS_FLUSH) != 0)
        {
            if (pDestBufEnd - pDestBufPtr >= 3)
            {
                *pDestBufPtr++ = 0x1B; /* ESC */
                *pDestBufPtr++ = 0x28; /* ( */
                *pDestBufPtr++ = 0x42; /* B */
                eSet = IMPL_UNICODE_TO_ISO_2022_JP_SET_ASCII;
            }
            else
                nInfo |= RTL_UNICODETOTEXT_INFO_DESTBUFFERTOSMALL;
        }
    }

    if (pContext)
    {
        ((ImplUnicodeToIso2022JpContext *) pContext)->m_nHighSurrogate
            = nHighSurrogate;
        ((ImplUnicodeToIso2022JpContext *) pContext)->m_eSet = eSet;
    }
    if (pInfo)
        *pInfo = nInfo;
    if (pSrcCvtChars)
        *pSrcCvtChars = nConverted;

    return pDestBufPtr - pDestBuf;
}
