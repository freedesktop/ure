/*************************************************************************
 *
 *  $RCSfile: bridge.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-03 12:48:30 $
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

#include "bridges/cpp_uno/shared/bridge.hxx"

#include "component.hxx"

#include "bridges/cpp_uno/shared/cppinterfaceproxy.hxx"
#include "bridges/cpp_uno/shared/unointerfaceproxy.hxx"

#include "com/sun/star/uno/XInterface.hpp"
#include "osl/diagnose.h"
#include "osl/interlck.h"
#include "rtl/ustring.h"
#include "sal/types.h"
#include "typelib/typedescription.h"
#include "uno/dispatcher.h"
#include "uno/environment.h"
#include "uno/mapping.h"

using bridges::cpp_uno::shared::Bridge;

uno_Mapping * Bridge::createMapping(
    uno_ExtEnvironment * pCppEnv, uno_ExtEnvironment * pUnoEnv,
    bool bExportCpp2Uno) SAL_THROW(())
{
    Bridge * bridge = new Bridge(pCppEnv, pUnoEnv, bExportCpp2Uno);
    return bExportCpp2Uno ? &bridge->aCpp2Uno : &bridge->aUno2Cpp;
}

void Bridge::freeMapping(uno_Mapping * pMapping) SAL_THROW(())
{
    delete static_cast< Mapping * >( pMapping )->pBridge;
}

void Bridge::acquire() SAL_THROW(())
{
    if (1 == osl_incrementInterlockedCount( &nRef ))
    {
        if (bExportCpp2Uno)
        {
            uno_Mapping * pMapping = &aCpp2Uno;
            ::uno_registerMapping(
                &pMapping, freeMapping,
                (uno_Environment *)pCppEnv, (uno_Environment *)pUnoEnv, 0 );
        }
        else
        {
            uno_Mapping * pMapping = &aUno2Cpp;
            ::uno_registerMapping(
                &pMapping, freeMapping,
                (uno_Environment *)pUnoEnv, (uno_Environment *)pCppEnv, 0 );
        }
    }
}

void Bridge::release() SAL_THROW(())
{
    if (! osl_decrementInterlockedCount( &nRef ))
    {
        ::uno_revokeMapping( bExportCpp2Uno ? &aCpp2Uno : &aUno2Cpp );
    }
}

Bridge::Bridge(
    uno_ExtEnvironment * pCppEnv_, uno_ExtEnvironment * pUnoEnv_,
    bool bExportCpp2Uno_) SAL_THROW(())
    : nRef( 1 )
    , pCppEnv( pCppEnv_ )
    , pUnoEnv( pUnoEnv_ )
    , bExportCpp2Uno( bExportCpp2Uno_ )
{
    bridges::cpp_uno::shared::g_moduleCount.modCnt.acquire(
        &bridges::cpp_uno::shared::g_moduleCount.modCnt );

    aCpp2Uno.pBridge = this;
    aCpp2Uno.acquire = acquireMapping;
    aCpp2Uno.release = releaseMapping;
    aCpp2Uno.mapInterface = cpp2unoMapping;

    aUno2Cpp.pBridge = this;
    aUno2Cpp.acquire = acquireMapping;
    aUno2Cpp.release = releaseMapping;
    aUno2Cpp.mapInterface = uno2cppMapping;

    (*((uno_Environment *)pCppEnv)->acquire)( (uno_Environment *)pCppEnv );
    (*((uno_Environment *)pUnoEnv)->acquire)( (uno_Environment *)pUnoEnv );
}

Bridge::~Bridge() SAL_THROW(())
{
    (*((uno_Environment *)pUnoEnv)->release)( (uno_Environment *)pUnoEnv );
    (*((uno_Environment *)pCppEnv)->release)( (uno_Environment *)pCppEnv );
    bridges::cpp_uno::shared::g_moduleCount.modCnt.release(
        &bridges::cpp_uno::shared::g_moduleCount.modCnt );
}

void Bridge::acquireMapping(uno_Mapping * pMapping) SAL_THROW(())
{
    static_cast< Mapping * >( pMapping )->pBridge->acquire();
}

void Bridge::releaseMapping(uno_Mapping * pMapping) SAL_THROW(())
{
    static_cast< Mapping * >( pMapping )->pBridge->release();
}

void Bridge::cpp2unoMapping(
    uno_Mapping * pMapping, void ** ppUnoI, void * pCppI,
    typelib_InterfaceTypeDescription * pTypeDescr) SAL_THROW(())
{
    OSL_ENSURE( ppUnoI && pTypeDescr, "### null ptr!" );
    if (*ppUnoI)
    {
        (*reinterpret_cast< uno_Interface * >( *ppUnoI )->release)(
            reinterpret_cast< uno_Interface * >( *ppUnoI ) );
        *ppUnoI = 0;
    }
    if (pCppI)
    {
        Bridge * pBridge = static_cast< Mapping * >( pMapping )->pBridge;

        // get object id of interface to be wrapped
        rtl_uString * pOId = 0;
        (*pBridge->pCppEnv->getObjectIdentifier)(
            pBridge->pCppEnv, &pOId, pCppI );
        OSL_ASSERT( pOId );

        // try to get any known interface from target environment
        (*pBridge->pUnoEnv->getRegisteredInterface)(
            pBridge->pUnoEnv, ppUnoI, pOId, pTypeDescr );

        if (! *ppUnoI) // no existing interface, register new proxy interface
        {
            // try to publish a new proxy (refcount initially 1)
            uno_Interface * pSurrogate
                = bridges::cpp_uno::shared::UnoInterfaceProxy::create(
                    pBridge,
                    static_cast< ::com::sun::star::uno::XInterface * >( pCppI ),
                    pTypeDescr, pOId );

            // proxy may be exchanged during registration
            (*pBridge->pUnoEnv->registerProxyInterface)(
                pBridge->pUnoEnv, reinterpret_cast< void ** >( &pSurrogate ),
                bridges::cpp_uno::shared::UnoInterfaceProxy::free, pOId,
                pTypeDescr );

            *ppUnoI = pSurrogate;
        }
        ::rtl_uString_release( pOId );
    }
}

void Bridge::uno2cppMapping(
    uno_Mapping * pMapping, void ** ppCppI, void * pUnoI,
    typelib_InterfaceTypeDescription * pTypeDescr) SAL_THROW(())
{
    OSL_ASSERT( ppCppI && pTypeDescr );
    if (*ppCppI)
    {
        static_cast< ::com::sun::star::uno::XInterface * >( *ppCppI )->
            release();
        *ppCppI = 0;
    }
    if (pUnoI)
    {
        Bridge * pBridge = static_cast< Mapping * >( pMapping )->pBridge;

        // get object id of uno interface to be wrapped
        rtl_uString * pOId = 0;
        (*pBridge->pUnoEnv->getObjectIdentifier)(
            pBridge->pUnoEnv, &pOId, pUnoI );
        OSL_ASSERT( pOId );

        // try to get any known interface from target environment
        (*pBridge->pCppEnv->getRegisteredInterface)(
            pBridge->pCppEnv, ppCppI, pOId, pTypeDescr );

        if (! *ppCppI) // no existing interface, register new proxy interface
        {
            // try to publish a new proxy (ref count initially 1)
            com::sun::star::uno::XInterface * pProxy
                = bridges::cpp_uno::shared::CppInterfaceProxy::create(
                    pBridge, static_cast< uno_Interface * >( pUnoI ),
                    pTypeDescr, pOId );

            // proxy may be exchanged during registration
            (*pBridge->pCppEnv->registerProxyInterface)(
                pBridge->pCppEnv, reinterpret_cast< void ** >( &pProxy ),
                bridges::cpp_uno::shared::CppInterfaceProxy::free, pOId,
                pTypeDescr );

            *ppCppI = pProxy;
        }
        ::rtl_uString_release( pOId );
    }
}
