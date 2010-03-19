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

#include "share.hxx"


using namespace ::rtl;
using namespace ::com::sun::star::uno;

namespace
{


//==================================================================================================
static void callVirtualMethod(
    void * pAdjustedThisPtr,
    sal_Int32 nVtableIndex,
    void * pRegisterReturn,
    typelib_TypeClass eReturnType,
    char * pPT,
    sal_Int32 * pStackLongs,
    sal_Int32 nStackLongs)
{

  // parameter list is mixed list of * and values
  // reference parameters are pointers

  // the basic idea here is to use gpr[8] as a storage area for
  // the future values of registers r3 to r10 needed for the call, 
  // and similarly fpr[8] as a storage area for the future values
  // of floating point registers f1 to f8

     unsigned long * mfunc;        // actual function to be invoked
     void (*ptr)();
     int gpr[8];                   // storage for gpregisters, map to r3-r10
     int off;                      // offset used to find function
     double fpr[8];                // storage for fpregisters, map to f1-f8
     int n;                        // number of gprs mapped so far
     int f;                        // number of fprs mapped so far
     long *p;                      // pointer to parameter overflow area
     int c;                        // character of parameter type being decoded
     double dret;                  // temporary function return values
     int iret, iret2;

     // Because of the Power PC calling conventions we could be passing
     // parameters in both register types and on the stack. To create the
     // stack parameter area we need we now simply allocate local
     // variable storage param[] that is at least the size of the parameter stack
     // (more than enough space) which we can overwrite the parameters into.

     // Note: This keeps us from having to decode the signature twice and
     // prevents problems with later local variables.

     // Note: could require up to  2*nStackLongs words of parameter stack area 
     // if the call has many float parameters (i.e. floats take up only 1 
     // word on the stack but take 2 words in parameter area in the 
     // stack frame .

     // Update! floats on the outgoing parameter stack only take up 1 word
     // (stfs is used) which is not correct according to the ABI but we
     // will match what the compiler does until this is figured out 

     // this grows the current stack to the appropriate size
     // and sets the outgoing stack pointer p to the right place
     __asm__ __volatile__ (
          "rlwinm %0,%0,3,3,28\n\t"
          "addi %0,%0,22\n\t"
          "rlwinm %0,%0,0,4,28\n\t"
          "lwz 0,0(1)\n\t"
          "subf 1,%0,1\n\t"
          "stw 0,0(1)\n\t"
          : : "r" (nStackLongs) : "0" );

     __asm__ __volatile__ ( "addi %0,1,8" : "=r" (p) : );

     // never called
     // if (! pAdjustedThisPtr ) dummy_can_throw_anything("xxx"); // address something


     // now begin to load the C++ function arguments into storage
     n = 0;
     f = 0;

     // now we need to parse the entire signature string */
     // until we get the END indicator */

     // treat complex return pointer like any other parameter //

#if 0
     /* Let's figure out what is really going on here*/
     fprintf(stderr,"callVirtualMethod paramters string is %s\n",pPT);
     int k = nStackLongs;
     long * q = (long *)pStackLongs;
     while (k > 0) {
       fprintf(stderr,"uno stack is: %x\n",*q);
       k--;
       q++;
     }
#endif

     /* parse the argument list up to the ending ) */
     while (*pPT != 'X') {
       c = *pPT;
       switch (c) {
       case 'D':                   /* type is double */
            if (f < 8) {
               fpr[f++] = *((double *)pStackLongs);   /* store in register */
        } else {
           if (((long) p) & 4)          
              p++;
               *p++ = *pStackLongs;       /* or on the parameter stack */
               *p++ = *(pStackLongs + 1);
        }
            pStackLongs += 2;
            break;

       case 'F':                   /* type is float */
     /* this assumes that floats are stored as 1 32 bit word on param
        stack and that if passed in parameter stack to C, should be
        as double word.

            Whoops: the abi is not actually followed by gcc, need to 
            store floats as a *single* word on outgoing parameter stack
            to match what gcc actually does
     */
            if (f < 8) {
               fpr[f++] = *((float *)pStackLongs);
        } else {
#if 0 /* if abi were followed */
           if (((long) p) & 4)          
              p++;
           *((double *)p) = *((float *)pStackLongs);
               p += 2;
#else
           *((float *)p) = *((float *)pStackLongs);
               p += 1;
#endif
        }
            pStackLongs += 1;
            break;

       case 'H':                /* type is long long */
            if (n & 1) n++; 	/* note even elements gpr[] will map to
                                   odd registers*/
            if (n <= 6) {
               gpr[n++] = *pStackLongs;
               gpr[n++] = *(pStackLongs+1);
        } else {
           if (((long) p) & 4)          
              p++;
               *p++ = *pStackLongs;
               *p++ = *(pStackLongs+1);
        }
            pStackLongs += 2;
            break;

       case 'S':
            if (n < 8) {
               gpr[n++] = *((unsigned short*)pStackLongs);
        } else {
               *p++ = *((unsigned short *)pStackLongs);
        }
            pStackLongs += 1;
            break;

       case 'B':
            if (n < 8) {
               gpr[n++] = *((char *)pStackLongs);
        } else {
               *p++ = *((char *)pStackLongs);
        }
            pStackLongs += 1;
            break;

       default:
            if (n < 8) {
               gpr[n++] = *pStackLongs;
        } else {
               *p++ = *pStackLongs;
        }
            pStackLongs += 1;
            break;
       }
       pPT++;
     }

     /* figure out the address of the function we need to invoke */
     off = nVtableIndex; 
     off = off * 4;                         // 4 bytes per slot
     mfunc = *((unsigned long **)pAdjustedThisPtr);    // get the address of the vtable
     mfunc = (unsigned long *)((char *)mfunc + off); // get the address from the vtable entry at offset 
     mfunc = *((unsigned long **)mfunc);                 // the function is stored at the address
     ptr = (void (*)())mfunc;   

    /* Set up the machine registers and invoke the function */

    __asm__ __volatile__ (
        "lwz	3,	0(%0)\n\t"
        "lwz	4,	4(%0)\n\t"
        "lwz	5,	8(%0)\n\t"
        "lwz	6,	12(%0)\n\t"
        "lwz	7,	16(%0)\n\t"
        "lwz	8,	20(%0)\n\t"
        "lwz	9,	24(%0)\n\t"
        "lwz	10,	28(%0)\n\t"
        "lfd	1,	0(%1)\n\t"
        "lfd	2,	8(%1)\n\t"
        "lfd	3,	16(%1)\n\t"
        "lfd	4,	24(%1)\n\t"
        "lfd	5,	32(%1)\n\t"
        "lfd	6,	40(%1)\n\t"
        "lfd	7,	48(%1)\n\t"
        "lfd	8,	56(%1)\n\t"
            : : "r" (gpr), "r" (fpr)
        : "0", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"
    );

    (*ptr)();

    __asm__ __volatile__ (
        "mr	%1,	3\n\t"
        "mr	%2,	4\n\t"
        "fmr	%0,	1\n\t"
        : "=f" (dret), "=r" (iret), "=r" (iret2) : );

    switch( eReturnType )
    {
        case typelib_TypeClass_HYPER:
        case typelib_TypeClass_UNSIGNED_HYPER:
                ((long*)pRegisterReturn)[0] = iret;
            ((long*)pRegisterReturn)[1] = iret2;
        case typelib_TypeClass_LONG:
        case typelib_TypeClass_UNSIGNED_LONG:
        case typelib_TypeClass_ENUM:
            ((long*)pRegisterReturn)[0] = iret;
            break;
        case typelib_TypeClass_CHAR:
        case typelib_TypeClass_SHORT:
        case typelib_TypeClass_UNSIGNED_SHORT:
                *(unsigned short*)pRegisterReturn = (unsigned short)iret;
            break;
        case typelib_TypeClass_BOOLEAN:
        case typelib_TypeClass_BYTE:
                *(unsigned char*)pRegisterReturn = (unsigned char)iret;
            break;
        case typelib_TypeClass_FLOAT:
                *(float*)pRegisterReturn = (float)dret;
            break;
        case typelib_TypeClass_DOUBLE:
            *(double*)pRegisterReturn = dret;
            break;
    }
}


//================================================================================================== 
static void cpp_call(
    bridges::cpp_uno::shared::UnoInterfaceProxy * pThis,
    bridges::cpp_uno::shared::VtableSlot  aVtableSlot,
    typelib_TypeDescriptionReference * pReturnTypeRef,
    sal_Int32 nParams, typelib_MethodParameter * pParams,
    void * pUnoReturn, void * pUnoArgs[], uno_Any ** ppUnoExc )
{
      // max space for: [complex ret ptr], values|ptr ...
      char * pCppStack		=
          (char *)alloca( sizeof(sal_Int32) + ((nParams+2) * sizeof(sal_Int64)) );
      char * pCppStackStart	= pCppStack;
    
        // need to know parameter types for callVirtualMethod so generate a signature string
        char * pParamType = (char *) alloca(nParams+2);
        char * pPT = pParamType;

    // return
    typelib_TypeDescription * pReturnTypeDescr = 0;
    TYPELIB_DANGER_GET( &pReturnTypeDescr, pReturnTypeRef );
    // OSL_ENSURE( pReturnTypeDescr, "### expected return type description!" );
    
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
            pCppReturn = *(void **)pCppStack = 
                              (bridges::cpp_uno::shared::relatesToInterfaceType( pReturnTypeDescr )
                   ? alloca( pReturnTypeDescr->nSize ): pUnoReturn); // direct way
                        *pPT++ = 'I'; //signify that a complex return type on stack
            pCppStack += sizeof(void *);
        }
    }
    // push this
        void* pAdjustedThisPtr = reinterpret_cast< void **>(pThis->getCppI()) + aVtableSlot.offset;
    *(void**)pCppStack = pAdjustedThisPtr;
    pCppStack += sizeof( void* );
        *pPT++ = 'I';

    // stack space
    // OSL_ENSURE( sizeof(void *) == sizeof(sal_Int32), "### unexpected size!" );
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
        
        if (!rParam.bOut && bridges::cpp_uno::shared::isSimpleType( pParamTypeDescr ))
        {
            uno_copyAndConvertData( pCppArgs[nPos] = pCppStack, pUnoArgs[nPos], pParamTypeDescr,
                                    pThis->getBridge()->getUno2Cpp() );
            
            switch (pParamTypeDescr->eTypeClass)
            {

                          // we need to know type of each param so that we know whether to use
                          // gpr or fpr to pass in parameters:
                          // Key: I - int, long, pointer, etc means pass in gpr
                          //      B - byte value passed in gpr
                          //      S - short value passed in gpr
                          //      F - float value pass in fpr
                          //      D - double value pass in fpr
                          //      H - long long int pass in proper pairs of gpr (3,4) (5,6), etc
                          //      X - indicates end of parameter description string

                  case typelib_TypeClass_LONG:
                  case typelib_TypeClass_UNSIGNED_LONG:
                  case typelib_TypeClass_ENUM:
                *pPT++ = 'I';
                break;
                   case typelib_TypeClass_SHORT:
                  case typelib_TypeClass_CHAR:
                  case typelib_TypeClass_UNSIGNED_SHORT:
                            *pPT++ = 'S';
                            break;
                  case typelib_TypeClass_BOOLEAN:
                  case typelib_TypeClass_BYTE:
                            *pPT++ = 'B';
                            break;
                  case typelib_TypeClass_FLOAT:
                            *pPT++ = 'F';
                break;
                case typelib_TypeClass_DOUBLE:
                *pPT++ = 'D';
                pCppStack += sizeof(sal_Int32); // extra long
                break;
            case typelib_TypeClass_HYPER:
            case typelib_TypeClass_UNSIGNED_HYPER:
                *pPT++ = 'H';
                pCppStack += sizeof(sal_Int32); // extra long
            }

            // no longer needed
            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        else // ptr to complex value | ref
        {
            if (! rParam.bIn) // is pure out
            {
                // cpp out is constructed mem, uno out is not!
                uno_constructData(
                    *(void **)pCppStack = pCppArgs[nPos] = alloca( pParamTypeDescr->nSize ),
                    pParamTypeDescr );
                pTempIndizes[nTempIndizes] = nPos; // default constructed for cpp call
                // will be released at reconversion
                ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
            }
            // is in/inout
            else if (bridges::cpp_uno::shared::relatesToInterfaceType( pParamTypeDescr ))
            {
                uno_copyAndConvertData(
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
                        // KBH: FIXME: is this the right way to pass these
                        *pPT++='I';
        }
        pCppStack += sizeof(sal_Int32); // standard parameter length
    }
  
        // terminate the signature string
        *pPT++='X';
        *pPT=0;

    try
    {
        OSL_ENSURE( !( (pCppStack - pCppStackStart ) & 3), "UNALIGNED STACK !!! (Please DO panic)" );
        callVirtualMethod(
            pAdjustedThisPtr, aVtableSlot.index,
            pCppReturn, pReturnTypeDescr->eTypeClass, pParamType,
            (sal_Int32 *)pCppStackStart, (pCppStack - pCppStackStart) / sizeof(sal_Int32) );
        // NO exception occured...
        *ppUnoExc = 0;
        
        // reconvert temporary params
        for ( ; nTempIndizes--; )
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            typelib_TypeDescription * pParamTypeDescr = ppTempParamTypeDescr[nTempIndizes];
            
            if (pParams[nIndex].bIn)
            {
                if (pParams[nIndex].bOut) // inout
                {
                    uno_destructData( pUnoArgs[nIndex], pParamTypeDescr, 0 ); // destroy uno value
                    uno_copyAndConvertData( pUnoArgs[nIndex], pCppArgs[nIndex], pParamTypeDescr,
                                            pThis->getBridge()->getCpp2Uno() );
                }
            }
            else // pure out
            {
                uno_copyAndConvertData( pUnoArgs[nIndex], pCppArgs[nIndex], pParamTypeDescr,
                                        pThis->getBridge()->getCpp2Uno() );
            }
            // destroy temp cpp param => cpp: every param was constructed
            uno_destructData( pCppArgs[nIndex], pParamTypeDescr, cpp_release );
            
            TYPELIB_DANGER_RELEASE( pParamTypeDescr );
        }
        // return value
        if (pCppReturn && pUnoReturn != pCppReturn)
        {
            uno_copyAndConvertData( pUnoReturn, pCppReturn, pReturnTypeDescr,
                                    pThis->getBridge()->getCpp2Uno() );
            uno_destructData( pCppReturn, pReturnTypeDescr, cpp_release );
        }
    }
     catch (...)
     {
          // fill uno exception
        fillUnoException( CPPU_CURRENT_NAMESPACE::__cxa_get_globals()->caughtExceptions, 
                                  *ppUnoExc, pThis->getBridge()->getCpp2Uno() );
        
        // temporary params
        for ( ; nTempIndizes--; )
        {
            sal_Int32 nIndex = pTempIndizes[nTempIndizes];
            // destroy temp cpp param => cpp: every param was constructed
            uno_destructData( pCppArgs[nIndex], ppTempParamTypeDescr[nTempIndizes], cpp_release );
            TYPELIB_DANGER_RELEASE( ppTempParamTypeDescr[nTempIndizes] );
        }
        // return type
        if (pReturnTypeDescr)
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
            = static_cast< bridges::cpp_uno::shared::UnoInterfaceProxy *> (pUnoI);
    typelib_InterfaceTypeDescription * pTypeDescr = pThis->pTypeDescr;
    
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
                        aVtableSlot.index += 1; //get then set method
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
