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
#include "precompiled_io.hxx"

#include "connector.hxx"
#include <rtl/ustrbuf.hxx>
#include <algorithm>

using namespace ::osl;
using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::connection;


namespace stoc_connector {
    template<class T>
    void notifyListeners(SocketConnection * pCon, sal_Bool * notified, T t)
    {
          XStreamListener_hash_set listeners;

        {
            ::osl::MutexGuard guard(pCon->_mutex);
            if(!*notified) 
            {
                *notified = sal_True;
                listeners = pCon->_listeners;
            }
        }

        ::std::for_each(listeners.begin(), listeners.end(), t);
    }


    static void callStarted(Reference<XStreamListener> xStreamListener)
    {
        xStreamListener->started();
    }

    struct callError {
        const Any & any;

        callError(const Any & any);

        void operator () (Reference<XStreamListener> xStreamListener);
    };

    callError::callError(const Any & aAny)
        : any(aAny)
    {
    }

    void callError::operator () (Reference<XStreamListener> xStreamListener)
    {
        xStreamListener->error(any);
    }

    static void callClosed(Reference<XStreamListener> xStreamListener)
    {
        xStreamListener->closed();
    }

    
    SocketConnection::SocketConnection( const OUString &sConnectionDescription ) :
        m_nStatus( 0 ),
        m_sDescription( sConnectionDescription ),
        _started(sal_False),
        _closed(sal_False),
        _error(sal_False)
    {
        // make it unique
        g_moduleCount.modCnt.acquire( &g_moduleCount.modCnt );
        m_sDescription += OUString( RTL_CONSTASCII_USTRINGPARAM( ",uniqueValue=" ) );
        m_sDescription += OUString::valueOf(
            sal::static_int_cast< sal_Int64 >(
                reinterpret_cast< sal_IntPtr >(&m_socket)),
            10 );
    }

    SocketConnection::~SocketConnection()
    {
        g_moduleCount.modCnt.release( &g_moduleCount.modCnt );
    }
    
    void SocketConnection::completeConnectionString()
    {
        sal_Int32 nPort;

        nPort = m_socket.getPeerPort();

        OUStringBuffer buf( 256 );
        buf.appendAscii( ",peerPort=" );
        buf.append( (sal_Int32) nPort );
        buf.appendAscii( ",peerHost=" );
        buf.append( m_socket.getPeerHost() );

        buf.appendAscii( ",localPort=" );
        buf.append( (sal_Int32) nPort );
        buf.appendAscii( ",localHost=" );
        buf.append( m_socket.getLocalHost( ) );

        m_sDescription += buf.makeStringAndClear();
    }
    
    sal_Int32 SocketConnection::read( Sequence < sal_Int8 > & aReadBytes , sal_Int32 nBytesToRead )
            throw(::com::sun::star::io::IOException,
                  ::com::sun::star::uno::RuntimeException)
    {
        if( ! m_nStatus )
        {
            notifyListeners(this, &_started, callStarted);

            if( aReadBytes.getLength() != nBytesToRead )
            {
                aReadBytes.realloc( nBytesToRead );
            }
            sal_Int32 i = m_socket.read( aReadBytes.getArray()  , aReadBytes.getLength() );

            if(i != nBytesToRead && m_socket.getError() != osl_Socket_E_None)
            {
                OUString message(RTL_CONSTASCII_USTRINGPARAM("ctr_socket.cxx:SocketConnection::read: error - "));
                message += m_socket.getErrorAsString();
                
                IOException ioException(message, Reference<XInterface>(static_cast<XConnection *>(this)));
                
                Any any;
                any <<= ioException;
                
                notifyListeners(this, &_error, callError(any));
                
                throw ioException;
            }

            return i;
        }
        else 
        {
            OUString message(RTL_CONSTASCII_USTRINGPARAM("ctr_socket.cxx:SocketConnection::read: error - connection already closed"));

            IOException ioException(message, Reference<XInterface>(static_cast<XConnection *>(this)));

            Any any;
            any <<= ioException;

            notifyListeners(this, &_error, callError(any));

            throw ioException;
        }
    }

    void SocketConnection::write( const Sequence < sal_Int8 > &seq )
            throw(::com::sun::star::io::IOException,
                  ::com::sun::star::uno::RuntimeException)
    {
        if( ! m_nStatus )
        {
            if( m_socket.write( seq.getConstArray() , seq.getLength() ) != seq.getLength() )
            {
                OUString message(RTL_CONSTASCII_USTRINGPARAM("ctr_socket.cxx:SocketConnection::write: error - "));
                message += m_socket.getErrorAsString();
                
                IOException ioException(message, Reference<XInterface>(static_cast<XConnection *>(this)));
                
                Any any;
                any <<= ioException;
                
                notifyListeners(this, &_error, callError(any));

                throw ioException;
            }
        }
        else 
        {
            OUString message(RTL_CONSTASCII_USTRINGPARAM("ctr_socket.cxx:SocketConnection::write: error - connection already closed"));

            IOException ioException(message, Reference<XInterface>(static_cast<XConnection *>(this)));

            Any any;
            any <<= ioException;

            notifyListeners(this, &_error, callError(any));

            throw ioException;
        }
    }

    void SocketConnection::flush( )
            throw(::com::sun::star::io::IOException,
                  ::com::sun::star::uno::RuntimeException)
    {

    }

    void SocketConnection::close()
            throw(::com::sun::star::io::IOException,
                  ::com::sun::star::uno::RuntimeException)
    {
            // ensure that close is called only once
        if( 1 == osl_incrementInterlockedCount( (&m_nStatus) ) )
        {
            m_socket.shutdown();
            notifyListeners(this, &_closed, callClosed);
        }
    }

    OUString SocketConnection::getDescription()
            throw( ::com::sun::star::uno::RuntimeException)
    {
        return m_sDescription;
    }



    // XConnectionBroadcaster
    void SAL_CALL SocketConnection::addStreamListener(const Reference<XStreamListener> & aListener) throw(RuntimeException)
    {
        MutexGuard guard(_mutex);

        _listeners.insert(aListener);
    }

    void SAL_CALL SocketConnection::removeStreamListener(const Reference<XStreamListener> & aListener) throw(RuntimeException)
    {
        MutexGuard guard(_mutex);

        _listeners.erase(aListener);
    }
}

