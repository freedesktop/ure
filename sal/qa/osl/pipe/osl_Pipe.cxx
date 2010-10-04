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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sal.hxx"

//------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------

#include "cppunit/TestAssert.h"
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/plugin/TestPlugIn.h"
#include <sal/types.h>
#include <rtl/ustring.hxx>

#include <osl/thread.hxx>

#include <osl/mutex.hxx>

#include <osl/pipe.hxx>
#include <osl/time.h>

#ifdef UNX
#include <unistd.h>
#endif
#include <string.h>

using namespace osl;
using namespace rtl;

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

/** print Boolean value.
 */
inline void printBool( sal_Bool bOk )
{
    printf("#printBool# " );
    ( sal_True == bOk ) ? printf("YES!\n" ): printf("NO!\n" );
}

/** print a UNI_CODE String.
 */
inline void printUString( const ::rtl::OUString & str )
{
    rtl::OString aString;

    printf("#printUString_u# " );
    aString = ::rtl::OUStringToOString( str, RTL_TEXTENCODING_ASCII_US );
    printf("%s\n", aString.getStr( ) );
}

/** print last error of pipe system.
 */
inline void printPipeError( ::osl::Pipe aPipe )
{
    oslPipeError nError = aPipe.getError( );
    printf("#printPipeError# " );
    switch ( nError ) {
    case osl_Pipe_E_None:
        printf("Success!\n" );
        break;
    case osl_Pipe_E_NotFound:
        printf("The returned error is: Not found!\n" );
        break;
    case osl_Pipe_E_AlreadyExists:
        printf("The returned error is: Already exist!\n" );
        break;
    case osl_Pipe_E_NoProtocol:
        printf("The returned error is: No protocol!\n" );
        break;
    case osl_Pipe_E_NetworkReset:
        printf("The returned error is: Network reset!\n" );
        break;
    case osl_Pipe_E_ConnectionAbort:
        printf("The returned error is: Connection aborted!\n" );
        break;
    case osl_Pipe_E_ConnectionReset:
        printf("The returned error is: Connection reset!\n" );
        break;
    case osl_Pipe_E_NoBufferSpace:
        printf("The returned error is: No buffer space!\n" );
        break;
    case osl_Pipe_E_TimedOut:
        printf("The returned error is: Timeout!\n" );
        break;
    case osl_Pipe_E_ConnectionRefused:
        printf("The returned error is: Connection refused!\n" );
        break;
    case osl_Pipe_E_invalidError:
        printf("The returned error is: Invalid error!\n" );
        break;
    default:
        printf("The returned error is: Number %d, Unknown Error\n", nError );
        break;
    }
}



//------------------------------------------------------------------------
// pipe name and transfer contents
//------------------------------------------------------------------------
const rtl::OUString aTestPipeName = rtl::OUString::createFromAscii( "testpipe2" );
const rtl::OUString aTestPipe1 = rtl::OUString::createFromAscii( "testpipe1" );
const rtl::OUString aTestString = rtl::OUString::createFromAscii( "Sun Microsystems" );

const OString m_pTestString1("Sun Microsystems");
const OString m_pTestString2("test pipe PASS/OK");

//------------------------------------------------------------------------
// test code start here
//------------------------------------------------------------------------

namespace osl_Pipe
{

//------------------------------------------------------------------------
// most return value -1 denote a fail of operation.
//------------------------------------------------------------------------
#define OSL_PIPE_FAIL   -1

    /** testing the methods:
        inline Pipe();
        inline Pipe(const ::rtl::OUString& strName, oslPipeOptions Options);
        inline Pipe(const ::rtl::OUString& strName, oslPipeOptions Options,const Security & rSecurity);
        inline Pipe(const Pipe& pipe);
        inline Pipe(oslPipe pipe, __sal_NoAcquire noacquire );
        inline Pipe(oslPipe Pipe);
    */
    class ctors : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void setUp( )
            {
            }

        void tearDown( )
            {
            }

        void ctors_none( )
            {
                ::osl::Pipe aPipe;
                bRes = aPipe.is( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with no parameter, yet no case to test.",
                                        sal_False == bRes );
            }

        void ctors_name_option( )
            {
                /// create a named pipe.
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_CREATE );
                ::osl::Pipe aAssignPipe( aTestPipeName, osl_Pipe_OPEN );

                bRes = aPipe.is( ) && aAssignPipe.is( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with name and option.",
                                        sal_True == bRes );
            }

        void ctors_name_option_security( )
            {
                /// create a security pipe.
                const ::osl::Security rSecurity;
                ::osl::Pipe aSecurityPipe( aTestPipeName, osl_Pipe_CREATE, rSecurity );

                bRes = aSecurityPipe.is( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with name, option and security, the test of security is not implemented yet.",
                                        sal_True == bRes );
            }

        void ctors_copy( )
            {
                /// create a pipe.
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_CREATE );
                /// create a pipe using copy constructor.
                ::osl::Pipe aCopyPipe( aPipe );

                bRes = aCopyPipe.is( ) && aCopyPipe == aPipe;

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test copy constructor.",
                                        sal_True == bRes );
            }

        /**  tester comment:

        When test the following two constructors, don't know how to test the
        acquire and no acquire action. possible plans:
        1.release one handle and check the other( did not success since the
        other still exist and valid. )
        2. release one handle twice to see getLastError( )(the getLastError
        always returns invalidError(LINUX)).
        */

        void ctors_no_acquire( )
            {
                /// create a pipe.
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_CREATE );
                /// constructs a pipe reference without acquiring the handle.
                ::osl::Pipe aNoAcquirePipe( aPipe.getHandle( ), SAL_NO_ACQUIRE );

                bRes = aNoAcquirePipe.is( );
                ///aPipe.clear( );
                ///bRes1 = aNoAcquirePipe.is( );


                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with no aquire of handle, only validation test, do not know how to test no acquire.",
                                        sal_True == bRes );
            }

        void ctors_acquire( )
            {
                /// create a base pipe.
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_CREATE );
                /// constructs two pipes without acquiring the handle on the base pipe.
                ::osl::Pipe aAcquirePipe( aPipe.getHandle( ) );
                ::osl::Pipe aAcquirePipe1( NULL );

                bRes = aAcquirePipe.is( );
                bRes1 = aAcquirePipe1.is( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with no aquire of handle.only validation test, do not know how to test no acquire.",
                                        sal_True == bRes && sal_False == bRes1 );
            }

        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_none );
        CPPUNIT_TEST( ctors_name_option );
        CPPUNIT_TEST( ctors_name_option_security );
        CPPUNIT_TEST( ctors_copy );
        CPPUNIT_TEST( ctors_no_acquire );
        CPPUNIT_TEST( ctors_acquire );
        CPPUNIT_TEST_SUITE_END( );
    }; // class ctors


    /** testing the method:
        inline sal_Bool SAL_CALL is() const;
    */
    class is : public CppUnit::TestFixture
    {
    public:
        void is_001( )
            {
                ::osl::Pipe aPipe;

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test is(), check if the pipe is a valid one.", sal_False == aPipe.is( ) );
            }

        void is_002( )
            {
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_CREATE );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test is(), a normal pipe creation.", sal_True == aPipe.is( ) );
            }

        void is_003( )
            {
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_CREATE );
                aPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test is(), an invalid case.", sal_False == aPipe.is( ) );
            }

        void is_004( )
            {
                ::osl::Pipe aPipe( NULL );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test is(), an invalid constructor.", sal_False == aPipe.is( ) );
            }

        CPPUNIT_TEST_SUITE( is );
        CPPUNIT_TEST( is_001 );
        CPPUNIT_TEST( is_002 );
        CPPUNIT_TEST( is_003 );
        CPPUNIT_TEST( is_004 );
        CPPUNIT_TEST_SUITE_END( );
    }; // class is


    /** testing the methods:
        inline sal_Bool create( const ::rtl::OUString & strName,
        oslPipeOptions Options, const Security &rSec );
        nline sal_Bool create( const ::rtl::OUString & strName,
        oslPipeOptions Options = osl_Pipe_OPEN );
    */
    class create : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        /**  tester comment:

        security create only be tested creation, security section is
        untested yet.
        */

        void create_named_security_001( )
            {
                const Security rSec;
                ::osl::Pipe aPipe;
                bRes = aPipe.create( aTestPipeName, osl_Pipe_CREATE, rSec );
                bRes1 = aPipe.create( aTestPipeName, osl_Pipe_CREATE, rSec );
                aPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test creation.",
                                        sal_True == bRes && sal_False == bRes1);
            }

        void create_named_security_002( )
            {
                const Security rSec;
                ::osl::Pipe aPipe, aPipe1;
                bRes = aPipe.create( aTestPipeName, osl_Pipe_CREATE, rSec );
                bRes1 = aPipe1.create( aTestPipeName, osl_Pipe_OPEN, rSec );
                aPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test creation and open.",
                                        sal_True == bRes && sal_True == bRes1);
            }

        void create_named_001( )
            {
                ::osl::Pipe aPipe;
                bRes = aPipe.create( aTestPipeName, osl_Pipe_CREATE );
                bRes1 = aPipe.create( aTestPipeName, osl_Pipe_CREATE );
                aPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test creation.",
                                        sal_True == bRes && sal_False == bRes1);
            }

        void create_named_002( )
            {
                ::osl::Pipe aPipe, aPipe1;
                bRes = aPipe.create( aTestPipeName, osl_Pipe_CREATE );
                bRes1 = aPipe1.create( aTestPipeName, osl_Pipe_OPEN );
                aPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test creation and open.",
                                        sal_True == bRes && sal_True == bRes1);
            }

        void create_named_003( )
            {
                ::osl::Pipe aPipe;
                bRes = aPipe.create( aTestPipeName );
                aPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test default option is open.",
                                        sal_False == bRes );
            }

        CPPUNIT_TEST_SUITE( create );
        CPPUNIT_TEST( create_named_security_001 );
        CPPUNIT_TEST( create_named_security_002 );
        CPPUNIT_TEST( create_named_001 );
        CPPUNIT_TEST( create_named_002 );
        CPPUNIT_TEST( create_named_003 );
        CPPUNIT_TEST_SUITE_END( );
    }; // class create


    /** testing the method:
        inline void SAL_CALL clear();
    */
    class clear : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void clear_001( )
            {
                ::osl::Pipe aPipe;
                aPipe.create( aTestPipeName, osl_Pipe_CREATE );
                aPipe.clear( );
                bRes = aPipe.is( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test clear.",
                                        sal_False == bRes );
            }

        CPPUNIT_TEST_SUITE( clear );
        CPPUNIT_TEST( clear_001 );
        CPPUNIT_TEST_SUITE_END( );
    }; // class clear


    /** testing the methods:
        inline Pipe& SAL_CALL operator= (const Pipe& pipe);
        inline Pipe& SAL_CALL operator= (const oslPipe pipe );
    */
    class assign : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void assign_ref( )
            {
                ::osl::Pipe aPipe, aPipe1;
                aPipe.create( aTestPipeName, osl_Pipe_CREATE );
                aPipe1 = aPipe;
                bRes = aPipe1.is( );
                bRes1 = aPipe == aPipe1;
                aPipe.close( );
                aPipe1.close( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test assign with reference.",
                                        sal_True == bRes && sal_True == bRes1 );
            }

        void assign_handle( )
            {
                ::osl::Pipe aPipe, aPipe1;
                aPipe.create( aTestPipeName, osl_Pipe_CREATE );
                aPipe1 = aPipe.getHandle( );
                bRes = aPipe1.is( );
                bRes1 = aPipe == aPipe1;
                aPipe.close( );
                aPipe1.close( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test assign with handle.",
                                        sal_True == bRes && sal_True == bRes1 );
            }

        CPPUNIT_TEST_SUITE( assign );
        CPPUNIT_TEST( assign_ref );
        CPPUNIT_TEST( assign_handle );
        CPPUNIT_TEST_SUITE_END( );
    }; // class assign


    /** testing the method:
        inline sal_Bool SAL_CALL isValid() const;
        isValid( ) has not been implemented under the following platforms, please refer to osl/pipe.hxx
    */
    /*class isValid : public CppUnit::TestFixture
      {
      public:
      sal_Bool bRes, bRes1;

      void isValid_001( )
      {
      CPPUNIT_ASSERT_MESSAGE( "#test comment#: isValid() has not been implemented on all platforms.",
      sal_False );
      }

      CPPUNIT_TEST_SUITE( isValid );
      CPPUNIT_TEST( isValid_001 );
      CPPUNIT_TEST_SUITE_END( );
      };*/ // class isValid


    /** testing the method:
        inline sal_Bool SAL_CALL operator==( const Pipe& rPipe ) const;
    */
    class isEqual : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void isEqual_001( )
            {
                ::osl::Pipe aPipe;
                aPipe.create( aTestPipeName, osl_Pipe_CREATE );
                bRes  = aPipe == aPipe;
                aPipe.close( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test isEqual(), compare its self.",
                                        sal_True == bRes );
            }

        void isEqual_002( )
            {
                ::osl::Pipe aPipe, aPipe1, aPipe2;
                aPipe.create( aTestPipeName, osl_Pipe_CREATE );

                aPipe1 = aPipe;
                aPipe2.create( aTestPipeName, osl_Pipe_CREATE );

                bRes  = aPipe == aPipe1;
                bRes1 = aPipe == aPipe2;
                aPipe.close( );
                aPipe1.close( );
                aPipe2.close( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test isEqual(),create one copy instance, and compare.",
                                        sal_True == bRes && sal_False == bRes1 );
            }

        CPPUNIT_TEST_SUITE( isEqual );
        CPPUNIT_TEST( isEqual_001 );
        CPPUNIT_TEST( isEqual_002 );
        CPPUNIT_TEST_SUITE_END( );
    }; // class isEqual


    /** testing the method:
        inline void SAL_CALL close();
    */
    class close : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void close_001( )
            {
                ::osl::Pipe aPipe( aTestPipe1, osl_Pipe_CREATE );
                aPipe.close( );
                bRes = aPipe.is( );

                aPipe.clear( );
                bRes1 = aPipe.is( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: difference between close and clear.",
                                        sal_True == bRes && sal_False == bRes1);
            }

        void close_002( )
            {
                ::osl::StreamPipe aPipe( aTestPipe1, osl_Pipe_CREATE );
                aPipe.close( );
                int nRet = aPipe.send( m_pTestString1.getStr(), 3 );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: use after close.",
                                        OSL_PIPE_FAIL == nRet );
            }

        CPPUNIT_TEST_SUITE( close );
        CPPUNIT_TEST( close_001 );
        CPPUNIT_TEST( close_002 );
        CPPUNIT_TEST_SUITE_END( );
    }; // class close


    /** testing the method:
        inline oslPipeError SAL_CALL accept(StreamPipe& Connection);
        please refer to StreamPipe::recv
    */
   /* class accept : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void accept_001( )
            {

                // CPPUNIT_ASSERT_MESSAGE( "#test comment#: accept, untested.", 1 == 1 );
                //CPPUNIT_ASSERT_STUB();
            }

        CPPUNIT_TEST_SUITE( accept );
        CPPUNIT_TEST( accept_001 );
        CPPUNIT_TEST_SUITE_END( );
    };*/ // class accept


    /** testing the method:
        inline oslPipeError SAL_CALL getError() const;
    */
    class getError : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;
        /*
          PipeError[]= {
          { 0,                     osl_Pipe_E_None                      },      // no error
          { EPROTOTYPE,    osl_Pipe_E_NoProtocol            },  // Protocol wrong type for socket
          { ENOPROTOOPT,           osl_Pipe_E_NoProtocol            },  // Protocol not available
          { EPROTONOSUPPORT, osl_Pipe_E_NoProtocol              },      // Protocol not supported
          { ESOCKTNOSUPPORT, osl_Pipe_E_NoProtocol              },      // Socket type not supported
          { EPFNOSUPPORT,          osl_Pipe_E_NoProtocol        },      // Protocol family not supported
          { EAFNOSUPPORT,          osl_Pipe_E_NoProtocol        },      // Address family not supported by
          // protocol family
          { ENETRESET,     osl_Pipe_E_NetworkReset              },      // Network dropped connection because
          // of reset
          { ECONNABORTED,          osl_Pipe_E_ConnectionAbort   },      // Software caused connection abort
          { ECONNRESET,    osl_Pipe_E_ConnectionReset   },      // Connection reset by peer
          { ENOBUFS,       osl_Pipe_E_NoBufferSpace     },      // No buffer space available
          { ETIMEDOUT,     osl_Pipe_E_TimedOut                  },      // Connection timed out
          { ECONNREFUSED,          osl_Pipe_E_ConnectionRefused },      // Connection refused
          { -1,            osl_Pipe_E_invalidError              }
          };
          did not define osl_Pipe_E_NotFound, osl_Pipe_E_AlreadyExists
        */

        void getError_001( )
            {
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_OPEN );
                oslPipeError nError = aPipe.getError( );
                printPipeError( aPipe );
                aPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: open a non-exist pipe.",
                                        nError != osl_Pipe_E_None );
            }

        void getError_002( )
            {
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_CREATE );
                ::osl::Pipe aPipe1( aTestPipeName, osl_Pipe_CREATE );
                oslPipeError nError = aPipe.getError( );
                printPipeError( aPipe );
                aPipe.clear( );
                aPipe1.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: create an already exist pipe.",
                                        nError != osl_Pipe_E_None );
            }

        CPPUNIT_TEST_SUITE( getError );
        CPPUNIT_TEST( getError_001 );
        CPPUNIT_TEST( getError_002 );
        CPPUNIT_TEST_SUITE_END( );
    }; // class getError


    /** testing the method:
        inline oslPipe SAL_CALL getHandle() const;
    */
    class getHandle : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void getHandle_001( )
            {
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_OPEN );
                bRes = aPipe == aPipe.getHandle( );
                aPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: one pipe should equal to its handle.",
                                        sal_True == bRes );
            }

        void getHandle_002( )
            {
                ::osl::Pipe aPipe( aTestPipeName, osl_Pipe_CREATE );
                ::osl::Pipe aPipe1( aPipe.getHandle( ) );
                bRes = aPipe == aPipe1;
                aPipe.clear( );
                aPipe1.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: one pipe derived from another pipe's handle.",
                                        sal_True == bRes );
            }

        CPPUNIT_TEST_SUITE( getHandle );
        CPPUNIT_TEST( getHandle_001 );
        CPPUNIT_TEST( getHandle_002 );
        CPPUNIT_TEST_SUITE_END( );
    }; // class getHandle


// -----------------------------------------------------------------------------
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::ctors);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::is);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::create);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::clear);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::assign);
//CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::isValid);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::isEqual);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::close);
    //CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::accept);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::getError);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_Pipe::getHandle);
// -----------------------------------------------------------------------------

} // namespace osl_Pipe


namespace osl_StreamPipe
{

    /** testing the methods:
        inline StreamPipe();
        inline StreamPipe(oslPipe Pipe);;
        inline StreamPipe(const StreamPipe& Pipe);
        inline StreamPipe(const ::rtl::OUString& strName, oslPipeOptions Options = osl_Pipe_OPEN);
        inline StreamPipe(const ::rtl::OUString& strName, oslPipeOptions Options, const Security &rSec );
        inline StreamPipe( oslPipe pipe, __sal_NoAcquire noacquire );
    */
    class ctors : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void ctors_none( )
            {
                // create a pipe.
                ::osl::StreamPipe aStreamPipe( aTestPipeName, osl_Pipe_CREATE );
                // create an unattached pipe.
                ::osl::StreamPipe aStreamPipe1;
                bRes  = aStreamPipe1.is( );

                // assign it and check.
                aStreamPipe1 = aStreamPipe;
                bRes1 = aStreamPipe1.is( );
                aStreamPipe.clear( );
                aStreamPipe1.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with no parameter, before and after assign.",
                                        sal_False == bRes && sal_True == bRes1 );
            }

        void ctors_handle( )
            {
                // create a pipe.
                ::osl::StreamPipe aStreamPipe( aTestPipeName, osl_Pipe_CREATE );
                // create a pipe with last handle.
                ::osl::StreamPipe aStreamPipe1( aStreamPipe.getHandle( ) );
                bRes  = aStreamPipe1.is( ) && aStreamPipe == aStreamPipe1;
                aStreamPipe.clear( );
                aStreamPipe1.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with other's handle.",
                                        sal_True == bRes );
            }

        void ctors_copy( )
            {
                // create a pipe.
                ::osl::StreamPipe aStreamPipe( aTestPipeName, osl_Pipe_CREATE );
                // create an unattached pipe.
                ::osl::StreamPipe aStreamPipe1( aStreamPipe );
                bRes  = aStreamPipe1.is( ) && aStreamPipe == aStreamPipe1;
                aStreamPipe.clear( );
                aStreamPipe1.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test copy constructor.",
                                        sal_True == bRes );
            }

        void ctors_name_option( )
            {
                // create a pipe.
                ::osl::StreamPipe aStreamPipe( aTestPipeName, osl_Pipe_CREATE );
                // create an unattached pipe.
                ::osl::StreamPipe aStreamPipe1( aTestPipeName, osl_Pipe_OPEN );
                bRes  = aStreamPipe1.is( ) && aStreamPipe.is( );
                aStreamPipe.clear( );
                aStreamPipe1.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with name and option.",
                                        sal_True == bRes );
            }

        void ctors_name_option_security( )
            {
                /// create a security pipe.
                const ::osl::Security rSecurity;
                ::osl::StreamPipe aSecurityPipe( aTestPipeName, osl_Pipe_CREATE, rSecurity );

                bRes = aSecurityPipe.is( );
                aSecurityPipe.clear( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with name, option and security, the test of security is not implemented yet.",
                                        sal_True == bRes );
            }

        /**  tester comment:

        When test the following constructor, don't know how to test the
        acquire and no acquire action. possible plans:
        1.release one handle and check the other( did not success since the
        other still exist and valid. )
        2. release one handle twice to see getLastError( )(the getLastError
        always returns invalidError(LINUX)).
        */

        void ctors_no_acquire( )
            {
                // create a pipe.
                ::osl::StreamPipe aPipe( aTestPipeName, osl_Pipe_CREATE );
                // constructs a pipe reference without acquiring the handle.
                ::osl::StreamPipe aNoAcquirePipe( aPipe.getHandle( ), SAL_NO_ACQUIRE );

                bRes = aNoAcquirePipe.is( );
                aPipe.clear( );
                // bRes1 = aNoAcquirePipe.is( );

                CPPUNIT_ASSERT_MESSAGE( "#test comment#: test constructor with no aquire of handle, only validation test, do not know how to test no acquire.",
                                        sal_True == bRes );
            }

        CPPUNIT_TEST_SUITE( ctors );
        CPPUNIT_TEST( ctors_none );
        CPPUNIT_TEST( ctors_handle );
        CPPUNIT_TEST( ctors_copy );
        CPPUNIT_TEST( ctors_name_option );
        CPPUNIT_TEST( ctors_name_option_security );
        CPPUNIT_TEST( ctors_no_acquire );
        CPPUNIT_TEST_SUITE_END( );
    }; // class ctors


    /** testing the methods:
        inline StreamPipe & SAL_CALL operator=(oslPipe Pipe);
        inline StreamPipe& SAL_CALL operator=(const Pipe& pipe);
        mindy: not implementated in osl/pipe.hxx, so remove the cases
    */
    /*
       class assign : public CppUnit::TestFixture
       {
       public:
       sal_Bool bRes, bRes1;

       void assign_ref( )
       {
       ::osl::StreamPipe aPipe, aPipe1;
       aPipe.create( aTestPipeName, osl_Pipe_CREATE );
       aPipe1 = aPipe;
       bRes = aPipe1.is( );
       bRes1 = aPipe == aPipe1;
       aPipe.close( );
       aPipe1.close( );

       CPPUNIT_ASSERT_MESSAGE( "#test comment#: test assign with reference.",
       sal_True == bRes && sal_True == bRes1 );
       }

       void assign_handle( )
       {
       ::osl::StreamPipe * pPipe = new ::osl::StreamPipe( aTestPipeName, osl_Pipe_CREATE );
       ::osl::StreamPipe * pAssignPipe = new ::osl::StreamPipe;
       *pAssignPipe = pPipe->getHandle( );

       bRes = pAssignPipe->is( );
       bRes1 = ( *pPipe == *pAssignPipe );
       pPipe->close( );

       delete pAssignPipe;

       CPPUNIT_ASSERT_MESSAGE( "#test comment#: test assign with handle., seems not implemented under (LINUX)(W32)",
       sal_True == bRes && sal_True == bRes1  );
       }

       CPPUNIT_TEST_SUITE( assign );
       CPPUNIT_TEST( assign_ref );
       CPPUNIT_TEST( assign_handle );
       CPPUNIT_TEST_SUITE_END( );
       };*/ // class assign


    /** wait _nSec seconds.
     */
    void thread_sleep( sal_Int32 _nSec )
    {
        /// print statement in thread process must use fflush() to force display.
        // printf("wait %d seconds. ", _nSec );
        fflush(stdout);

#ifdef WNT                               //Windows
        Sleep( _nSec * 1000 );
#endif
#if ( defined UNX ) || ( defined OS2 )   //Unix
        sleep( _nSec );
#endif
        // printf("done\n" );
    }
    // test read/write & send/recv data to pipe
    // -----------------------------------------------------------------------------

    class Pipe_DataSink_Thread : public Thread
    {
    public:
        sal_Char buf[256];
        Pipe_DataSink_Thread( ) { }

        ~Pipe_DataSink_Thread( )
            {
            }
    protected:
        void SAL_CALL run( )
            {
                sal_Int32 nChars = 0;

                printf("open pipe\n");
                ::osl::StreamPipe aSenderPipe( aTestPipeName, osl_Pipe_OPEN );  // aTestPipeName is a string = "TestPipe"
                if ( aSenderPipe.is() == sal_False )
                {
                    printf("pipe open failed! \n");
                }
                else
                {
                    printf("read\n");
                    nChars = aSenderPipe.read( buf, m_pTestString1.getLength() + 1 );
                    if ( nChars < 0 )
                    {
                        printf("read failed! \n");
                        return;
                    }
                    printf("buffer is %s \n", buf);
                    printf("send\n");
                    nChars = aSenderPipe.send( m_pTestString2.getStr(), m_pTestString2.getLength() + 1 );
                    if ( nChars < 0 )
                    {
                        printf("client send failed! \n");
                        return;
                    }
                }
            }

    };

    // -----------------------------------------------------------------------------

    class Pipe_DataSource_Thread : public Thread
    {
    public:
        sal_Char buf[256];
        //::osl::StreamPipe aListenPipe;  //( aTestPipeName, osl_Pipe_CREATE );
        ::osl::Pipe aListenPipe;
        ::osl::StreamPipe aConnectionPipe;
        Pipe_DataSource_Thread( )
            {
                printf("create pipe\n");
                aListenPipe.create( aTestPipeName, osl_Pipe_CREATE );
            }
        ~Pipe_DataSource_Thread( )
            {
                aListenPipe.close();
            }
    protected:
        void SAL_CALL run( )
            {
                //create pipe.
                sal_Int32 nChars;
                //::osl::StreamPipe aListenPipe( aTestPipeName, osl_Pipe_CREATE );
                printf("listen\n");
                if ( aListenPipe.is() == sal_False )
                {
                    printf("pipe create failed! \n");
                }
                else
                {
                    //::osl::StreamPipe aConnectionPipe;

                    //start server and wait for connection.
                    printf("accept\n");
                    if ( osl_Pipe_E_None != aListenPipe.accept( aConnectionPipe ) )
                    {
                        printf("pipe accept failed!");
                        return;
                    }
                    printf("write\n");
                    // write to pipe
                    nChars = aConnectionPipe.write( m_pTestString1.getStr(), m_pTestString1.getLength() + 1 );
                    if ( nChars < 0)
                    {
                        printf("server write failed! \n");
                        return;
                    }
                    printf("recv\n");
                    nChars = aConnectionPipe.recv( buf, 256 );

                    if ( nChars < 0)
                    {
                        printf("server receive failed! \n");
                        return;
                    }
                    //thread_sleep( 2 );
                    printf("received message is: %s\n", buf );
                    //aConnectionPipe.close();
                }
            }
    };

    /** testing the method: read/write/send/recv and Pipe::accept
     */
    class recv : public CppUnit::TestFixture
    {
    public:
        sal_Bool bRes, bRes1;

        void recv_001( )
            {
                //launch threads.
                Pipe_DataSource_Thread myDataSourceThread;
                Pipe_DataSink_Thread myDataSinkThread;
                myDataSourceThread.create( );
                thread_sleep( 1 );
                myDataSinkThread.create( );

                //wait until the thread terminate
                myDataSinkThread.join( );
                myDataSourceThread.join( );

                int nCompare1 = strcmp( myDataSinkThread.buf, m_pTestString1.getStr() );
                int nCompare2 = strcmp( myDataSourceThread.buf, m_pTestString2.getStr() );
                CPPUNIT_ASSERT_MESSAGE( "test send/recv/write/read.", nCompare1 == 0 && nCompare2 == 0 );
            }
        //close pipe when accept
        void recv_002()
            {
                thread_sleep( 1 );

                Pipe_DataSource_Thread myDataSourceThread;
                Pipe_DataSink_Thread myDataSinkThread;
                myDataSourceThread.create( );
                thread_sleep( 1 );
                myDataSourceThread.aListenPipe.close();
                myDataSourceThread.join( );
                //no condition judgement here, if the case could finish excuting within 1 or 2 seconds, it passes.
            }

        CPPUNIT_TEST_SUITE( recv );
        CPPUNIT_TEST( recv_001 );
        CPPUNIT_TEST( recv_002 );
        CPPUNIT_TEST_SUITE_END( );
    }; // class recv

// -----------------------------------------------------------------------------
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_StreamPipe::ctors);
//CPPUNIT_TEST_SUITE_REGISTRATION(osl_StreamPipe::assign);
    CPPUNIT_TEST_SUITE_REGISTRATION(osl_StreamPipe::recv);
// -----------------------------------------------------------------------------

} // namespace osl_StreamPipe

CPPUNIT_PLUGIN_IMPLEMENT();
