/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: rdbtype.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 05:06:14 $
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

#include <stdio.h>
#ifndef _RTL_ALLOC_H_
#include	<rtl/alloc.h>
#endif

#ifndef _RTL_USTRING_HXX_
#include	<rtl/ustring.hxx>
#endif

#ifndef _RTL_STRBUF_HXX_
#include	<rtl/strbuf.hxx>
#endif

#include	"rdbtype.hxx"
#include	"rdboptions.hxx"

using namespace rtl;

sal_Bool isBaseType(const OString& type)
{
    if ( type.equals("long") ||
          type.equals("short") ||
          type.equals("hyper") ||
          type.equals("string") ||
          type.equals("boolean") ||
          type.equals("char") ||
          type.equals("byte") ||
          type.equals("any") ||
          type.equals("type") ||
          type.equals("float") ||
          type.equals("double") ||
          type.equals("octet") ||
          type.equals("void") ||
          type.equals("unsigned long") ||
          type.equals("unsigned short") ||
          type.equals("unsigned hyper") )
        return sal_True;
        
    return sal_False;
}	

sal_Bool produceDependedTypes(const OString& typeName,
                              TypeManager& typeMgr, 
                              TypeDependency& typeDependencies,
                              RdbOptions* pOptions,
                              FileStream& o, 
                              RegistryKey& regKey,
                              StringSet& filterTypes)
    throw( CannotDumpException )
{
    sal_Bool ret = sal_True;
    
    TypeUsingSet usingSet(typeDependencies.getDependencies(typeName));

    TypeUsingSet::const_iterator iter = usingSet.begin();
    OString sTypeName;
    sal_Int32 index = 0;
    while (iter != usingSet.end())
    {
        sTypeName = (*iter).m_type;
        if ((index = sTypeName.lastIndexOf(']')) > 0)
            sTypeName = sTypeName.copy(index + 1);
        
        if ( !isBaseType(sTypeName) )
        {
            if (!produceType(sTypeName,
                                typeMgr,
                             typeDependencies,
                             pOptions,
                             o, regKey,
                             filterTypes,
                             sal_True))
            {
                fprintf(stderr, "%s ERROR: %s\n", 
                        pOptions->getProgramName().getStr(), 
                        OString("cannot dump Type '" + sTypeName + "'").getStr());
                cleanUp(sal_True);
                exit(99);
            }
        }
        iter++;
    }		

    return ret; 
}

//*************************************************************************
// produceType
//*************************************************************************
sal_Bool produceType(const OString& typeName,
                     TypeManager& typeMgr, 
                     TypeDependency& typeDependencies,
                     RdbOptions* pOptions,
                     FileStream& o, 
                     RegistryKey& regKey,
                     StringSet& filterTypes,
                     sal_Bool bDepend)
    throw( CannotDumpException )
{
    if (typeDependencies.isGenerated(typeName) )
        return sal_True;
/*
    RegistryKey		typeKey = typeMgr.getTypeKey(typeName);

    if (!typeKey.isValid())
        return sal_False;	
*/		
    if( !checkTypeDependencies(typeMgr, typeDependencies, typeName, bDepend))
        return sal_False;

    if ( !checkFilterTypes(typeName) )
    {
        if ( pOptions->generateTypeList() )
        {
            o << typeName.getStr() << "\n";	
        } else
        {
/*
            RegValueType 	valueType;
            sal_uInt32		valueSize;
            
            if (typeKey.getValueInfo(OUString(), &valueType, &valueSize))
            {
                if (typeName.equals("/"))
                    return sal_True;
                else
                    return sal_False;
            }

            sal_uInt8* pBuffer = (sal_uInt8*)rtl_allocateMemory(valueSize);
        
            if (typeKey.getValue(OUString(), pBuffer))
            {
                rtl_freeMemory(pBuffer);
                return sal_False;
            }
*/
            TypeReader reader = typeMgr.getTypeReader(typeName);

            if (!reader.isValid())
            {
                if (typeName.equals("/"))
                {
                    return sal_True;
                } else
                {
                    return sal_False;
                }
            }
            RegistryKey typeKey;
            if ( regKey.createKey( OStringToOUString(typeName, RTL_TEXTENCODING_UTF8), typeKey) )
            {
//				rtl_freeMemory(pBuffer);
                return sal_False;
            }
            
            if ( typeKey.setValue(OUString(), RG_VALUETYPE_BINARY, (void*)reader.getBlop(), reader.getBlopSize()) )
//			if ( typeKey.setValue(OUString(), valueType, pBuffer, valueSize) )
            {
//				rtl_freeMemory(pBuffer);
                return sal_False;
            }
        
//			rtl_freeMemory(pBuffer);
        }
    }
    
    typeDependencies.setGenerated(typeName);
    sal_Bool ret = produceDependedTypes(typeName, typeMgr, typeDependencies, 
                                        pOptions, o, regKey, filterTypes);	

    return ret;
}



