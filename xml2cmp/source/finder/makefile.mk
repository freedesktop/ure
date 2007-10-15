#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.10 $
#
#   last change: $Author: vg $ $Date: 2007-10-15 13:08:09 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

PRJ=..$/..

PRJNAME=xml2cmp
TARGET=srvdepy
TARGETTYPE=CUI

# Windows /DELAYLOAD depends on objects delivered from sal, but is not needed
# for this internal tool, anyway:
DELAYLOADOBJ=

# --- Settings -----------------------------------------------------

ENABLE_EXCEPTIONS=TRUE

.INCLUDE :  settings.mk



# --- Files --------------------------------------------------------

OBJFILES= \
    $(OBJ)$/dependy.obj	\
    $(OBJ)$/dep_main.obj


# --- Targets ------------------------------------------------------

APP1TARGET=	$(TARGET)
APP1STACK=	1000000

LIBSALCPPRT=$(0)

.IF "$(GUI)"=="WNT"
UWINAPILIB=$(0)
.ENDIF


APP1LIBS=	$(LB)$/$(TARGET).lib $(LB)$/x2c_xcdl.lib $(LB)$/x2c_support.lib
APP1DEPN=	$(LB)$/$(TARGET).lib $(LB)$/x2c_xcdl.lib $(LB)$/x2c_support.lib


.INCLUDE :  target.mk


