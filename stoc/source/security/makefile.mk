#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************
PRJ=..$/..

PRJNAME=stoc
TARGET = security
ENABLE_EXCEPTIONS=TRUE
BOOTSTRAP_SERVICE=TRUE
UNOUCROUT=$(OUT)$/inc$/bootstrap

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# ------------------------------------------------------------------

SLOFILES= \
        $(SLO)$/permissions.obj			\
        $(SLO)$/access_controller.obj		\
        $(SLO)$/file_policy.obj

.IF "$(debug)" != ""

# some diagnose
.IF "$(diag)" == "full"
CFLAGS += -D__DIAGNOSE -D__CACHE_DIAGNOSE
.ELIF "$(diag)" == "cache"
CFLAGS += -D__CACHE_DIAGNOSE
.ELIF "$(diag)" != ""
CFLAGS += -D__DIAGNOSE
.ENDIF

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

