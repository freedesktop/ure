/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: constr.hxx,v $
 * $Revision: 1.13 $
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
#ifndef CONSTR_HXX
#define CONSTR_HXX

#include "prim.hxx"


namespace cppu
{

//##################################################################################################
//#### construction ################################################################################
//##################################################################################################

//--------------------------------------------------------------------------------------------------
inline void _defaultConstructUnion(
    void * pMem,
    typelib_TypeDescription * pTypeDescr )
    SAL_THROW( () )
{
    ::uno_type_constructData(
        (char *)pMem + ((typelib_UnionTypeDescription *)pTypeDescr)->nValueOffset,
        ((typelib_UnionTypeDescription *)pTypeDescr)->pDefaultTypeRef );
    *(sal_Int64 *)pMem = ((typelib_UnionTypeDescription *)pTypeDescr)->nDefaultDiscriminant;
}
//==================================================================================================
void defaultConstructStruct(
    void * pMem,
    typelib_CompoundTypeDescription * pCompType )
    SAL_THROW( () );
//--------------------------------------------------------------------------------------------------
inline void _defaultConstructStruct(
    void * pMem,
    typelib_CompoundTypeDescription * pTypeDescr )
    SAL_THROW( () )
{
    if (pTypeDescr->pBaseTypeDescription)
    {
        defaultConstructStruct( pMem, pTypeDescr->pBaseTypeDescription );
    }
    
    typelib_TypeDescriptionReference ** ppTypeRefs = (pTypeDescr)->ppTypeRefs;
    sal_Int32 * pMemberOffsets = pTypeDescr->pMemberOffsets;
    sal_Int32 nDescr = pTypeDescr->nMembers;
    
    while (nDescr--)
    {
        ::uno_type_constructData( (char *)pMem + pMemberOffsets[nDescr], ppTypeRefs[nDescr] );
    }
}

//--------------------------------------------------------------------------------------------------
inline void _defaultConstructArray(
    void * pMem,
    typelib_ArrayTypeDescription * pTypeDescr )
{
    typelib_TypeDescription * pElementType = NULL;
    TYPELIB_DANGER_GET( &pElementType, ((typelib_IndirectTypeDescription *)pTypeDescr)->pType );
    sal_Int32 nTotalElements = pTypeDescr->nTotalElements;
    sal_Int32 nElementSize = pElementType->nSize;
    sal_Int32 i;
    switch ( pElementType->eTypeClass )
    {
    case typelib_TypeClass_CHAR:
    case typelib_TypeClass_BOOLEAN:
    case typelib_TypeClass_BYTE:
    case typelib_TypeClass_SHORT:
    case typelib_TypeClass_UNSIGNED_SHORT:
    case typelib_TypeClass_LONG:
    case typelib_TypeClass_UNSIGNED_LONG:
    case typelib_TypeClass_HYPER:
    case typelib_TypeClass_UNSIGNED_HYPER:
    case typelib_TypeClass_FLOAT:
    case typelib_TypeClass_DOUBLE:
    case typelib_TypeClass_INTERFACE:
        ::rtl_zeroMemory(pMem, nElementSize * nTotalElements);
        break;

    case typelib_TypeClass_STRING:
        for (i=0; i < nTotalElements; i++)
        {
            rtl_uString** ppElement = (rtl_uString **)pMem + i;
            *ppElement = 0;
            rtl_uString_new( ppElement);			
        }		
        break;
    case typelib_TypeClass_TYPE:
        for (i=0; i < nTotalElements; i++)
        {
            typelib_TypeDescriptionReference** ppElement = (typelib_TypeDescriptionReference **)pMem + i;
            *ppElement = _getVoidType();
        }
        break;
    case typelib_TypeClass_ANY:
        for (i=0; i < nTotalElements; i++)
        {
            CONSTRUCT_EMPTY_ANY( (uno_Any *)pMem + i );
        }
        break;
    case typelib_TypeClass_ENUM:
        for (i=0; i < nTotalElements; i++)
        {
            *((sal_Int32 *)pMem + i) = ((typelib_EnumTypeDescription *)pElementType)->nDefaultEnumValue;
        }
        break;
    case typelib_TypeClass_STRUCT:
    case typelib_TypeClass_EXCEPTION:
        for (i=0; i < nTotalElements; i++)
        {
            _defaultConstructStruct( (sal_Char*)pMem + i * nElementSize, (typelib_CompoundTypeDescription *)pElementType );
        }
        break;
    case typelib_TypeClass_UNION:
        for (i=0; i < nTotalElements; i++)
        {
            _defaultConstructUnion( (sal_Char*)pMem + i * nElementSize, pElementType );
        }
        break;
    case typelib_TypeClass_SEQUENCE:
        for (i=0; i < nTotalElements; i++)
        {
            uno_Sequence** ppElement = (uno_Sequence **)pMem + i; 
            *ppElement = createEmptySequence();
        }
        break;
    default:
        OSL_ASSERT(false);
        break;
    }
    TYPELIB_DANGER_RELEASE( pElementType );
}

//--------------------------------------------------------------------------------------------------
inline void _defaultConstructData(
    void * pMem,
    typelib_TypeDescriptionReference * pType,
    typelib_TypeDescription * pTypeDescr )
    SAL_THROW( () )
{	
    switch (pType->eTypeClass)
    {
    case typelib_TypeClass_CHAR:
        *(sal_Unicode *)pMem = '\0';
        break;
    case typelib_TypeClass_BOOLEAN:
        *(sal_Bool *)pMem = sal_False;
        break;
    case typelib_TypeClass_BYTE:
        *(sal_Int8 *)pMem = 0;
        break;
    case typelib_TypeClass_SHORT:
    case typelib_TypeClass_UNSIGNED_SHORT:
        *(sal_Int16 *)pMem = 0;
        break;
    case typelib_TypeClass_LONG:
    case typelib_TypeClass_UNSIGNED_LONG:
        *(sal_Int32 *)pMem = 0;
        break;
    case typelib_TypeClass_HYPER:
    case typelib_TypeClass_UNSIGNED_HYPER:
        *(sal_Int64 *)pMem = 0;
        break;
    case typelib_TypeClass_FLOAT:
        *(float *)pMem = 0.0;
        break;
    case typelib_TypeClass_DOUBLE:
        *(double *)pMem = 0.0;
        break;
    case typelib_TypeClass_STRING:
        *(rtl_uString **)pMem = 0;
        ::rtl_uString_new( (rtl_uString **)pMem );
        break;
    case typelib_TypeClass_TYPE:
        *(typelib_TypeDescriptionReference **)pMem = _getVoidType();
        break;
    case typelib_TypeClass_ANY:
        CONSTRUCT_EMPTY_ANY( (uno_Any *)pMem );
        break;
    case typelib_TypeClass_ENUM:
        if (pTypeDescr)
        {
            *(sal_Int32 *)pMem = ((typelib_EnumTypeDescription *)pTypeDescr)->nDefaultEnumValue;
        }
        else
        {
            TYPELIB_DANGER_GET( &pTypeDescr, pType );
            *(sal_Int32 *)pMem = ((typelib_EnumTypeDescription *)pTypeDescr)->nDefaultEnumValue;
            TYPELIB_DANGER_RELEASE( pTypeDescr );
        }
        break;
    case typelib_TypeClass_STRUCT:
    case typelib_TypeClass_EXCEPTION:
        if (pTypeDescr)
        {
            _defaultConstructStruct( pMem, (typelib_CompoundTypeDescription *)pTypeDescr );
        }
        else
        {
            TYPELIB_DANGER_GET( &pTypeDescr, pType );
            _defaultConstructStruct( pMem, (typelib_CompoundTypeDescription *)pTypeDescr );
            TYPELIB_DANGER_RELEASE( pTypeDescr );
        }
        break;
    case typelib_TypeClass_ARRAY:
        if (pTypeDescr)
        {
            _defaultConstructArray( pMem, (typelib_ArrayTypeDescription *)pTypeDescr );
        }
        else
        {
            TYPELIB_DANGER_GET( &pTypeDescr, pType );
            _defaultConstructArray( pMem, (typelib_ArrayTypeDescription *)pTypeDescr );
            TYPELIB_DANGER_RELEASE( pTypeDescr );
        }
        break;
    case typelib_TypeClass_UNION:
        if (pTypeDescr)
        {
            _defaultConstructUnion( pMem, pTypeDescr );
        }
        else
        {
            TYPELIB_DANGER_GET( &pTypeDescr, pType );
            _defaultConstructUnion( pMem, pTypeDescr );
            TYPELIB_DANGER_RELEASE( pTypeDescr );
        }
        break;
    case typelib_TypeClass_SEQUENCE:
        *(uno_Sequence **)pMem = createEmptySequence();
        break;
    case typelib_TypeClass_INTERFACE:
        *(void **)pMem = 0; // either cpp or c-uno interface
        break;
    default:
        OSL_ASSERT(false);
        break;
    }
}

}

#endif
