#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.1 $
#
#   last change: $Author: sz $ $Date: 2002-01-18 09:57:03 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************
PRJ=..

PRJNAME=sal
TARGET=tests

# --- Settings -----------------------------------------------------
.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk

.IF "$(OS)" == "WNT"
REGEXP="s/^[\#].*$$//"
.ELSE
REGEXP='s/^[\#].*$$//'
.ENDIF

DEPOBJFILES = \
            $(SLO)$/rtl_String_Utils.obj \
            $(SLO)$/rtl_OString.obj \
            $(SLO)$/rtl_OUString.obj

SHL1OBJS = \
            $(SLO)$/rtl_String_Utils.obj \
            $(SLO)$/rtl_OString.obj

SHL1TARGET= rtl_OString
SHL1STDLIBS+=\
            $(SALLIB) \
            $(SALHELPERLIB)

SHL1DEPN=
SHL1IMPLIB= i$(SHL1TARGET)
SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)
DEF1EXPORTFILE= $(MISC)$/$(SHL1TARGET).exp



SHL2OBJS = \
            $(SLO)$/rtl_String_Utils.obj \
            $(SLO)$/rtl_OUString.obj

SHL2TARGET= rtl_OUString
SHL2STDLIBS+=\
            $(SALLIB) \
            $(SALHELPERLIB)

SHL2DEPN=
SHL2IMPLIB= i$(SHL2TARGET)
SHL2DEF=    $(MISC)$/$(SHL2TARGET).def

DEF2NAME    =$(SHL2TARGET)
DEF2EXPORTFILE= $(MISC)$/$(SHL2TARGET).exp

#SHL2OBJS = \
#            $(SLO)$/rtl_String_Utils.obj \
#            $(SLO)$/rtl_OUString.obj \
#            $(SLO)$/osl_apitest_conditn.obj \
#            $(SLO)$/osl_apitest_diagnose.obj \
#            $(SLO)$/osl_apitest_file.obj \
#            $(SLO)$/osl_apitest_interlck.obj \
#            $(SLO)$/osl_apitest_mutex.obj \
#            $(SLO)$/osl_apitest_nlsupport.obj \
#            $(SLO)$/osl_apitest_pipe.obj \
#            $(SLO)$/osl_apitest_profile.obj \
#            $(SLO)$/osl_apitest_time.obj \
#            $(SLO)$/osl_apitest_util.obj


#SHL1TARGET= rtl_apitest
#SHL1STDLIBS+=\
#$(SALLIB)
#SHL1DEPN=
#SHL1IMPLIB= i$(SHL1TARGET)
#SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

#DEF1NAME    =$(SHL1TARGET)
#DEF1EXPORTFILE= $(MISC)$/$(SHL1TARGET).exp

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

#$(MISC)$/$(SHL1TARGET).exp : sce$/$(SHL1TARGET).sce
#    +$(TYPE) $< | sed $(REGEXP) | sed "s/^/test_/" | sed "/test_./ w $@"
#$(MISC)$/$(SHL2TARGET).exp : sce$/$(SHL2TARGET).sce
#    +$(TYPE) $< | sed $(REGEXP) | sed "s/^/test_/" | sed "/test_./ w $@"

$(MISC)$/$(SHL1TARGET).exp : sce$/$(SHL1TARGET).sce
    +$(TYPE) $< | sed $(REGEXP) > $@
    +$(TYPE) $@ | sed "s/^/test_/" > $(MISC)$/$(SHL1TARGET).tst
    +$(TYPE) $(MISC)$/$(SHL1TARGET).tst | sed "/test_./ w $@"

$(MISC)$/$(SHL2TARGET).exp : sce$/$(SHL2TARGET).sce
    +$(TYPE) $< | sed $(REGEXP) > $@
    +$(TYPE) $@ | sed "s/^/test_/" > $(MISC)$/$(SHL2TARGET).tst
    +$(TYPE) $(MISC)$/$(SHL2TARGET).tst | sed "/test_./ w $@"

cleanup:
    +$(RM) sce$/$(SHL1TARGET).exp
    +$(RM) sce$/$(SHL2TARGET).exp
    +$(RM) sce$/$(SHL1TARGET).tst


