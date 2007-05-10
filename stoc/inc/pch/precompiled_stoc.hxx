/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: precompiled_stoc.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 15:14:53 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): Generated on 2006-09-01 17:50:03.092339

// breaks some builds (e.g. pyuno and i18npool on Mac OS X)
#if 0
#ifdef PRECOMPILED_HEADERS
//---MARKER---
#include "sal/alloca.h"
#include "sal/config.h"
#include "sal/main.h"
#include "sal/types.h"

#include "boost/scoped_array.hpp"

#include "com/sun/star/beans/IllegalTypeException.hpp"
#include "com/sun/star/beans/MethodConcept.hpp"
#include "com/sun/star/beans/NamedValue.hpp"
#include "com/sun/star/beans/Property.hpp"
#include "com/sun/star/beans/PropertyAttribute.hpp"
#include "com/sun/star/beans/PropertyChangeEvent.hpp"
#include "com/sun/star/beans/PropertyConcept.hpp"
#include "com/sun/star/beans/PropertyState.hpp"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/beans/PropertyVetoException.hpp"
#include "com/sun/star/beans/UnknownPropertyException.hpp"
#include "com/sun/star/beans/XExactName.hpp"
#include "com/sun/star/beans/XFastPropertySet.hpp"
#include "com/sun/star/beans/XIntrospection.hpp"
#include "com/sun/star/beans/XIntrospectionAccess.hpp"
#include "com/sun/star/beans/XMaterialHolder.hpp"
#include "com/sun/star/beans/XPropertyChangeListener.hpp"
#include "com/sun/star/beans/XPropertySet.hpp"
#include "com/sun/star/beans/XPropertySetInfo.hpp"
#include "com/sun/star/beans/XVetoableChangeListener.hpp"
#include "com/sun/star/connection/SocketPermission.hpp"
#include "com/sun/star/container/ContainerEvent.hpp"
#include "com/sun/star/container/ElementExistException.hpp"
#include "com/sun/star/container/NoSuchElementException.hpp"
#include "com/sun/star/container/XContainer.hpp"
#include "com/sun/star/container/XContainerListener.hpp"
#include "com/sun/star/container/XContentEnumerationAccess.hpp"
#include "com/sun/star/container/XElementAccess.hpp"
#include "com/sun/star/container/XEnumeration.hpp"
#include "com/sun/star/container/XEnumerationAccess.hpp"
#include "com/sun/star/container/XHierarchicalNameAccess.hpp"
#include "com/sun/star/container/XIndexAccess.hpp"
#include "com/sun/star/container/XIndexContainer.hpp"
#include "com/sun/star/container/XIndexReplace.hpp"
#include "com/sun/star/container/XNameAccess.hpp"
#include "com/sun/star/container/XNameContainer.hpp"
#include "com/sun/star/container/XNameReplace.hpp"
#include "com/sun/star/container/XSet.hpp"
#include "com/sun/star/io/FilePermission.hpp"
#include "com/sun/star/java/InvalidJavaSettingsException.hpp"
#include "com/sun/star/java/JavaDisabledException.hpp"
#include "com/sun/star/java/JavaInitializationException.hpp"
#include "com/sun/star/java/JavaNotConfiguredException.hpp"
#include "com/sun/star/java/JavaNotFoundException.hpp"
#include "com/sun/star/java/JavaVMCreationFailureException.hpp"
#include "com/sun/star/java/MissingJavaRuntimeException.hpp"
#include "com/sun/star/java/RestartRequiredException.hpp"
#include "com/sun/star/java/XJavaThreadRegister_11.hpp"
#include "com/sun/star/java/XJavaVM.hpp"
#include "com/sun/star/lang/ArrayIndexOutOfBoundsException.hpp"
#include "com/sun/star/lang/DisposedException.hpp"
#include "com/sun/star/lang/EventObject.hpp"
#include "com/sun/star/lang/IllegalAccessException.hpp"
#include "com/sun/star/lang/IllegalArgumentException.hpp"
#include "com/sun/star/lang/IndexOutOfBoundsException.hpp"
#include "com/sun/star/lang/NoSuchMethodException.hpp"
#include "com/sun/star/lang/WrappedTargetException.hpp"
#include "com/sun/star/lang/WrappedTargetRuntimeException.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/lang/XEventListener.hpp"
#include "com/sun/star/lang/XInitialization.hpp"
#include "com/sun/star/lang/XMain.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/lang/XSingleComponentFactory.hpp"
#include "com/sun/star/lang/XSingleServiceFactory.hpp"
#include "com/sun/star/lang/XTypeProvider.hpp"
#include "com/sun/star/lang/XUnoTunnel.hpp"
#include "com/sun/star/loader/CannotActivateFactoryException.hpp"
#include "com/sun/star/loader/XImplementationLoader.hpp"
#include "com/sun/star/reflection/FieldAccessMode.hpp"
#include "com/sun/star/reflection/InvalidTypeNameException.hpp"
#include "com/sun/star/reflection/InvocationTargetException.hpp"
#include "com/sun/star/reflection/MethodMode.hpp"
#include "com/sun/star/reflection/NoSuchTypeNameException.hpp"
#include "com/sun/star/reflection/ParamInfo.hpp"
#include "com/sun/star/reflection/ParamMode.hpp"
#include "com/sun/star/reflection/TypeDescriptionSearchDepth.hpp"
#include "com/sun/star/reflection/XArrayTypeDescription.hpp"
#include "com/sun/star/reflection/XCompoundTypeDescription.hpp"
#include "com/sun/star/reflection/XConstantTypeDescription.hpp"
#include "com/sun/star/reflection/XConstantsTypeDescription.hpp"
#include "com/sun/star/reflection/XEnumTypeDescription.hpp"
#include "com/sun/star/reflection/XIdlArray.hpp"
#include "com/sun/star/reflection/XIdlClass.hpp"
#include "com/sun/star/reflection/XIdlClassProvider.hpp"
#include "com/sun/star/reflection/XIdlField.hpp"
#include "com/sun/star/reflection/XIdlField2.hpp"
#include "com/sun/star/reflection/XIdlMember.hpp"
#include "com/sun/star/reflection/XIdlMethod.hpp"
#include "com/sun/star/reflection/XIdlReflection.hpp"
#include "com/sun/star/reflection/XIndirectTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceAttributeTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceAttributeTypeDescription2.hpp"
#include "com/sun/star/reflection/XInterfaceMemberTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceMethodTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceTypeDescription.hpp"
#include "com/sun/star/reflection/XInterfaceTypeDescription2.hpp"
#include "com/sun/star/reflection/XMethodParameter.hpp"
#include "com/sun/star/reflection/XModuleTypeDescription.hpp"
#include "com/sun/star/reflection/XParameter.hpp"
#include "com/sun/star/reflection/XPropertyTypeDescription.hpp"
#include "com/sun/star/reflection/XProxyFactory.hpp"
#include "com/sun/star/reflection/XPublished.hpp"
#include "com/sun/star/reflection/XServiceConstructorDescription.hpp"
#include "com/sun/star/reflection/XServiceTypeDescription.hpp"
#include "com/sun/star/reflection/XServiceTypeDescription2.hpp"
#include "com/sun/star/reflection/XSingletonTypeDescription.hpp"
#include "com/sun/star/reflection/XSingletonTypeDescription2.hpp"
#include "com/sun/star/reflection/XStructTypeDescription.hpp"
#include "com/sun/star/reflection/XTypeDescription.hpp"
#include "com/sun/star/reflection/XTypeDescriptionEnumeration.hpp"
#include "com/sun/star/reflection/XTypeDescriptionEnumerationAccess.hpp"
#include "com/sun/star/registry/CannotRegisterImplementationException.hpp"
#include "com/sun/star/registry/InvalidRegistryException.hpp"
#include "com/sun/star/registry/InvalidValueException.hpp"
#include "com/sun/star/registry/MergeConflictException.hpp"
#include "com/sun/star/registry/RegistryKeyType.hpp"
#include "com/sun/star/registry/RegistryValueType.hpp"
#include "com/sun/star/registry/XImplementationRegistration.hpp"
#include "com/sun/star/registry/XRegistryKey.hpp"
#include "com/sun/star/registry/XSimpleRegistry.hpp"
#include "com/sun/star/script/CannotConvertException.hpp"
#include "com/sun/star/script/FailReason.hpp"
#include "com/sun/star/script/InvocationInfo.hpp"
#include "com/sun/star/script/MemberType.hpp"
#include "com/sun/star/script/XInvocation.hpp"
#include "com/sun/star/script/XInvocation2.hpp"
#include "com/sun/star/script/XInvocationAdapterFactory.hpp"
#include "com/sun/star/script/XInvocationAdapterFactory2.hpp"
#include "com/sun/star/script/XTypeConverter.hpp"
#include "com/sun/star/security/AccessControlException.hpp"
#include "com/sun/star/security/AllPermission.hpp"
#include "com/sun/star/security/RuntimePermission.hpp"
#include "com/sun/star/security/XAccessControlContext.hpp"
#include "com/sun/star/security/XAccessController.hpp"
#include "com/sun/star/security/XAction.hpp"
#include "com/sun/star/security/XPolicy.hpp"
#include "com/sun/star/task/XInteractionAbort.hpp"
#include "com/sun/star/task/XInteractionContinuation.hpp"
#include "com/sun/star/task/XInteractionHandler.hpp"
#include "com/sun/star/task/XInteractionRequest.hpp"
#include "com/sun/star/task/XInteractionRetry.hpp"
#include "com/sun/star/uno/Any.h"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/DeploymentException.hpp"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.h"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/RuntimeException.hpp"
#include "com/sun/star/uno/SecurityException.hpp"
#include "com/sun/star/uno/Sequence.h"
#include "com/sun/star/uno/Sequence.hxx"
#include "com/sun/star/uno/Type.h"
#include "com/sun/star/uno/Type.hxx"
#include "com/sun/star/uno/TypeClass.hpp"
#include "com/sun/star/uno/Uik.hpp"
#include "com/sun/star/uno/XAdapter.hpp"
#include "com/sun/star/uno/XAggregation.hpp"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/uno/XCurrentContext.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/uno/XNamingService.hpp"
#include "com/sun/star/uno/XReference.hpp"
#include "com/sun/star/uno/XUnloadingPreference.hpp"
#include "com/sun/star/uri/RelativeUriExcessParentSegments.hpp"
#include "com/sun/star/uri/XExternalUriReferenceTranslator.hpp"
#include "com/sun/star/uri/XUriReference.hpp"
#include "com/sun/star/uri/XUriReferenceFactory.hpp"
#include "com/sun/star/uri/XUriSchemeParser.hpp"
#include "com/sun/star/uri/XVndSunStarPkgUrlReferenceFactory.hpp"
#include "com/sun/star/uri/XVndSunStarScriptUrlReference.hpp"
#include "com/sun/star/util/XMacroExpander.hpp"

#include "cppu/macros.hxx"
#include "cppu/unotype.hxx"

#include "cppuhelper/access_control.hxx"
#include "cppuhelper/bootstrap.hxx"
#include "cppuhelper/component_context.hxx"
#include "cppuhelper/exc_hlp.hxx"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "cppuhelper/queryinterface.hxx"
#include "cppuhelper/servicefactory.hxx"
#include "cppuhelper/shlib.hxx"
#include "cppuhelper/typeprovider.hxx"
#include "cppuhelper/weak.hxx"
#include "cppuhelper/weakref.hxx"




#include "jvmfwk/framework.h"

#include "osl/diagnose.h"
#include "osl/doublecheckedlocking.h"
#include "osl/file.h"
#include "osl/file.hxx"
#include "osl/interlck.h"
#include "osl/module.h"
#include "osl/module.hxx"
#include "osl/mutex.hxx"
#include "osl/process.h"
#include "osl/security.h"
#include "osl/socket.hxx"
#include "osl/thread.h"
#include "osl/thread.hxx"

#include "registry/refltype.hxx"
#include "registry/registry.hxx"
#include "registry/types.h"
#include "registry/version.h"

#include "rtl/alloc.h"
#include "rtl/bootstrap.hxx"
#include "rtl/byteseq.hxx"
#include "rtl/process.h"
#include "rtl/strbuf.hxx"
#include "rtl/string.h"
#include "rtl/string.hxx"
#include "rtl/textenc.h"
#include "rtl/unload.h"
#include "rtl/uri.h"
#include "rtl/ustrbuf.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"

#include "salhelper/simplereferenceobject.hxx"


#include "typelib/typeclass.h"
#include "typelib/typedescription.h"
#include "typelib/typedescription.hxx"

#include "uno/any2.h"
#include "uno/current_context.h"
#include "uno/current_context.hxx"
#include "uno/data.h"
#include "uno/dispatcher.h"
#include "uno/dispatcher.hxx"
#include "uno/environment.h"
#include "uno/environment.hxx"
#include "uno/lbnames.h"
#include "uno/mapping.hxx"
//---MARKER---
#endif
#endif
