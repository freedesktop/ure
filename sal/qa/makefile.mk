#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: lwang $ $Date: 2002-11-06 03:09:23 $
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
TARGET=qa
TESTDIR=TRUE

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk

.IF "$(OS)" == "WNT"
REGEXP="s/^[\#].*$$//"
.ELSE
REGEXP='s/^[\#].*$$//'
.ENDIF

DEPOBJFILES = \
            $(SLO)$/rtl_String_Utils.obj \
            $(SLO)$/rtl_OString.obj \
            $(SLO)$/rtl_OUString.obj \
            $(SLO)$/rtl_OUStringBuffer.obj

SHL1OBJS = \
            $(SLO)$/rtl_String_Utils.obj \
            $(SLO)$/rtl_OString.obj

SHL1TARGET= rtl_OString
SHL1STDLIBS+=\
            $(SALLIB) \
            $(SALHELPERLIB)

SHL1VERSIONMAP = $(MISC)$/$(SHL1TARGET).map
SHL1DEPN=
SHL1IMPLIB= i$(SHL1TARGET)
SHL1DEF=    $(MISC)$/$(SHL1TARGET).def

DEF1NAME    =$(SHL1TARGET)

#-------------------------------------------------------------------------------

.IF "$(COM)"=="GCC"
NOOPTFILES= \
    $(SLO)$/rtl_String_Utils.obj \
   $(SLO)$/rtl_OUString.obj
.ENDIF

SHL2OBJS = \
            $(SLO)$/rtl_String_Utils.obj \
            $(SLO)$/rtl_OUString.obj

SHL2TARGET= rtl_OUString
SHL2STDLIBS+=\
            $(SALLIB) \
            $(SALHELPERLIB)

SHL2VERSIONMAP = $(MISC)$/$(SHL2TARGET).map
SHL2DEPN=
SHL2IMPLIB= i$(SHL2TARGET)
SHL2DEF=    $(MISC)$/$(SHL2TARGET).def

DEF2NAME    =$(SHL2TARGET)
#-------------------------------------------------------------------------------

.IF "$(COM)"=="GCC"
NOOPTFILES= \
    $(SLO)$/rtl_String_Utils.obj \
   $(SLO)$/rtl_OUStringBuffer.obj
.ENDIF

SHL3OBJS = \
            $(SLO)$/rtl_String_Utils.obj \
            $(SLO)$/rtl_OUStringBuffer.obj

SHL3TARGET= rtl_OUStringBuffer
SHL3STDLIBS+=\
            $(SALLIB) \
            $(SALHELPERLIB)

SHL3VERSIONMAP = $(MISC)$/$(SHL3TARGET).map
SHL3DEPN=
SHL3IMPLIB= i$(SHL3TARGET)
SHL3DEF=    $(MISC)$/$(SHL3TARGET).def

DEF3NAME    =$(SHL3TARGET)


# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

$(MISC)$/$(SHL1TARGET).map : sce$/$(SHL1TARGET).sce
        +$(RM)  $(MISC)$/$(SHL1TARGET).tst
        +$(RM)  $(MISC)$/$(SHL1TARGET).map
        +$(TYPE) $< | sed $(REGEXP) > $@
        +$(TYPE) $@ | sed "s/^/test_/" >> $(MISC)$/$(SHL1TARGET).tst
        +$(TYPE) mapHeader > $(MISC)$/$(SHL1TARGET).map
        +$(TYPE) $(MISC)$/$(SHL1TARGET).tst | sed "/test_./ w $(MISC)$/$(SHL1TARGET).map1"
        +$(TYPE) $(MISC)$/$(SHL1TARGET).map1 | sed "s/.*/&;/" >> $(MISC)$/$(SHL1TARGET).map
        +$(RM) $(MISC)$/$(SHL1TARGET).map1
        +$(TYPE) mapFooter >> $(MISC)$/$(SHL1TARGET).map

$(MISC)$/$(SHL2TARGET).map : sce$/$(SHL2TARGET).sce
        +$(RM)  $(MISC)$/$(SHL2TARGET).tst
        +$(RM)  $(MISC)$/$(SHL2TARGET).map
        +$(TYPE) $< | sed $(REGEXP) > $@
        +$(TYPE) $@ | sed "s/^/test_/" >> $(MISC)$/$(SHL2TARGET).tst
        +$(TYPE) mapHeader > $(MISC)$/$(SHL2TARGET).map
        +$(TYPE) $(MISC)$/$(SHL2TARGET).tst | sed "/test_./ w $(MISC)$/$(SHL2TARGET).map1"
        +$(TYPE) $(MISC)$/$(SHL2TARGET).map1 | sed "s/.*/&;/" >> $(MISC)$/$(SHL2TARGET).map
        +$(RM) $(MISC)$/$(SHL2TARGET).map1
        +$(TYPE) mapFooter >> $(MISC)$/$(SHL2TARGET).map
        
$(MISC)$/$(SHL3TARGET).map : sce$/$(SHL3TARGET).sce
        +$(RM)  $(MISC)$/$(SHL3TARGET).tst
        +$(RM)  $(MISC)$/$(SHL3TARGET).map
        +$(TYPE) $< | sed $(REGEXP) > $@
        +$(TYPE) $@ | sed "s/^/test_/" >> $(MISC)$/$(SHL3TARGET).tst
        +$(TYPE) mapHeader > $(MISC)$/$(SHL3TARGET).map
        +$(TYPE) $(MISC)$/$(SHL3TARGET).tst | sed "/test_./ w $(MISC)$/$(SHL3TARGET).map1"
        +$(TYPE) $(MISC)$/$(SHL3TARGET).map1 | sed "s/.*/&;/" >> $(MISC)$/$(SHL3TARGET).map
        +$(RM) $(MISC)$/$(SHL3TARGET).map1
        +$(TYPE) mapFooter >> $(MISC)$/$(SHL3TARGET).map

cleanup:
    +$(RM) $(MISCX)$/$(SHL1TARGET).map
    +$(RM) $(MISCX)$/$(SHL2TARGET).map
    +$(RM) $(MISCX)$/$(SHL3TARGET).map
    +$(RM) $(MISCX)$/$(SHL1TARGET).tst
    +$(RM) $(MISCX)$/$(SHL2TARGET).tst
    +$(RM) $(MISCX)$/$(SHL3TARGET).tst
    +$(RM) $(MISCX)$/$(SHL1TARGET).def
    +$(RM) $(MISCX)$/$(SHL2TARGET).def
    +$(RM) $(MISCX)$/$(SHL3TARGET).def
    +$(RM) sce$/$(SHL1TARGET).out
    +$(RM) sce$/$(SHL2TARGET).out
    +$(RM) sce$/$(SHL3TARGET).out
    +$(RM) sce$/$(SHL1TARGET).qadev
    +$(RM) sce$/$(SHL2TARGET).qadev
    +$(RM) sce$/$(SHL3TARGET).qadev

