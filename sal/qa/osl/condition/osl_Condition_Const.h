/*************************************************************************
 *
 *  $RCSfile: osl_Condition_Const.h,v $ 
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2003-09-08 13:20:42 $
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

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifndef _OSL_CONDITION_CONST_H_ 
#define _OSL_CONDITION_CONST_H_

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifndef	_SAL_TYPES_H_
#include <sal/types.h>
#endif 

#ifndef	_RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef	_OSL_THREAD_HXX_
#include <osl/thread.hxx>
#endif

#ifndef	_OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif

#ifndef	_OSL_PIPE_HXX_
#include <osl/pipe.hxx>
#endif

#ifndef	_OSL_SEMAPHOR_HXX_
#include <osl/semaphor.hxx>
#endif

#ifndef	_OSL_CONDITION_HXX_
#include <osl/conditn.hxx>
#endif

#ifndef	_OSL_TIME_H_
#include <osl/time.h>
#endif 

#ifdef UNX
#include <unistd.h>
#endif

#include <cppunit/simpleheader.hxx>

#define OSLTEST_DECLARE_USTRING( str_name, str_value ) \
    ::rtl::OUString a##str_name = rtl::OUString::createFromAscii( str_value )

//------------------------------------------------------------------------
// condition names
//------------------------------------------------------------------------
OSLTEST_DECLARE_USTRING( TestCon,  "testcondition" );

const char pTestString[17] = "Sun Microsystems";


#endif /* _OSL_CONDITION_CONST_H_ */
