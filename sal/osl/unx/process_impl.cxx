/*************************************************************************
 *
 *  $RCSfile: process_impl.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2004-10-28 16:25:26 $
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
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include "osl/process.h"

#ifndef INCLUDED_LIMITS_H
#include <limits.h>
#define INCLUDED_LIMITS_H
#endif

#ifndef INCLUDED_PTHREAD_H
#include <pthread.h>
#define INCLUDED_PTHREAD_H
#endif

#ifndef INCLUDED_STDLIB_H
#include <stdlib.h>
#define INCLUDED_STDLIB_H
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>
#define INCLUDED_STRING_H
#endif

#ifndef _OSL_DIAGNOSE_H_
#include "osl/diagnose.h"
#endif

#ifndef _OSL_FILE_H_
#include <osl/file.h>
#endif

#ifndef _OSL_THREAD_H_
#include "osl/thread.h"
#endif

#ifndef _RTL_USTRING_HXX_
#include "rtl/ustring.hxx"
#endif

#ifndef _OSL_FILE_PATH_HELPER_H_
#include "file_path_helper.h"
#endif

#ifndef _OSL_UUNXAPI_H_
#include "uunxapi.h"
#endif

/***************************************
 CommandArgs_Impl.
 **************************************/
struct CommandArgs_Impl
{
    pthread_mutex_t m_mutex;
    sal_uInt32      m_nCount;
    rtl_uString **  m_ppArgs;
};

static struct CommandArgs_Impl g_command_args =
{
    PTHREAD_MUTEX_INITIALIZER,
    0,
    0
};

/***************************************
  osl_getExecutableFile().
 **************************************/
oslProcessError SAL_CALL osl_getExecutableFile (rtl_uString ** ppustrFile)
{
    oslProcessError result = osl_Process_E_NotFound;

    pthread_mutex_lock (&(g_command_args.m_mutex));
    OSL_ENSURE (g_command_args.m_nCount > 0, "osl_getExecutableFile(): CommandArgs not set.");
    if (g_command_args.m_nCount > 0)
    {
        /* CommandArgs set. Obtain argv[0]. */
        rtl_uString_assign (ppustrFile, g_command_args.m_ppArgs[0]);
        result = osl_Process_E_None;
    }
    pthread_mutex_unlock (&(g_command_args.m_mutex));

    return (result);
}

/***************************************
 osl_getCommandArgCount().
 **************************************/
sal_uInt32 SAL_CALL osl_getCommandArgCount (void)
{
    sal_uInt32 result = 0;

    pthread_mutex_lock (&(g_command_args.m_mutex));
    OSL_ENSURE (g_command_args.m_nCount > 0, "osl_getCommandArgCount(): CommandArgs not set.");
    if (g_command_args.m_nCount > 0)
        result = g_command_args.m_nCount - 1;
    pthread_mutex_unlock (&(g_command_args.m_mutex));

    return (result);
}

/***************************************
 osl_getCommandArg().
 **************************************/
oslProcessError SAL_CALL osl_getCommandArg (sal_uInt32 nArg, rtl_uString ** strCommandArg)
{
    oslProcessError result = osl_Process_E_NotFound;

    pthread_mutex_lock (&(g_command_args.m_mutex));
    OSL_ENSURE (g_command_args.m_nCount > 0, "osl_getCommandArg(): CommandArgs not set.");
    if (g_command_args.m_nCount > (nArg + 1))
    {
        rtl_uString_assign (strCommandArg, g_command_args.m_ppArgs[nArg + 1]);
        result = osl_Process_E_None;
    }
    pthread_mutex_unlock (&(g_command_args.m_mutex));

    return (result);
}

/***************************************
 osl_setCommandArgs().
 **************************************/
void SAL_CALL osl_setCommandArgs (int argc, char ** argv)
{
    pthread_mutex_lock (&(g_command_args.m_mutex));
    OSL_ENSURE (g_command_args.m_nCount == 0, "osl_setCommandArgs(): CommandArgs already set.");
    if (g_command_args.m_nCount == 0)
    {
        rtl_uString** ppArgs = (rtl_uString**)rtl_allocateZeroMemory (argc * sizeof(rtl_uString*));
        if (ppArgs != 0)
        {
            rtl_TextEncoding encoding = osl_getThreadTextEncoding();
            for (int i = 0; i < argc; i++)
            {
                rtl_string2UString (
                    &(ppArgs[i]),
                    argv[i], rtl_str_getLength (argv[i]), encoding,
                    OSTRING_TO_OUSTRING_CVTFLAGS);
            }
            if (ppArgs[0] != 0)
            {
                /* see @ osl_getExecutableFile(). */
                if (rtl_ustr_indexOfChar (rtl_uString_getStr(ppArgs[0]), sal_Unicode('/')) == -1)
                {
                    const rtl::OUString PATH (rtl::OUString::createFromAscii ("PATH"));

                    rtl_uString * pSearchPath = 0;
                    osl_getEnvironment (PATH.pData, &pSearchPath);
                    if (pSearchPath)
                    {
                        rtl_uString * pSearchResult = 0;
                        osl_searchPath (ppArgs[0], pSearchPath, &pSearchResult);
                        if (pSearchResult)
                        {
                            rtl_uString_assign (&(ppArgs[0]), pSearchResult);
                            rtl_uString_release (pSearchResult);
                        }
                        rtl_uString_release (pSearchPath);
                    }
                }

                rtl_uString * pArg0 = 0;
                if (realpath_u (ppArgs[0], &pArg0))
                {
                    osl_getFileURLFromSystemPath (pArg0, &(ppArgs[0]));
                    rtl_uString_release (pArg0);
                }
            }
            g_command_args.m_nCount = argc;
            g_command_args.m_ppArgs = ppArgs;
        }
    }
    pthread_mutex_unlock (&(g_command_args.m_mutex));
}

/***************************************
 osl_getEnvironment().
 **************************************/
oslProcessError SAL_CALL osl_getEnvironment(rtl_uString* pustrEnvVar, rtl_uString** ppustrValue)
{
    oslProcessError  result   = osl_Process_E_NotFound;
    rtl_TextEncoding encoding = osl_getThreadTextEncoding();
    rtl_String* pstr_env_var  = 0;

    OSL_PRECOND(pustrEnvVar, "osl_getEnvironment(): Invalid parameter");
    OSL_PRECOND(ppustrValue, "osl_getEnvironment(): Invalid parameter");

    rtl_uString2String(
        &pstr_env_var,
        rtl_uString_getStr(pustrEnvVar), rtl_uString_getLength(pustrEnvVar), encoding,
        OUSTRING_TO_OSTRING_CVTFLAGS);
    if (pstr_env_var != 0)
    {
        const char* p_env_var = getenv (rtl_string_getStr (pstr_env_var));
        if (p_env_var != 0)
        {
            rtl_string2UString(
                ppustrValue,
                p_env_var, strlen(p_env_var), encoding,
                OSTRING_TO_OUSTRING_CVTFLAGS);
            OSL_ASSERT(*ppustrValue != NULL);

            result = osl_Process_E_None;
        }
        rtl_string_release(pstr_env_var);
    }

    return (result);
}

/***************************************
 osl_getProcessWorkingDir().
 **************************************/
oslProcessError SAL_CALL osl_getProcessWorkingDir(rtl_uString **ppustrWorkingDir)
{
    oslProcessError result = osl_Process_E_Unknown;
    char buffer[PATH_MAX];
    
    OSL_PRECOND(ppustrWorkingDir, "osl_getProcessWorkingDir(): Invalid parameter");

    if (getcwd (buffer, sizeof(buffer)) != 0)
    {
        rtl_uString* ustrTmp = 0;

        rtl_string2UString( 
            &ustrTmp, 
            buffer, strlen(buffer), osl_getThreadTextEncoding(),
            OSTRING_TO_OUSTRING_CVTFLAGS);
        if (ustrTmp != 0)
        {
            if (osl_getFileURLFromSystemPath (ustrTmp, ppustrWorkingDir) == osl_File_E_None)
                result = osl_Process_E_None;
            rtl_uString_release (ustrTmp);
        }
    }

    return (result);
}

/******************************************************************************
 *
 *              new functions to set/return the current process locale
 *
 *****************************************************************************/

struct ProcessLocale_Impl
{
    pthread_mutex_t m_mutex;
    rtl_Locale *    m_pLocale;
};

static struct ProcessLocale_Impl g_process_locale =
{
    PTHREAD_MUTEX_INITIALIZER,
    0
};

extern "C" void _imp_getProcessLocale( rtl_Locale ** );
extern "C" int  _imp_setProcessLocale( rtl_Locale * );

/**********************************************
 osl_getProcessLocale().
 *********************************************/
oslProcessError SAL_CALL osl_getProcessLocale( rtl_Locale ** ppLocale )
{
    OSL_PRECOND(ppLocale, "osl_getProcessLocale(): Invalid parameter.");

    pthread_mutex_lock(&(g_process_locale.m_mutex));

    if (g_process_locale.m_pLocale == 0)
        _imp_getProcessLocale (&(g_process_locale.m_pLocale));
    *ppLocale = g_process_locale.m_pLocale;

    pthread_mutex_unlock (&(g_process_locale.m_mutex));

    return (osl_Process_E_None);
}

/**********************************************
 osl_setProcessLocale().
 *********************************************/
oslProcessError SAL_CALL osl_setProcessLocale( rtl_Locale * pLocale )
{
    oslProcessError result = osl_Process_E_Unknown;

    OSL_PRECOND(pLocale, "osl_setProcessLocale(): Invalid parameter.");

    pthread_mutex_lock(&(g_process_locale.m_mutex));
    if (_imp_setProcessLocale (pLocale) == 0)
    {
        g_process_locale.m_pLocale = pLocale;
        result = osl_Process_E_None;
    }
    pthread_mutex_unlock (&(g_process_locale.m_mutex));

    return (result);
}
