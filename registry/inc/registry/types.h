/*************************************************************************
 *
 *  $RCSfile: types.h,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-03-30 16:33:56 $
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

#ifndef INCLUDED_registry_types_h
#define INCLUDED_registry_types_h

#include "sal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** specifies the typeclass of a binary type blob.

    The general structure of a binary type blob is always the same.  It depends
    on the typeclass which parts of the blob are filled with data or not.
 */
enum RTTypeClass {
    /** specifies that the structure of the given blob is unknown and can't be
        read.
     */
    RT_TYPE_INVALID,

    /** specifies that the blob represents an interface type.  An interface blob
        can contain a base interface, attributes and methods.
     */
    RT_TYPE_INTERFACE,

    /** specifies that the blob represents a module type.  A module blob can
        contain a base module and constant members (fields).
     */
    RT_TYPE_MODULE,

    /** specifies that the blob represents a struct type.  A struct blob can
        contain a base struct and members (fields).
     */
    RT_TYPE_STRUCT,

    /** specifies that the blob represents an enum type.  An enum blob can
        contain enum values which are accessible as fields.
     */
    RT_TYPE_ENUM,

    /** specifies that the blob represents an exception type.  An exception blob
        can contain a base exception and members (fields).
     */
    RT_TYPE_EXCEPTION,

    /** specifies that the blob represents a typedef type.  A typedef blob can
        contain a base type.
     */
    RT_TYPE_TYPEDEF,

    /** specifies that the blob represents a service type.  A service blob can
        contain a base service, properties (fields), references to services or
        interfaces.
     */
    RT_TYPE_SERVICE,

    /** specifies that the blob represents a singleton type (a named object)
        which refers exactly one existing service.
     */
    RT_TYPE_SINGLETON,

    /// deprecated, not used.
    RT_TYPE_OBJECT,

    /** specifies that the blob represents a constants type.  A constants blob
        can contain constant types as fields.
     */
    RT_TYPE_CONSTANTS,

    /** @deprecated
        a union type was evaluated but currently not supported.
     */
    RT_TYPE_UNION
};

/** specifies the type for the field access.

    Fields in a type blob are used for different types.  Among others they were
    used for properties of services and these poperties can have several flags.

    @see RT_ACCESS_INVALID
    @see RT_ACCESS_READONLY
    @see RT_ACCESS_OPTIONAL
    @see RT_ACCESS_MAYBEVOID
    @see RT_ACCESS_BOUND
    @see RT_ACCESS_CONSTRAINED
    @see RT_ACCESS_TRANSIENT
    @see RT_ACCESS_MAYBEAMBIGUOUS
    @see RT_ACCESS_MAYBEDEFAULT
    @see RT_ACCESS_REMOVEABLE
    @see RT_ACCESS_ATTRIBUTE
    @see RT_ACCESS_PROPERTY
    @see RT_ACCESS_CONST
    @see RT_ACCESS_READWRITE
    @see RT_ACCESS_DEFAULT
 */
typedef sal_uInt16 RTFieldAccess;

/// specifies a unknown flag
#define RT_ACCESS_INVALID 0x0000
/// specifies a readonly property/attribute
#define RT_ACCESS_READONLY 0x0001
/// specifies a property as optional that means that it must not be implemented.
#define RT_ACCESS_OPTIONAL 0x0002
/// @see com::sun::star::beans::PropertyAttribute
#define RT_ACCESS_MAYBEVOID 0x0004
/// @see com::sun::star::beans::PropertyAttribute
#define RT_ACCESS_BOUND 0x0008
/// @see com::sun::star::beans::PropertyAttribute
#define RT_ACCESS_CONSTRAINED 0x0010
/// @see com::sun::star::beans::PropertyAttribute
#define RT_ACCESS_TRANSIENT 0x0020
/// @see com::sun::star::beans::PropertyAttribute
#define RT_ACCESS_MAYBEAMBIGUOUS 0x0040
/// @see com::sun::star::beans::PropertyAttribute
#define RT_ACCESS_MAYBEDEFAULT 0x0080
/// @see com::sun::star::beans::PropertyAttribute
#define RT_ACCESS_REMOVEABLE 0x0100
/// @see com::sun::star::beans::PropertyAttribute
#define RT_ACCESS_ATTRIBUTE 0x0200
/// specifies that the field is a property
#define RT_ACCESS_PROPERTY 0x0400
/// specifies that the field is a constant or enum value
#define RT_ACCESS_CONST 0x0800
/// specifies that the property/attribute has read/write access
#define RT_ACCESS_READWRITE 0x1000
/// only to describe a union default label
#define RT_ACCESS_DEFAULT 0x2000

/** specifies the type of a field value.

    A field can have a value if it repsresents a constant or an enum value.
 */
enum RTValueType {
    RT_TYPE_NONE,
    RT_TYPE_BOOL,
    RT_TYPE_BYTE,
    RT_TYPE_INT16,
    RT_TYPE_UINT16,
    RT_TYPE_INT32,
    RT_TYPE_UINT32,
    RT_TYPE_INT64,
    RT_TYPE_UINT64,
    RT_TYPE_FLOAT,
    RT_TYPE_DOUBLE,
    RT_TYPE_STRING
};

/** specifies a variable container for field values.
 */
union RTConstValueUnion {
    sal_Bool aBool;
    sal_uInt8 aByte;
    sal_Int16 aShort;
    sal_uInt16 aUShort;
    sal_Int32 aLong;
    sal_uInt32 aULong;
    sal_Int64 aHyper;
    sal_uInt64 aUHyper;
    float aFloat;
    double aDouble;
    sal_Unicode const * aString;
};

/** specifies the mode of a method.

    A method can be synchron or asynchron (oneway).  The const attribute for
    methods was removed so that the const values are deprecated.
 */
enum RTMethodMode {
    /// indicates an invalid mode
    RT_MODE_INVALID,

    /// indicates the asynchronous mode of a method
    RT_MODE_ONEWAY,

    /// @deprecated
    RT_MODE_ONEWAY_CONST,

    /// indicated the synchronous mode of a method
    RT_MODE_TWOWAY,

    /// @deprecated
    RT_MODE_TWOWAY_CONST,

    /**
       Indicates an extended attribute getter (that has a 'raises' clause) of an
       interface type.

       @since #i21150#
     */
    RT_MODE_ATTRIBUTE_GET,

    /**
       Indicates an extended attribute setter (that has a 'raises' clause) of an
       interface type.

       @since #i21150#
     */
    RT_MODE_ATTRIBUTE_SET
};

/** specifies the mode of a parameter.

    There are three paramter modes which have impact of the handling of the
    paramter in the UNO bridges and the UNO code generation.
 */
enum RTParamMode {
    /// indicates an invalid parameter mode
    RT_PARAM_INVALID = 0,

    /// indicates a pure in parameter which is used by value
    RT_PARAM_IN = 1,

    /// indicates a pure out parameter which is used by reference
    RT_PARAM_OUT = 2,

    /// indicates a in and out parameter which is used also by reference
    RT_PARAM_INOUT = 3,

    /**
       Indicates a rest parameter (currently only valid for service
       constructors).

       This value can be combined with any of RT_PARAM_IN, RT_PARAM_OUT, and
       RT_PARAM_INOUT (however, service constructors currently only allow
       RT_PARAM_IN, anyway).

       @since #i21150#
     */
    RT_PARAM_REST = 4
};

/** specifies the type of a reference used in a service description.
 */
enum RTReferenceType {
    /// the reference type is unknown
    RT_REF_INVALID,

    /** the service support the interface that means a implementation of this
        service must implement this interface.
     */
    RT_REF_SUPPORTS,

    /** @deprecated
        the service observes the interface.
     */
    RT_REF_OBSERVES,

    /** the service exports the specified service that means this service
        provides also the specified service.
     */
    RT_REF_EXPORTS,

    /** @deprecated
        the service needs the specified service that means in the context of
        this service the specified service will be used or must be available.
     */
    RT_REF_NEEDS
};

#ifdef __cplusplus
}
#endif

#endif
