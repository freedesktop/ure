/*************************************************************************
 *
 *  $RCSfile: Type.java,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: kz $ $Date: 2004-02-26 13:40:24 $
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

package com.sun.star.uno;

import java.util.HashMap;

/**
 * Represents the UNO built-in type <code>TYPE</code>.
 *
 * <p>The UNO type is not directly mapped to <code>java.lang.Class</code> for at
 * least two reasons.  For one, some UNO types (like <code>UNSIGNED
 * SHORT</code>) do not have a matching Java class.  For another, it can be
 * necessary to describe a type which is unknown to the Java runtime system
 * (for example, for delaying the need of a class, so that it is possible to
 * generate it on the fly.)</p>
 *
 * <p>A <code>Type</code> is uniquely determined by its type class (a
 * <code>TypeClass</code>) and its type name (a <code>String</code>); these two
 * will never be <code>null</code>.  A <code>Type</code> may have an additional
 * "z class" (a <code>java.lang.Class</code>), giving a Java class type that
 * corresponds to the UNO type.  Also, a <code>Type</code> can cache a type
 * description (a <code>com.sun.star.uno.ITypeDescription</code>), which can be
 * computed and set by <code>TypeDescription.getTypeDescription</code>.
 *
 * @since UDK1.0
 */
public class Type {
    // The following private static members and static initializer must come
    // first in the class definition, so that the class can be initialized
    // sucessfully:

    private static final String TYPE_NAME_VOID = "void";
    private static final String TYPE_NAME_BOOLEAN = "boolean";
    private static final String TYPE_NAME_BYTE = "byte";
    private static final String TYPE_NAME_SHORT = "short";
    private static final String TYPE_NAME_UNSIGNED_SHORT = "unsigned short";
    private static final String TYPE_NAME_LONG = "long";
    private static final String TYPE_NAME_UNSIGNED_LONG = "unsigned long";
    private static final String TYPE_NAME_HYPER = "hyper";
    private static final String TYPE_NAME_UNSIGNED_HYPER = "unsigned hyper";
    private static final String TYPE_NAME_FLOAT = "float";
    private static final String TYPE_NAME_DOUBLE = "double";
    private static final String TYPE_NAME_CHAR = "char";
    private static final String TYPE_NAME_STRING = "string";
    private static final String TYPE_NAME_TYPE = "type";
    private static final String TYPE_NAME_ANY = "any";

    // must be sorted same as TypeClass:
    private static final String[] __typeClassToTypeName = new String[] {
        TYPE_NAME_VOID,
        TYPE_NAME_CHAR,
        TYPE_NAME_BOOLEAN,
        TYPE_NAME_BYTE,
        TYPE_NAME_SHORT,
        TYPE_NAME_UNSIGNED_SHORT,
        TYPE_NAME_LONG,
        TYPE_NAME_UNSIGNED_LONG,
        TYPE_NAME_HYPER,
        TYPE_NAME_UNSIGNED_HYPER,
        TYPE_NAME_FLOAT,
        TYPE_NAME_DOUBLE,
        TYPE_NAME_STRING,
        TYPE_NAME_TYPE,
        TYPE_NAME_ANY
    };

    private static final HashMap __javaClassToTypeClass = new HashMap();
    static {
        __javaClassToTypeClass.put(
            void.class, new TypeClass[] { TypeClass.VOID, TypeClass.VOID });
        __javaClassToTypeClass.put(
            Void.class, new TypeClass[] { TypeClass.VOID, TypeClass.VOID });
        __javaClassToTypeClass.put(
            boolean.class,
            new TypeClass[] { TypeClass.BOOLEAN, TypeClass.BOOLEAN });
        __javaClassToTypeClass.put(
            Boolean.class,
            new TypeClass[] { TypeClass.BOOLEAN, TypeClass.BOOLEAN });
        __javaClassToTypeClass.put(
            byte.class, new TypeClass[] { TypeClass.BYTE, TypeClass.BYTE });
        __javaClassToTypeClass.put(
            Byte.class, new TypeClass[] { TypeClass.BYTE, TypeClass.BYTE });
        __javaClassToTypeClass.put(
            short.class,
            new TypeClass[] { TypeClass.SHORT, TypeClass.UNSIGNED_SHORT });
        __javaClassToTypeClass.put(
            Short.class,
            new TypeClass[] { TypeClass.SHORT, TypeClass.UNSIGNED_SHORT });
        __javaClassToTypeClass.put(
            int.class,
            new TypeClass[] { TypeClass.LONG, TypeClass.UNSIGNED_LONG });
        __javaClassToTypeClass.put(
            Integer.class,
            new TypeClass[] { TypeClass.LONG, TypeClass.UNSIGNED_LONG });
        __javaClassToTypeClass.put(
            long.class,
            new TypeClass[] { TypeClass.HYPER, TypeClass.UNSIGNED_HYPER });
        __javaClassToTypeClass.put(
            Long.class,
            new TypeClass[] { TypeClass.HYPER, TypeClass.UNSIGNED_HYPER });
        __javaClassToTypeClass.put(
            float.class, new TypeClass[] { TypeClass.FLOAT, TypeClass.FLOAT });
        __javaClassToTypeClass.put(
            Float.class, new TypeClass[] { TypeClass.FLOAT, TypeClass.FLOAT });
        __javaClassToTypeClass.put(
            double.class,
            new TypeClass[] { TypeClass.DOUBLE, TypeClass.DOUBLE });
        __javaClassToTypeClass.put(
            Double.class,
            new TypeClass[] { TypeClass.DOUBLE, TypeClass.DOUBLE });
        __javaClassToTypeClass.put(
            char.class, new TypeClass[] { TypeClass.CHAR, TypeClass.CHAR });
        __javaClassToTypeClass.put(
            Character.class,
            new TypeClass[] { TypeClass.CHAR, TypeClass.CHAR });
        __javaClassToTypeClass.put(
            String.class,
            new TypeClass[] { TypeClass.STRING, TypeClass.STRING });
        __javaClassToTypeClass.put(
            Type.class, new TypeClass[] { TypeClass.TYPE, TypeClass.TYPE });
        __javaClassToTypeClass.put(
            Any.class, new TypeClass[] { TypeClass.ANY, TypeClass.ANY });
        __javaClassToTypeClass.put(
            Object.class,
            new TypeClass[] { TypeClass.ANY, TypeClass.INTERFACE });
    }

    public static final Type VOID = new Type(void.class);
    public static final Type CHAR = new Type(char.class);
    public static final Type BOOLEAN = new Type(boolean.class);
    public static final Type BYTE = new Type(byte.class);
    public static final Type SHORT = new Type(short.class);
    public static final Type UNSIGNED_SHORT = new Type(
        TYPE_NAME_UNSIGNED_SHORT, TypeClass.UNSIGNED_SHORT);
    public static final Type LONG = new Type(int.class);
    public static final Type UNSIGNED_LONG = new Type(
        TYPE_NAME_UNSIGNED_LONG, TypeClass.UNSIGNED_LONG);
    public static final Type HYPER = new Type(long.class);
    public static final Type UNSIGNED_HYPER = new Type(
        TYPE_NAME_UNSIGNED_HYPER, TypeClass.UNSIGNED_HYPER);
    public static final Type FLOAT = new Type(float.class);
    public static final Type DOUBLE = new Type(double.class);
    public static final Type STRING = new Type(String.class);
    public static final Type TYPE = new Type(Type.class);
    public static final Type ANY = new Type(Any.class);

    /**
     * Constructs a new <code>Type</code> which defaults to <code>VOID</code>.
     *
     * @since UDK3.0
     */
    public Type() {
        this(void.class);
    }

    /**
     * Constructs a new <code>Type</code> with the given type class and type
     * name.
     *
     * @param typeName the type name.  Must not be <code>null</code>.
     * @param typeClass the type class.  Must not be <code>null</code>, and must
     *     match the <code>typeName</code> (for example, it is illegal to
     *     combine a <code>typeName</code> of <code>"void"</code> with a
     *     <code>typeClass</code> of <code>BOOLEAN</code>).
     */
    public Type(String typeName, TypeClass typeClass) {
        _typeClass = typeClass;
        _typeName = typeName;
    }

    /**
     * Constructs a new <code>Type</code> from the given
     * <code>java.lang.Class</code>.
     *
     * <p>This is equivalent to <code>Type(zClass, false)</code>.</p>
     *
     * @param zClass the Java class of this type.  Must not be
     *     <code>null</code>.
     *
     * @since UDK3.0
     */
    public Type(Class zClass) {
        this(zClass, false);
    }

    /**
     * Constructs a new <code>Type</code> from the given
     * <code>java.lang.Class</code>, handling ambiguous cases.
     *
     * <p>In certain cases, one Java class corresponds to two UNO types (e.g.,
     * the Java class <code>short[].class</code> corresponds to both a sequence
     * of <codde>SHORT</code> and a sequence of <code>UNSIGNED SHORT</code> in
     * UNO).  In such ambiguous cases, the parameter <code>alternative</code>
     * controls which UNO type is chosen:</p>
     * <ul>
     *   <li>If the Java type is (an array type with element type)
     *   <code>short</code> or <code>java.lang.Short</code>:  If
     *   <code>alternative</code> is <code>false</code>, the chosen UNO type is
     *   (a sequence type with element type) <code>SHORT</code>.  If
     *   <code>alternative</code> is <code>true</code>, the chosen UNO type is
     *   (a sequence type with element type) <code>UNSIGNED SHORT</code>.</li>
     *
     *   <li>If the Java type is (an array type with element type)
     *   <code>int</code> or <code>java.lang.Integer</code>:  If
     *   <code>alternative</code> is <code>false</code>, the chosen UNO type is
     *   (a sequence type with element type) <code>LONG</code>.  If
     *   <code>alternative</code> is <code>true</code>, the chosen UNO type is
     *   (a sequence type with element type) <code>UNSIGNED LONG</code>.</li>
     *
     *   <li>If the Java type is (an array type with element type)
     *   <code>long</code> or <code>java.lang.Long</code>:  If
     *   <code>alternative</code> is <code>false</code>, the chosen UNO type is
     *   (a sequence type with element type) <code>HYPER</code>.  If
     *   <code>alternative</code> is <code>true</code>, the chosen UNO type is
     *   (a sequence type with element type) <code>UNSIGNED HYPER</code>.</li>
     *
     *   <li>If the Java type is (an array type with element type)
     *   <code>java.lang.Object</code>:  If <code>alternative</code> is
     *   <code>false</code>, the chosen UNO type is (a sequence type with
     *   element type) <code>ANY</code>.  If <code>alternative</code> is
     *   <code>true</code>, the chosen UNO type is (a sequence type with element
     *   type) <code>com.sun.star.uno.XInterface</code>.</li>
     * </ul>
     * <p>In all other cases, the value of <code>alternative</code> is
     * ignored.</p>
     *
     * @param zClass the Java class of this type; must not be <code>null</code>
     * @param alternative controls which UNO type to choose in case of
     *     ambiguities
     *
     * @since UDK 3.2
     */
    public Type(Class zClass, boolean alternative) {
        TypeClass[] tc = (TypeClass[]) __javaClassToTypeClass.get(zClass);
        if (tc != null) {
            // tc only contains primitive type classes, except for
            // TypeClass.INTERFACE, which stands for XInterface (the alternative
            // interpretation of java.lang.Object):
            _typeClass = tc[alternative ? 1 : 0];
            _typeName = _typeClass == TypeClass.INTERFACE
                ? XInterface.class.getName()
                : __typeClassToTypeName[_typeClass.getValue()];
            // do not assign _class from zClass, as _class should always be
            // normalized (e.g., boolean.class instead of
            // java.lang.Boolean.class); getZClass will later calculate the
            // correct class when needed
        } else if (zClass.isArray()) {
            Type t = new Type(zClass.getComponentType(), alternative);
            _typeClass = t.getTypeClass() != TypeClass.UNKNOWN
                ? TypeClass.SEQUENCE : TypeClass.UNKNOWN;
            _typeName = "[]" + t.getTypeName();
            // do not assign _class from zClass, as _class should always be
            // normalized (e.g., boolean[].class instead of
            // java.lang.Boolean[].class); getZClass will later calculate the
            // correct class when needed
        } else if (Enum.class.isAssignableFrom(zClass)) {
            _typeClass = zClass != Enum.class
                ? TypeClass.ENUM : TypeClass.UNKNOWN;
            _typeName = zClass.getName();
            _class = zClass;
        } else if (Throwable.class.isAssignableFrom(zClass)) {
            _typeClass
                = com.sun.star.uno.Exception.class.isAssignableFrom(zClass)
                || com.sun.star.uno.RuntimeException.class.isAssignableFrom(
                    zClass)
                ? TypeClass.EXCEPTION : TypeClass.UNKNOWN;
            _typeName = zClass.getName();
            _class = zClass;
        } else if (zClass.isInterface()) {
            _typeClass = XInterface.class.isAssignableFrom(zClass)
                ? TypeClass.INTERFACE : TypeClass.UNKNOWN;
            _typeName = zClass.getName();
            _class = zClass;
        } else if (XInterface.class.isAssignableFrom(zClass)) {
            // This case is needed by code that uses this constructor to
            // calculate the UNO type corresponding to a Java object:
            _typeClass = TypeClass.INTERFACE;
            _typeName = XInterface.class.getName();
            _class = XInterface.class;
        } else {
            // assert zClass != Object.class && !zClass.isPrimitive();
            _typeClass = TypeClass.STRUCT;
            _typeName = zClass.getName();
            _class = zClass;
        }
    }

    /**
     * Constructs a new <code>Type</code> from the given type description.
     *
     * @param typeDescription a type description.  Must not be
     *     <code>null</code>.
     *
     * @since UDK3.0
     */
    public Type(ITypeDescription typeDescription) {
        _typeName         = typeDescription.getTypeName();
        _typeClass        = typeDescription.getTypeClass();
        _iTypeDescription = typeDescription;
    }

    /**
     * Constructs a new <code>Type</code> with the given type name.
     *
     * @param typeName the name of this type; must not be <code>null</code>.
     *     For simple types (<code>VOID</code>, <code>BOOLEAN</code>,
     *     <code>CHAR</code>, <code>BYTE</code>, <code>SHORT</code>,
     *     <code>UNSIGNED SHORT</code>, <code>LONG</code>, <code>UNSIGNED
     *     LONG</code>, <code>HYPER</code>, <code>UNSIGNED HYPER</code>,
     *     <code>FLOAT</code>, <code>DOUBLE</code>, <code>STRING</code>,
     *     <code>TYPE</code>, <code>ANY</code>), the type class is calculated;
     *     for other types, the type class is set to <code>UNKNOWN</code>.
     *
     * @since UDK3.0
     *
     * @deprecated This constructor is deprecated as of UDK3.2.  Using it is
     * dangerous, as it can create a <code>Type</code> with an
     * <code>UNKNOWN</code> type class.  One of the other constructors like
     * <code>Type(Class)</code> or <code>Type(String, TypeClass)</code> should
     * be used instead.
     */
    public Type(String typeName) {
        TypeClass tc = TypeClass.UNKNOWN;
        for (int i = 0; i < __typeClassToTypeName.length; ++i) {
            if (__typeClassToTypeName[i].equals(typeName)) {
                tc = TypeClass.fromInt(i);
                break;
            }
        }
        _typeClass = tc;
        _typeName = typeName;
    }

    /**
     * Constructs a new <code>Type</code> with the given type class.
     *
     * @param typeClass the type class of this type; must not be
     *     <code>null</code>.  Only type classes for simple types are allowed
     *     here.
     *
     * @throws IllegalArgumentException if the given <code>typeClass</code> is
     *     not simple (for example, a struct or an interface type).  This
     *     constructor could not find out the type name in such a case.
     *
     * @since UDK3.0
     */
    public Type(TypeClass typeClass) {
        if(__isTypeClassPrimitive(typeClass)) {
            _typeClass = typeClass;
            _typeName = __typeClassToTypeName[typeClass.getValue()];
        }
        else
            throw new IllegalArgumentException(typeClass + " is not primitive");
    }

    /**
     * Gets the type class.
     *
     * @return the type class.  Will never be <code>null</code>, but might be
     *     <code>UNKNOWN</code>.
     *
     * @since UDK1.0
     */
    public TypeClass getTypeClass() {
        return _typeClass;
    }

    /**
     * Gets the type name.
     *
     * @return the type name; will never be <code>null</code>
     *
     * @since UDK1.0
     */
    public String getTypeName() {
        return _typeName;
    }

    /**
     * Gets the Java class.
     *
     * @return the type name; may be <code>null</code> in extreme situations
     *     (inconsistent <code>TypeClass</code>, error loading a class)
     *
     * @since UDK1.0
     */
    public Class getZClass() {
        synchronized (this) {
            if (_class == null) {
                _class = determineClass();
            }
        }
        return _class;
    }

    /**
     * Gives the type description of this type.
     *
     * @return the type description; may be <code>null</code>
     *
     * @since UDK3.0
     */
    public ITypeDescription getTypeDescription() {
        return _iTypeDescription;
    }

    /**
     * Sets the type description for this type.
     *
     * @param typeDescription the type description
     *
     * @since UDK3.0
     */
    public void setTypeDescription(ITypeDescription typeDescription) {
        _iTypeDescription = typeDescription;
    }

    /**
     * Determines whether this UNO type is a supertype of another UNO type.
     *
     * UNO only defines the following supertype relations:
     * (1)  A struct type t1 is a supertype of a struct type t2, if either t1
     * and t2 are the same, or t1 is a direct or indirect parent of t2.
     * (2)  An exception type t1 is a supertype of an exception type t2, if
     * either t1 and t2 are the same, or t1 is a direct or indirect parent
     * of t2.
     * (3)  An interface type t1 is a supertype of an interface type t2, if
     * either t1 and t2 are the same, or t1 is a direct or indirect parent
     * of t2.
     *
     * Following the conventions of the Java UNO language binding,
     * com.sun.star.uno.Exception is not considered a supertype of
     * com.sun.star.uno.RuntimeException or any exception type derived from
     * com.sun.star.uno.RuntimeException.
     *
     * @param type some Type
     * @return true if this type is a supertype of the given type
     *
     * @since OOo 2.0
     */
    public boolean isSupertypeOf(Type type) {
        switch (_typeClass.getValue()) {
        case TypeClass.STRUCT_value:
        case TypeClass.EXCEPTION_value:
        case TypeClass.INTERFACE_value:
            if (type._typeClass.equals(_typeClass)) {
                Class c1 = getZClass();
                Class c2 = type.getZClass();
                return c1 != null && c2 != null && c1.isAssignableFrom(c2);
            } else {
                return false;
            }

        default:
            return this.equals(type);
        }
    }

    // @see java.lang.Object#equals
    public boolean equals(Object obj) {
        return obj instanceof Type
            && _typeClass == ((Type) obj)._typeClass
            && _typeName.equals(((Type) obj)._typeName);
    }

    // @see java.lang.Object#hashCode
    public int hashCode() {
        return _typeName.hashCode();
    }

    // @see java.lang.Object#toString
    public String toString() {
        return "Type[" + _typeName + "]";
    }

    private Class determineClass() {
        switch (_typeClass.getValue()) {
        case TypeClass.VOID_value:
            return _typeName.equals(TYPE_NAME_VOID) ? void.class : null;

        case TypeClass.BOOLEAN_value:
            return _typeName.equals(TYPE_NAME_BOOLEAN) ? boolean.class : null;

        case TypeClass.BYTE_value:
            return _typeName.equals(TYPE_NAME_BYTE) ? byte.class : null;

        case TypeClass.SHORT_value:
            return _typeName.equals(TYPE_NAME_SHORT) ? short.class : null;

        case TypeClass.UNSIGNED_SHORT_value:
            return _typeName.equals(TYPE_NAME_UNSIGNED_SHORT)
                ? short.class : null;

        case TypeClass.LONG_value:
            return _typeName.equals(TYPE_NAME_LONG) ? int.class : null;

        case TypeClass.UNSIGNED_LONG_value:
            return _typeName.equals(TYPE_NAME_UNSIGNED_LONG) ? int.class : null;

        case TypeClass.HYPER_value:
            return _typeName.equals(TYPE_NAME_HYPER) ? long.class : null;

        case TypeClass.UNSIGNED_HYPER_value:
            return _typeName.equals(TYPE_NAME_UNSIGNED_HYPER)
                ? long.class : null;

        case TypeClass.FLOAT_value:
            return _typeName.equals(TYPE_NAME_FLOAT) ? float.class : null;

        case TypeClass.DOUBLE_value:
            return _typeName.equals(TYPE_NAME_DOUBLE) ? double.class : null;

        case TypeClass.CHAR_value:
            return _typeName.equals(TYPE_NAME_CHAR) ? char.class : null;

        case TypeClass.STRING_value:
            return _typeName.equals(TYPE_NAME_STRING) ? String.class : null;

        case TypeClass.TYPE_value:
            return _typeName.equals(TYPE_NAME_TYPE) ? Type.class : null;

        case TypeClass.ANY_value:
            return _typeName.equals(TYPE_NAME_ANY) ? Object.class : null;

        case TypeClass.SEQUENCE_value:
            StringBuffer buf = new StringBuffer();
            int offset = 0;
            for (; _typeName.startsWith("[]", offset); offset += "[]".length())
            {
                buf.append('[');
            }
            String base = _typeName.substring(offset);
            if (base.equals(TYPE_NAME_VOID)) {
                buf.append('V');
            } else if (base.equals(TYPE_NAME_BOOLEAN)) {
                buf.append('Z');
            } else if (base.equals(TYPE_NAME_BYTE)) {
                buf.append('B');
            } else if (base.equals(TYPE_NAME_SHORT)
                       || base.equals(TYPE_NAME_UNSIGNED_SHORT)) {
                buf.append('S');
            } else if (base.equals(TYPE_NAME_LONG)
                       || base.equals(TYPE_NAME_UNSIGNED_LONG)) {
                buf.append('I');
            } else if (base.equals(TYPE_NAME_HYPER)
                       || base.equals(TYPE_NAME_UNSIGNED_HYPER)) {
                buf.append('J');
            } else if (base.equals(TYPE_NAME_FLOAT)) {
                buf.append('F');
            } else if (base.equals(TYPE_NAME_DOUBLE)) {
                buf.append('D');
            } else if (base.equals(TYPE_NAME_CHAR)) {
                buf.append('C');
            } else if (base.equals(TYPE_NAME_STRING)) {
                buf.append("Ljava.lang.String;");
            } else if (base.equals(TYPE_NAME_TYPE)) {
                buf.append("Lcom.sun.star.uno.Type;");
            } else if (base.equals(TYPE_NAME_ANY)) {
                buf.append("Ljava.lang.Object;");
            } else {
                Class c = null;
                try {
                    c = Class.forName(base);
                } catch (ClassNotFoundException e) {}
                if (c == null
                    || new Type(c).getTypeClass() == TypeClass.UNKNOWN)
                {
                    return null;
                }
                buf.append('L');
                buf.append(base);
                buf.append(';');
            }
            try {
                return Class.forName(buf.toString());
            } catch (ClassNotFoundException e) {
                return null;
            }

        case TypeClass.ENUM_value:
        case TypeClass.STRUCT_value:
        case TypeClass.EXCEPTION_value:
        case TypeClass.INTERFACE_value:
            Class c;
            try {
                    c = Class.forName(_typeName);
            } catch (ClassNotFoundException e) {
                return null;
            }
            return new Type(c).equals(this) ? c : null;

        default:
            return null;
        }
    }

    private static boolean __isTypeClassPrimitive(TypeClass typeClass) {
        return typeClass.getValue() < __typeClassToTypeName.length;
    }

    protected TypeClass _typeClass; // TODO should be final
    protected String _typeName; // TODO should be final

    protected Class _class;
    protected ITypeDescription _iTypeDescription;
}
