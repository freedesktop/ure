/*************************************************************************
 *
 *  $RCSfile: compbase7.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: dbo $ $Date: 2001-03-15 15:47:29 $
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
#ifndef _CPPUHELPER_COMPBASE7_HXX_
#define _CPPUHELPER_COMPBASE7_HXX_

#ifndef _CPPUHELPER_IMPLBASE7_HXX_
#include <cppuhelper/implbase7.hxx>
#endif
#ifndef _CPPUHELPER_COMPBASE_HXX_
#include <cppuhelper/compbase.hxx>
#endif

/*
__DEF_COMPIMPLHELPER( 7 )
*/

namespace cppu
{
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4, class Ifc5, class Ifc6, class Ifc7 >
    class WeakComponentImplHelper7
        : public ::cppu::OWeakObject
        , public ::com::sun::star::lang::XComponent
        , public ImplHelperBase7< Ifc1, Ifc2, Ifc3, Ifc4, Ifc5, Ifc6, Ifc7 >
    {
        static ClassData7 s_aCD;
    protected:
        ::cppu::OBroadcastHelper rBHelper;
        virtual void SAL_CALL disposing()
            {}
    public:
        WeakComponentImplHelper7( ::osl::Mutex & rMutex ) SAL_THROW( () )
            : rBHelper( rMutex )
            {}
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            {
                ::com::sun::star::uno::Any aRet( getClassData( s_aCD ).query( rType, (ImplHelperBase7< Ifc1, Ifc2, Ifc3, Ifc4, Ifc5, Ifc6, Ifc7 > *)this ) );
                if (aRet.hasValue())
                    return aRet;
                if (rType == ::getCppuType( (const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > *)0 ))
                {
                    void * p = static_cast< ::com::sun::star::lang::XComponent * >( this );
                    return ::com::sun::star::uno::Any( &p, rType );
                }
                return OWeakObject::queryInterface( rType );
            }
        virtual void SAL_CALL acquire() throw ()
            { OWeakObject::acquire(); }
        virtual void SAL_CALL release() throw ()
            {
                if (1 == m_refCount && !rBHelper.bDisposed)
                {
                    dispose();
                }
                OWeakObject::release();
            }
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getTypes(); }
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getImplementationId(); }
        virtual void SAL_CALL dispose() throw (::com::sun::star::uno::RuntimeException)
            {
                ::osl::ClearableMutexGuard aGuard( rBHelper.rMutex );
                if (!rBHelper.bDisposed && !rBHelper.bInDispose)
                {
                    rBHelper.bInDispose = sal_True;
                    aGuard.clear();
                    ::com::sun::star::lang::EventObject aEvt( static_cast< ::cppu::OWeakObject * >( this ) );
                    rBHelper.aLC.disposeAndClear( aEvt );
                    disposing();
                    rBHelper.bDisposed = sal_True;
                    rBHelper.bInDispose = sal_False;
                }
            }
        virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & xListener ) throw (::com::sun::star::uno::RuntimeException)
            {
                rBHelper.addListener( ::getCppuType( &xListener ), xListener );
            }
        virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & xListener ) throw (::com::sun::star::uno::RuntimeException)
            {
                rBHelper.removeListener( ::getCppuType( &xListener ), xListener );
            }
    };
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4, class Ifc5, class Ifc6, class Ifc7 >
    class WeakAggComponentImplHelper7
        : public ::cppu::OWeakAggObject
        , public ::com::sun::star::lang::XComponent
        , public ImplHelperBase7< Ifc1, Ifc2, Ifc3, Ifc4, Ifc5, Ifc6, Ifc7 >
    {
        static ClassData7 s_aCD;
    protected:
        ::cppu::OBroadcastHelper rBHelper;
        virtual void SAL_CALL disposing()
            {}
    public:
        WeakAggComponentImplHelper7( ::osl::Mutex & rMutex ) SAL_THROW( () )
            : rBHelper( rMutex )
            {}
        virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            { return OWeakAggObject::queryInterface( rType ); }
        virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException)
            {
                ::com::sun::star::uno::Any aRet( getClassData( s_aCD ).query( rType, (ImplHelperBase7< Ifc1, Ifc2, Ifc3, Ifc4, Ifc5, Ifc6, Ifc7 > *)this ) );
                if (aRet.hasValue())
                    return aRet;
                if (rType == ::getCppuType( (const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > *)0 ))
                {
                    void * p = static_cast< ::com::sun::star::lang::XComponent * >( this );
                    return ::com::sun::star::uno::Any( &p, rType );
                }
                return OWeakAggObject::queryAggregation( rType );
            }
        virtual void SAL_CALL acquire() throw ()
            { OWeakAggObject::acquire(); }
        virtual void SAL_CALL release() throw ()
            {
                ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > x( xDelegator );
                if (!x.is() && 1 == m_refCount && !rBHelper.bDisposed)
                    dispose();
                OWeakAggObject::release();
            }
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getTypes(); }
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw (::com::sun::star::uno::RuntimeException)
            { return getClassData( s_aCD ).getImplementationId(); }
        virtual void SAL_CALL dispose() throw(::com::sun::star::uno::RuntimeException)
            {
                ::osl::ClearableMutexGuard aGuard( rBHelper.rMutex );
                if (!rBHelper.bDisposed && !rBHelper.bInDispose)
                {
                    rBHelper.bInDispose = sal_True;
                    aGuard.clear();
                    ::com::sun::star::lang::EventObject aEvt( static_cast< ::cppu::OWeakObject * >( this ) );
                    rBHelper.aLC.disposeAndClear( aEvt );
                    disposing();
                    rBHelper.bDisposed = sal_True;
                    rBHelper.bInDispose = sal_False;
                }
            }
        virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & xListener ) throw (::com::sun::star::uno::RuntimeException)
            {
                rBHelper.addListener( ::getCppuType( &xListener ), xListener );
            }
        virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & xListener ) throw (::com::sun::star::uno::RuntimeException)
            {
                rBHelper.removeListener( ::getCppuType( &xListener ), xListener );
            }
    };

#ifndef MACOSX
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4, class Ifc5, class Ifc6, class Ifc7 >
    ClassData7 WeakComponentImplHelper7< Ifc1, Ifc2, Ifc3, Ifc4, Ifc5, Ifc6, Ifc7 >::s_aCD = ClassData7( 4 );
    template< class Ifc1, class Ifc2, class Ifc3, class Ifc4, class Ifc5, class Ifc6, class Ifc7 >
    ClassData7 WeakAggComponentImplHelper7< Ifc1, Ifc2, Ifc3, Ifc4, Ifc5, Ifc6, Ifc7 >::s_aCD = ClassData7( 3 );
#endif

}

#endif
