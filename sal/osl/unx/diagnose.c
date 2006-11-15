/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: diagnose.c,v $
 *
 *  $Revision: 1.21 $
 *
 *  last change: $Author: ihi $ $Date: 2006-11-15 12:33:20 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#include "osl/diagnose.h"
#include "system.h"


#ifndef HAVE_DLFCN_H

#if defined(LINUX) || defined(SOLARIS)
#define HAVE_DLFCN_H
#endif  /* LINUX || SOLARIS */

#endif  /* HAVE_DLFCN_H */


#ifdef  HAVE_DLFCN_H

#ifndef INCLUDED_DLFCN_H
#include <dlfcn.h>
#define INCLUDED_DLFCN_H
#endif

#endif  /* HAVE_DLFCN_H */

#ifndef _OSL_THREAD_H_
#include "osl/thread.h"
#endif

#ifndef INCLUDED_PTHREAD_H
#include <pthread.h>
#define INCLUDED_PTHREAD_H
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

/************************************************************************/
/* Internal data structures and functions */
/************************************************************************/

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef pfunc_osl_printDebugMessage oslDebugMessageFunc;
static oslDebugMessageFunc volatile g_pDebugMessageFunc = 0;

typedef pfunc_osl_printDetailedDebugMessage oslDetailedDebugMessageFunc;
static oslDetailedDebugMessageFunc volatile g_pDetailedDebugMessageFunc = 0;

static void osl_diagnose_backtrace_Impl (
    oslDebugMessageFunc f);

static void osl_diagnose_frame_Impl (
    oslDebugMessageFunc f,
    int                 depth,
    void *              pc);

#define OSL_DIAGNOSE_OUTPUTMESSAGE(f, s) \
((f != 0) ? (*(f))((s)) : (void)fprintf(stderr, "%s", (s)))

/************************************************************************/
/* osl_diagnose_frame_Impl */
/************************************************************************/
static void osl_diagnose_frame_Impl (
    oslDebugMessageFunc f,
    int                 depth,
    void *              pc)
{
    const char *fname = 0, *sname = 0;
    void       *fbase = 0, *saddr = 0;
    ptrdiff_t   offset;
    char        szMessage[1024];

#ifdef INCLUDED_DLFCN_H
    Dl_info dli;
    if (dladdr (pc, &dli) != 0)
    {
        fname = dli.dli_fname;
        fbase = dli.dli_fbase;
        sname = dli.dli_sname;
        saddr = dli.dli_saddr;
    }
#endif /* INCLUDED_DLFCN_H */

    if (saddr)
        offset = (ptrdiff_t)(pc) - (ptrdiff_t)(saddr);
    else if (fbase)
        offset = (ptrdiff_t)(pc) - (ptrdiff_t)(fbase);
    else
        offset = (ptrdiff_t)(pc);

    snprintf (szMessage, sizeof(szMessage),
              "Backtrace: [%d] %s: %s+0x%x\n",
              depth,
              fname ? fname : "<unknown>",
              sname ? sname : "???",
              offset);

    OSL_DIAGNOSE_OUTPUTMESSAGE(f, szMessage);
}

/************************************************************************/
/* osl_diagnose_backtrace_Impl */
/************************************************************************/
#if defined(LINUX)

#include <execinfo.h>

#define FRAME_COUNT 64
#define FRAME_OFFSET 1

static void osl_diagnose_backtrace_Impl (oslDebugMessageFunc f)
{
    void * ppFrames[FRAME_COUNT];
    int    i, n;

    n = backtrace (ppFrames, FRAME_COUNT);
    for (i = FRAME_OFFSET; i < n; i++)
    {
        osl_diagnose_frame_Impl (f, (i - FRAME_OFFSET), ppFrames[i]);
    }
}

#elif defined(SOLARIS)

#include <pthread.h>
#include <setjmp.h>
#include <sys/frame.h>

#if defined(SPARC)

#define FRAME_PTR_OFFSET 1
#define FRAME_OFFSET     0

#if defined(__sparcv9)
#define STACK_BIAS 0x7ff
#else
#define STACK_BIAS 0
#endif

#elif defined(INTEL)

#define FRAME_PTR_OFFSET 3
#define FRAME_OFFSET     0
#define STACK_BIAS       0

#endif /* (SPARC || INTEL) */

static void osl_diagnose_backtrace_Impl (oslDebugMessageFunc f)
{
    jmp_buf        ctx;
    long           fpval;
    struct frame * fp;
    int            i;

#if defined(SPARC)
    asm("ta 3");
#endif /* SPARC */
    setjmp (ctx);

    fpval = ((long*)(ctx))[FRAME_PTR_OFFSET];
    fp = (struct frame*)((char*)(fpval) + STACK_BIAS);

    for (i = 0; (i < FRAME_OFFSET) && (fp != 0); i++)
        fp = (struct frame*)((char*)(fp->fr_savfp) + STACK_BIAS);

    for (i = 0; (fp != 0) && (fp->fr_savpc != 0); i++)
    {
        struct frame * prev = (struct frame*)((char*)(fp->fr_savfp) + STACK_BIAS);
        osl_diagnose_frame_Impl (f, i, (void*)(fp->fr_savpc));
        fp = (prev > fp) ? prev : 0;
    }
}

#else  /* (LINUX || SOLARIS) */

static void osl_diagnose_backtrace_Impl (oslDebugMessageFunc f)
{
    /* not yet implemented */
}

#endif /* (LINUX || SOLARIS) */

/************************************************************************/
/* osl_assertFailedLine */
/************************************************************************/
sal_Bool SAL_CALL osl_assertFailedLine (
    const sal_Char* pszFileName,
    sal_Int32       nLine,
    const sal_Char* pszMessage)
{
    oslDebugMessageFunc f = g_pDebugMessageFunc;
    char                szMessage[1024];

    /* If there's a callback for detailed messages, use it */
    if ( g_pDetailedDebugMessageFunc != NULL )
    {
        g_pDetailedDebugMessageFunc( pszFileName, nLine, pszMessage );
        return sal_False;
    }

    /* if SAL assertions are disabled in general, stop here */
    if ( getenv("DISABLE_SAL_DBGBOX") )
        return sal_False;

    /* format message into buffer */
    if (pszMessage != 0)
    {
        snprintf(szMessage, sizeof(szMessage),
                 "Error: File %s, Line %lu: %s\n",
                 pszFileName, nLine, pszMessage);
    }
    else
    {
        snprintf(szMessage, sizeof(szMessage),
                 "Error: File %s, Line %lu\n",
                 pszFileName, nLine);
    }

    /* acquire lock to serialize output message(s) */
    pthread_mutex_lock(&g_mutex);

    /* output message buffer */
    OSL_DIAGNOSE_OUTPUTMESSAGE(f, szMessage);

    /* output backtrace */
    osl_diagnose_backtrace_Impl(f);

    /* release lock and leave, w/o calling osl_breakDebug() */
    pthread_mutex_unlock(&g_mutex);
    return sal_False;
}

/************************************************************************/
/* osl_breakDebug */
/************************************************************************/
void SAL_CALL osl_breakDebug()
{
    exit(0);
}

/************************************************************************/
/* osl_reportError */
/************************************************************************/
sal_Int32 SAL_CALL osl_reportError (
    sal_uInt32      nType,
    const sal_Char* pszMessage)
{
    (void) nType; /* unused */
    fputs(pszMessage, stderr);
    return 0;
}

/************************************************************************/
/* osl_setDebugMessageFunc */
/************************************************************************/
oslDebugMessageFunc SAL_CALL osl_setDebugMessageFunc (
    oslDebugMessageFunc pNewFunc)
{
    oslDebugMessageFunc pOldFunc = g_pDebugMessageFunc;
    g_pDebugMessageFunc = pNewFunc;
    return pOldFunc;
}

/************************************************************************/
/* osl_setDetailedDebugMessageFunc */
/************************************************************************/
pfunc_osl_printDetailedDebugMessage SAL_CALL osl_setDetailedDebugMessageFunc (
    pfunc_osl_printDetailedDebugMessage pNewFunc)
{
    oslDetailedDebugMessageFunc pOldFunc = g_pDetailedDebugMessageFunc;
    g_pDetailedDebugMessageFunc = pNewFunc;
    return pOldFunc;
}

/************************************************************************/
/* osl_trace */
/************************************************************************/
/* comment this define to stop output thread identifier*/
#define OSL_TRACE_THREAD 1
void SAL_CALL osl_trace (
    const sal_Char* lpszFormat, ...)
{
    va_list args;

#if defined(OSL_PROFILING)
    fprintf(stderr, "Time: %06lu : ", osl_getGlobalTimer() );
#else
#if defined(OSL_TRACE_THREAD)
    fprintf(
        stderr, "Thread: %6lu :",
        SAL_INT_CAST(unsigned long, osl_getThreadIdentifier(NULL)));
#else
    fprintf(stderr, "Trace Message: ");
#endif
#endif

    va_start(args, lpszFormat);
    vfprintf(stderr, lpszFormat, args);
    va_end(args);

    fprintf(stderr,"\n");
    fflush(stderr);
}

/************************************************************************/

