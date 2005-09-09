#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.9 $
#
#   last change: $Author: rt $ $Date: 2005-09-09 15:20:15 $
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

PRJ=..$/..$/..$/..

PRJNAME=api

TARGET=csscontainer
PACKAGE=com$/sun$/star$/container

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    XIdentifierAccess.idl\
    XIdentifierReplace.idl\
    XIdentifierContainer.idl\
    ContainerEvent.idl\
    ElementExistException.idl\
    NoSuchElementException.idl\
    XChild.idl\
    XUniqueIDAccess.idl\
    XComponentEnumeration.idl\
    XComponentEnumerationAccess.idl\
    XContainer.idl\
    XContainerListener.idl\
    XContainerQuery.idl\
    XContentEnumerationAccess.idl\
    XElementAccess.idl\
    XEnumeration.idl\
    XEnumerationAccess.idl\
    XHierarchicalName.idl\
    XHierarchicalNameAccess.idl\
    XHierarchicalNameReplace.idl\
    XHierarchicalNameContainer.idl\
    XImplicitIDAccess.idl\
    XImplicitIDContainer.idl\
    XImplicitIDReplace.idl\
    XIndexAccess.idl\
    XIndexContainer.idl\
    XIndexReplace.idl\
    XNameAccess.idl\
    XNameContainer.idl\
    XNamed.idl\
    XNameReplace.idl\
    XSet.idl\

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
