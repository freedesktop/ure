#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.2 $
#
#   last change: $Author: vg $ $Date: 2003-07-09 09:20:50 $
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
#*************************************************************************

PRJ := ..$/..$/..
PRJNAME := bridges

TARGET := test_javauno_acquire
PACKAGE := test$/java_uno$/acquire

ENABLE_EXCEPTIONS := TRUE

.INCLUDE: settings.mk

DLLPRE = # no leading "lib" on .so files
INCPRE += $(MISC)$/$(TARGET)$/inc

SLOFILES = $(SLO)$/testacquire.obj

SHL1TARGET = testacquire.uno
SHL1OBJS = $(SLOFILES)
SHL1STDLIBS =
SHL1VERSIONMAP = testacquire.map
SHL1IMPLIB = itestacquire

JAVAFILES = TestAcquire.java
JARFILES = juh.jar jurt.jar ridl.jar

.INCLUDE: target.mk

ALLTAR: \
    $(BIN)$/testacquire-client$(SCRIPTEXT) \
    $(BIN)$/testacquire-server$(SCRIPTEXT)

.IF "$(GUI)" == "WNT"
GIVE_EXEC_RIGHTS = @echo
.ELSE # GUI, WNT
GIVE_EXEC_RIGHTS = chmod +x
.ENDIF # GUI, WNT

EXEC_CLASSPATH_TMP = $(foreach,i,$(JARFILES) $(SOLARBINDIR)$/$i)
EXEC_CLASSPATH = \
    $(strip $(subst,!,$(PATH_SEPERATOR) $(EXEC_CLASSPATH_TMP:s/ /!/)))

$(MISC)$/$(TARGET).rdb: types.idl
    - rm $@
    - $(MKDIR) $(MISC)$/$(TARGET)
    - $(MKDIR) $(MISC)$/$(TARGET)$/inc
    idlc -I$(SOLARIDLDIR) -O$(MISC)$/$(TARGET) $<
    regmerge $(MISC)$/$(TARGET).rdb /UCR $(MISC)$/$(TARGET)$/types.urd
    cppumaker -BUCR -C -O$(MISC)$/$(TARGET)$/inc $@ -X$(SOLARBINDIR)$/types.rdb
    javamaker -BUCR -nD -O$(CLASSDIR) $@ -X$(SOLARBINDIR)$/types.rdb

$(SLOFILES) $(JAVACLASSFILES): $(MISC)$/$(TARGET).rdb

TEST_JAVAUNO_ACQUIRE_UNO_URL := \
    \"'uno:socket,host=localhost,port=2002;urp;test'\"

$(BIN)$/testacquire-client$(SCRIPTEXT):
    echo java -classpath \
        ..$/class$/test$(PATH_SEPERATOR)..$/class$(PATH_SEPERATOR)\
..$/class$/java_uno.jar$(PATH_SEPERATOR)$(EXEC_CLASSPATH)$(PATH_SEPERATOR)\
$(SOLARBINDIR)$/sandbox.jar \
        test.java_uno.acquire.TestAcquire $(TEST_JAVAUNO_ACQUIRE_UNO_URL) > $@
    $(GIVE_EXEC_RIGHTS) $@

$(BIN)$/testacquire-server$(SCRIPTEXT):
    echo uno -c com.sun.star.test.bridges.testacquire.impl -l $(SHL1TARGETN:f) \
        -ro $(SOLARBINDIR)$/types.rdb -ro ..$/misc$/$(TARGET).rdb \
        -u $(TEST_JAVAUNO_ACQUIRE_UNO_URL) > $@
    $(GIVE_EXEC_RIGHTS) $@
