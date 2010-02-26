/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#define UNICODE
#define _UNICODE
#define _WIN32_WINNT_0x0500
#include "systools/win32/uwinapi.h"

#include "file_url.h"
#include "file_error.h"

#include "rtl/alloc.h"
#include "osl/diagnose.h"
#include "osl/file.h"
#include "osl/mutex.h"

#include <stdio.h>
#include <tchar.h>

#if OSL_DEBUG_LEVEL > 0
#define OSL_ENSURE_FILE( cond, msg, file ) ( (cond) ?  (void)0 : _osl_warnFile( msg, file ) )
#else
#define OSL_ENSURE_FILE( cond, msg, file ) ((void)0)
#endif

#define ELEMENTS_OF_ARRAY(arr) (sizeof(arr)/(sizeof((arr)[0])))

//##################################################################
// FileURL functions
//##################################################################

extern "C" oslMutex g_CurrentDirectoryMutex; /* Initialized in dllentry.c */
oslMutex g_CurrentDirectoryMutex = 0;

//#####################################################
static BOOL IsValidFilePathComponent(
    LPCTSTR lpComponent, LPCTSTR *lppComponentEnd, DWORD dwFlags)
{
        LPCTSTR	lpComponentEnd = NULL;
        LPCTSTR	lpCurrent = lpComponent;
        BOOL	fValid = TRUE;	/* Assume success */
        TCHAR	cLast = 0;

        /* Path component length must not exceed MAX_PATH */

        while ( !lpComponentEnd && lpCurrent && lpCurrent - lpComponent < MAX_PATH )
        {
            switch ( *lpCurrent )
            {
                /* Both backslash and slash determine the end of a path component */
            case '\0':
            case '/':
            case '\\':
                switch ( cLast )
                {
                    /* Component must not end with '.' or blank and can't be empty */

                case '.':
                    if ( dwFlags & VALIDATEPATH_ALLOW_ELLIPSE )
                    {
                        if ( 1 == lpCurrent - lpComponent )
                        {
                            /* Current directory is O.K. */
                            lpComponentEnd = lpCurrent;
                            break;
                        }
                        else if ( 2 == lpCurrent - lpComponent && '.' == *lpComponent )
                        {
                            /* Parent directory is O.K. */
                            lpComponentEnd = lpCurrent;
                            break;
                        }
                    }
                case 0:
                case ' ':
                    lpComponentEnd = lpCurrent - 1;
                    fValid = FALSE;
                    break;
                default:
                    lpComponentEnd = lpCurrent;
                    break;
                }
                break;
                /* '?' and '*' are valid wildcards but not valid file name characters */
            case '?':
            case '*':
                if ( dwFlags & VALIDATEPATH_ALLOW_WILDCARDS )
                    break;
                /* The following characters are reserved */
            case '<':
            case '>':
            case '\"':
            case '|':
            case ':':
                lpComponentEnd = lpCurrent;
                fValid = FALSE;
                break;
            default:
                /* Characters below ASCII 32 are not allowed */
                if ( *lpCurrent < ' ' )
                {
                    lpComponentEnd = lpCurrent;
                    fValid = FALSE;
                }
                break;
            }
            cLast = *lpCurrent++;
        }

        /*	If we don't reached the end of the component the length of the component was to long 
            ( See condition of while loop ) */
        if ( !lpComponentEnd )
        {
            fValid = FALSE;
            lpComponentEnd = lpCurrent;
        }

        /* Test wether the component specifies a device name what is not allowed */
        
        // MT: PERFORMANCE: 
        // This is very expensive. A lot of calls to _tcsicmp.
        // in SRC6870m71 67.000 calls of this method while empty office start result into more than 1.500.00 calls of _tcsicmp!
        // Possible optimizations
        // - Array should be const static
        // - Sorted array, use binary search
        // - More intelligent check for com1-9, lpt1-9
        // Maybe make szComponent upper case, don't search case intensitive
        // Talked to HRO: Could be removed. Shouldn't be used in OOo, and if used for something like a filename, it will lead to an error anyway.
        /*
        if ( fValid )
        {
            LPCTSTR	alpDeviceNames[] =
            {
                TEXT("CON"),
                TEXT("PRN"),
                TEXT("AUX"),
                TEXT("CLOCK$"),
                TEXT("NUL"),
                TEXT("LPT1"),
                TEXT("LPT2"),
                TEXT("LPT3"),
                TEXT("LPT4"),
                TEXT("LPT5"),
                TEXT("LPT6"),
                TEXT("LPT7"),
                TEXT("LPT8"),
                TEXT("LPT9"),
                TEXT("COM1"),
                TEXT("COM2"),
                TEXT("COM3"),
                TEXT("COM4"),
                TEXT("COM5"),
                TEXT("COM6"),
                TEXT("COM7"),
                TEXT("COM8"),
                TEXT("COM9")
            };
            
            TCHAR	szComponent[MAX_PATH];
            int		nComponentLength;
            LPCTSTR	lpDot;
            int		i;

            // A device name with an extension is also invalid
            lpDot = _tcschr( lpComponent, '.' );

            if ( !lpDot || lpDot > lpComponentEnd )
                nComponentLength = lpComponentEnd - lpComponent;
            else
                nComponentLength = lpDot - lpComponent;

            _tcsncpy( szComponent, lpComponent, nComponentLength );
            szComponent[nComponentLength] = 0;

            for ( i = 0; i < sizeof( alpDeviceNames ) / sizeof(LPCTSTR); i++ )
            {
                if ( 0 == _tcsicmp( szComponent, alpDeviceNames[i] ) )
                {
                    lpComponentEnd = lpComponent;
                    fValid = FALSE;
                    break;
                }
            }
        }
        */

        if ( fValid )
        {
            // Empty components are not allowed
            if ( lpComponentEnd - lpComponent < 1 )
                fValid = FALSE;

            // If we reached the end of the string NULL is returned
            else if ( !*lpComponentEnd )
                lpComponentEnd = NULL;

        }

        if ( lppComponentEnd )
            *lppComponentEnd = lpComponentEnd;

        return fValid;
}

//##################################################### 
#define	CHARSET_SEPARATOR TEXT("\\/")

DWORD IsValidFilePath(rtl_uString *path, LPCTSTR *lppError, DWORD dwFlags, rtl_uString **corrected)
{
        LPCTSTR lpszPath = reinterpret_cast< LPCTSTR >(path->buffer);
        LPCTSTR	lpComponent;
        BOOL	fValid = TRUE;
        DWORD	dwPathType = PATHTYPE_ERROR;

        if ( dwFlags & VALIDATEPATH_ALLOW_RELATIVE )
            dwFlags |= VALIDATEPATH_ALLOW_ELLIPSE;

        if ( !lpszPath )
        {
            fValid = FALSE;
            lpComponent = lpszPath;
        }

        /* Test for UNC path notation */
        if ( 2 == _tcsspn( lpszPath, CHARSET_SEPARATOR ) )
        {
            /* Place the pointer behind the leading to backslashes */

            lpComponent = lpszPath + 2;

            fValid = IsValidFilePathComponent( lpComponent, &lpComponent, VALIDATEPATH_ALLOW_ELLIPSE );

            /* So far we have a valid servername. Now let's see if we also have a network resource */

            dwPathType = PATHTYPE_ABSOLUTE_UNC;

            if ( fValid )
            {
                if ( lpComponent &&	 !*++lpComponent )
                    lpComponent = NULL;
                
                if ( !lpComponent )
                {
    #if 0
                    /* We only have a Server specification what is invalid */

                    lpComponent = lpszPath;
                    fValid = FALSE;
    #else
                    dwPathType |= PATHTYPE_IS_SERVER;
    #endif
                }
                else
                {
                    /* Now test the network resource */

                    fValid = IsValidFilePathComponent( lpComponent, &lpComponent, 0 );

                    /* If we now reached the end of the path, everything is O.K. */


                    if ( fValid && (!lpComponent || lpComponent && !*++lpComponent ) )
                    {
                        lpComponent = NULL;
                        dwPathType |= PATHTYPE_IS_VOLUME;
                    }
                }
            }
        }

        /* Local path verification. Must start with <drive>: */
        else if ( _istalpha( lpszPath[0] ) && ':' == lpszPath[1] )
        {
            /* Place pointer behind correct drive specification */

            lpComponent = lpszPath + 2;

            if ( 1 == _tcsspn( lpComponent, CHARSET_SEPARATOR ) )
                lpComponent++;
            else if ( *lpComponent )
                fValid = FALSE;

            dwPathType = PATHTYPE_ABSOLUTE_LOCAL;

            /* Now we are behind the backslash or it was a simple drive without backslash */

            if ( fValid && !*lpComponent )
            {
                lpComponent = NULL;
                dwPathType |= PATHTYPE_IS_VOLUME;
            }
        }

        /* Can be a relative path */
        else if ( dwFlags & VALIDATEPATH_ALLOW_RELATIVE )
        {
            lpComponent = lpszPath;

            /* Relative path can start with a backslash */

            if ( 1 == _tcsspn( lpComponent, CHARSET_SEPARATOR ) )
            {
                lpComponent++;
                if ( !*lpComponent )
                    lpComponent = NULL;
            }

            dwPathType = PATHTYPE_RELATIVE;
        }

        /* Anything else is an error */
        else
        {
            fValid = FALSE;
            lpComponent = lpszPath;
        }

        /* Now validate each component of the path */
        while ( fValid && lpComponent )
        {
            // Correct path by merging consecutive slashes:
            if (*lpComponent == '\\' && corrected != NULL) {
                sal_Int32 i = lpComponent - lpszPath;
                rtl_uString_newReplaceStrAt(corrected, path, i, 1, NULL);
                    //TODO: handle out-of-memory
                lpszPath = reinterpret_cast< LPCTSTR >((*corrected)->buffer);
                lpComponent = lpszPath + i;
            }

            fValid = IsValidFilePathComponent( lpComponent, &lpComponent, dwFlags );

            if ( fValid && lpComponent )
            {
                lpComponent++;

                /* If the string behind the backslash is empty, we've done */

                if ( !*lpComponent )
                    lpComponent = NULL;
            }
        }
        
        if ( fValid && _tcslen( lpszPath ) >= MAX_PATH )
        {
            fValid = FALSE;
            lpComponent = lpszPath + MAX_PATH;
        }

        if ( lppError )
            *lppError = lpComponent;

        return fValid ? dwPathType : PATHTYPE_ERROR;
}

//############################################# 
//#####################################################
//Undocumented in SHELL32.DLL ordinal 35 
static BOOL PathRemoveFileSpec(LPTSTR lpPath)
{
    BOOL	fSuccess = FALSE;	// Assume failure
    LPTSTR	lpLastBkSlash = _tcsrchr( lpPath, '\\' );
    LPTSTR	lpLastSlash = _tcsrchr( lpPath, '/' );
    LPTSTR	lpLastDelimiter = lpLastSlash > lpLastBkSlash ? lpLastSlash : lpLastBkSlash;

    if ( lpLastDelimiter )
    {
            if ( 0 == *(lpLastDelimiter + 1) )
            {
                if ( lpLastDelimiter > lpPath && *(lpLastDelimiter - 1) != ':' )
                {
                    *lpLastDelimiter = 0;
                    fSuccess = TRUE;
                }
            }
            else
            {
                *(++lpLastDelimiter) = 0;
                fSuccess = TRUE;
            }
    }
    return fSuccess;
}
        
//#####################################################
// Undocumented in SHELL32.DLL ordinal 32 
static LPTSTR PathAddBackslash(LPTSTR lpPath)
{
    LPTSTR	lpEndPath = NULL;

    if ( lpPath )
    {
            int		nLen = _tcslen(lpPath);

            if ( !nLen || lpPath[nLen-1] != '\\' && lpPath[nLen-1] != '/' && nLen < MAX_PATH - 1 )
            {
                lpEndPath = lpPath + nLen;
                *lpEndPath++ = '\\';
                *lpEndPath = 0;
            }
    }
    return lpEndPath;
}

//#####################################################
// Same as GetLongPathName but also 95/NT4 
static DWORD GetCaseCorrectPathNameEx(
    LPCTSTR	lpszShortPath,	// file name
    LPTSTR	lpszLongPath,	// path buffer
    DWORD	cchBuffer,		// size of path buffer 
    DWORD	nSkipLevels
)
{
        TCHAR	szPath[MAX_PATH];
        BOOL	fSuccess;

        cchBuffer = cchBuffer; /* avoid warnings */
        
        _tcscpy( szPath, lpszShortPath );

        fSuccess = PathRemoveFileSpec( szPath );

        if ( fSuccess )
        {		
            int nLen = _tcslen( szPath );
            LPCTSTR	lpszFileSpec = lpszShortPath + nLen;
            BOOL	bSkipThis;

            if ( 0 == _tcscmp( lpszFileSpec, TEXT("..") ) )
            {
                bSkipThis = TRUE;
                nSkipLevels += 1;
            }
            else if ( 
                0 == _tcscmp( lpszFileSpec, TEXT(".") ) || 
                0 == _tcscmp( lpszFileSpec, TEXT("\\") ) ||
                0 == _tcscmp( lpszFileSpec, TEXT("/") ) 
                )
            {
                bSkipThis = TRUE;
            }
            else if ( nSkipLevels )
            {
                bSkipThis = TRUE;
                nSkipLevels--;
            }
            else
                bSkipThis = FALSE;

            GetCaseCorrectPathNameEx( szPath, szPath, MAX_PATH, nSkipLevels );

            PathAddBackslash( szPath );

            /* Analyze parent if not only a trailing backslash was cutted but a real file spec */
            if ( !bSkipThis )
            {
                WIN32_FIND_DATA	aFindFileData;
                HANDLE	hFind = FindFirstFile( lpszShortPath, &aFindFileData );

                if ( IsValidHandle(hFind) )
                {
                    _tcscat( szPath, aFindFileData.cFileName[0] ? aFindFileData.cFileName : aFindFileData.cAlternateFileName );

                    FindClose( hFind );
                }
                else
                    return 0;
            }
        }
        else
        {
            /* File specification can't be removed therefore the short path is either a drive
               or a network share. If still levels to skip are left, the path specification 
               tries to travel below the file system root */
            if ( nSkipLevels )
                return 0;

            _tcsupr( szPath );
        }

        _tcscpy( lpszLongPath, szPath );

        return _tcslen( lpszLongPath );
}

//##################################################### 
#define WSTR_SYSTEM_ROOT_PATH				L"\\\\.\\"

DWORD GetCaseCorrectPathName(
    LPCTSTR	lpszShortPath,	// file name
    LPTSTR	lpszLongPath,	// path buffer
    DWORD	cchBuffer		// size of path buffer 
)
{
    /* Special handling for "\\.\" as system root */
    if ( lpszShortPath && 0 == wcscmp( lpszShortPath, WSTR_SYSTEM_ROOT_PATH ) )
    {
        if ( cchBuffer >= ELEMENTS_OF_ARRAY(WSTR_SYSTEM_ROOT_PATH) )
        {
            wcscpy( lpszLongPath, WSTR_SYSTEM_ROOT_PATH );
            return ELEMENTS_OF_ARRAY(WSTR_SYSTEM_ROOT_PATH) - 1;
        }
        else
        {
            return ELEMENTS_OF_ARRAY(WSTR_SYSTEM_ROOT_PATH) - 1;
        }
    }
    else
    {
        return GetCaseCorrectPathNameEx( lpszShortPath, lpszLongPath, cchBuffer, 0 );
    }
}

//############################################# 
static sal_Bool _osl_decodeURL( rtl_String* strUTF8, rtl_uString** pstrDecodedURL )
{
    sal_Char		*pBuffer;
    const sal_Char	*pSrcEnd;
    const sal_Char	*pSrc;
    sal_Char		*pDest;
    sal_Int32		nSrcLen;
    sal_Bool		bValidEncoded = sal_True;	/* Assume success */

    /* The resulting decoded string length is shorter or equal to the source length */

    nSrcLen = rtl_string_getLength(strUTF8);
    pBuffer = reinterpret_cast<sal_Char*>(rtl_allocateMemory(nSrcLen + 1));

    pDest = pBuffer;
    pSrc = rtl_string_getStr(strUTF8);
    pSrcEnd = pSrc + nSrcLen;

    /* Now decode the URL what should result in an UTF8 string */
    while ( bValidEncoded && pSrc < pSrcEnd )
    {
        switch ( *pSrc )
        {
        case '%':
            {
                sal_Char	aToken[3];
                sal_Char	aChar;

                pSrc++;
                aToken[0] = *pSrc++;
                aToken[1] = *pSrc++;
                aToken[2] = 0;

                aChar = (sal_Char)strtoul( aToken, NULL, 16 );

                /* The chars are path delimiters and must not be encoded */

                if ( 0 == aChar || '\\' == aChar || '/' == aChar || ':' == aChar )
                    bValidEncoded = sal_False;
                else
                    *pDest++ = aChar;
            }
            break;
        default:
            *pDest++ = *pSrc++;
            break;
        }
    }

    *pDest++ = 0;
        
    if ( bValidEncoded )
    {
        rtl_string2UString( pstrDecodedURL, pBuffer, rtl_str_getLength(pBuffer), RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS );
        OSL_ASSERT(*pstrDecodedURL != 0);
    }
        
    rtl_freeMemory( pBuffer );

    return bValidEncoded;
}

//############################################# 
static void _osl_encodeURL( rtl_uString *strURL, rtl_String **pstrEncodedURL )
{
    /* Encode non ascii characters within the URL */

    rtl_String		*strUTF8 = NULL;
    sal_Char		*pszEncodedURL;
    const sal_Char	*pURLScan;
    sal_Char		*pURLDest;
    sal_Int32		nURLScanLen;
    sal_Int32		nURLScanCount;
        
    rtl_uString2String( &strUTF8, rtl_uString_getStr( strURL ), rtl_uString_getLength( strURL ), RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS );

    pszEncodedURL = (sal_Char*) rtl_allocateMemory( (rtl_string_getLength( strUTF8 ) * 3 + 1)  * sizeof(sal_Char) );

    pURLDest = pszEncodedURL;
    pURLScan = rtl_string_getStr( strUTF8 );
    nURLScanLen = rtl_string_getLength( strUTF8 );
    nURLScanCount = 0;

    while ( nURLScanCount < nURLScanLen )
    {
        sal_Char cCurrent = *pURLScan;
        switch ( cCurrent )
        {
        default:
            if (!( ( cCurrent >= 'a' && cCurrent <= 'z' ) || ( cCurrent >= 'A' && cCurrent <= 'Z' ) || ( cCurrent >= '0' && cCurrent <= '9' ) ) )
            {
                sprintf( pURLDest, "%%%02X", (unsigned char)cCurrent );
                pURLDest += 3;
                break;
            }
        case '!':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '-':
        case '.':
        case '_':
        case '~':
        case '$':
        case '&':
        case '+':
        case ',':
        case '=':
        case '@':
        case ':':
        case '/':
        case '\\':
        case '|':
            *pURLDest++ = cCurrent;
            break;
        case 0:
            break;
        }

        pURLScan++;
        nURLScanCount++;
    }

    *pURLDest = 0;

    rtl_string_release( strUTF8 );
    rtl_string_newFromStr( pstrEncodedURL, pszEncodedURL );
    rtl_freeMemory( pszEncodedURL );
}

//#############################################      
#define WSTR_SYSTEM_ROOT_PATH				L"\\\\.\\"

oslFileError _osl_getSystemPathFromFileURL( rtl_uString *strURL, rtl_uString **pustrPath, sal_Bool bAllowRelative )
{
    rtl_String			*strUTF8 = NULL;
    rtl_uString			*strDecodedURL = NULL;
    rtl_uString			*strTempPath = NULL;
    const sal_Unicode	*pDecodedURL;
    sal_uInt32			nDecodedLen;
    sal_Bool			bValidEncoded;
    oslFileError		nError = osl_File_E_INVAL;	/* Assume failure */

    /*  If someone hasn't encoded the complete URL we convert it to UTF8 now to prevent from
        having a mixed encoded URL later */

    rtl_uString2String( &strUTF8, rtl_uString_getStr( strURL ), rtl_uString_getLength( strURL ), RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS );

    /* If the length of strUTF8 and strURL differs it indicates that the URL was not correct encoded */

    OSL_ENSURE_FILE( 
        strUTF8->length == strURL->length || 
        0 != rtl_ustr_ascii_shortenedCompareIgnoreAsciiCase_WithLength( strURL->buffer, strURL->length, "file:\\\\", 7 )
        ,"osl_getSystemPathFromFileURL: \"%s\" is not encoded !!!", strURL );

    bValidEncoded = _osl_decodeURL( strUTF8, &strDecodedURL );

    /* Release the encoded UTF8 string */
    rtl_string_release( strUTF8 );

    if ( bValidEncoded )
    {
        /* Replace backslashes and pipes */

        rtl_uString_newReplace( &strDecodedURL, strDecodedURL, '/', '\\' );
        rtl_uString_newReplace( &strDecodedURL, strDecodedURL, '|', ':' );

        pDecodedURL = rtl_uString_getStr( strDecodedURL );
        nDecodedLen = rtl_uString_getLength( strDecodedURL );

        /* Must start with "file://" */
        if ( 0 == rtl_ustr_ascii_shortenedCompareIgnoreAsciiCase_WithLength( pDecodedURL, nDecodedLen, "file:\\\\", 7 ) )
        {
            sal_uInt32	nSkip;

            if ( 0 == rtl_ustr_ascii_shortenedCompareIgnoreAsciiCase_WithLength( pDecodedURL, nDecodedLen, "file:\\\\\\", 8 ) )
                nSkip = 8;
            else if ( 
                0 == rtl_ustr_ascii_shortenedCompareIgnoreAsciiCase_WithLength( pDecodedURL, nDecodedLen, "file:\\\\localhost\\", 17 ) ||
                0 == rtl_ustr_ascii_shortenedCompareIgnoreAsciiCase_WithLength( pDecodedURL, nDecodedLen, "file:\\\\127.0.0.1\\", 17 ) 
                      )
                nSkip = 17;
            else 
                nSkip = 5;

            /* Indicates local root */
            if ( nDecodedLen == nSkip )
                rtl_uString_newFromStr_WithLength( &strTempPath, reinterpret_cast<const sal_Unicode*>(WSTR_SYSTEM_ROOT_PATH), ELEMENTS_OF_ARRAY(WSTR_SYSTEM_ROOT_PATH) - 1 );
            else
                rtl_uString_newFromStr_WithLength( &strTempPath, pDecodedURL + nSkip, nDecodedLen - nSkip );

            if ( IsValidFilePath( strTempPath, NULL, VALIDATEPATH_ALLOW_ELLIPSE, &strTempPath ) )
                nError = osl_File_E_None;
        }
        else if ( bAllowRelative )	/* This maybe a relative file URL */
        {
            rtl_uString_assign( &strTempPath, strDecodedURL );

            if ( IsValidFilePath( strTempPath, NULL, VALIDATEPATH_ALLOW_RELATIVE | VALIDATEPATH_ALLOW_ELLIPSE, &strTempPath ) )
                nError = osl_File_E_None;
        }
        /*
          else
          OSL_ENSURE_FILE( !nError, "osl_getSystemPathFromFileURL: \"%s\" is not an absolute FileURL !!!", strURL );
        */

    }
        
    if ( strDecodedURL )
        rtl_uString_release( strDecodedURL );
            
    if ( osl_File_E_None == nError )
        rtl_uString_assign( pustrPath, strTempPath );
      
    if ( strTempPath )
        rtl_uString_release( strTempPath );

    /*
      OSL_ENSURE_FILE( !nError, "osl_getSystemPathFromFileURL: \"%s\" is not a FileURL !!!", strURL );
    */

    return nError;
}

//#############################################      
oslFileError _osl_getFileURLFromSystemPath( rtl_uString* strPath, rtl_uString** pstrURL )
{
    oslFileError nError = osl_File_E_INVAL; /* Assume failure */
    rtl_uString	*strTempURL = NULL;
    DWORD dwPathType = PATHTYPE_ERROR;

    if (strPath)
        dwPathType = IsValidFilePath(strPath, NULL, VALIDATEPATH_ALLOW_RELATIVE, NULL);
                
    if (dwPathType)
    {
        rtl_uString	*strTempPath = NULL;

        /* Replace backslashes */
        rtl_uString_newReplace( &strTempPath, strPath, '\\', '/' );

        switch ( dwPathType & PATHTYPE_MASK_TYPE )
        {
        case PATHTYPE_RELATIVE:
            rtl_uString_assign( &strTempURL, strTempPath );
            nError = osl_File_E_None;
            break;
        case PATHTYPE_ABSOLUTE_UNC:
            rtl_uString_newFromAscii( &strTempURL, "file:" );
            rtl_uString_newConcat( &strTempURL, strTempURL, strTempPath );
            nError = osl_File_E_None;
            break;
        case PATHTYPE_ABSOLUTE_LOCAL:
            rtl_uString_newFromAscii( &strTempURL, "file:///" );
            rtl_uString_newConcat( &strTempURL, strTempURL, strTempPath );
            nError = osl_File_E_None;
            break;
        default:
            break;
        }

        /* Release temp path */
        rtl_uString_release( strTempPath );
    }

    if ( osl_File_E_None == nError )
    {
        rtl_String	*strEncodedURL = NULL;

        /* Encode the URL */
        _osl_encodeURL( strTempURL, &strEncodedURL );

        /* Provide URL via unicode string */
        rtl_string2UString( pstrURL, rtl_string_getStr(strEncodedURL), rtl_string_getLength(strEncodedURL), RTL_TEXTENCODING_ASCII_US, OUSTRING_TO_OSTRING_CVTFLAGS );
        OSL_ASSERT(*pstrURL != 0);
        rtl_string_release( strEncodedURL );
    }

    /* Release temp URL */
    if ( strTempURL )
        rtl_uString_release( strTempURL );

    /*
      OSL_ENSURE_FILE( !nError, "osl_getFileURLFromSystemPath: \"%s\" is not a systemPath !!!", strPath );
    */
    return nError;
}

//#####################################################
oslFileError SAL_CALL osl_getFileURLFromSystemPath( 
    rtl_uString* ustrPath, rtl_uString** pustrURL )
{
    return _osl_getFileURLFromSystemPath( ustrPath, pustrURL );
}

//#####################################################
oslFileError SAL_CALL osl_getSystemPathFromFileURL( 
    rtl_uString *ustrURL, rtl_uString **pustrPath)
{
    return _osl_getSystemPathFromFileURL( ustrURL, pustrPath, sal_True );
}

//#####################################################
oslFileError SAL_CALL osl_searchFileURL( 
    rtl_uString *ustrFileName, 
    rtl_uString *ustrSystemSearchPath, 
    rtl_uString **pustrPath)
{
    rtl_uString		*ustrUNCPath = NULL;
    rtl_uString		*ustrSysPath = NULL;
    oslFileError	error;

    /* First try to interpret the file name as an URL even a relative one */
    error = _osl_getSystemPathFromFileURL( ustrFileName, &ustrUNCPath, sal_True );

    /* So far we either have an UNC path or something invalid 
       Now create a system path */
    if ( osl_File_E_None == error )
        error = _osl_getSystemPathFromFileURL( ustrUNCPath, &ustrSysPath, sal_True );

    if ( osl_File_E_None == error )
    {
        DWORD	nBufferLength;
        DWORD	dwResult;		
        LPTSTR	lpBuffer = NULL;
        LPTSTR	lpszFilePart;

        /* Repeat calling SearchPath ... 
           Start with MAX_PATH for the buffer. In most cases this 
           will be enough and does not force the loop to runtwice */
        dwResult = MAX_PATH;

        do
        {
            /* If search path is empty use a NULL pointer instead according to MSDN documentation of SearchPath */
            LPCTSTR	lpszSearchPath = ustrSystemSearchPath && ustrSystemSearchPath->length ? reinterpret_cast<LPCTSTR>(ustrSystemSearchPath->buffer) : NULL;
            LPCTSTR	lpszSearchFile = reinterpret_cast<LPCTSTR>(ustrSysPath->buffer);

            /* Allocate space for buffer according to previous returned count of required chars */
            /* +1 is not neccessary if we follow MSDN documentation but for robustness we do so */
            nBufferLength = dwResult + 1;
            lpBuffer = lpBuffer ? 
                reinterpret_cast<LPTSTR>(rtl_reallocateMemory(lpBuffer, nBufferLength * sizeof(TCHAR))) : 
                reinterpret_cast<LPTSTR>(rtl_allocateMemory(nBufferLength * sizeof(TCHAR)));

            dwResult = SearchPath( lpszSearchPath, lpszSearchFile, NULL, nBufferLength, lpBuffer, &lpszFilePart ); 
        } while ( dwResult && dwResult >= nBufferLength );

        /*	... until an error occures or buffer is large enough. 
            dwResult == nBufferLength can not happen according to documentation but lets be robust ;-) */

        if ( dwResult )
        {
            rtl_uString_newFromStr( &ustrSysPath, reinterpret_cast<const sal_Unicode*>(lpBuffer) );
            error = osl_getFileURLFromSystemPath( ustrSysPath, pustrPath );
        }
        else
        {
            WIN32_FIND_DATA	aFindFileData;
            HANDLE	hFind;

            /* Somthing went wrong, perhaps the path was absolute */
            error = oslTranslateFileError( GetLastError() );

            hFind = FindFirstFile( reinterpret_cast<LPCTSTR>(ustrSysPath->buffer), &aFindFileData );

            if ( IsValidHandle(hFind) )
            {
                error = osl_getFileURLFromSystemPath( ustrSysPath, pustrPath );
                FindClose( hFind );
            }
        }

        rtl_freeMemory( lpBuffer );
    }
    
    if ( ustrSysPath )
        rtl_uString_release( ustrSysPath );

    if ( ustrUNCPath )
        rtl_uString_release( ustrUNCPath );

    return error;
}

//#####################################################

oslFileError SAL_CALL osl_getAbsoluteFileURL( rtl_uString* ustrBaseURL, rtl_uString* ustrRelativeURL, rtl_uString** pustrAbsoluteURL )
{
    oslFileError	eError;
    rtl_uString		*ustrRelSysPath = NULL;
    rtl_uString		*ustrBaseSysPath = NULL;

    if ( ustrBaseURL && ustrBaseURL->length )
    {
        eError = _osl_getSystemPathFromFileURL( ustrBaseURL, &ustrBaseSysPath, sal_False );
        OSL_ENSURE( osl_File_E_None == eError, "osl_getAbsoluteFileURL called with relative or invalid base URL" );

        eError = _osl_getSystemPathFromFileURL( ustrRelativeURL, &ustrRelSysPath, sal_True );
    }
    else
    {
        eError = _osl_getSystemPathFromFileURL( ustrRelativeURL, &ustrRelSysPath, sal_False );
        OSL_ENSURE( osl_File_E_None == eError, "osl_getAbsoluteFileURL called with empty base URL and/or invalid relative URL" );
    }

    if ( !eError )
    {
        TCHAR	szBuffer[MAX_PATH];
        TCHAR	szCurrentDir[MAX_PATH];
        LPTSTR	lpFilePart = NULL;
        DWORD	dwResult;

/*@@@ToDo
  Bad, bad hack, this only works if the base path
  really exists which is not necessary according
  to RFC2396
  The whole FileURL implementation should be merged 
  with the rtl/uri class.
*/
        if ( ustrBaseSysPath )
        {
            osl_acquireMutex( g_CurrentDirectoryMutex );

            GetCurrentDirectory( MAX_PATH, szCurrentDir );
            SetCurrentDirectory( reinterpret_cast<LPCTSTR>(ustrBaseSysPath->buffer) );
        }

        dwResult = GetFullPathName( reinterpret_cast<LPCTSTR>(ustrRelSysPath->buffer), MAX_PATH, szBuffer, &lpFilePart );

        if ( ustrBaseSysPath )
        {
            SetCurrentDirectory( szCurrentDir );

            osl_releaseMutex( g_CurrentDirectoryMutex );
        }

        if ( dwResult )
        {
            if ( dwResult >= MAX_PATH )
                eError = osl_File_E_INVAL;
            else
            {
                rtl_uString	*ustrAbsSysPath = NULL;

                rtl_uString_newFromStr( &ustrAbsSysPath, reinterpret_cast<const sal_Unicode*>(szBuffer) );

                eError = osl_getFileURLFromSystemPath( ustrAbsSysPath, pustrAbsoluteURL );

                if ( ustrAbsSysPath )
                    rtl_uString_release( ustrAbsSysPath );
            }
        }
        else
            eError = oslTranslateFileError( GetLastError() );
    }

    if ( ustrBaseSysPath )
        rtl_uString_release( ustrBaseSysPath );

    if ( ustrRelSysPath )
        rtl_uString_release( ustrRelSysPath );

    return	eError;
}

//#####################################################
oslFileError SAL_CALL osl_getCanonicalName( rtl_uString *strRequested, rtl_uString **strValid )
{
    rtl_uString_newFromString(strValid, strRequested);
    return osl_File_E_None;
}
