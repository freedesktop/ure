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
#include "precompiled_bridges.hxx"

#include <malloc.h>

#include <com/sun/star/uno/genfunc.hxx>
#include <uno/data.h>

#include "bridges/cpp_uno/shared/bridge.hxx"
#include "bridges/cpp_uno/shared/types.hxx"
#include "bridges/cpp_uno/shared/unointerfaceproxy.hxx"
#include "bridges/cpp_uno/shared/vtables.hxx"

#include "msci.hxx"

using namespace ::rtl;
using namespace ::com::sun::star::uno;

namespace
{

//==================================================================================================
inline static void callVirtualMethod(
    void * pAdjustedThisPtr, sal_Int32 nVtableIndex,
    void * pRegisterReturn, typelib_TypeClass eReturnTypeClass,
    sal_Int32 * pStackLongs, sal_Int32 nStackLongs )
{
    // parameter list is mixed list of * and values
    // reference parameters are pointers

    OSL_ENSURE( pStackLongs && pAdjustedThisPtr, "### null ptr!" );
    OSL_ENSURE( (sizeof(void *) == 4) &&
                 (sizeof(sal_Int32) == 4), "### unexpected size of int!" );
    
__asm
    {
        mov		eax, nStackLongs
        test	eax, eax
        je		Lcall
        // copy values
        mov		ecx, eax
        shl		eax, 2			 // sizeof(sal_Int32) == 4
        add		eax, pStackLongs // params stack space
Lcopy:	sub		eax, 4
        push	dword ptr [eax]
        dec		ecx
        jne		Lcopy
Lcall:
        // call
        mov		ecx, pAdjustedThisPtr
        push	ecx				// this ptr
        mov		edx, [ecx]		// pvft
        mov		eax, nVtableIndex
        shl		eax, 2			// sizeof(void *) == 4
        add		edx, eax
        call	[edx]			// interface method call must be __cdecl!!!

        // register return
        mov		ecx, eReturnTypeClass
        cmp		ecx, typelib_TypeClass_VOID
        je		Lcleanup
        mov		ebx, pRegisterReturn
// int32
        cmp		ecx, typelib_TypeClass_LONG
        je		Lint32
        cmp		ecx, typelib_TypeClass_UNSIGNED_LONG
        je		Lint32
        cmp		ecx, typelib_TypeClass_ENUM
        je		Lint32
// int8
        cmp		ecx, typelib_TypeClass_BOOLEAN
        je		Lint8
        cmp		ecx, typelib_TypeClass_BYTE
        je		Lint8
// int16
        cmp		ecx, typelib_TypeClass_CHAR
        je		Lint16
        cmp		ecx, typelib_TypeClass_SHORT
        je		Lint16
        cmp		ecx, typelib_TypeClass_UNSIGNED_SHORT
        je		Lint16
// float
        cmp		ecx, typelib_TypeClass_FLOAT
        je		Lfloat
// double
        cmp		ecx, typelib_TypeClass_DOUBLE
        je		Ldouble
// int64
        cmp		ecx, typelib_TypeClass_HYPER
        je		Lint64
        cmp		ecx, typelib_TypeClass_UNSIGNED_HYPER
          je		Lint64
        jmp		Lcleanup // no simple type
Lint8:
        mov		byte ptr [ebx], al
        jmp		Lcleanup
Lint16:
        mov		word ptr [ebx], ax
        jmp		Lcleanup
Lfloat:
        fstp	dword ptr [ebx]
        jmp		Lcleanup
Ldouble:
        fstp	qword ptr [ebx]
        jmp		Lcleanup
Lint64:
        mov		dword ptr [ebx], eax
        mov		dword ptr [ebx+4], edx
        jmp		Lcleanup
Lint32:
        mov		dword ptr [ebx], eax
        jmp		Lcleanup
Lcleanup:
        // cleanup stack (obsolete though because of function)
        mov		eax, nStackLongs
        shl		eax, 2			// sizeof(sal_Int32) == 4
        add		eax, 4			// this ptr
        add		esp, eax
    }
}

//==================================================================================================
static void cpp_call(
    bridges::cpp_uno::shared::UnoInterfaceProxy * pThis,
    bridges::cpp_uno::shared::VtableSlot aVtableSlot,
    typelib_TypeDescriptionReference * pReturnTypeRef,
    sal_Int32 nParams, typelib_MethodParameter * pParams,
    void * pUnoReturn, void * pUnoArgs[], uno_Any ** ppUnoExc ) throw ()
{
    // max space for: [complex ret ptr], values|ptr ...
    char * pCppStack		= (char *)alloca( sizeof(sal_Int32) + (nParams * sizeof(sal_Int64)) );
    char * pCppStackStart	= pCppStack;
    
    // return
    typelib_TypeDescription * pReturnTypeDescr = 0;
    TYPELIB_DANGER_GET( &pReturnTypeDescr, pReturnTypeRef );
    OSL_ENSURE( pReturnTypeDescr, "### expected return type description!" );
    
    void * pCppReturn = 0; // if != 0 && != pUnoReturn, needs reconversion
    
    if (pReturnTypeDescr)
    {
        if (bridges::cpp_uno::shared::isSimpleType( pReturnTypeDescr ))
        {
            pCppReturn = pUnoReturn; // direct way for simple types
        }
        else
        {
            // complex return via ptr
            pCppReturn = *(void **)pCppStack
                = (bridges::cpp_uno::shared::relatesToInterfaceType(
                       pReturnTypeDescr )
                   ? alloca( pReturnTypeDescr->nSize )
                   : pUnoReturn); // direct way
            pCppStack += sizeof(void *);
        }
    }

    // stack space

    OSL_ENSURE( sizeof(void *) == sizeof(sal_Int32), "### unexpected size!" );
    // args
    void ** pCppArgs  = (void **)alloca( 3 * sizeof(void *) * nParams );
    // indizes of values this have to be converted (interface conversion cpp<=>uno)
    sal_Int32 * pTempIndizes = (sal_Int32 *)(pCppArgs + nParams);
    // type descriptions for reconversions
    typelib_TypeDescription ** ppTempParamTypeDescr = (typelib_TypeDescription **)(pCppArgs + (2 * nParams));
    
    sal_Int32 nTempIndizes   = 0;
    
    for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
    {
        const typelib_MethodParameter & rParam = pParams[nPos];
        typelib_TypeDescription * pParamTypeDescr = 0;
        TYPELIB_DANGER_GET( &pParamTypeDescr, rParam.pTypeRef );
        
        if (!rParam.bOut
            && bridges::cpp_uno::shared::isSimpleType( pParamTypeDescr ))
        {
            ::uno_copyAndConvertData(
                pCppArgs[nPos] = pCppStack, pUnoArgs[nPos], pParamTypeDescr,
                pThis->getBridge()->getUno2Cpp() );
            
            switch (pParamTypeDescr->eTypeClass)
            {
            case typelib_TypeClass_HYPER:
            case typelib_TypeClass_UNSIGNED_HYPER:
            case typelib_TypeClass_DOUBLE:
                pCppStack += sizeof(sal_Int32); // extra long
                break;
            default:
                break;
            }
            // no longer needed
            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        else // ptr to complex value | ref
        {
            if (! rParam.bIn) // is pure out
            {
                // cpp out is constructed mem, uno out is not!
                ::uno_constructData(
                    *(void **)pCppStack = pCppArgs[nPos] = alloca( pParamTypeDescr->nSize ),
                    pParamTypeDescr );
                pTempIndizes[nTempIndizes] = nPos; // default constructed for cpp call
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            // is in/inout
            else if (bridges::cpp_uno::shared::relatesToInterfaceType(
                         pParamTypeDescr ))
            {
                ::uno_copyAndConvertData(
                    *(void **)pCppStack = pCppArgs[nPos] = alloca( pParamTypeDescr->nSize ),
                    pUnoArgs[nPos], pParamTypeDescr,
                    pThis->getBridge()->getUno2Cpp() );
                
                pTempIndizes[nTempIndizes] = nPos; // has to be reconverted
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            else // direct way
            {
                *(void **)pCppStack = pCppArgs[nPos] = pUnoArgs[nPos];
                // no longer needed
                TYPELIB_DANGER_RELEASE( pParamTypeDescr );
            }
        }
        pCppStack += sizeof(sal_Int32); // standard parameter length
    }

    __try
    {
        // pCppI is msci this pointer
        callVirtualMethod(
            reinterpret_cast< void ** >(pThis->getCppI()) + aVtableSlot.offset,
            aVtableSlot.index,
            pCppReturn, pReturnTypeDescr->eTypeClass,
            (sal_Int32 *)pCppStackStart,
            (pCppStack - pCppStackStart) / sizeof(sal_Int32) );
    }
    __except (CPPU_CURRENT_NAMESPACE::msci_filterCppException(
                  GetExceptionInformation(),
                  *ppUnoExc, pThis->getBridge()->getCpp2Uno() ))
   {
        // *ppUnoExc was constructed by filter function
        // temporary params
        while (nTempIndizes--)
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            // destroy temp cpp param => cpp: every param was constructed
            ::uno_destructData(
                pCppArgs[nIndex], ppTempParamTypeDescr[nTempIndizes],
                cpp_release );
            TYPELIB_DANGER_RELEASE( ppTempParamTypeDescr[nTempIndizes] );
        }
        // return type
        if (pReturnTypeDescr)
        {
            TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
        }
        // end here
        return;
    }
    
    // NO exception occured
    *ppUnoExc = 0;
    
    // reconvert temporary params
    while (nTempIndizes--)
    {
        sal_Int32 nIndex = pTempIndizes[nTempIndizes];
        typelib_TypeDescription * pParamTypeDescr =
            ppTempParamTypeDescr[nTempIndizes];
        
        if (pParams[nIndex].bIn)
        {
            if (pParams[nIndex].bOut) // inout
            {
                ::uno_destructData(
                    pUnoArgs[nIndex], pParamTypeDescr, 0 ); // destroy uno value
                ::uno_copyAndConvertData(
                    pUnoArgs[nIndex], pCppArgs[nIndex], pParamTypeDescr,
                    pThis->getBridge()->getCpp2Uno() );
            }
        }
        else // pure out
        {
            ::uno_copyAndConvertData(
                pUnoArgs[nIndex], pCppArgs[nIndex], pParamTypeDescr,
                pThis->getBridge()->getCpp2Uno() );
        }
        // destroy temp cpp param => cpp: every param was constructed
        ::uno_destructData(
            pCppArgs[nIndex], pParamTypeDescr, cpp_release );
        
        TYPELIB_DANGER_RELEASE( pParamTypeDescr );
    }
    // return value
    if (pCppReturn && pUnoReturn != pCppReturn)
    {
        ::uno_copyAndConvertData(
            pUnoReturn, pCppReturn, pReturnTypeDescr,
            pThis->getBridge()->getCpp2Uno() );
        ::uno_destructData(
            pCppReturn, pReturnTypeDescr, cpp_release );
    }
    // return type
    if (pReturnTypeDescr)
    {
        TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
    }
}

}

namespace bridges { namespace cpp_uno { namespace shared {

void unoInterfaceProxyDispatch(
    uno_Interface * pUnoI, const typelib_TypeDescription * pMemberDescr,
    void * pReturn, void * pArgs[], uno_Any ** ppException )
{
    // is my surrogate
    bridges::cpp_uno::shared::UnoInterfaceProxy * pThis
        = static_cast< bridges::cpp_uno::shared::UnoInterfaceProxy * >(pUnoI);
    
    switch (pMemberDescr->eTypeClass)
    {
    case typelib_TypeClass_INTERFACE_ATTRIBUTE:
    {
        VtableSlot aVtableSlot(
            getVtableSlot(
                reinterpret_cast<
                    typelib_InterfaceAttributeTypeDescription const * >(
                        pMemberDescr)));
        if (pReturn)
        {
            // dependent dispatch
            cpp_call(
                pThis, aVtableSlot,
                ((typelib_InterfaceAttributeTypeDescription *)pMemberDescr)->pAttributeTypeRef,
                0, 0, // no params
                pReturn, pArgs, ppException );
        }
        else
        {
            // is SET
            typelib_MethodParameter aParam;
            aParam.pTypeRef =
                ((typelib_InterfaceAttributeTypeDescription *)pMemberDescr)->pAttributeTypeRef;
            aParam.bIn		= sal_True;
            aParam.bOut		= sal_False;
            
            typelib_TypeDescriptionReference * pReturnTypeRef = 0;
            OUString aVoidName( RTL_CONSTASCII_USTRINGPARAM("void") );
            typelib_typedescriptionreference_new(
                &pReturnTypeRef, typelib_TypeClass_VOID, aVoidName.pData );
            
            // dependent dispatch
            aVtableSlot.index += 1; // get, then set method
            cpp_call(
                pThis, aVtableSlot,
                pReturnTypeRef,
                1, &aParam,
                pReturn, pArgs, ppException );
            
            typelib_typedescriptionreference_release( pReturnTypeRef );
        }
        
        break;
    }
    case typelib_TypeClass_INTERFACE_METHOD:
    {
        VtableSlot aVtableSlot(
            getVtableSlot(
                reinterpret_cast<
                    typelib_InterfaceMethodTypeDescription const * >(
                        pMemberDescr)));
        switch (aVtableSlot.index)
        {
            // standard calls
        case 1: // acquire uno interface
            (*pUnoI->acquire)( pUnoI );
            *ppException = 0;
            break;
        case 2: // release uno interface
            (*pUnoI->release)( pUnoI );
            *ppException = 0;
            break;
        case 0: // queryInterface() opt
        {
            typelib_TypeDescription * pTD = 0;
            TYPELIB_DANGER_GET( &pTD, reinterpret_cast< Type * >( pArgs[0] )->getTypeLibType() );
            if (pTD)
            {
                uno_Interface * pInterface = 0;
                (*pThis->pBridge->getUnoEnv()->getRegisteredInterface)(
                    pThis->pBridge->getUnoEnv(),
                    (void **)&pInterface, pThis->oid.pData, (typelib_InterfaceTypeDescription *)pTD );
            
                if (pInterface)
                {
                    ::uno_any_construct(
                        reinterpret_cast< uno_Any * >( pReturn ),
                        &pInterface, pTD, 0 );
                    (*pInterface->release)( pInterface );
                    TYPELIB_DANGER_RELEASE( pTD );
                    *ppException = 0;
                    break;
                }
                TYPELIB_DANGER_RELEASE( pTD );
            }
        } // else perform queryInterface()
        default:
            // dependent dispatch
            cpp_call(
                pThis, aVtableSlot,
                ((typelib_InterfaceMethodTypeDescription *)pMemberDescr)->pReturnTypeRef,
                ((typelib_InterfaceMethodTypeDescription *)pMemberDescr)->nParams,
                ((typelib_InterfaceMethodTypeDescription *)pMemberDescr)->pParams,
                pReturn, pArgs, ppException );
        }
        break;
    }
    default:
    {
        ::com::sun::star::uno::RuntimeException aExc(
            OUString( RTL_CONSTASCII_USTRINGPARAM("illegal member type description!") ),
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >() );
        
        Type const & rExcType = ::getCppuType( &aExc );
        // binary identical null reference
        ::uno_type_any_construct( *ppException, &aExc, rExcType.getTypeLibType(), 0 );
    }
    }
}

} } }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
