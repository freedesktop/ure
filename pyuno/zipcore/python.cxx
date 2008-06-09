/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: python.cxx,v $
 * $Revision: 1.2 $
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

#include <cstddef>
#include <cstdlib>
#include <cwchar>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "tools/pathutils.hxx"

#include "pyversion.hxx"

#define MY_LENGTH(s) (sizeof (s) / sizeof *(s) - 1)
#define MY_STRING(s) (s), MY_LENGTH(s)

wchar_t * encode(wchar_t * buffer, wchar_t const * text) {
    *buffer++ = L'"';
    std::size_t n = 0;
    for (;;) {
        wchar_t c = *text++;
        if (c == L'\0') {
            break;
        } else if (c == L'$') {
            *buffer++ = L'\\';
            *buffer++ = L'$';
            n = 0;
        } else if (c == L'\\') {
            *buffer++ = L'\\';
            *buffer++ = L'\\';
            n += 2;
        } else {
            *buffer++ = c;
            n = 0;
        }
    }
    // The command line will continue with a double quote, so double any
    // preceding backslashes as required by Windows:
    for (std::size_t i = 0; i < n; ++i) {
        *buffer++ = L'\\';
    }
    *buffer++ = L'"';
    return buffer;
}

int wmain(int argc, wchar_t ** argv, wchar_t **) {
    wchar_t path[MAX_PATH];
    DWORD n = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) {
        std::exit(EXIT_FAILURE);
    }
    wchar_t * pathEnd = tools::filename(path);
    wchar_t bootstrap[MAX_PATH];
    wchar_t * bootstrapEnd = tools::buildPath(
        bootstrap, path, pathEnd, MY_STRING(L"fundamental.ini"));
    if (bootstrapEnd == NULL ||
        (tools::buildPath(path, path, pathEnd, MY_STRING(L"..\\basis-link"))
         == NULL))
    {
        std::exit(EXIT_FAILURE);
    }
    pathEnd = tools::resolveLink(path);
    wchar_t path1[MAX_PATH];
    wchar_t * path1End = tools::buildPath(
        path1, path, pathEnd, MY_STRING(L"\\program"));
    if (path1End == NULL) {
        std::exit(EXIT_FAILURE);
    }
    wchar_t pythonpath2[MAX_PATH];
    wchar_t * pythonpath2End = tools::buildPath(
        pythonpath2, path, pathEnd,
        MY_STRING(L"\\program\\python-core-" MY_PYVERSION L"\\lib"));
    if (pythonpath2End == NULL) {
        std::exit(EXIT_FAILURE);
    }
    wchar_t pythonpath3[MAX_PATH];
    wchar_t * pythonpath3End = tools::buildPath(
        pythonpath3, path, pathEnd,
        MY_STRING(
            L"\\program\\python-core-" MY_PYVERSION L"\\lib\\site-packages"));
    if (pythonpath3End == NULL) {
        std::exit(EXIT_FAILURE);
    }
    wchar_t pythonhome[MAX_PATH];
    wchar_t * pythonhomeEnd = tools::buildPath(
        pythonhome, path, pathEnd,
        MY_STRING(L"\\program\\python-core-" MY_PYVERSION));
    if (pythonhomeEnd == NULL) {
        std::exit(EXIT_FAILURE);
    }
    wchar_t pythonexe[MAX_PATH];
    wchar_t * pythonexeEnd = tools::buildPath(
        pythonexe, path, pathEnd,
        MY_STRING(
            L"\\program\\python-core-" MY_PYVERSION L"\\bin\\python.exe"));
    if (pythonexeEnd == NULL) {
        std::exit(EXIT_FAILURE);
    }
    if (tools::buildPath(path, path, pathEnd, MY_STRING(L"\\ure-link")) == NULL)
    {
        std::exit(EXIT_FAILURE);
    }
    pathEnd = tools::resolveLink(path);
    if (pathEnd == NULL) {
        std::exit(EXIT_FAILURE);
    }
    pathEnd = tools::buildPath(path, path, pathEnd, MY_STRING(L"\\bin"));
    if (pathEnd == NULL) {
        std::exit(EXIT_FAILURE);
    }
    std::size_t clSize = MY_LENGTH(L"\"") + 4 * (pythonexeEnd - pythonexe) +
        MY_LENGTH(L"\"\0"); //TODO: overflow
        // 4 * len: each char preceded by backslash, each trailing backslash
        // doubled
    for (int i = 1; i < argc; ++i) {
        clSize += MY_LENGTH(L" \"") + 4 * std::wcslen(argv[i]) +
            MY_LENGTH(L"\""); //TODO: overflow
    }
    wchar_t * cl = new wchar_t[clSize];
    if (cl == NULL) {
        std::exit(EXIT_FAILURE);
    }
    wchar_t * cp = encode(cl, pythonhome);
    for (int i = 1; i < argc; ++i) {
        *cp++ = L' ';
        cp = encode(cp, argv[i]);
    }
    *cp = L'\0';
    n = GetEnvironmentVariableW(L"PATH", NULL, 0);
    wchar_t * orig;
    if (n == 0) {
        if (GetLastError() != ERROR_ENVVAR_NOT_FOUND) {
            std::exit(EXIT_FAILURE);
        }
        orig = L"";
    } else {
        orig = new wchar_t[n];
        if (orig == NULL ||
            GetEnvironmentVariableW(L"PATH", orig, n) != n - 1)
        {
            std::exit(EXIT_FAILURE);
        }
    }
    wchar_t * value = new wchar_t[
        (pathEnd - path) + MY_LENGTH(L";") + (path1End - path1) +
        (n == 0 ? 0 : MY_LENGTH(L";") + (n - 1)) + 1]; //TODO: overflow
    wsprintfW(value, L"%s;%s%s%s", path, path1, n == 0 ? L"" : L";", orig);
    if (!SetEnvironmentVariableW(L"PATH", value)) {
        std::exit(EXIT_FAILURE);
    }
    if (n != 0) {
        delete orig;
    }
    delete value;
    n = GetEnvironmentVariableW(L"PYTHONPATH", NULL, 0);
    if (n == 0) {
        if (GetLastError() != ERROR_ENVVAR_NOT_FOUND) {
            std::exit(EXIT_FAILURE);
        }
        orig = L"";
    } else {
        orig = new wchar_t[n];
        if (orig == NULL ||
            GetEnvironmentVariableW(L"PYTHONPATH", orig, n) != n - 1)
        {
            std::exit(EXIT_FAILURE);
        }
    }
    value = new wchar_t[
        (path1End - path1) + MY_LENGTH(L";") + (pythonpath2End - pythonpath2) +
        MY_LENGTH(L";") + (pythonpath3End - pythonpath3) +
        (n == 0 ? 0 : MY_LENGTH(L";") + (n - 1)) + 1]; //TODO: overflow
    wsprintfW(
        value, L"%s;%s;%s%s%s", path1, pythonpath2, pythonpath3,
        n == 0 ? L"" : L";", orig);
    if (!SetEnvironmentVariableW(L"PYTHONPATH", value)) {
        std::exit(EXIT_FAILURE);
    }
    if (n != 0) {
        delete orig;
    }
    delete value;
    if (!SetEnvironmentVariableW(L"PYTHONHOME", pythonhome)) {
        std::exit(EXIT_FAILURE);
    }
    n = GetEnvironmentVariableW(L"URE_BOOTSTRAP", NULL, 0);
    if (n == 0) {
        if (GetLastError() != ERROR_ENVVAR_NOT_FOUND ||
            !SetEnvironmentVariableW(L"URE_BOOTSTRAP", bootstrap))
        {
            std::exit(EXIT_FAILURE);
        }
    }
    STARTUPINFOW startinfo;
    ZeroMemory(&startinfo, sizeof (STARTUPINFOW));
    startinfo.cb = sizeof (STARTUPINFOW);
    PROCESS_INFORMATION procinfo;
    if (!CreateProcessW(
            pythonexe, cl, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, NULL,
            NULL, &startinfo, &procinfo)) {
        std::exit(EXIT_FAILURE);
    }
    std::exit(EXIT_SUCCESS);
}
