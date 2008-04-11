/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: starter.cxx,v $
 * $Revision: 1.3 $
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
#include "precompiled_bridges.hxx"

#include <stdio.h>
#include "share.h"


using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

static void some_more(  t_throws_exc p )
{
    (*p)();
}

extern "C" void SAL_CALL start( t_throws_exc p )
{
    try
    {
        some_more( p );
    }
    catch (lang::IllegalArgumentException & exc)
    {
        OString msg( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
        printf( "starter.cxx: caught IllegalArgumentException: %s\n", msg.getStr() );
    }
    catch (Exception & exc)
    {
        OString msg( OUStringToOString( exc.Message, RTL_TEXTENCODING_ASCII_US ) );
        printf( "starter.cxx: caught some UNO exc: %s\n", msg.getStr() );
    }
    catch (...)
    {
        printf( "starter.cxx: caught something\n" );
    }
}
