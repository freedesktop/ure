/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: regimpl.cxx,v $
 * $Revision: 1.28.10.1 $
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
#include "precompiled_registry.hxx"

#include <memory>
#include    <string.h>
#include	<stdio.h>

#if defined(UNX) || defined(OS2)
#include    <unistd.h>
#endif
#ifdef __MINGW32__
#include    <unistd.h>
#endif

#ifdef MACOSX
// Get the store.hxx inlines non-inline, solves crashes in cppumaker
#define inline
#endif

#include    "regimpl.hxx"

#ifdef MACOSX
// Get the store.hxx inlines non-inline, solves crashes in cppumaker
#undef inline
#endif

#ifndef __REGISTRY_REFLREAD_HXX__
#include    <registry/reflread.hxx>
#endif

#ifndef __REGISTRY_REFLWRIT_HXX__
#include    <registry/reflwrit.hxx>
#endif

#include "registry/reader.hxx"
#include "registry/refltype.hxx"
#include "registry/types.h"
#include "registry/version.h"

#ifndef __REFLCNST_HXX__
#include    "reflcnst.hxx"
#endif
#include    "keyimpl.hxx"
#include    <osl/thread.h>
#include    <rtl/alloc.h>
#include    <rtl/memory.h>
#include    <rtl/ustring.hxx>
#include    <rtl/ustrbuf.hxx>
#ifndef _ODL_FILE_HXX_
#include    <osl/file.hxx>
#endif


#if defined ( GCC ) && ( defined ( SCO ) )
sal_helper::ORealDynamicLoader* sal_helper::ODynamicLoader<RegistryTypeReader_Api>::m_pLoader = NULL;
#endif

namespace {

void printString(rtl::OUString const & s) {
    printf("\"");
    for (sal_Int32 i = 0; i < s.getLength(); ++i) {
        sal_Unicode c = s[i];
        if (c == '"' || c == '\\') {
            printf("\\%c", static_cast< char >(c));
        } else if (s[i] >= ' ' && s[i] <= '~') {
            printf("%c", static_cast< char >(c));
        } else {
            printf("\\u%04X", static_cast< unsigned int >(c));
        }
    }
    printf("\"");
}

void printFieldOrReferenceFlag(
    RTFieldAccess * flags, RTFieldAccess flag, char const * name, bool * first)
{
    if ((*flags & flag) != 0) {
        if (!*first) {
            printf("|");
        }
        *first = false;
        printf("%s", name);
        *flags &= ~flag;
    }
}

void printFieldOrReferenceFlags(RTFieldAccess flags) {
    if (flags == 0) {
        printf("none");
    } else {
        bool first = true;
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_READONLY, "readonly", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_OPTIONAL, "optional", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_MAYBEVOID, "maybevoid", &first);
        printFieldOrReferenceFlag(&flags, RT_ACCESS_BOUND, "bound", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_CONSTRAINED, "constrained", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_TRANSIENT, "transient", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_MAYBEAMBIGUOUS, "maybeambiguous", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_MAYBEDEFAULT, "maybedefault", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_REMOVEABLE, "removeable", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_ATTRIBUTE, "attribute", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_PROPERTY, "property", &first);
        printFieldOrReferenceFlag(&flags, RT_ACCESS_CONST, "const", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_READWRITE, "readwrite", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_PARAMETERIZED_TYPE, "parameterized type", &first);
        printFieldOrReferenceFlag(
            &flags, RT_ACCESS_PUBLISHED, "published", &first);
        if (flags != 0) {
            if (!first) {
                printf("|");
            }
            printf("<invalid (0x%04X)>", static_cast< unsigned int >(flags));
        }
    }
}

void dumpType(typereg::Reader const & reader, rtl::OString const & indent) {
    if (reader.isValid()) {
        printf("version: %ld\n", static_cast< long >(reader.getVersion()));
        printf("%sdocumentation: ", indent.getStr());
        printString(reader.getDocumentation());
        printf("\n");
        printf("%sfile name: ", indent.getStr());
        printString(reader.getFileName());
        printf("\n");
        printf("%stype class: ", indent.getStr());
        if (reader.isPublished()) {
            printf("published ");
        }
        switch (reader.getTypeClass()) {
        case RT_TYPE_INTERFACE:
            printf("interface");
            break;

        case RT_TYPE_MODULE:
            printf("module");
            break;

        case RT_TYPE_STRUCT:
            printf("struct");
            break;

        case RT_TYPE_ENUM:
            printf("enum");
            break;

        case RT_TYPE_EXCEPTION:
            printf("exception");
            break;

        case RT_TYPE_TYPEDEF:
            printf("typedef");
            break;

        case RT_TYPE_SERVICE:
            printf("service");
            break;

        case RT_TYPE_SINGLETON:
            printf("singleton");
            break;

        case RT_TYPE_CONSTANTS:
            printf("constants");
            break;

        default:
            printf(
                "<invalid (%ld)>", static_cast< long >(reader.getTypeClass()));
            break;
        }
        printf("\n");
        printf("%stype name: ", indent.getStr());
        printString(reader.getTypeName());
        printf("\n");
        printf(
            "%ssuper type count: %u\n", indent.getStr(),
            static_cast< unsigned int >(reader.getSuperTypeCount()));
        {for (sal_uInt16 i = 0; i < reader.getSuperTypeCount(); ++i) {
            printf(
                "%ssuper type name %u: ", indent.getStr(),
                static_cast< unsigned int >(i));
            printString(reader.getSuperTypeName(i));
            printf("\n");
        }}
        printf(
            "%sfield count: %u\n", indent.getStr(),
            static_cast< unsigned int >(reader.getFieldCount()));
        {for (sal_uInt16 i = 0; i < reader.getFieldCount(); ++i) {
            printf(
                "%sfield %u:\n", indent.getStr(),
                static_cast< unsigned int >(i));
            printf("%s    documentation: ", indent.getStr());
            printString(reader.getFieldDocumentation(i));
            printf("\n");
            printf("%s    file name: ", indent.getStr());
            printString(reader.getFieldFileName(i));
            printf("\n");
            printf("%s    flags: ", indent.getStr());
            printFieldOrReferenceFlags(reader.getFieldFlags(i));
            printf("\n");
            printf("%s    name: ", indent.getStr());
            printString(reader.getFieldName(i));
            printf("\n");
            printf("%s    type name: ", indent.getStr());
            printString(reader.getFieldTypeName(i));
            printf("\n");
            printf("%s    value: ", indent.getStr());
            RTConstValue value(reader.getFieldValue(i));
            switch (value.m_type) {
            case RT_TYPE_NONE:
                printf("none");
                break;

            case RT_TYPE_BOOL:
                printf("boolean %s", value.m_value.aBool ? "true" : "false");
                break;

            case RT_TYPE_BYTE:
                printf(
                    "byte 0x%02X",
                    static_cast< unsigned int >(value.m_value.aByte));
                break;

            case RT_TYPE_INT16:
                printf("short %d", static_cast< int >(value.m_value.aShort));
                break;

            case RT_TYPE_UINT16:
                printf(
                    "unsigned short %u",
                    static_cast< unsigned int >(value.m_value.aUShort));
                break;

            case RT_TYPE_INT32:
                printf("long %ld", static_cast< long >(value.m_value.aLong));
                break;

            case RT_TYPE_UINT32:
                printf(
                    "unsigned long %lu",
                    static_cast< unsigned long >(value.m_value.aULong));
                break;

            case RT_TYPE_INT64:
                // TODO: no portable way to print hyper values
                printf("hyper");
                break;

            case RT_TYPE_UINT64:
                // TODO: no portable way to print unsigned hyper values
                printf("unsigned hyper");
                break;

            case RT_TYPE_FLOAT:
                // TODO: no portable way to print float values
                printf("float");
                break;

            case RT_TYPE_DOUBLE:
                // TODO: no portable way to print double values
                printf("double");
                break;

            case RT_TYPE_STRING:
                printf("string ");
                printString(value.m_value.aString);
                break;

            default:
                printf("<invalid (%ld)>", static_cast< long >(value.m_type));
                break;
            }
            printf("\n");
        }}
        printf(
            "%smethod count: %u\n", indent.getStr(),
            static_cast< unsigned int >(reader.getMethodCount()));
        {for (sal_uInt16 i = 0; i < reader.getMethodCount(); ++i) {
            printf(
                "%smethod %u:\n", indent.getStr(),
                static_cast< unsigned int >(i));
            printf("%s    documentation: ", indent.getStr());
            printString(reader.getMethodDocumentation(i));
            printf("\n");
            printf("%s    flags: ", indent.getStr());
            switch (reader.getMethodFlags(i)) {
            case RT_MODE_ONEWAY:
                printf("oneway");
                break;

            case RT_MODE_TWOWAY:
                printf("synchronous");
                break;

            case RT_MODE_ATTRIBUTE_GET:
                printf("attribute get");
                break;

            case RT_MODE_ATTRIBUTE_SET:
                printf("attribute set");
                break;

            default:
                printf(
                    "<invalid (%ld)>",
                    static_cast< long >(reader.getMethodFlags(i)));
                break;
            }
            printf("\n");
            printf("%s    name: ", indent.getStr());
            printString(reader.getMethodName(i));
            printf("\n");
            printf("%s    return type name: ", indent.getStr());
            printString(reader.getMethodReturnTypeName(i));
            printf("\n");
            printf(
                "%s    parameter count: %u\n", indent.getStr(),
                static_cast< unsigned int >(reader.getMethodParameterCount(i)));
            for (sal_uInt16 j = 0; j < reader.getMethodParameterCount(i); ++j)
            {
                printf(
                    "%s    parameter %u:\n", indent.getStr(),
                    static_cast< unsigned int >(j));
                printf("%s        flags: ", indent.getStr());
                RTParamMode flags = reader.getMethodParameterFlags(i, j);
                bool rest = (flags & RT_PARAM_REST) != 0;
                switch (flags & ~RT_PARAM_REST) {
                case RT_PARAM_IN:
                    printf("in");
                    break;

                case RT_PARAM_OUT:
                    printf("out");
                    break;

                case RT_PARAM_INOUT:
                    printf("inout");
                    break;

                default:
                    printf("<invalid (%ld)>", static_cast< long >(flags));
                    rest = false;
                    break;
                }
                if (rest) {
                    printf("|rest");
                }
                printf("\n");
                printf("%s        name: ", indent.getStr());
                printString(reader.getMethodParameterName(i, j));
                printf("\n");
                printf("%s        type name: ", indent.getStr());
                printString(reader.getMethodParameterTypeName(i, j));
                printf("\n");
            }
            printf(
                "%s    exception count: %u\n", indent.getStr(),
                static_cast< unsigned int >(reader.getMethodExceptionCount(i)));
            for (sal_uInt16 j = 0; j < reader.getMethodExceptionCount(i); ++j)
            {
                printf(
                    "%s    exception type name %u: ", indent.getStr(),
                    static_cast< unsigned int >(j));
                printString(reader.getMethodExceptionTypeName(i, j));
                printf("\n");
            }
        }}
        printf(
            "%sreference count: %u\n", indent.getStr(),
            static_cast< unsigned int >(reader.getReferenceCount()));
        {for (sal_uInt16 i = 0; i < reader.getReferenceCount(); ++i) {
            printf(
                "%sreference %u:\n", indent.getStr(),
                static_cast< unsigned int >(i));
            printf("%s    documentation: ", indent.getStr());
            printString(reader.getReferenceDocumentation(i));
            printf("\n");
            printf("%s    flags: ", indent.getStr());
            printFieldOrReferenceFlags(reader.getReferenceFlags(i));
            printf("\n");
            printf("%s    sort: ", indent.getStr());
            switch (reader.getReferenceSort(i)) {
            case RT_REF_SUPPORTS:
                printf("supports");
                break;

            case RT_REF_EXPORTS:
                printf("exports");
                break;

            case RT_REF_TYPE_PARAMETER:
                printf("type parameter");
                break;

            default:
                printf(
                    "<invalid (%ld)>",
                    static_cast< long >(reader.getReferenceSort(i)));
                break;
            }
            printf("\n");
            printf("%s    type name: ", indent.getStr());
            printString(reader.getReferenceTypeName(i));
            printf("\n");
        }}
    } else {
        printf("<invalid>\n");
    }
}

}

//*********************************************************************
//  ORegistry()
//
ORegistry::ORegistry()
    : m_refCount(1)
    , m_readOnly(sal_False)
    , m_isOpen(sal_False)
    , ROOT( RTL_CONSTASCII_USTRINGPARAM("/") )
{
}

//*********************************************************************
//  ~ORegistry()
//
ORegistry::~ORegistry()
{
    if (m_openKeyTable.count(ROOT) > 0)
    {
        m_openKeyTable[ROOT]->release();
        delete(m_openKeyTable[ROOT]);
    }

    if (m_file.isValid())
        m_file.close();
}


//*********************************************************************
//  initRegistry
//
RegError ORegistry::initRegistry(const OUString& regName, RegAccessMode accessMode)
{
    OStoreFile      rRegFile;
    storeAccessMode sAccessMode = REG_MODE_OPEN;
    storeError      errCode;

    if (accessMode & REG_CREATE)
    {
        sAccessMode = REG_MODE_CREATE;
    } else
    if (accessMode & REG_READONLY)
    {
        sAccessMode = REG_MODE_OPENREAD;
        m_readOnly = sal_True;
    }

    if (0 == regName.getLength() &&
        store_AccessCreate == sAccessMode)
    {
        errCode = rRegFile.createInMemory();
    }
    else
    {
        errCode = rRegFile.create(regName, sAccessMode, REG_PAGESIZE);
    }

    if (errCode)
    {
        switch (errCode)
        {
            case  store_E_NotExists:
                return REG_REGISTRY_NOT_EXISTS;
            case store_E_LockingViolation:
                return REG_CANNOT_OPEN_FOR_READWRITE;
            default:
                return REG_INVALID_REGISTRY;
        }
    } else
    {
        OStoreDirectory rStoreDir;
        storeError      _err = rStoreDir.create(rRegFile, OUString(), OUString(), sAccessMode);

        if ( _err == store_E_None )
        {
            m_file = rRegFile;
            m_name = regName;
            m_isOpen = sal_True;

            m_openKeyTable[ROOT] = new ORegKey(ROOT, this);
            return REG_NO_ERROR;
        } else
            return REG_INVALID_REGISTRY;
    }
}


//*********************************************************************
//  closeRegistry
//
RegError ORegistry::closeRegistry()
{
    REG_GUARD(m_mutex);

    if (m_file.isValid())
    {
        closeKey(m_openKeyTable[ROOT]);
        m_file.close();
        m_isOpen = sal_False;
        return REG_NO_ERROR;
    } else
    {
        return REG_REGISTRY_NOT_EXISTS;
    }
}


//*********************************************************************
//  destroyRegistry
//
RegError ORegistry::destroyRegistry(const OUString& regName)
{
    REG_GUARD(m_mutex);

    if (regName.getLength())
    {
        ORegistry* pReg = new ORegistry();

        if (!pReg->initRegistry(regName, REG_READWRITE))
        {
            delete pReg;

            OUString systemName;
            if ( FileBase::getSystemPathFromFileURL(regName, systemName) != FileBase::E_None )
                systemName = regName;

            OString name( OUStringToOString(systemName, osl_getThreadTextEncoding()) );
            if (unlink(name) != 0)
            {
                return REG_DESTROY_REGISTRY_FAILED;
            }
        } else
        {
            return REG_DESTROY_REGISTRY_FAILED;
        }
    } else
    {
        if (m_refCount != 1 || isReadOnly())
        {
            return REG_DESTROY_REGISTRY_FAILED;
        }

        if (m_file.isValid())
        {
            closeKey(m_openKeyTable[ROOT]);
            m_file.close();
            m_isOpen = sal_False;

            if (m_name.getLength())
            {
                OUString systemName;
                if ( FileBase::getSystemPathFromFileURL(m_name, systemName) != FileBase::E_None )
                    systemName = m_name;

                OString name( OUStringToOString(systemName, osl_getThreadTextEncoding()) );
                if (unlink(name.getStr()) != 0)
                {
                    return REG_DESTROY_REGISTRY_FAILED;
                }
            }
        } else
        {
            return REG_REGISTRY_NOT_EXISTS;
        }
    }

    return REG_NO_ERROR;
}

//*********************************************************************
//  createKey
//
RegError ORegistry::createKey(RegKeyHandle hKey, const OUString& keyName,
                              RegKeyHandle* phNewKey)
{
    ORegKey*    pKey;

    *phNewKey = NULL;

    if ( !keyName.getLength() )
        return REG_INVALID_KEYNAME;

    REG_GUARD(m_mutex);

    if (hKey)
        pKey = (ORegKey*)hKey;
    else
        pKey = m_openKeyTable[ROOT];

    OUString sFullKeyName = pKey->getFullPath(keyName);

    if (m_openKeyTable.count(sFullKeyName) > 0)
    {
        *phNewKey = m_openKeyTable[sFullKeyName];
        ((ORegKey*)*phNewKey)->acquire();
        ((ORegKey*)*phNewKey)->setDeleted(sal_False);
        return REG_NO_ERROR;
    }

    OStoreDirectory rStoreDir;
    OUStringBuffer  sFullPath(sFullKeyName.getLength());
    OUString        token;

    sFullPath.append((sal_Unicode)'/');

    sal_Int32 nIndex = 0;
    do
    {
        token = sFullKeyName.getToken( 0, '/', nIndex );
        if (token.getLength())
        {
            if (rStoreDir.create(pKey->getStoreFile(), sFullPath.getStr(), token, KEY_MODE_CREATE))
            {
                return REG_CREATE_KEY_FAILED;
            }

            sFullPath.append(token);
            sFullPath.append((sal_Unicode)'/');
        }
    } while( nIndex != -1 );


    pKey = new ORegKey(sFullKeyName, this);
    *phNewKey = pKey;
    m_openKeyTable[sFullKeyName] = pKey;

    return REG_NO_ERROR;
}


//*********************************************************************
//  openKey
//
RegError ORegistry::openKey(RegKeyHandle hKey, const OUString& keyName,
                            RegKeyHandle* phOpenKey)
{
    ORegKey*        pKey;

    *phOpenKey = NULL;

    if ( !keyName.getLength() )
    {
        return REG_INVALID_KEYNAME;
    }

    REG_GUARD(m_mutex);

    if (hKey)
        pKey = (ORegKey*)hKey;
    else
        pKey = m_openKeyTable[ROOT];

    OUString path(pKey->getFullPath(keyName));
    KeyMap::iterator i(m_openKeyTable.find(path));
    if (i == m_openKeyTable.end()) {
        sal_Int32 n = path.lastIndexOf('/') + 1;
        switch (OStoreDirectory().create(
                    pKey->getStoreFile(), path.copy(0, n), path.copy(n),
                    isReadOnly() ? KEY_MODE_OPENREAD : KEY_MODE_OPEN))
        {
        case store_E_NotExists:
            return REG_KEY_NOT_EXISTS;
        case store_E_WrongFormat:
            return REG_INVALID_KEY;
        default:
            break;
        }
        std::auto_ptr< ORegKey > p(new ORegKey(path, this));
        i = m_openKeyTable.insert(std::make_pair(path, p.get())).first;
        p.release();
    } else {
        i->second->acquire();
    }
    *phOpenKey = i->second;
    return REG_NO_ERROR;
}


//*********************************************************************
//  closeKey
//
RegError ORegistry::closeKey(RegKeyHandle hKey)
{
    ORegKey* pKey = (ORegKey*)hKey;

    REG_GUARD(m_mutex);

    if (m_openKeyTable.count(pKey->getName()) > 0)
    {
        if (pKey->getRefCount() == 1)
        {
            m_openKeyTable.erase(pKey->getName());
            delete(pKey);
            hKey = NULL;
        } else
        {
            pKey->release();
        }

        return REG_NO_ERROR;
    } else
    {
        return REG_KEY_NOT_OPEN;
    }
}


//*********************************************************************
//  deleteKey
//
RegError ORegistry::deleteKey(RegKeyHandle hKey, const OUString& keyName)
{
    ORegKey*    pKey;
    RegError    _ret = REG_NO_ERROR;

    if ( !keyName.getLength() )
    {
        return REG_INVALID_KEYNAME;
    }

    REG_GUARD(m_mutex);

    if (hKey)
        pKey = (ORegKey*)hKey;
    else
        pKey = m_openKeyTable[ROOT];

    OUString sFullKeyName = pKey->getFullPath(keyName);

    pKey = m_openKeyTable[ROOT];
    _ret = eraseKey(pKey, sFullKeyName);

    return _ret;
}

RegError ORegistry::eraseKey(ORegKey* pKey, const OUString& keyName)
{
    RegError _ret = REG_NO_ERROR;

    if ( !keyName.getLength() )
    {
        return REG_INVALID_KEYNAME;
    }

    OUString     sFullKeyName(pKey->getName());
    OUString     sFullPath(sFullKeyName);
    OUString     sRelativKey;
    sal_Int32	 lastIndex = keyName.lastIndexOf('/');

    if ( lastIndex >= 0 )
    {
        sRelativKey += keyName.copy(lastIndex + 1);

        if (sFullKeyName.getLength() > 1)
            sFullKeyName += keyName;
        else
            sFullKeyName += (keyName+1);

        sFullPath = sFullKeyName.copy(0, keyName.lastIndexOf('/') + 1);
    } else
    {
        if (sFullKeyName.getLength() > 1)
            sFullKeyName += ROOT;

        sRelativKey += keyName;
        sFullKeyName += keyName;

        if (sFullPath.getLength() > 1)
            sFullPath += ROOT;
    }

    RegKeyHandle    hOldKey;
    _ret = pKey->openKey(keyName, &hOldKey);
    if (_ret)
    {
        return _ret;
    }

    _ret = deleteSubkeysAndValues((ORegKey*)hOldKey);
    if (_ret)
    {
        pKey->closeKey(hOldKey);
        return _ret;
    }

    OUString tmpName(sRelativKey);
    tmpName += ROOT;

    OStoreFile sFile(pKey->getStoreFile());

    if ( sFile.isValid() && sFile.remove(sFullPath, tmpName) )
    {
        return REG_DELETE_KEY_FAILED;
    }

    // set flag deleted !!!
    ((ORegKey*)hOldKey)->setDeleted(sal_True);

    _ret = pKey->closeKey(hOldKey);
    if (_ret)
    {
        return _ret;
    }

    return REG_NO_ERROR;
}


//*********************************************************************
//  deleteSubKeys
//
RegError ORegistry::deleteSubkeysAndValues(ORegKey* pKey)
{
    OStoreDirectory::iterator   iter;
    OUString               		keyName;
    RegError                    _ret = REG_NO_ERROR;
    OStoreDirectory             rStoreDir(pKey->getStoreDir());
    storeError                  _err = rStoreDir.first(iter);

    while ( _err == store_E_None )
    {
        keyName = iter.m_pszName;

        if (iter.m_nAttrib & STORE_ATTRIB_ISDIR)
        {
            _ret = eraseKey(pKey, keyName);
            if (_ret)
                return _ret;
        } else
        {
            OUString sFullPath(pKey->getName());

            if (sFullPath.getLength() > 1)
                sFullPath += ROOT;

            if ( ((OStoreFile&)pKey->getStoreFile()).remove(sFullPath, keyName) )
            {
                return REG_DELETE_VALUE_FAILED;
            }
        }

        _err = rStoreDir.next(iter);
    }

    return REG_NO_ERROR;
}


//*********************************************************************
//  loadKey
//
RegError ORegistry::loadKey(RegKeyHandle hKey, const OUString& regFileName,
                            sal_Bool bWarnings, sal_Bool bReport)
{
    RegError        _ret = REG_NO_ERROR;
    ORegistry*      pReg;
    ORegKey         *pKey, *pRootKey;

    pReg = new ORegistry();
    _ret = pReg->initRegistry(regFileName, REG_READONLY);
    if (_ret)
    {
        return _ret;
    }

    pKey = (ORegKey*)hKey;
    pRootKey = pReg->getRootKey();

    REG_GUARD(m_mutex);

    OStoreDirectory::iterator   iter;
    OUString                    keyName;
    OStoreDirectory             rStoreDir(pRootKey->getStoreDir());
    storeError                  _err = rStoreDir.first(iter);

    while ( _err == store_E_None )
    {
        keyName = iter.m_pszName;

        if ( iter.m_nAttrib & STORE_ATTRIB_ISDIR )
        {
            _ret = loadAndSaveKeys(pKey, pRootKey, keyName, 0, bWarnings, bReport);
        } else
        {
            _ret = loadAndSaveValue(pKey, pRootKey, keyName, 0, bWarnings, bReport);
        }

        if (_ret == REG_MERGE_ERROR ||
            (_ret == REG_MERGE_CONFLICT && bWarnings))
        {
            rStoreDir = OStoreDirectory();
            pRootKey->release();
            delete(pReg);
            return _ret;
        }

        _err = rStoreDir.next(iter);
    }

    rStoreDir = OStoreDirectory();
    pRootKey->release();
    delete(pReg);
    return _ret;
}


//*********************************************************************
//  loadKey
//
RegError ORegistry::saveKey(RegKeyHandle hKey, const OUString& regFileName,
                            sal_Bool bWarnings, sal_Bool bReport)
{
    RegError        _ret = REG_NO_ERROR;
    ORegistry*      pReg;
    ORegKey         *pKey, *pRootKey;

    pReg = new ORegistry();
    _ret = pReg->initRegistry(regFileName, REG_CREATE);
    if (_ret)
    {
        return _ret;
    }

    pKey = (ORegKey*)hKey;
    pRootKey = pReg->getRootKey();

    REG_GUARD(m_mutex);

    OStoreDirectory::iterator   iter;
    OUString                    keyName;
    OStoreDirectory             rStoreDir(pKey->getStoreDir());
    storeError                  _err = rStoreDir.first(iter);

    while ( _err == store_E_None )
    {
        keyName = iter.m_pszName;

        if ( iter.m_nAttrib & STORE_ATTRIB_ISDIR )
        {
            _ret = loadAndSaveKeys(pRootKey, pKey, keyName,
                                   pKey->getName().getLength(),
                                   bWarnings, bReport);
        } else
        {
            _ret = loadAndSaveValue(pRootKey, pKey, keyName,
                                    pKey->getName().getLength(),
                                    bWarnings, bReport);
        }

        if (_ret)
        {
            pRootKey->release();
            delete(pReg);
            return _ret;
        }

        _err = rStoreDir.next(iter);
    }

    pRootKey->release();
    delete(pReg);
    return REG_NO_ERROR;
}


//*********************************************************************
//  loadValue()
//
RegError ORegistry::loadAndSaveValue(ORegKey* pTargetKey,
                                     ORegKey* pSourceKey,
                                     const OUString& valueName,
                                     sal_uInt32 nCut,
                                     sal_Bool bWarnings,
                                     sal_Bool bReport)
{
    OStoreStream    rValue;
    sal_uInt8*      pBuffer;
    RegValueType    valueType;
    sal_uInt32      valueSize;
    sal_uInt32      nSize;
    storeAccessMode sourceAccess = VALUE_MODE_OPEN;
    OUString        sTargetPath(pTargetKey->getName());
    OUString        sSourcePath(pSourceKey->getName());

    if (pSourceKey->isReadOnly())
    {
        sourceAccess = VALUE_MODE_OPENREAD;
    }

    if (nCut)
    {
        sTargetPath = sSourcePath.copy(nCut);
    } else
    {
        if (sTargetPath.getLength() > 1)
        {
            if (sSourcePath.getLength() > 1)
                sTargetPath += sSourcePath;
        } else
            sTargetPath = sSourcePath;
    }

    if (sTargetPath.getLength() > 1) sTargetPath += ROOT;
    if (sSourcePath.getLength() > 1) sSourcePath += ROOT;

    if (rValue.create(pSourceKey->getStoreFile(), sSourcePath, valueName, sourceAccess))
    {
        return REG_VALUE_NOT_EXISTS;
    }

    pBuffer = (sal_uInt8*)rtl_allocateMemory(VALUE_HEADERSIZE);

    sal_uInt32  rwBytes;
    if (rValue.readAt(0, pBuffer, VALUE_HEADERSIZE, rwBytes))
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }
    if (rwBytes != VALUE_HEADERSIZE)
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }

    RegError _ret = REG_NO_ERROR;
    sal_uInt8   type = *((sal_uInt8*)pBuffer);
    valueType = (RegValueType)type;
    readUINT32(pBuffer+VALUE_TYPEOFFSET, valueSize);
    rtl_freeMemory(pBuffer);

    nSize = VALUE_HEADERSIZE + valueSize;
    pBuffer = (sal_uInt8*)rtl_allocateMemory(nSize);

    if (rValue.readAt(0, pBuffer, nSize, rwBytes))
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }
    if (rwBytes != nSize)
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }

    OStoreFile  rTargetFile(pTargetKey->getStoreFile());

    if (!rValue.create(rTargetFile, sTargetPath, valueName, VALUE_MODE_OPEN))
    {
        if (valueType == RG_VALUETYPE_BINARY)
        {
            _ret = checkBlop(
                rValue, sTargetPath, valueSize, pBuffer+VALUE_HEADEROFFSET,
                bReport);
            if (_ret)
            {
                if (_ret == REG_MERGE_ERROR ||
                    (_ret == REG_MERGE_CONFLICT && bWarnings))
                {
                    rtl_freeMemory(pBuffer);
                    return _ret;
                }
            } else
            {
                rtl_freeMemory(pBuffer);
                return _ret;
            }
        }
    }

    // write
    if (rValue.create(rTargetFile, sTargetPath, valueName, VALUE_MODE_CREATE))
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }
    if (rValue.writeAt(0, pBuffer, nSize, rwBytes))
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }

    if (rwBytes != nSize)
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }

    rtl_freeMemory(pBuffer);
    return _ret;
}


//*********************************************************************
//  checkblop()
//
RegError ORegistry::checkBlop(OStoreStream& rValue,
                              const OUString& sTargetPath,
                              sal_uInt32 srcValueSize,
                              sal_uInt8* pSrcBuffer,
                              sal_Bool bReport)
{
    RegistryTypeReader reader(pSrcBuffer, srcValueSize, sal_False);

    if (reader.getTypeClass() == RT_TYPE_INVALID)
    {
        return REG_INVALID_VALUE;
    }

    sal_uInt8*      pBuffer = (sal_uInt8*)rtl_allocateMemory(VALUE_HEADERSIZE);
    RegValueType    valueType;
    sal_uInt32      valueSize;
    sal_uInt32      rwBytes;
    OString 		targetPath( OUStringToOString(sTargetPath, RTL_TEXTENCODING_UTF8) );

    if (!rValue.readAt(0, pBuffer, VALUE_HEADERSIZE, rwBytes) &&
        (rwBytes == VALUE_HEADERSIZE))
    {
        sal_uInt8 type = *((sal_uInt8*)pBuffer);
        valueType = (RegValueType)type;
        readUINT32(pBuffer+VALUE_TYPEOFFSET, valueSize);
        rtl_freeMemory(pBuffer);

        if (valueType == RG_VALUETYPE_BINARY)
        {
            pBuffer = (sal_uInt8*)rtl_allocateMemory(valueSize);
            if (!rValue.readAt(VALUE_HEADEROFFSET, pBuffer, valueSize, rwBytes) &&
                (rwBytes == valueSize))
            {
                RegistryTypeReader reader2(pBuffer, valueSize, sal_False);

                if ((reader.getTypeClass() != reader2.getTypeClass())
                    || reader2.getTypeClass() == RT_TYPE_INVALID)
                {
                    rtl_freeMemory(pBuffer);

                    if (bReport)
                    {
                        fprintf(stdout, "ERROR: values of blop from key \"%s\" has different types.\n",
                                targetPath.getStr());
                    }
                    return REG_MERGE_ERROR;
                }

                if (reader.getTypeClass() == RT_TYPE_MODULE)
                {
                    if (reader.getFieldCount() > 0 &&
                        reader2.getFieldCount() > 0)
                    {
                        mergeModuleValue(rValue, reader, reader2);

                        rtl_freeMemory(pBuffer);
                        return REG_NO_ERROR;
                    } else
                    if (reader2.getFieldCount() > 0)
                    {
                        rtl_freeMemory(pBuffer);
                        return REG_NO_ERROR;
                    } else
                    {
                        rtl_freeMemory(pBuffer);
                        return REG_MERGE_CONFLICT;
                    }
                } else
                {
                    rtl_freeMemory(pBuffer);

                    if (bReport)
                    {
                        fprintf(stdout, "WARNING: value of key \"%s\" already exists.\n",
                                targetPath.getStr());
                    }
                    return REG_MERGE_CONFLICT;
                }
            } else
            {
                rtl_freeMemory(pBuffer);
                if (bReport)
                {
                    fprintf(stdout, "ERROR: values of key \"%s\" contains bad data.\n",
                            targetPath.getStr());
                }
                return REG_MERGE_ERROR;
            }
        } else
        {
            rtl_freeMemory(pBuffer);
            if (bReport)
            {
                fprintf(stdout, "ERROR: values of key \"%s\" has different types.\n",
                        targetPath.getStr());
            }
            return REG_MERGE_ERROR;
        }
    } else
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }
}

static sal_uInt32 checkTypeReaders(RegistryTypeReader& reader1,
                                   RegistryTypeReader& reader2,
                                   std::set< OUString >& nameSet)
{
    sal_uInt32 count=0;
    sal_uInt16 i;
    for (i=0 ; i < reader1.getFieldCount(); i++)
    {
        nameSet.insert(reader1.getFieldName(i));
        count++;
    }
    for (i=0 ; i < reader2.getFieldCount(); i++)
    {
        if (nameSet.find(reader2.getFieldName(i)) == nameSet.end())
        {
            nameSet.insert(reader2.getFieldName(i));
            count++;
        }
    }
    return count;
}

//*********************************************************************
//  mergeModuleValue()
//
RegError ORegistry::mergeModuleValue(OStoreStream& rTargetValue,
                                     RegistryTypeReader& reader,
                                     RegistryTypeReader& reader2)
{
    sal_uInt16                  index = 0;

    std::set< OUString > nameSet;
    sal_uInt32 count = checkTypeReaders(reader, reader2, nameSet);

    if (count != reader.getFieldCount())
    {
        RegistryTypeWriter writer(reader.getTypeClass(),
                                  reader.getTypeName(),
                                  reader.getSuperTypeName(),
                                  (sal_uInt16)count,
                                  0,
                                  0);

        sal_uInt16 i;
        for (i=0 ; i < reader.getFieldCount(); i++)
        {
            writer.setFieldData(index,
                               reader.getFieldName(i),
                               reader.getFieldType(i),
                               reader.getFieldDoku(i),
                               reader.getFieldFileName(i),
                               reader.getFieldAccess(i),
                               reader.getFieldConstValue(i));
            index++;
        }
        for (i=0 ; i < reader2.getFieldCount(); i++)
        {
            if (nameSet.find(reader2.getFieldName(i)) == nameSet.end())
            {
                writer.setFieldData(index,
                                   reader2.getFieldName(i),
                                   reader2.getFieldType(i),
                                   reader2.getFieldDoku(i),
                                   reader2.getFieldFileName(i),
                                   reader2.getFieldAccess(i),
                                   reader2.getFieldConstValue(i));
                index++;
            }
        }

        const sal_uInt8*    pBlop = writer.getBlop();
        sal_uInt32          aBlopSize = writer.getBlopSize();

        sal_uInt8   type = (sal_uInt8)RG_VALUETYPE_BINARY;
        sal_uInt8*  pBuffer = (sal_uInt8*)rtl_allocateMemory(VALUE_HEADERSIZE + aBlopSize);

        rtl_copyMemory(pBuffer, &type, 1);
        writeUINT32(pBuffer+VALUE_TYPEOFFSET, aBlopSize);
        rtl_copyMemory(pBuffer+VALUE_HEADEROFFSET, pBlop, aBlopSize);

        sal_uInt32  rwBytes;
        if (rTargetValue.writeAt(0, pBuffer, VALUE_HEADERSIZE+aBlopSize, rwBytes))
        {
            rtl_freeMemory(pBuffer);
            return REG_INVALID_VALUE;
        }

        if (rwBytes != VALUE_HEADERSIZE+aBlopSize)
        {
            rtl_freeMemory(pBuffer);
            return REG_INVALID_VALUE;
        }

        rtl_freeMemory(pBuffer);
    }
    return REG_NO_ERROR;
}

//*********************************************************************
//  loadKeys()
//
RegError ORegistry::loadAndSaveKeys(ORegKey* pTargetKey,
                                    ORegKey* pSourceKey,
                                    const OUString& keyName,
                                    sal_uInt32 nCut,
                                    sal_Bool bWarnings,
                                    sal_Bool bReport)
{
    ORegKey*    pTmpKey;
    RegError    _ret = REG_NO_ERROR;
    OUString    sRelPath(pSourceKey->getName().copy(nCut));
    OUString    sFullPath;

    if(pTargetKey->getName().getLength() > 1)
        sFullPath += pTargetKey->getName();
    sFullPath += sRelPath;
    if (sRelPath.getLength() > 1 || sFullPath.getLength() == 0)
        sFullPath += ROOT;

    OUString        sFullKeyName = sFullPath;
    OStoreDirectory rStoreDir;

    sFullKeyName += keyName;

    if (rStoreDir.create(pTargetKey->getStoreFile(), sFullPath, keyName, KEY_MODE_CREATE))
    {
        return REG_CREATE_KEY_FAILED;
    }

    if (m_openKeyTable.count(sFullKeyName) > 0)
    {
        m_openKeyTable[sFullKeyName]->setDeleted(sal_False);
    }

    _ret = pSourceKey->openKey(
        keyName, (RegKeyHandle*)&pTmpKey);
    if (_ret)
    {
        return _ret;
    }

    OStoreDirectory::iterator   iter;
    OUString                    sName;
    OStoreDirectory             rTmpStoreDir(pTmpKey->getStoreDir());
    storeError                  _err = rTmpStoreDir.first(iter);

    while ( _err == store_E_None)
    {
        sName = iter.m_pszName;

        if (iter.m_nAttrib & STORE_ATTRIB_ISDIR)
        {
            _ret = loadAndSaveKeys(pTargetKey, pTmpKey,
                                   sName, nCut, bWarnings, bReport);
        } else
        {
            _ret = loadAndSaveValue(pTargetKey, pTmpKey,
                                    sName, nCut, bWarnings, bReport);
        }

        if (_ret == REG_MERGE_ERROR ||
            (_ret == REG_MERGE_CONFLICT && bWarnings))
        {
            pSourceKey->closeKey(pTmpKey);
            return _ret;
        }

        _err = rTmpStoreDir.next(iter);
    }

    pSourceKey->closeKey(pTmpKey);
    return _ret;
}


//*********************************************************************
//  getRootKey()
//
ORegKey* ORegistry::getRootKey()
{
    m_openKeyTable[ROOT]->acquire();
    return m_openKeyTable[ROOT];
}


//*********************************************************************
//  dumpRegistry()
//
RegError ORegistry::dumpRegistry(RegKeyHandle hKey) const
{
    ORegKey                     *pKey = (ORegKey*)hKey;
    OUString                    sName;
    RegError                    _ret = REG_NO_ERROR;
    OStoreDirectory::iterator   iter;
    OStoreDirectory             rStoreDir(pKey->getStoreDir());
    storeError                  _err = rStoreDir.first(iter);

    OString regName( OUStringToOString( getName(), osl_getThreadTextEncoding() ) );
    OString keyName( OUStringToOString( pKey->getName(), RTL_TEXTENCODING_UTF8 ) );
    fprintf(stdout, "Registry \"%s\":\n\n%s\n", regName.getStr(), keyName.getStr());

    while ( _err == store_E_None )
    {
        sName = iter.m_pszName;

        if (iter.m_nAttrib & STORE_ATTRIB_ISDIR)
        {
            _ret = dumpKey(pKey->getName(), sName, 1);
        } else
        {
            _ret = dumpValue(pKey->getName(), sName, 1);
        }

        if (_ret)
        {
            return _ret;
        }

        _err = rStoreDir.next(iter);
    }

    return REG_NO_ERROR;
}

RegError ORegistry::flush()
{
    REG_GUARD(m_mutex);

    if (m_file.isValid())
    {
        m_file.flush();
        return REG_NO_ERROR;
    } else
    {
        return REG_REGISTRY_NOT_EXISTS;
    }
}


//*********************************************************************
//  dumpValue()
//
RegError ORegistry::dumpValue(const OUString& sPath, const OUString& sName, sal_Int16 nSpc) const
{
    OStoreStream    rValue;
    sal_uInt8*      pBuffer;
    sal_uInt32      valueSize;
    RegValueType    valueType;
    OUString        sFullPath(sPath);
    OString         sIndent;
    storeAccessMode accessMode = VALUE_MODE_OPEN;

    if (isReadOnly())
    {
        accessMode = VALUE_MODE_OPENREAD;
    }

    for (int i= 0; i < nSpc; i++) sIndent += " ";

    if (sFullPath.getLength() > 1)
    {
        sFullPath += ROOT;
    }
    if (rValue.create(m_file, sFullPath, sName, accessMode))
    {
        return REG_VALUE_NOT_EXISTS;
    }

    pBuffer = (sal_uInt8*)rtl_allocateMemory(VALUE_HEADERSIZE);

    sal_uInt32  rwBytes;
    if (rValue.readAt(0, pBuffer, VALUE_HEADERSIZE, rwBytes))
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }
    if (rwBytes != (VALUE_HEADERSIZE))
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }

    sal_uInt8 type = *((sal_uInt8*)pBuffer);
    valueType = (RegValueType)type;
    readUINT32(pBuffer+VALUE_TYPEOFFSET, valueSize);

    pBuffer = (sal_uInt8*)rtl_allocateMemory(valueSize);
    if (rValue.readAt(VALUE_HEADEROFFSET, pBuffer, valueSize, rwBytes))
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }
    if (rwBytes != valueSize)
    {
        rtl_freeMemory(pBuffer);
        return REG_INVALID_VALUE;
    }

    const sal_Char* indent = sIndent.getStr();
    switch (valueType)
    {
        case 0:
            fprintf(stdout, "%sValue: Type = VALUETYPE_NOT_DEFINED\n", indent);
            break;
        case 1:
            {
                fprintf(stdout, "%sValue: Type = RG_VALUETYPE_LONG\n", indent);
                fprintf(
                    stdout, "%s       Size = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(valueSize));
                fprintf(stdout, "%s       Data = ", indent);

                sal_Int32 value;
                readINT32(pBuffer, value);
                fprintf(stdout, "%ld\n", sal::static_int_cast< long >(value));
            }
            break;
        case 2:
            {
                sal_Char* value = (sal_Char*)rtl_allocateMemory(valueSize);
                readUtf8(pBuffer, value, valueSize);
                fprintf(stdout, "%sValue: Type = RG_VALUETYPE_STRING\n", indent);
                fprintf(
                    stdout, "%s       Size = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(valueSize));
                fprintf(stdout, "%s       Data = \"%s\"\n", indent, value);
                rtl_freeMemory(value);
            }
            break;
        case 3:
            {
                sal_uInt32 size = (valueSize / 2) * sizeof(sal_Unicode);
                fprintf(stdout, "%sValue: Type = RG_VALUETYPE_UNICODE\n", indent);
                fprintf(
                    stdout, "%s       Size = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(valueSize));
                fprintf(stdout, "%s       Data = ", indent);

                sal_Unicode* value = new sal_Unicode[size];
                readString(pBuffer, value, size);

                OString uStr = OUStringToOString(value, RTL_TEXTENCODING_UTF8);
                fprintf(stdout, "L\"%s\"\n", uStr.getStr());
                delete[] value;
            }
            break;
        case 4:
            {
                fprintf(stdout, "%sValue: Type = RG_VALUETYPE_BINARY\n", indent);
                fprintf(
                    stdout, "%s       Size = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(valueSize));
                fprintf(stdout, "%s       Data = ", indent);
                dumpType(
                    typereg::Reader(
                        pBuffer, valueSize, false, TYPEREG_VERSION_1),
                    sIndent + "              ");
            }
            break;
        case 5:
            {
                sal_uInt32 offset = 4; // initial 4 Bytes fuer die Laenge des Arrays
                sal_uInt32 len = 0;

                readUINT32(pBuffer, len);

                fprintf(stdout, "%sValue: Type = RG_VALUETYPE_LONGLIST\n", indent);
                fprintf(
                    stdout, "%s       Size = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(valueSize));
                fprintf(
                    stdout, "%s       Len  = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(len));
                fprintf(stdout, "%s       Data = ", indent);

                sal_Int32 longValue;
                for (sal_uInt32 i=0; i < len; i++)
                {
                    readINT32(pBuffer+offset, longValue);

                    if (offset > 4)
                        fprintf(stdout, "%s              ", indent);

                    fprintf(
                        stdout, "%lu = %ld\n",
                        sal::static_int_cast< unsigned long >(i),
                        sal::static_int_cast< long >(longValue));
                    offset += 4; // 4 Bytes fuer sal_Int32
                }
            }
            break;
        case 6:
            {
                sal_uInt32 offset = 4; // initial 4 Bytes fuer die Laenge des Arrays
                sal_uInt32 sLen = 0;
                sal_uInt32 len = 0;

                readUINT32(pBuffer, len);

                fprintf(stdout, "%sValue: Type = RG_VALUETYPE_STRINGLIST\n", indent);
                fprintf(
                    stdout, "%s       Size = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(valueSize));
                fprintf(
                    stdout, "%s       Len  = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(len));
                fprintf(stdout, "%s       Data = ", indent);

                sal_Char *pValue;
                for (sal_uInt32 i=0; i < len; i++)
                {
                    readUINT32(pBuffer+offset, sLen);

                    offset += 4; // 4 Bytes (sal_uInt32) fuer die Groesse des strings in Bytes

                    pValue = (sal_Char*)rtl_allocateMemory(sLen);
                    readUtf8(pBuffer+offset, pValue, sLen);

                    if (offset > 8)
                        fprintf(stdout, "%s              ", indent);

                    fprintf(
                        stdout, "%lu = \"%s\"\n",
                        sal::static_int_cast< unsigned long >(i), pValue);
                    offset += sLen;
                }
            }
            break;
        case 7:
            {
                sal_uInt32 offset = 4; // initial 4 Bytes fuer die Laenge des Arrays
                sal_uInt32 sLen = 0;
                sal_uInt32 len = 0;

                readUINT32(pBuffer, len);

                fprintf(stdout, "%sValue: Type = RG_VALUETYPE_UNICODELIST\n", indent);
                fprintf(
                    stdout, "%s       Size = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(valueSize));
                fprintf(
                    stdout, "%s       Len  = %lu\n", indent,
                    sal::static_int_cast< unsigned long >(len));
                fprintf(stdout, "%s       Data = ", indent);

                sal_Unicode *pValue;
                OString uStr;
                for (sal_uInt32 i=0; i < len; i++)
                {
                    readUINT32(pBuffer+offset, sLen);

                    offset += 4; // 4 Bytes (sal_uInt32) fuer die Groesse des strings in Bytes

                    pValue = (sal_Unicode*)rtl_allocateMemory((sLen / 2) * sizeof(sal_Unicode));
                    readString(pBuffer+offset, pValue, sLen);

                    if (offset > 8)
                        fprintf(stdout, "%s              ", indent);

                    uStr = OUStringToOString(pValue, RTL_TEXTENCODING_UTF8);
                    fprintf(
                        stdout, "%lu = L\"%s\"\n",
                        sal::static_int_cast< unsigned long >(i),
                        uStr.getStr());

                    offset += sLen;

                    rtl_freeMemory(pValue);
                }
            }
            break;
    }

    fprintf(stdout, "\n");

    rtl_freeMemory(pBuffer);
    return REG_NO_ERROR;
}

//*********************************************************************
//  dumpKey()
//
RegError ORegistry::dumpKey(const OUString& sPath, const OUString& sName, sal_Int16 nSpace) const
{
    OStoreDirectory 	rStoreDir;
    OUString          	sFullPath(sPath);
    OString             sIndent;
    storeAccessMode     accessMode = KEY_MODE_OPEN;
    RegError            _ret = REG_NO_ERROR;

    if (isReadOnly())
    {
        accessMode = KEY_MODE_OPENREAD;
    }

    for (int i= 0; i < nSpace; i++) sIndent += " ";

    if (sFullPath.getLength() > 1)
        sFullPath += ROOT;

    storeError _err = rStoreDir.create(m_file, sFullPath, sName, accessMode);

    if (_err == store_E_NotExists)
        return REG_KEY_NOT_EXISTS;
    else
    if (_err == store_E_WrongFormat)
        return REG_INVALID_KEY;

    fprintf(stdout, "%s/ %s\n", sIndent.getStr(), OUStringToOString(sName, RTL_TEXTENCODING_UTF8).getStr());

    OUString sSubPath(sFullPath);
    OUString sSubName;
    sSubPath += sName;

    OStoreDirectory::iterator   iter;

    _err = rStoreDir.first(iter);

    while ( _err == store_E_None)
    {
        sSubName = iter.m_pszName;

        if ( iter.m_nAttrib & STORE_ATTRIB_ISDIR )
        {
            _ret = dumpKey(sSubPath, sSubName, nSpace+2);
        } else
        {
            _ret = dumpValue(sSubPath, sSubName, nSpace+2);
        }

        if (_ret)
        {
            return _ret;
        }

        _err = rStoreDir.next(iter);
    }

    return REG_NO_ERROR;
}
