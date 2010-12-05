/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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
#include "precompiled_cli_ure.hxx"
#include "typelib/typedescription.h"
#include "rtl/ustrbuf.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "osl/mutex.hxx"
#include "cli_proxy.h"
#include "cli_base.h"
#include "cli_bridge.h"

#using <mscorlib.dll>
#using <cli_ure.dll>
#using <cli_uretypes.dll>

namespace sr = System::Reflection;
namespace st = System::Text;
namespace sre = System::Reflection::Emit;
namespace sc = System::Collections;
namespace srrm = System::Runtime::Remoting::Messaging;
namespace srr = System::Runtime::Remoting;
namespace srrp = System::Runtime::Remoting::Proxies;
namespace sri = System::Runtime::InteropServices;
namespace sd = System::Diagnostics;
namespace css = com::sun::star;
namespace ucss = unoidl::com::sun::star;

using namespace cli_uno;
using namespace rtl;
extern "C"
{
//------------------------------------------------------------------------------
void SAL_CALL cli_proxy_free( uno_ExtEnvironment * env, void * proxy )
    SAL_THROW_EXTERN_C();
//------------------------------------------------------------------------------
void SAL_CALL cli_proxy_acquire( uno_Interface * pUnoI )
    SAL_THROW_EXTERN_C();
//------------------------------------------------------------------------------
void SAL_CALL cli_proxy_release( uno_Interface * pUnoI )
    SAL_THROW_EXTERN_C();
//------------------------------------------------------------------------------
void SAL_CALL cli_proxy_dispatch(
    uno_Interface * pUnoI, typelib_TypeDescription const * member_td,
    void * uno_ret, void * uno_args[], uno_Any ** uno_exc )
    SAL_THROW_EXTERN_C();


}

namespace cli_uno
{

UnoInterfaceInfo::UnoInterfaceInfo(Bridge const * bridge, uno_Interface* unoI,
                                   typelib_InterfaceTypeDescription* td):

    m_unoI(unoI),
    m_typeDesc(td),
    m_bridge(bridge)
{
    m_bridge->acquire();
    m_type = mapUnoType(reinterpret_cast<typelib_TypeDescription*>(td));
    m_unoI->acquire(m_unoI);
    typelib_typedescription_acquire(&m_typeDesc->aBase);
       if ( ! m_typeDesc->aBase.bComplete)
    {
        typelib_TypeDescription* _pt = &m_typeDesc->aBase;
        sal_Bool bComplete = ::typelib_typedescription_complete( & _pt);
        if( ! bComplete)
        {
            OUStringBuffer buf( 128 );
            buf.appendAscii(RTL_CONSTASCII_STRINGPARAM(
                                 "cannot make type complete: ") );
            buf.append( *reinterpret_cast< OUString const * >(
                            & m_typeDesc->aBase.pTypeName));
            throw BridgeRuntimeError(buf.makeStringAndClear());
        }
    }
}
UnoInterfaceInfo::~UnoInterfaceInfo()
{
    //accessing unmanaged objects is ok.
   m_bridge->m_uno_env->revokeInterface(
            m_bridge->m_uno_env, m_unoI );
   m_bridge->release();

    m_unoI->release(m_unoI);
    typelib_typedescription_release(
        reinterpret_cast<typelib_TypeDescription*>(m_typeDesc));
}

UnoInterfaceProxy::UnoInterfaceProxy(
    Bridge * bridge,
    uno_Interface * pUnoI,
    typelib_InterfaceTypeDescription* pTD,
    const OUString& oid )
    :RealProxy(__typeof(MarshalByRefObject)),
     m_bridge(bridge),
     m_oid(mapUnoString(oid.pData)),
     m_sTypeName(m_system_Object_String)
{
    m_bridge->acquire();
    // create the list that holds all UnoInterfaceInfos
    m_listIfaces = new ArrayList(10);
    m_numUnoIfaces = 0;
    m_listAdditionalProxies = new ArrayList();
    m_nlistAdditionalProxies = 0;
    //put the information of the first UNO interface into the arraylist
#if OSL_DEBUG_LEVEL >= 2
    _numInterfaces = 0;
    _sInterfaces = NULL;
#endif    
    addUnoInterface(pUnoI, pTD);

}

UnoInterfaceProxy::~UnoInterfaceProxy()
{
#if OSL_DEBUG_LEVEL >= 2
    sd::Trace::WriteLine(System::String::Format(
               new System::String(S"cli uno bridge: Destroying proxy "
               S"for UNO object, OID: \n\t{0} \n\twith uno interfaces: "),
               m_oid));

    sd::Trace::WriteLine( mapUnoString(_sInterfaces));
    rtl_uString_release(_sInterfaces);
#endif
    //m_bridge is unmanaged, therefore we can access it in this finalizer
    CliEnvHolder::g_cli_env->revokeInterface(m_oid);
    m_bridge->release();
}


System::Object* UnoInterfaceProxy::create(
    Bridge * bridge,
    uno_Interface * pUnoI,
    typelib_InterfaceTypeDescription* pTD,
    const OUString& oid)
{
    UnoInterfaceProxy* proxyHandler=
        new UnoInterfaceProxy(bridge, pUnoI, pTD, oid);
    System::Object* proxy= proxyHandler->GetTransparentProxy();
    CliEnvHolder::g_cli_env->registerInterface(proxy, mapUnoString(oid.pData));
    return proxy;
}


void UnoInterfaceProxy::addUnoInterface(uno_Interface* pUnoI,
                                        typelib_InterfaceTypeDescription* pTd)
{
    sc::IEnumerator* enumInfos = m_listIfaces->GetEnumerator();
    System::Threading::Monitor::Enter(this);
    try
    {
        while (enumInfos->MoveNext())
        {
            UnoInterfaceInfo* info = static_cast<UnoInterfaceInfo*>(
                enumInfos->Current);
#if OSL_DEBUG_LEVEL > 1
            System::Type * t1;
            System::Type * t2;
            t1 = mapUnoType(
                reinterpret_cast<typelib_TypeDescription*>(info->m_typeDesc) );
            t2 = mapUnoType(
                reinterpret_cast<typelib_TypeDescription*>(pTd) );
#endif
            if (typelib_typedescription_equals(
               reinterpret_cast<typelib_TypeDescription*>(info->m_typeDesc),
               reinterpret_cast<typelib_TypeDescription*>(pTd)))
            {
                return;
            }           
        }
        OUString oid(mapCliString(m_oid));
        (*m_bridge->m_uno_env->registerInterface)(
            m_bridge->m_uno_env, reinterpret_cast< void ** >( &pUnoI ),
            oid.pData, pTd);
        //This proxy does not contain the uno_Interface. Add it.
        m_listIfaces->Add(new UnoInterfaceInfo(m_bridge, pUnoI, pTd));
        m_numUnoIfaces = m_listIfaces->Count;
#if OSL_DEBUG_LEVEL >= 2
        System::String * sInterfaceName = static_cast<UnoInterfaceInfo*>(
            m_listIfaces->get_Item(m_numUnoIfaces - 1))->m_type->FullName;
        sd::Trace::WriteLine(System::String::Format(
             new System::String(S"cli uno bridge: Creating proxy for uno object, "
                 S"id:\n\t{0}\n\t{1}"), m_oid, sInterfaceName));
        // add to the string that contains all interface names
         _numInterfaces ++;
         OUStringBuffer buf(512);
        buf.appendAscii("\t");
        buf.append( OUString::valueOf((sal_Int32)_numInterfaces));
        buf.appendAscii(". ");
        buf.append(mapCliString(sInterfaceName));
        buf.appendAscii("\n");
        OUString _sNewInterface = buf.makeStringAndClear();
        rtl_uString * __pin * pp_sInterfaces = & _sInterfaces;
        rtl_uString_newConcat( pp_sInterfaces, * pp_sInterfaces,
                               _sNewInterface.pData);
#endif
    }
    __finally {
        System::Threading::Monitor::Exit(this);
    }
}


// IRemotingTypeInfo
bool UnoInterfaceProxy::CanCastTo(System::Type* fromType,
                                  System::Object*)
{
    if (fromType == __typeof(System::Object)) // trivial case
        return true;
    
    System::Threading::Monitor::Enter(this);
    try
    {
        if (0 != findInfo( fromType )) // proxy supports demanded interface
            return true;
        
        //query an uno interface for the required type
        
        // we use the first interface in the list (m_listIfaces) to make
        // the queryInterface call
        UnoInterfaceInfo* info =
            static_cast<UnoInterfaceInfo*>(m_listIfaces->get_Item(0));
        css::uno::TypeDescription membertd(
            reinterpret_cast<typelib_InterfaceTypeDescription*>(
                info->m_typeDesc)->ppAllMembers[0]);
        System::Object *args[] = new System::Object*[1];
        
        args[0] = fromType;
        __box uno::Any  * pAny;
        System::Object* pException = NULL;
        
        pAny= static_cast<__box uno::Any *>(
            m_bridge->call_uno(
                info->m_unoI,
                membertd.get(),
                ((typelib_InterfaceMethodTypeDescription*)
                 membertd.get())->pReturnTypeRef,
                1,
                ((typelib_InterfaceMethodTypeDescription*)
                 membertd.get())->pParams,
                args, NULL, &pException) );
        
        // handle regular exception from target
        OSL_ENSURE(
            0 == pException,
            OUStringToOString(
                mapCliString( pException->ToString()),
                RTL_TEXTENCODING_UTF8 ).getStr() );
        
        if (pAny->Type != __typeof (void)) // has value?
        {
            if (0 != findInfo( fromType ))
            {
                // proxy now supports demanded interface
                return true;
            }
            
            // via aggregation: it is possible that queryInterface() returns
            //                  and interface with a different oid.
            //                  That way, this type is supported for the CLI
            //                  interpreter (CanCastTo() returns true)
            ::System::Object * obj = pAny->Value;
            OSL_ASSERT( srr::RemotingServices::IsTransparentProxy( obj ) );
            if (srr::RemotingServices::IsTransparentProxy( obj ))
            {
                UnoInterfaceProxy * proxy =
                    static_cast< UnoInterfaceProxy * >(
                        srr::RemotingServices::GetRealProxy( obj ) );
                OSL_ASSERT( 0 != proxy->findInfo( fromType ) );
                m_listAdditionalProxies->Add( proxy );
                m_nlistAdditionalProxies = m_listAdditionalProxies->Count;
                OSL_ASSERT( 0 != findInfo( fromType ) );
                return true;
            }
        }
    }
    catch (BridgeRuntimeError& e)
    {
        (void) e; // avoid warning
        OSL_ENSURE(
            0, OUStringToOString(
                e.m_message, RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    catch (System::Exception* e)
    {
        System::String* msg= new System::String(
            S"An unexpected CLI exception occurred in "
            S"UnoInterfaceProxy::CanCastTo().  Original"
            S"message: \n");
        msg= System::String::Concat(msg, e->get_Message());
        OSL_ENSURE(
            0, OUStringToOString(
                mapCliString(msg), RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    catch (...)
    {
        OSL_ENSURE(
            0, "An unexpected native C++ exception occurred in "
            "UnoInterfaceProxy::CanCastTo()" );
    }    
    __finally
    {
        System::Threading::Monitor::Exit(this);
    }
    return false;
}

srrm::IMessage* UnoInterfaceProxy::invokeObject(
    sc::IDictionary* props,                    
    srrm::LogicalCallContext* context,
    srrm::IMethodCallMessage* mcm)
{
    System::Object* retMethod = 0;
    System::String* sMethod = static_cast<System::String*>
        (props->get_Item(m_methodNameString));
    System::Object* args[] = static_cast<System::Object*[]>(
        props->get_Item(m_ArgsString));
    if (m_Equals_String->Equals(sMethod))
    {
        // Object.Equals
        OSL_ASSERT(args->get_Length() == 1);
        srrp::RealProxy* rProxy = srr::RemotingServices::GetRealProxy(args[0]);
        bool bDone = false;
        if (rProxy)
        {
            UnoInterfaceProxy* unoProxy =
                dynamic_cast<UnoInterfaceProxy*>(rProxy);
            if (unoProxy)
            {
                bool b = m_oid->Equals(unoProxy->getOid());
                retMethod = __box(b);
                bDone = true;
            }
        }
        if (bDone == false)
        {
            //no proxy or not our proxy, therefore Equals must be false
            retMethod = __box(false);
        }
    }
    else if (m_GetHashCode_String->Equals(sMethod))
    {
        // Object.GetHashCode
        int nHash = m_oid->GetHashCode();
        retMethod = __box(nHash);
    }
    else if (m_GetType_String->Equals(sMethod))
    {
        // Object.GetType
        retMethod = __typeof(System::Object);
    }
    else if (m_ToString_String->Equals(sMethod))
    {
        // Object.ToString
        st::StringBuilder* sb = new st::StringBuilder(256);
// 				sb->AppendFormat(S"Uno object proxy. Implemented interface: {0}"
// 					S". OID: {1}", m_type->ToString(), m_oid);
        sb->AppendFormat(S"Uno object proxy. OID: {0}", m_oid);
        retMethod = sb->ToString();
    }                     
    else 
    {
        //Either Object has new functions or a protected method was called
        //which should not be possible
        OSL_ASSERT(0);
    }
    srrm::IMessage* retVal= new srrm::ReturnMessage(
        retMethod, new System::Object*[0], 0, context, mcm);
    return retVal;
}

UnoInterfaceInfo * UnoInterfaceProxy::findInfo( ::System::Type * type )
{
    for (int i = 0; i < m_numUnoIfaces; i++)
    {
        UnoInterfaceInfo* tmpInfo = static_cast<UnoInterfaceInfo*>(
            m_listIfaces->get_Item(i));
        if (type->IsAssignableFrom(tmpInfo->m_type))
            return tmpInfo;
    }
    for ( int i = 0; i < m_nlistAdditionalProxies; ++i )
    {
        UnoInterfaceProxy * proxy =
            static_cast< UnoInterfaceProxy * >(
                m_listAdditionalProxies->get_Item( i ) );
        UnoInterfaceInfo * info = proxy->findInfo( type );
        if (0 != info)
            return info;
    }
    return 0;
}

srrm::IMessage* UnoInterfaceProxy::Invoke(srrm::IMessage* callmsg)
{
    try
    {
        sc::IDictionary* props= callmsg->Properties;
        srrm::LogicalCallContext* context=
            static_cast<srrm::LogicalCallContext*>(
                props->get_Item(m_CallContextString));
        srrm::IMethodCallMessage* mcm=
            static_cast<srrm::IMethodCallMessage*>(callmsg);
        
        //Find out which UNO interface is being called
        System::String* sTypeName = static_cast<System::String*>(
            props->get_Item(m_typeNameString));
        sTypeName = sTypeName->Substring(0, sTypeName->IndexOf(','));
        
        // Special Handling for System.Object methods
        if(sTypeName->IndexOf(m_system_Object_String) != -1)
        {
            return invokeObject(props, context, mcm);
        }
        
        System::Type* typeBeingCalled = loadCliType(sTypeName);
        UnoInterfaceInfo* info = findInfo( typeBeingCalled );
        OSL_ASSERT( 0 != info );
        
        // ToDo do without string conversion, a OUString is not needed here
        // get the type description of the call
        OUString usMethodName(mapCliString(static_cast<System::String*>(
                 props->get_Item(m_methodNameString))));
        typelib_TypeDescriptionReference ** ppAllMembers =
            info->m_typeDesc->ppAllMembers;
        sal_Int32 numberMembers = info->m_typeDesc->nAllMembers;
        for ( sal_Int32 nPos = numberMembers; nPos--; )
        {
            typelib_TypeDescriptionReference * member_type = ppAllMembers[nPos];
            
            // check usMethodName against fully qualified usTypeName
            // of member_type; usTypeName is of the form
            //  <name> "::" <usMethodName> *(":@" <idx> "," <idx> ":" <name>)
            OUString const & usTypeName =
                OUString::unacquired( & member_type->pTypeName );

#if OSL_DEBUG_LEVEL >= 2
        System::String * pTypeName;
        pTypeName = mapUnoString(usTypeName.pData);
#endif
            sal_Int32 offset = usTypeName.indexOf( ':' ) + 2;
            OSL_ASSERT(
                offset >= 2 && offset < usTypeName.getLength()
                && usTypeName[offset - 1] == ':' );
            sal_Int32 remainder = usTypeName.getLength() - offset;

            if (typelib_TypeClass_INTERFACE_METHOD == member_type->eTypeClass)
            {
                if ((usMethodName.getLength() == remainder
                     || (usMethodName.getLength() < remainder
                         && usTypeName[offset + usMethodName.getLength()] == ':'))
                    && usTypeName.match(usMethodName, offset))
                 {
                    TypeDescr member_td( member_type );
                    typelib_InterfaceMethodTypeDescription * method_td =
                        (typelib_InterfaceMethodTypeDescription *)
                        member_td.get();

                    System::Object* args[] = static_cast<System::Object*[]>(
                        props->get_Item(m_ArgsString));
                    System::Type* argTypes[] = static_cast<System::Type*[]>(
                        props->get_Item(m_methodSignatureString));
                    System::Object* pExc = NULL;
                    System::Object * cli_ret = m_bridge->call_uno(
                        info->m_unoI, member_td.get(),
                        method_td->pReturnTypeRef, method_td->nParams,
                        method_td->pParams, args, argTypes, &pExc);
                    return constructReturnMessage(cli_ret, args, method_td,
                                                  callmsg, pExc);
                    break;
                }
            }
            else
            {
                OSL_ASSERT( typelib_TypeClass_INTERFACE_ATTRIBUTE ==
                            member_type->eTypeClass );
                if (usMethodName.getLength() > 4
                    && (usMethodName.getLength() - 4 == remainder
                        || (usMethodName.getLength() - 4 < remainder
                            && usTypeName[
                                offset + (usMethodName.getLength() - 4)] == ':'))
                    && usMethodName[1] == 'e' && usMethodName[2] == 't'
                    && rtl_ustr_compare_WithLength(
                        usTypeName.getStr() + offset,
                        usMethodName.getLength() - 4,
                        usMethodName.getStr() + 4,
                        usMethodName.getLength() - 4) == 0)
                 {
                    if ('g' == usMethodName[0])
                    {
                        TypeDescr member_td( member_type );
                        typelib_InterfaceAttributeTypeDescription * attribute_td =
                            (typelib_InterfaceAttributeTypeDescription*)
                            member_td.get();

                        System::Object* pExc = NULL;
                        System::Object* cli_ret= m_bridge->call_uno(
                            info->m_unoI, member_td.get(),
                            attribute_td->pAttributeTypeRef,
                            0, 0,
                            NULL, NULL, &pExc);
                        return constructReturnMessage(cli_ret, NULL, NULL,
                                                      callmsg, pExc);
                    }
                    else if ('s' == usMethodName[0])
                    {
                        TypeDescr member_td( member_type );
                        typelib_InterfaceAttributeTypeDescription * attribute_td =
                            (typelib_InterfaceAttributeTypeDescription *)
                            member_td.get();
                        if (! attribute_td->bReadOnly)
                        {
                            typelib_MethodParameter param;
                            param.pTypeRef = attribute_td->pAttributeTypeRef;
                            param.bIn = sal_True;
                            param.bOut = sal_False;
                            
                            System::Object* args[] =
                                static_cast<System::Object*[]>(
                                    props->get_Item(m_ArgsString));
                            System::Object* pExc = NULL;
                            m_bridge->call_uno(
                                info->m_unoI, member_td.get(),
                                ::getCppuVoidType().getTypeLibType(),
                                1, &param, args, NULL, &pExc);
                            return constructReturnMessage(NULL, NULL, NULL,
                                                          callmsg, pExc);
                        }
                        else
                        {
                            return constructReturnMessage(NULL, NULL, NULL,
                                                          callmsg, NULL);
                        }
                    }
                    break;
                }
            }
        }
        // ToDo check if the message of the exception is not crippled
        // the thing that should not be... no method info found!
        OUStringBuffer buf( 64 );
        buf.appendAscii(RTL_CONSTASCII_STRINGPARAM(
                        "[cli_uno bridge]calling undeclared function on "
                        "interface ") );
        buf.append( *reinterpret_cast< OUString const * >(
                  & ((typelib_TypeDescription *)info->m_typeDesc)->pTypeName));
        buf.appendAscii( RTL_CONSTASCII_STRINGPARAM(": ") );
        buf.append( usMethodName );
        throw BridgeRuntimeError( buf.makeStringAndClear() );
    }
    catch (BridgeRuntimeError & err)
    {
        srrm::IMethodCallMessage* mcm =
            static_cast<srrm::IMethodCallMessage*>(callmsg);
        return new srrm::ReturnMessage(new ucss::uno::RuntimeException(
                         mapUnoString(err.m_message.pData), NULL), mcm);
    }
    catch (System::Exception* e)
    {
        st::StringBuilder * sb = new st::StringBuilder(512);
        sb->Append(new System::String(
            S"An unexpected CLI exception occurred in "
            S"UnoInterfaceProxy::Invoke. Original"
            S"message: \n"));
        sb->Append(e->get_Message());
        sb->Append((__wchar_t) '\n');
        sb->Append(e->get_StackTrace());
        srrm::IMethodCallMessage* mcm =
            static_cast<srrm::IMethodCallMessage*>(callmsg);
        return new srrm::ReturnMessage(new ucss::uno::RuntimeException(
                                           sb->ToString(), NULL), mcm);
    }
    catch (...)
    {
        System::String* msg = new System::String(
            S"An unexpected native C++ exception occurred in "
            S"UnoInterfaceProxy::Invoke.");
        srrm::IMethodCallMessage* mcm =
            static_cast<srrm::IMethodCallMessage*>(callmsg);
        return new srrm::ReturnMessage(new ucss::uno::RuntimeException(
                                       msg, NULL), mcm);        
    }
    return NULL;
}
/** If the argument args is NULL then this function is called for an attribute
    method (either setXXX or getXXX).
    For attributes the argument mtd is also NULL.
*/    
srrm::IMessage* UnoInterfaceProxy::constructReturnMessage(
    System::Object* cliReturn,
    System::Object* args[],
    typelib_InterfaceMethodTypeDescription* mtd,
    srrm::IMessage* msg, System::Object* exc)
{
    srrm::IMessage * retVal= NULL;
    srrm::IMethodCallMessage* mcm = static_cast<srrm::IMethodCallMessage*>(msg);
    if (exc)
    {
        retVal = new srrm::ReturnMessage(
            dynamic_cast<System::Exception*>(exc), mcm);
    }
    else
    {
        sc::IDictionary* props= msg->get_Properties();
        srrm::LogicalCallContext* context=
            static_cast<srrm::LogicalCallContext*>(
            props->get_Item(m_CallContextString));
        if (args != NULL)
        {
            // Method
            //build the array of out parameters, allocate max length
            System::Object* arOut[]= new System::Object*[mtd->nParams]; 
            int nOut = 0;
            for (int i= 0; i < mtd->nParams; i++)
            {
                if (mtd->pParams[i].bOut)
                {
                    arOut[i]= args[i];
                    nOut++;
                }
            }
            retVal= new srrm::ReturnMessage(cliReturn, arOut, nOut,
                                            context, mcm);
        }
        else
        {
            // Attribute  (getXXX)
            retVal= new srrm::ReturnMessage(cliReturn, NULL, 0,
                                            context, mcm);
        }
    }
    return retVal;
}

//################################################################################
CliProxy::CliProxy(Bridge const* bridge, System::Object* cliI,
                         typelib_TypeDescription const* td,
                         const rtl::OUString& usOid):
    m_ref(1),
    m_bridge(bridge),
    m_cliI(cliI),
    m_unoType(const_cast<typelib_TypeDescription*>(td)),
    m_usOid(usOid),
    m_oid(mapUnoString(usOid.pData)),
    m_nInheritedInterfaces(0)
{
    m_bridge->acquire();
    uno_Interface::acquire = cli_proxy_acquire;
    uno_Interface::release = cli_proxy_release;
    uno_Interface::pDispatcher = cli_proxy_dispatch;

    m_unoType.makeComplete();
    m_type= mapUnoType(m_unoType.get());

    makeMethodInfos();
#if OSL_DEBUG_LEVEL >= 2
    sd::Trace::WriteLine(System::String::Format(
      new System::String(S"cli uno bridge: Creating proxy for cli object, "
                         S"id:\n\t{0}\n\t{1}"), m_oid, m_type));
#endif    

}

void CliProxy::makeMethodInfos()
{
#if OSL_DEBUG_LEVEL >= 2
    System::Object* cliI;
    System::Type* type;
    cliI = m_cliI;
    type = m_type;
#endif    

    if (m_type->get_IsInterface() == false)
        return;
    sr::MethodInfo* arThisMethods[] = m_type->GetMethods();
    //get the inherited interfaces
    System::Type* arInheritedIfaces[] = m_type->GetInterfaces();
    m_nInheritedInterfaces = arInheritedIfaces->get_Length();
    //array containing the number of methods for the interface and its
    //inherited interfaces
    m_arInterfaceMethodCount = new int __gc [m_nInheritedInterfaces + 1];
    //determine the number of all interface methods, including the inherited
    //interfaces
    int numMethods = arThisMethods->get_Length();
    for (int i= 0; i < m_nInheritedInterfaces; i++)
    {
        numMethods += arInheritedIfaces[i]->GetMethods()->get_Length();
    }
    //array containing MethodInfos of the cli object
    m_arMethodInfos = new sr::MethodInfo*[numMethods];
    //array containing MethodInfos of the interface
    m_arInterfaceMethodInfos = new sr::MethodInfo*[numMethods];
    //array containing the mapping of Uno interface pos to pos in
    //m_arMethodInfos
    m_arUnoPosToCliPos = new System::Int32[numMethods];
    // initialize with -1
    for (int i = 0; i < numMethods; i++)
        m_arUnoPosToCliPos[i] = -1;

#if OSL_DEBUG_LEVEL >= 2
    sr::MethodInfo* arMethodInfosDbg[];
    sr::MethodInfo* arInterfaceMethodInfosDbg[];
    System::Int32 arInterfaceMethodCountDbg[];
    arMethodInfosDbg = m_arMethodInfos;
    arInterfaceMethodInfosDbg = m_arInterfaceMethodInfos;
    arInterfaceMethodCountDbg = m_arInterfaceMethodCount;
#endif    


    //fill m_arMethodInfos with the mappings
    // !!! InterfaceMapping.TargetMethods should be MethodInfo*[] according
    // to documentation
    // but it is Type*[] instead. Bug in the framework?
    System::Type* objType = m_cliI->GetType();
    try
    {        
        int index = 0;
        // now get the methods from the inherited interface
        //arInheritedIfaces[0] is the direct base interface
        //arInheritedIfaces[n] is the furthest inherited interface
        //Start with the base interface
        int nArLength = arInheritedIfaces->get_Length();
        for (;nArLength > 0; nArLength--)
        {
            sr::InterfaceMapping mapInherited = objType->GetInterfaceMap(
                arInheritedIfaces[nArLength - 1]);
            int numMethods = mapInherited.TargetMethods->get_Length();
            m_arInterfaceMethodCount[nArLength - 1] = numMethods;
            for (int i = 0; i < numMethods; i++, index++)
            {
                m_arMethodInfos[index] = __try_cast<sr::MethodInfo*>(
                    mapInherited.TargetMethods[i]);

                m_arInterfaceMethodInfos[index] = __try_cast<sr::MethodInfo*>(
                    mapInherited.InterfaceMethods[i]);
            }
        }
        //At last come the methods of the furthest derived interface
        sr::InterfaceMapping map = objType->GetInterfaceMap(m_type);
        nArLength = map.TargetMethods->get_Length();
        m_arInterfaceMethodCount[m_nInheritedInterfaces] = nArLength;
        for (int i = 0; i < nArLength; i++,index++)
        {
            m_arMethodInfos[index]= __try_cast<sr::MethodInfo*>(
                map.TargetMethods[i]);
            m_arInterfaceMethodInfos[index]= __try_cast<sr::MethodInfo*>(
                map.InterfaceMethods[i]);
        }
    }
    catch (System::InvalidCastException* )
    {
        OUStringBuffer buf( 128 );
        buf.appendAscii(RTL_CONSTASCII_STRINGPARAM(
                            "[cli_uno bridge] preparing proxy for "
                            "cli interface: ") );
        buf.append(mapCliString(m_type->ToString() ));
        buf.appendAscii(RTL_CONSTASCII_STRINGPARAM(" \nfailed!"));
        throw BridgeRuntimeError( buf.makeStringAndClear() );
    }
}

sr::MethodInfo* CliProxy::getMethodInfo(int nUnoFunctionPos,
                                           const rtl::OUString& usMethodName, MethodKind methodKind)
{
    sr::MethodInfo* ret = NULL;
#if OSL_DEBUG_LEVEL >= 2
    System::String* sMethodNameDbg;
    sr::MethodInfo* arMethodInfosDbg[];
    sr::MethodInfo* arInterfaceMethodInfosDbg[];
    System::Int32 arInterfaceMethodCountDbg[];
    System::Int32 arUnoPosToCliPosDbg[];
    sMethodNameDbg = mapUnoString(usMethodName.pData);
    arMethodInfosDbg = m_arMethodInfos;
    arInterfaceMethodInfosDbg = m_arInterfaceMethodInfos;
    arInterfaceMethodCountDbg = m_arInterfaceMethodCount;
    arUnoPosToCliPosDbg = m_arUnoPosToCliPos;
#endif    
    //deduct 3 for XInterface methods
    nUnoFunctionPos -= 3;
    System::Threading::Monitor::Enter(m_arUnoPosToCliPos);
    try
    {
        int cliPos = m_arUnoPosToCliPos[nUnoFunctionPos];
        if (cliPos != -1)
            return m_arMethodInfos[cliPos];

        //create the method function name
        System::String* sMethodName = mapUnoString(usMethodName.pData);
        switch (methodKind)
        {
        case MK_METHOD:
            break;
        case MK_SET:
            sMethodName = System::String::Concat(
                const_cast<System::String*>(Constants::sAttributeSet),
                sMethodName);
            break;
        case MK_GET:
            sMethodName = System::String::Concat(
                const_cast<System::String*>(Constants::sAttributeGet),
                sMethodName);
            break;
        default:
            OSL_ASSERT(0);
        }
        //Find the cli interface method that corresponds to the Uno method
//        System::String* sMethodName= mapUnoString(usMethodName.pData);
        int indexCliMethod = -1;
        //If the cli interfaces and their methods are in the same order
        //as they were declared (inheritance chain and within the interface)
        //then nUnoFunctionPos should lead to the correct method. However,
        //the documentation does not say that this ordering is given.
        if (sMethodName->Equals(m_arInterfaceMethodInfos[nUnoFunctionPos]->Name))
            indexCliMethod = nUnoFunctionPos;
        else
        {
            int cMethods = m_arInterfaceMethodInfos->get_Length();
            for (int i = 0; i < cMethods; i++)
            {
                System::String* cliMethod = m_arInterfaceMethodInfos[i]->Name;
                if (cliMethod->Equals(sMethodName))
                {
                    indexCliMethod = i;
                    break;
                }
            }
        }
        if (indexCliMethod == -1)
        {
            OUStringBuffer buf(256);
            buf.appendAscii(RTL_CONSTASCII_STRINGPARAM(
                                "[cli_uno bridge] CliProxy::getMethodInfo():"
                                "cli object does not implement interface method: "));
            buf.append(usMethodName);
            throw BridgeRuntimeError(buf.makeStringAndClear());
            return 0;
        }
        m_arUnoPosToCliPos[nUnoFunctionPos] = indexCliMethod;
        ret = m_arMethodInfos[indexCliMethod];
    }
    __finally
    {
        System::Threading::Monitor::Exit(m_arUnoPosToCliPos);
    }
    
    return ret;
}

CliProxy::~CliProxy()
{
#if OSL_DEBUG_LEVEL >= 2
    sd::Trace::WriteLine(System::String::Format(
                  new System::String(
                  S"cli uno bridge: Destroying proxy for cli object, "
                  S"id:\n\t{0}\n\t{1}\n"),
                  m_oid, m_type));
#endif    
    CliEnvHolder::g_cli_env->revokeInterface(m_oid, mapUnoType(m_unoType.get()));
    m_bridge->release();
}

uno_Interface* CliProxy::create(Bridge const * bridge,
                                 System::Object* cliI,
                                 typelib_TypeDescription const* pTD,
                                 const rtl::OUString& ousOid)
{
    uno_Interface* proxy= static_cast<uno_Interface*>(
        new CliProxy(bridge, cliI, pTD, ousOid));

    //register proxy with target environment (uno)
    (*bridge->m_uno_env->registerProxyInterface)(
       bridge->m_uno_env,
       reinterpret_cast<void**>(&proxy),
       cli_proxy_free,
       ousOid.pData, (typelib_InterfaceTypeDescription*) pTD);
    //register original interface
    CliEnvHolder::g_cli_env->registerInterface(cliI, mapUnoString(ousOid.pData),
                       mapUnoType((pTD)));

    return proxy;
}



void SAL_CALL CliProxy::uno_DispatchMethod(
        struct _uno_Interface *, 
        const struct _typelib_TypeDescription *,
        void *,
        void **,
        uno_Any ** )
{
}
inline void CliProxy::acquire() const
{
    if (1 == osl_incrementInterlockedCount( &m_ref ))
    {
        // rebirth of proxy zombie
        void * that = const_cast< CliProxy * >( this );
        // register at uno env
        (*m_bridge->m_uno_env->registerProxyInterface)(
            m_bridge->m_uno_env, &that,
            cli_proxy_free, m_usOid.pData,
            (typelib_InterfaceTypeDescription *)m_unoType.get() );
#if OSL_DEBUG_LEVEL >= 2
        OSL_ASSERT( this == (void const * const)that );
#endif
    }
}
//---------------------------------------------------------------------------
inline void CliProxy::release() const
{
    if (0 == osl_decrementInterlockedCount( &m_ref ))
    {
        // revoke from uno env on last release,
        // The proxy can be resurrected if acquire is called before the uno
        // environment calls cli_proxy_free. cli_proxy_free will
        //delete the proxy. The environment does not acquire a registered
        //proxy.
        (*m_bridge->m_uno_env->revokeInterface)(
            m_bridge->m_uno_env, const_cast< CliProxy * >( this ) );
    }
}
}




extern "C"
void SAL_CALL cli_proxy_free( uno_ExtEnvironment *, void * proxy )
    SAL_THROW_EXTERN_C()
{
    cli_uno::CliProxy * cliProxy = reinterpret_cast<
        cli_uno::CliProxy * >( proxy );

    delete cliProxy;
}

extern "C"
void SAL_CALL cli_proxy_acquire( uno_Interface * pUnoI )
    SAL_THROW_EXTERN_C()
{
    CliProxy const * cliProxy = static_cast< CliProxy const * >( pUnoI );
    cliProxy->acquire();
}
//-----------------------------------------------------------------------------
extern "C"
void SAL_CALL cli_proxy_release( uno_Interface * pUnoI )
    SAL_THROW_EXTERN_C()
{
    CliProxy * cliProxy = static_cast< CliProxy * >( pUnoI );
    cliProxy->release();
}

//------------------------------------------------------------------------------
extern "C"

void SAL_CALL cli_proxy_dispatch(
    uno_Interface * pUnoI, typelib_TypeDescription const * member_td,
    void * uno_ret, void * uno_args [], uno_Any ** uno_exc )
    SAL_THROW_EXTERN_C()
{
    CliProxy * proxy = static_cast< CliProxy* >( pUnoI );
    try
    {
        Bridge const* bridge = proxy->m_bridge;
        
        switch (member_td->eTypeClass)
        {
        case typelib_TypeClass_INTERFACE_ATTRIBUTE:
        {
            
            sal_Int32 member_pos = ((typelib_InterfaceMemberTypeDescription *)
                                    member_td)->nPosition;
            typelib_InterfaceTypeDescription * iface_td =
                (typelib_InterfaceTypeDescription *)proxy->m_unoType.get();
            OSL_ENSURE(
                member_pos < iface_td->nAllMembers,
                "### member pos out of range!" );
            sal_Int32 function_pos =
                iface_td->pMapMemberIndexToFunctionIndex[ member_pos ];
            OSL_ENSURE(
                function_pos < iface_td->nMapFunctionIndexToMemberIndex,
                "### illegal function index!" );
            
            if (uno_ret) // is getter method
            {
               OUString const& usAttrName= *(rtl_uString**)&
                   ((typelib_InterfaceMemberTypeDescription*) member_td)
                   ->pMemberName;
               sr::MethodInfo* info = proxy->getMethodInfo(function_pos,
                                             usAttrName, CliProxy::MK_GET);
               bridge->call_cli( 
                    proxy->m_cliI,
                    info,
                    ((typelib_InterfaceAttributeTypeDescription *)member_td)
                    ->pAttributeTypeRef,
                    0, 0, // no params
                    uno_ret, 0, uno_exc );
            }
            else // is setter method
            {
                OUString const& usAttrName= *(rtl_uString**) &
                    ((typelib_InterfaceMemberTypeDescription*) member_td)
                    ->pMemberName;
                sr::MethodInfo* info = proxy->getMethodInfo(function_pos + 1,
                                              usAttrName, CliProxy::MK_SET);
                typelib_MethodParameter param;
                param.pTypeRef =
                    ((typelib_InterfaceAttributeTypeDescription *)member_td)
                    ->pAttributeTypeRef;
                param.bIn = sal_True;
                param.bOut = sal_False;
                
                bridge->call_cli(
                    proxy->m_cliI,
                    // set follows get method
                    info,
                    0 /* indicates void return */, &param, 1,
                    0, uno_args, uno_exc );
           }
            break;
        }
        case typelib_TypeClass_INTERFACE_METHOD:
        {
            sal_Int32 member_pos = ((typelib_InterfaceMemberTypeDescription *)
                                    member_td)->nPosition;
            typelib_InterfaceTypeDescription * iface_td =
                (typelib_InterfaceTypeDescription *)proxy->m_unoType.get();
            OSL_ENSURE(
                member_pos < iface_td->nAllMembers,
                "### member pos out of range!" );
            sal_Int32 function_pos =
                iface_td->pMapMemberIndexToFunctionIndex[ member_pos ];
            OSL_ENSURE(
                function_pos < iface_td->nMapFunctionIndexToMemberIndex,
                "### illegal function index!" );
            
            switch (function_pos)
            {
            case 0: // queryInterface()
            {
                TypeDescr demanded_td(
                    *reinterpret_cast<typelib_TypeDescriptionReference **>(
                        uno_args[0]));
                if (typelib_TypeClass_INTERFACE
                    != demanded_td.get()->eTypeClass)
                {
                    throw BridgeRuntimeError(
                    OUSTR("queryInterface() call demands an INTERFACE type!"));
                }
                
                uno_Interface * pInterface = 0;
                (*bridge->m_uno_env->getRegisteredInterface)(
                    bridge->m_uno_env,
                    (void **)&pInterface, proxy->m_usOid.pData,
                    (typelib_InterfaceTypeDescription *)demanded_td.get() );
                
                if (0 == pInterface)
                {
                    System::Type* mgdDemandedType =
                        mapUnoType(demanded_td.get());
                    if (mgdDemandedType->IsInstanceOfType( proxy->m_cliI ))
                    {
#if OSL_DEBUG_LEVEL > 0
                        OUString usOid(
                            mapCliString(
                            CliEnvHolder::g_cli_env->getObjectIdentifier(
                                    proxy->m_cliI )));
                        OSL_ENSURE(usOid.equals( proxy->m_usOid ),
                                    "### different oids!");
#endif
                        uno_Interface* pUnoI = bridge->map_cli2uno(
                            proxy->m_cliI, demanded_td.get() );
                        uno_any_construct(
                            (uno_Any *)uno_ret, &pUnoI, demanded_td.get(), 0 );
                        (*pUnoI->release)( pUnoI );
                    }
                    else // object does not support demanded interface
                    {
                        uno_any_construct( (uno_Any *)uno_ret, 0, 0, 0 );
                    }
                    // no excetpion occurred
                    *uno_exc = 0;                    
                }
                else
                {
                    uno_any_construct(
                        reinterpret_cast< uno_Any * >( uno_ret ),
                        &pInterface, demanded_td.get(), 0 );
                    (*pInterface->release)( pInterface );
                    *uno_exc = 0;
                }
                break;
            }
            case 1: // acquire this proxy
                cli_proxy_acquire(proxy);
                *uno_exc = 0;
                break;
            case 2: // release this proxy
                cli_proxy_release(proxy);
                *uno_exc = 0;
                break;
            default: // arbitrary method call
            {
                typelib_InterfaceMethodTypeDescription * method_td =
                    (typelib_InterfaceMethodTypeDescription *)member_td;
               OUString const& usMethodName= *(rtl_uString**) &
                   ((typelib_InterfaceMemberTypeDescription*) member_td)
                   ->pMemberName;

               sr::MethodInfo* info = proxy->getMethodInfo(function_pos,
                                             usMethodName, CliProxy::MK_METHOD); 
               bridge->call_cli(
                    proxy->m_cliI,
                    info,
                    method_td->pReturnTypeRef, method_td->pParams,
                    method_td->nParams,
                    uno_ret, uno_args, uno_exc);
                return;
            }
            }
            break;
        }
        default:
        {
            throw BridgeRuntimeError(
                OUSTR("illegal member type description!") );
        }
        }
    }
    catch (BridgeRuntimeError & err)
    {
        // binary identical struct
        ::com::sun::star::uno::RuntimeException exc(
            OUSTR("[cli_uno bridge error] ") + err.m_message,
            ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XInterface >() );
        ::com::sun::star::uno::Type const & exc_type = ::getCppuType( & exc);
        uno_type_any_construct( *uno_exc, &exc, exc_type.getTypeLibType(), 0);
#if OSL_DEBUG_LEVEL >= 1
        OString cstr_msg(OUStringToOString(exc.Message,
                                             RTL_TEXTENCODING_ASCII_US ) );
        OSL_ENSURE(0, cstr_msg.getStr());
#endif
    }
}





/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
