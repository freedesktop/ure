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
#ifndef _CPPUHELPER_IMPLBASE_HXX_
#define _CPPUHELPER_IMPLBASE_HXX_

#include <osl/mutex.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/weakagg.hxx>
#include <rtl/instance.hxx>

#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

/* This header should not be used anymore.
   @deprecated
*/

namespace cppu
{

/** Struct used for inline template implementation helpers: type entries.
    Not for plublic use.
    @internal
*/
struct Type_Offset
{
    /** binary offset of vtable pointer from object base
    */
    sal_Int32 nOffset;
    /** interface type description of interface entry
    */
    typelib_InterfaceTypeDescription * pTD;
};
/** Struct used for inline template implementation helpers: class data of implementation.
    Not for plublic use.
    @internal
*/
struct ClassDataBase
{
    /** determines whether the class data has been statically initialized
    */
    sal_Bool  bOffsetsInit;
    /** length of static array ClassDataN
    */
    sal_Int32 nType2Offset;

    /** class code determines which standard types are supported (and returned on
        com.sun.star.lang.XTypeProvider::getTypes()) by the helper:

        - 1 -- com.sun.star.uno.XWeak
        - 2 -- com.sun.star.uno.XWeak, com.sun.star.uno.XAggregation
        - 3 -- com.sun.star.uno.XWeak, com.sun.star.uno.XAggregation, com.sun.star.lang.XComponent
        - 4 -- com.sun.star.uno.XWeak, com.sun.star.lang.XComponent
    */
    sal_Int32 nClassCode;

    /** pointer to types sequence (com.sun.star.lang.XTypeProvider)
    */
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > * pTypes;
    /** pointer to class id (com.sun.star.lang.XTypeProvider)
    */
    ::com::sun::star::uno::Sequence< sal_Int8 > * pId;

    /** def ctor
    */
    ClassDataBase() SAL_THROW( () );
    /** class code ctor

        @param nClassCode class code, see ClassDataBase::nClassCode
    */
    ClassDataBase( sal_Int32 nClassCode ) SAL_THROW( () );
    /** dtor
    */
    ~ClassDataBase() SAL_THROW( () );
};
/** Struct used for inline template implementation helpers:
    There will be versions of this struct with varying arType2Offset[] array sizes, each of which
    is binary compatible with this one to be casted and used uniform. The size of the varying array
    is set in ClassDataBase::nType2Offset (base class).
    Not for plublic use.
    @internal
*/
struct ClassData : public ClassDataBase
{
    /** type entries array
    */
    Type_Offset arType2Offset[1];
    
    /** init call for supporting com.sun.star.lang.XTypeProvider
    */
    void SAL_CALL initTypeProvider() SAL_THROW( () );
    /** initial writing type offsets for vtables

        @param rType type of interface
        @param nOffset offset to vtable entry
    */
    void SAL_CALL writeTypeOffset( const ::com::sun::star::uno::Type & rType, sal_Int32 nOffset )
        SAL_THROW( () );

    /** Queries for an interface.

        @param rType demanded interface type
        @pBase base this pointer related when writing type offsets (writeTypeOffset())
        @return demanded interface or empty any
    */
    ::com::sun::star::uno::Any SAL_CALL query(
        const ::com::sun::star::uno::Type & rType, ::com::sun::star::lang::XTypeProvider * pBase )
        SAL_THROW( () );
    /** Gets the types for supporting com.sun.star.lang.XTypeProvider

        @return sequence of types supported
    */
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes()
        SAL_THROW( () );
    /** Gets the class id of implemtation supporting com.sun.star.lang.XTypeProvider

        @return class identifier (sequence< byte >)
    */
    ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId()
        SAL_THROW( () );
};

/** Shared mutex for implementation helper initialization.
    Not for public use.
    @internal
*/
::osl::Mutex & SAL_CALL getImplHelperInitMutex(void) SAL_THROW( () );
}

//
// settle down beavis, here comes the macro template hell :]
//

//==================================================================================================

/** Implementation helper macros
    Not for common use. There are expanded forms of the macro usage in implbaseN.hxx/compbaseN.hxx.
    So there is commonly no need to use these macros. Though, you may need to implement more than
    12 interfaces. Then you have to declare something like the following in your headers
    (where N is your demanded number of interfaces):
    
    #define __IFC3 Ifc1, Ifc2, Ifc3, ... up to N
    #define __CLASS_IFC3 class Ifc1, class Ifc2, class Ifc3, ... up to N
    #define __PUBLIC_IFC3 public Ifc1, public Ifc2, public Ifc3, ... up to N
    __DEF_IMPLHELPER_PRE( N )
    __IFC_WRITEOFFSET( 1 ) __IFC_WRITEOFFSET( 2 ) __IFC_WRITEOFFSET( 3 ), ... up to N
    __DEF_IMPLHELPER_POST( N )
    
    @internal
*/
#define __DEF_IMPLHELPER_PRE( N ) \
namespace cppu \
{ \
struct ClassData##N : public ClassDataBase \
{ \
    Type_Offset arType2Offset[ N ]; \
    ClassData##N( sal_Int32 nClassCode ) SAL_THROW( () ) \
        : ClassDataBase( nClassCode ) \
        {} \
}; \
template< __CLASS_IFC##N > \
class SAL_NO_VTABLE ImplHelperBase##N \
    : public ::com::sun::star::lang::XTypeProvider \
    , __PUBLIC_IFC##N \
{ \
protected: \
    ClassData & SAL_CALL getClassData( ClassDataBase & s_aCD ) SAL_THROW( () ) \
    { \
        ClassData & rCD = * static_cast< ClassData * >( &s_aCD ); \
        if (! rCD.bOffsetsInit) \
        { \
            ::osl::MutexGuard aGuard( getImplHelperInitMutex() ); \
            if (! rCD.bOffsetsInit) \
            { \
                char * pBase = (char *)this;
/** Implementation helper macro: have a look at __DEF_IMPLHELPER_PRE
    @internal
*/
#define __IFC_WRITEOFFSET( N ) \
                rCD.writeTypeOffset( ::getCppuType( (const ::com::sun::star::uno::Reference< Ifc##N > *)0 ), \
                                     (char *)(Ifc##N *)this - pBase );
/** Implementation helper macro: have a look at __DEF_IMPLHELPER_PRE
    @internal
*/
#define __DEF_IMPLHELPER_POST_A( N ) \
                rCD.bOffsetsInit = sal_True; \
            } \
        } \
        return rCD; \
    } \
}; \
template< __CLASS_IFC##N > \
class SAL_NO_VTABLE ImplHelper##N \
    : public ImplHelperBase##N< __IFC##N > \
{ \
    static ClassData##N s_aCD; \
public: \
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException) \
        { return this->getClassData( s_aCD ).query( rType, (ImplHelperBase##N< __IFC##N > *)this ); } \
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException) \
        { return this->getClassData( s_aCD ).getTypes(); } \
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw (::com::sun::star::uno::RuntimeException) \
        { return this->getClassData( s_aCD ).getImplementationId(); } \
}; \
template< __CLASS_IFC##N > \
class SAL_NO_VTABLE WeakImplHelper##N \
    : public ::cppu::OWeakObject \
    , public ImplHelperBase##N< __IFC##N > \
{ \
    static ClassData##N s_aCD; \
public: \
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException) \
    { \
        ::com::sun::star::uno::Any aRet( this->getClassData( s_aCD ).query( rType, (ImplHelperBase##N< __IFC##N > *)this ) ); \
        return (aRet.hasValue() ? aRet : OWeakObject::queryInterface( rType )); \
    } \
    virtual void SAL_CALL acquire() throw () \
        { OWeakObject::acquire(); } \
    virtual void SAL_CALL release() throw () \
        { OWeakObject::release(); } \
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException) \
        { return this->getClassData( s_aCD ).getTypes(); } \
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw (::com::sun::star::uno::RuntimeException) \
        { return this->getClassData( s_aCD ).getImplementationId(); } \
}; \
template< __CLASS_IFC##N > \
class SAL_NO_VTABLE WeakAggImplHelper##N \
    : public ::cppu::OWeakAggObject \
    , public ImplHelperBase##N< __IFC##N > \
{ \
    static ClassData##N s_aCD; \
public: \
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException) \
        { return OWeakAggObject::queryInterface( rType ); } \
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType ) throw (::com::sun::star::uno::RuntimeException) \
    { \
        ::com::sun::star::uno::Any aRet( this->getClassData( s_aCD ).query( rType, (ImplHelperBase##N< __IFC##N > *)this ) ); \
        return (aRet.hasValue() ? aRet : OWeakAggObject::queryAggregation( rType )); \
    } \
    virtual void SAL_CALL acquire() throw () \
        { OWeakAggObject::acquire(); } \
    virtual void SAL_CALL release() throw () \
        { OWeakAggObject::release(); } \
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException) \
        { return this->getClassData( s_aCD ).getTypes(); } \
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId() throw (::com::sun::star::uno::RuntimeException) \
        { return this->getClassData( s_aCD ).getImplementationId(); } \
};

/** Implementation helper macro: have a look at __DEF_IMPLHELPER_PRE
    @internal
*/
#define __DEF_IMPLHELPER_POST_B( N ) \
template< __CLASS_IFC##N > \
ClassData##N ImplHelper##N< __IFC##N >::s_aCD = ClassData##N( 0 ); \
template< __CLASS_IFC##N > \
ClassData##N WeakImplHelper##N< __IFC##N >::s_aCD = ClassData##N( 1 ); \
template< __CLASS_IFC##N > \
ClassData##N WeakAggImplHelper##N< __IFC##N >::s_aCD = ClassData##N( 2 );
/** Implementation helper macro: have a look at __DEF_IMPLHELPER_PRE
    @internal
*/
#define __DEF_IMPLHELPER_POST_C( N ) \
}
//==================================================================================================
/** Implementation helper macro: have a look at __DEF_IMPLHELPER_PRE
    @internal
*/
#define __DEF_IMPLHELPER_POST( N ) \
__DEF_IMPLHELPER_POST_A( N ) \
__DEF_IMPLHELPER_POST_B( N ) \
__DEF_IMPLHELPER_POST_C( N )
#endif

