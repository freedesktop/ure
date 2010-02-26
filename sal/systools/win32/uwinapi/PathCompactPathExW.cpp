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

#include "macros.h"

#define _SHLWAPI_
#include <shlwapi.h>

IMPLEMENT_THUNK( shlwapi, WINDOWS, BOOL, WINAPI, PathCompactPathExW,
(
    LPWSTR pszOut,
    LPCWSTR lpPathW,
    UINT cchMax,
    DWORD dwFlags
))
{
    AUTO_WSTR2STR(lpPath);
    char* pOutA = (LPSTR)_alloca( cchMax * sizeof(CHAR) );
    BOOL bret = PathCompactPathExA(pOutA, lpPathA, cchMax, dwFlags);
    MultiByteToWideChar(CP_ACP, 0, pOutA, -1, pszOut, (int) cchMax);
    return bret;
}