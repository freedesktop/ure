/*************************************************************************
 *
 *  $RCSfile: interactionhandler.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: jl $ $Date: 2002-07-23 14:02:28 $
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


#include <jni.h>

//#include <iostream>
#include <stdio.h>
#include <rtl/process.h>

#include <vos/dynload.hxx>

#include <cppuhelper/servicefactory.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/bootstrap.hxx>

#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/java/XJavaVM.hpp>
#include <com/sun/star/registry/XImplementationRegistration.hpp>
#include <com/sun/star/java/XJavaThreadRegister_11.hpp>

#include <com/sun/star/uno/XCurrentContext.hpp>
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <com/sun/star/task/XInteractionRequest.hpp>
#include <com/sun/star/task/XInteractionContinuation.hpp>
#include <com/sun/star/task/XInteractionAbort.hpp>
#include <com/sun/star/task/XInteractionRetry.hpp>
#include <com/sun/star/java/JavaNotConfiguredException.hpp>
#include <com/sun/star/java/MissingJavaRuntimeException.hpp>
#include <com/sun/star/java/JavaDisabledException.hpp>
#include <com/sun/star/java/JavaVMCreationFailureException.hpp>
#include <cppuhelper/implbase1.hxx>
#include <uno/current_context.hxx>
using namespace std;
using namespace rtl;
using namespace cppu;
using namespace vos;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
//using namespace com::sun::star::reflection;
using namespace com::sun::star::lang;
using namespace com::sun::star::registry;
using namespace com::sun::star::java;
using namespace com::sun::star::task;

#if defined(WNT)
#define LIBJEN    "jen.dll"
#elif defined(UNX)
#define LIBJEN    "libjen.so"
#endif

#define OUSTR( x ) OUString(RTL_CONSTASCII_USTRINGPARAM( x ))
#define INTERACTION_HANDLER_NAME "java-vm.interaction-handler"

class Context: public WeakImplHelper1<XCurrentContext>
{
    virtual Any SAL_CALL getValueByName( const OUString& Name ) throw (RuntimeException);
};

class InteractionHandler: public WeakImplHelper1<XInteractionHandler>
{
    virtual void SAL_CALL handle( const Reference< XInteractionRequest >& Request )
        throw (RuntimeException);
};

Any SAL_CALL Context::getValueByName( const OUString& Name) throw (RuntimeException)
{
    Any retVal;
    if( Name.equals( OUSTR(INTERACTION_HANDLER_NAME)))
    {
        Reference<XInteractionHandler> handler( static_cast<XWeak*>(new InteractionHandler()),
                                                UNO_QUERY);
        retVal <<= handler;
    }
    return retVal;
}

void SAL_CALL InteractionHandler::handle( const Reference< XInteractionRequest >& Request )
        throw (RuntimeException)
{
    Any anyExc= Request->getRequest();
    Sequence<Reference< XInteractionContinuation> >seqCont= Request->getContinuations();

    Reference<XInteractionAbort> abort;
    Reference<XInteractionRetry> retry;

    for (sal_Int32 i= 0; i < seqCont.getLength(); i++)
    {
        abort= Reference<XInteractionAbort>::query( seqCont[i]);
        if(abort.is())
            break;
    }
    for ( i= 0; i < seqCont.getLength(); i++)
    {
        retry= Reference<XInteractionRetry>::query( seqCont[i]);
        if(retry.is())
            break;
    }
    
    if( abort.is())
        abort->select();

     if( retry.is())
         retry->select();
}

sal_Bool test1(const Reference< XMultiServiceFactory > & xMgr )
{
    setCurrentContext( Reference<XCurrentContext>( static_cast<XWeak*>(new Context()), UNO_QUERY)); 

      OUString sVMService( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.java.JavaVirtualMachine"));
    Reference<XInterface> xXInt= xMgr->createInstance(sVMService);
    if( ! xXInt.is())
        return sal_False;
    Reference<XJavaVM> xVM( xXInt, UNO_QUERY);
    if( ! xVM.is()) 
        return sal_False;
    Reference<XJavaThreadRegister_11> xreg11(xVM, UNO_QUERY);
    if( ! xreg11.is())
        return sal_False;


    sal_Int8 arId[16];
    rtl_getGlobalProcessId((sal_uInt8*) arId);
    Any anyVM;
    try
    {
        anyVM = xVM->getJavaVM( Sequence<sal_Int8>(arId, 16));
    }
    catch (JavaNotConfiguredException& e)
    {
        printf("JavaNotConfiguredException: %S",(const sal_Unicode*) e.Message);
    }
    catch (JavaVMCreationFailureException& e)
    {
        printf("JavaVMCreationFailureException: %S",e.Message.getStr());
    }
    catch (MissingJavaRuntimeException& e)
    {
        printf("MissingJavaRuntimeException: %S",e.Message.getStr());
    }
    catch (JavaDisabledException& e)
    {
        printf("JavaDisabledException: %S",e.Message.getStr());
    }
    
    JavaVM* _jvm= *(JavaVM**) anyVM.getValue();
    if( _jvm)
    {
        xreg11->registerThread();
    
        JNIEnv *p_env;

        if( _jvm->AttachCurrentThread((void**) &p_env, 0)) 
        return sal_False;



        _jvm->DetachCurrentThread();
    }
    xreg11->revokeThread();
    return sal_True;
}

extern
#if (defined UNX) || (defined OS2)
int main( int argc, char * argv[] )
#else
int __cdecl main( int argc, char * argv[] )
#endif
{
    Reference<XSimpleRegistry> xreg= createSimpleRegistry();
    xreg->open( OUString( RTL_CONSTASCII_USTRINGPARAM("applicat.rdb")),
                               sal_False, sal_False );

    Reference< XComponentContext > context= bootstrap_InitialComponentContext(xreg);
    Reference<XMultiComponentFactory> fac= context->getServiceManager();
    Reference<XMultiServiceFactory> xMgr( fac, UNO_QUERY);
    
    sal_Bool bSucc = sal_False;
    try
    {
        bSucc= test1(xMgr);
    }
    catch (Exception & rExc)
    {
        OSL_ENSURE( sal_False, "### exception occured!" );
        OString aMsg( OUStringToOString( rExc.Message, RTL_TEXTENCODING_ASCII_US ) );
        OSL_TRACE( "### exception occured: " );
        OSL_TRACE( aMsg.getStr() );
        OSL_TRACE( "\n" );
    }

    Reference< XComponent > xCompContext( context, UNO_QUERY );
    xCompContext->dispose();
    printf("javavm %s", bSucc ? "succeeded" : "failed");
    return (bSucc ? 0 : -1);
}


