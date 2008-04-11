/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: ParameterizedTypeAttribute.cs,v $
 * $Revision: 1.4 $
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

using System;


namespace uno
{
/** is used to mark a UNO entity to have a parameterized type.

    <p>Currently it is only applied to members of  polymorphic structs. That is structs,
    which have a type parameter list.
    </p>

    @see TypeParametersAttribute
 */
[AttributeUsage(AttributeTargets.Field, Inherited=false)]
public sealed class ParameterizedTypeAttribute: System.Attribute
{
    /** initializes an instance with the specified value.

        @param parameter
        the name of parameter from the parameter list from
        <type scope="uno.idl">TypeParametersAttribute</type>
        It must not be null.
     */
    public ParameterizedTypeAttribute(string parameter)
    {
        m_parameter = parameter;
    }

    public string Type
    {
        get
        {
            return m_parameter;
        }
    }

    private string m_parameter;
}

} 

