/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 *  $RCSfile: corbamaker.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: ts $ $Date: 2000-12-11 14:20:04 $
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

#include <stdio.h>

#ifndef _CODEMAKER_TYPEMANAGER_HXX_
#include <codemaker/typemanager.hxx>
#endif
#ifndef _CODEMAKER_DEPENDENCY_HXX_
#include <codemaker/dependency.hxx>
#endif

#include "corbaoptions.hxx"
#include "corbatype.hxx"

using namespace rtl;

sal_Bool produceAllTypes(const OString& typeName,
                        TypeManager& typeMgr, 
                        TypeDependency& typeDependencies,
                        CorbaOptions* pOptions,
                        sal_Bool bFullScope,
                        FileStream& o,
                        TypeSet* pAllreadyDumped,
                        TypeSet* generatedConversion)

    throw( CannotDumpException )
{
    if (!produceType(typeName, typeMgr,	typeDependencies, pOptions, o, pAllreadyDumped, generatedConversion))
    {
        fprintf(stderr, "%s ERROR: %s\n", 
                pOptions->getProgramName().getStr(), 
                OString("cannot dump Type '" + typeName + "'").getStr());
        exit(99);
    }

    RegistryKey	typeKey = typeMgr.getTypeKey(typeName);
    RegistryKeyNames subKeys;
    
    if (typeKey.getKeyNames(OUString(), subKeys))
        return sal_False;
    
    OString tmpName;
    for (sal_uInt32 i=0; i < subKeys.getLength(); i++)
    {
        tmpName = OUStringToOString(subKeys.getElement(i), RTL_TEXTENCODING_UTF8);

        if (pOptions->isValid("-B"))
            tmpName = tmpName.copy(tmpName.indexOf('/', 1) + 1);
        else
            tmpName = tmpName.copy(1);

        if (bFullScope)
        {
            if (!produceAllTypes(tmpName, typeMgr, typeDependencies, pOptions, sal_True, o, pAllreadyDumped, generatedConversion))
                return sal_False;
        } else
        {
            if (!produceType(tmpName, typeMgr, typeDependencies, pOptions, o, pAllreadyDumped, generatedConversion))
                return sal_False;
        }
    }
    
    return sal_True;			
}

#if (defined UNX) || (defined OS2)
int main( int argc, char * argv[] )
#else
int _cdecl main( int argc, char * argv[] )
#endif
{
    CorbaOptions options;

    try 
    {
        if (!options.initOptions(argc, argv))
        {
            exit(1);
        }
    }
    catch( IllegalArgument& e)
    {
        fprintf(stderr, "Illegal option: %s\n", e.m_message.getStr());
        exit(99);
    }

    RegistryTypeManager typeMgr;
    TypeDependency		typeDependencies;
    
    if (!typeMgr.init(!options.isValid("-T"), options.getInputFiles()))
    {
        fprintf(stderr, "%s : init registries failed, check your registry files.\n", options.getProgramName().getStr());
        exit(99);
    }

    if (options.isValid("-B"))
    {
        typeMgr.setBase(options.getOption("-B"));
    }

    try 
    {
        TypeSet generatedConversion;
        FileStream cppFile;
        OString outPath;
        if (options.isValid("-O"))
            outPath = options.getOption("-O");
        
        cppFile.openFile(outPath);
        
        if(!cppFile.isValid())
        {
            OString message("cannot open ");
            message += outPath + " for writing";
            throw CannotDumpException(message);
        }
        
        if (options.isValid("-H"))
        {
            OString corbaHeader = options.getOption("-H");
            
            cppFile << "#include <"
                    << corbaHeader
                    << ">" << endl << endl;
            
            CorbaType::dumpDefaultHxxIncludes(cppFile);
            cppFile << endl;
        }
        
        if (options.isValid("-T"))
        {
            OString tOption(options.getOption("-T"));
            sal_uInt32 count = tOption.getTokenCount(';');

            OString typeName, tmpName;
            sal_Bool ret = sal_False;
            for (sal_uInt32 i = 0; i < count; i++)
            {
                typeName = tOption.getToken(i, ';');
                
                tmpName = typeName.getToken(typeName.getTokenCount('.') - 1, '.');
                if (tmpName == "*")
                {
                    // produce this type and his scope, but the scope is not recursively  generated.
                    if (typeName.equals("*"))
                    {
                        tmpName = "/";
                    } else
                    {
                        tmpName = typeName.copy(0, typeName.lastIndexOf('.')).replace('.', '/');
                        if (tmpName.getLength() == 0) 
                            tmpName = "/";
                        else
                            tmpName.replace('.', '/');
                    }
                    ret = produceAllTypes(tmpName, typeMgr, typeDependencies, &options, sal_False, cppFile, NULL, &generatedConversion);
                } else
                {
                    // produce only this type
                    ret = produceType(typeName.replace('.', '/'), typeMgr, typeDependencies, &options, cppFile, NULL, &generatedConversion);
                }

                if (!ret)
                {
                    fprintf(stderr, "%s ERROR: %s\n", 
                            options.getProgramName().getStr(), 
                            OString("cannot dump Type '" + typeName + "'").getStr());
                    exit(99);
                }
            }
        } else
        {
            // produce all types
            if (!produceAllTypes("/", typeMgr, typeDependencies, &options, sal_True, cppFile, NULL, &generatedConversion))
            {
                fprintf(stderr, "%s ERROR: %s\n", 
                        options.getProgramName().getStr(), 
                        "an error occurs while dumping all types.");
                exit(99);
            }
        }
        
        cppFile << "namespace bonobobridge {\n"
                << "const ConversionInfo* get_conversion_functions() {\n"
                << "  static ConversionInfo allFunctions[" << generatedConversion.size()+1<< "] = {\n";
        
        for (TypeSet::iterator iter = generatedConversion.begin(); iter != generatedConversion.end(); iter++)
        {
            cppFile << "    {\"" << (*iter).getStr() << "\""
                    << ", &TC_" << (*iter).replace('/','_').getStr() << "_struct"
                    << ", sizeof(" << (*iter).replace('/','_').getStr() << ")"
                    << ", convert_b2u_" << (*iter).replace('/','_').getStr()
                    << ", convert_u2b_" <<  (*iter).replace('/','_').getStr()
                    << " },\n";
        } 
        
        cppFile << "    {NULL, NULL, 0 , NULL, NULL} };\n"
                << "  return allFunctions;\n"
                << "}\n"
                << "}; // namespace bonobobridge\n";

        cppFile.close();
    }
    catch( CannotDumpException& e)
    {
        fprintf(stderr, "%s ERROR: %s\n", 
                options.getProgramName().getStr(), 
                e.m_message.getStr());
        exit(99);
    }

    return 0;
}


