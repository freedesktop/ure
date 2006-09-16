/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: transport.cxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-16 16:05:06 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_bridges.hxx"

#include "jni.h"

#include "uno/mapping.hxx"
#include "uno/environment.hxx"
#include "jvmaccess/virtualmachine.hxx"
#include "jvmaccess/unovirtualmachine.hxx"
#include "cppuhelper/implbase1.hxx"

#include "test/java_uno/anytest/XTransport.hpp"
#include "test/java_uno/anytest/DerivedInterface.hpp"


using namespace ::com::sun::star::uno;
using ::test::java_uno::anytest::XTransport;
using ::rtl::OUString;

namespace
{
//==================================================================================================
class Transport : public ::cppu::WeakImplHelper1< XTransport >
{
public:
    virtual Any SAL_CALL mapAny( Any const & any )
        throw (RuntimeException);
};
//__________________________________________________________________________________________________
Any Transport::mapAny( Any const & any )
    throw (RuntimeException)
{
    return any;
}
}

//##################################################################################################
extern "C" JNIEXPORT jobject JNICALL Java_test_java_1uno_anytest_TestJni_create_1jni_1transport(
    JNIEnv * jni_env, jclass, jobject loader )
    SAL_THROW_EXTERN_C()
{
    // publish some idl types
    ::getCppuType( (Reference< XTransport > const *)0 );
    ::getCppuType( (Reference< ::test::java_uno::anytest::DerivedInterface > const *)0 );
    
    Reference< XTransport > xRet( new Transport() );
    
    // get java vm
    JavaVM * java_vm;
    OSL_VERIFY( 0 == jni_env->GetJavaVM( &java_vm ) );
    // create jvmaccess vm
    ::rtl::Reference< ::jvmaccess::UnoVirtualMachine > vm;
    try {
        vm = new ::jvmaccess::UnoVirtualMachine(
            new ::jvmaccess::VirtualMachine(
                java_vm, JNI_VERSION_1_2, false, jni_env ),
            loader );
    } catch ( ::jvmaccess::UnoVirtualMachine::CreationException & ) {
        OSL_ASSERT( false );
        throw;
    }
    // create uno envs
    OUString java_name( RTL_CONSTASCII_USTRINGPARAM(UNO_LB_JAVA) );
    OUString cpp_name( RTL_CONSTASCII_USTRINGPARAM(CPPU_CURRENT_LANGUAGE_BINDING_NAME) );
    Environment java_env, cpp_env;
    uno_getEnvironment( (uno_Environment **)&java_env, java_name.pData, vm.get() );
    OSL_ASSERT( java_env.is() );
    uno_getEnvironment( (uno_Environment **)&cpp_env, cpp_name.pData, 0 );
    OSL_ASSERT( cpp_env.is() );
    
    // map interface
    Mapping mapping( cpp_env.get(), java_env.get() );
    OSL_ASSERT( mapping.is() );
    jobject jo_global = (jobject)mapping.mapInterface( xRet.get(), ::getCppuType( &xRet ) );
    OSL_ASSERT( 0 != jo_global );
    
    // return
    jobject jo_ret = jni_env->NewLocalRef( jo_global );
    jni_env->DeleteGlobalRef( jo_global );
    return jo_ret;
}
