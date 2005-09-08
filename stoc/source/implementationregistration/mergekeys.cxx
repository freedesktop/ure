/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mergekeys.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 07:55:25 $
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

#include <vector>

#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/registry/MergeConflictException.hpp>

#define OUSTR(x) ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(x) )

using namespace ::rtl;
using namespace ::osl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;

namespace stoc_impreg
{

struct Link
{
    OUString m_name;
    OUString m_target;
    
    inline Link( OUString const & name, OUString const & target ) SAL_THROW( () )
        : m_name( name )
        , m_target( target )
        {}
};
typedef ::std::vector< Link > t_links;

//==================================================================================================
static void mergeKeys(
    Reference< registry::XRegistryKey > const & xDest,
    Reference< registry::XRegistryKey > const & xSource,
    t_links & links )
    SAL_THROW( (registry::InvalidRegistryException, registry::MergeConflictException) )
{
    if (!xSource.is() || !xSource->isValid()) {
        throw registry::InvalidRegistryException(
            OUSTR("source key is null or invalid!"),
            Reference<XInterface>() );
    }
    if (!xDest.is() || !xDest->isValid()) {
        throw registry::InvalidRegistryException(
            OUSTR("destination key is null or invalid!"),
            Reference<XInterface>() );
    }
    
    // write value
    switch (xSource->getValueType())
    {
    case registry::RegistryValueType_NOT_DEFINED:
        break;
    case registry::RegistryValueType_LONG:
        xDest->setLongValue( xSource->getLongValue() );
        break;
    case registry::RegistryValueType_ASCII:
        xDest->setAsciiValue( xSource->getAsciiValue() );
        break;
    case registry::RegistryValueType_STRING:
        xDest->setStringValue( xSource->getStringValue() );
        break;
    case registry::RegistryValueType_BINARY:
        xDest->setBinaryValue( xSource->getBinaryValue() );
        break;
    case registry::RegistryValueType_LONGLIST:
        xDest->setLongListValue( xSource->getLongListValue() );
        break;
    case registry::RegistryValueType_ASCIILIST:
        xDest->setAsciiListValue( xSource->getAsciiListValue() );
        break;
    case registry::RegistryValueType_STRINGLIST:
        xDest->setStringListValue( xSource->getStringListValue() );
        break;
    }
    
    // sub keys
    Sequence< OUString > sourceKeys( xSource->getKeyNames() );
    OUString const * pSourceKeys = sourceKeys.getConstArray();
    for ( sal_Int32 nPos = sourceKeys.getLength(); nPos--; )
    {
        // key name
        OUString name( pSourceKeys[ nPos ] );
        sal_Int32 nSlash = name.lastIndexOf( '/' );
        if (nSlash >= 0)
        {
            name = name.copy( nSlash +1 );
        }
        
        if (xSource->getKeyType( name ) == registry::RegistryKeyType_KEY)
        {
            // try to open exisiting dest key or create new one
            Reference< registry::XRegistryKey > xDestKey( xDest->createKey( name ) );
            Reference< registry::XRegistryKey > xSourceKey( xSource->openKey( name ) );
            mergeKeys( xDestKey, xSourceKey, links );
            xSourceKey->closeKey();
            xDestKey->closeKey();
        }
        else // link
        {
            // remove existing key
            Reference< registry::XRegistryKey > xDestKey( xDest->openKey( name ) );
            if (xDestKey.is() && xDestKey->isValid()) // something to remove
            {
                xDestKey->closeKey();
                if (xDest->getKeyType( name ) == registry::RegistryKeyType_LINK)
                {
                    xDest->deleteLink( name );
                }
                else
                {
                    xDest->deleteKey( name );
                }
            }
            
            links.push_back( Link(
                pSourceKeys[ nPos ], // abs path
                xSource->getResolvedName( name ) // abs resolved name
                ) );
        }
    }
}

//==================================================================================================
void SAL_CALL mergeKeys(
    Reference< registry::XRegistryKey > const & xDest,
    Reference< registry::XRegistryKey > const & xSource )
    SAL_THROW( (registry::InvalidRegistryException, registry::MergeConflictException) )
{
    if (!xDest.is() || !xDest->isValid()) {
        throw registry::InvalidRegistryException(
            OUSTR("destination key is null or invalid!"),
            Reference<XInterface>() );
    }
    if (xDest->isReadOnly())
    {
        throw registry::InvalidRegistryException(
            OUString( RTL_CONSTASCII_USTRINGPARAM(
                          "destination registry is read-only!  cannot merge!") ),
            Reference< XInterface >() );
    }
    
    t_links links;
    links.reserve( 16 );
    mergeKeys( xDest, xSource, links );
    
    for ( size_t nPos = links.size(); nPos--; )
    {
        Link const & r = links[ nPos ];
        OSL_VERIFY( xDest->createLink( r.m_name, r.m_target ) );
    }
}

}
