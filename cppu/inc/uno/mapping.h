/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: mapping.h,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 08:41:30 $
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
#ifndef _UNO_MAPPING_H_
#define _UNO_MAPPING_H_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _RTL_USTRING_H_
#include <rtl/ustring.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

struct _typelib_InterfaceTypeDescription;
struct _uno_Mapping;
struct _uno_Environment;

/** Function pointer declaration to map an interface from one environment to another.
    
    @param pMapping			mapping
    @param ppOut			[inout] destination interface; existing interfaces are released
    @param pInterface		source interface
    @param pInterfaceTypeDescr type description of the interface
*/
typedef void (SAL_CALL * uno_MapInterfaceFunc)(
    struct _uno_Mapping * pMapping,
    void ** ppOut, void * pInterface,
    struct _typelib_InterfaceTypeDescription * pInterfaceTypeDescr );


#ifdef SAL_W32
#pragma pack(push, 8)
#elif defined(SAL_OS2)
#pragma pack(8)
#endif

/** This is the binary specification of a mapping.
*/
typedef struct _uno_Mapping
{
    /** Acquires mapping
        
        @param pMapping		mapping
    */
    void (SAL_CALL * acquire)( struct _uno_Mapping * pMapping );
    /** Releases mapping. The last release may unload bridges.
        
        @param pMapping		mapping
    */
    void (SAL_CALL * release)( struct _uno_Mapping * pMapping );

    /** mapping function
    */
    uno_MapInterfaceFunc mapInterface;
} uno_Mapping;

#ifdef SAL_W32
#pragma pack(pop)
#elif defined(SAL_OS2)
#pragma pack()
#endif

/** Gets an interface mapping from one environment to another.
    
    @param ppMapping	[inout] mapping; existing mapping will be released
    @param pFrom		source environment
    @param pTo			destination environment
                        (interfaces resulting in mapInterface() call can be used
                        in this language environment)
    @param pAddPurpose	additional purpose of mapping (e.g., protocolling); defaults to 0 (none)
*/
void SAL_CALL uno_getMapping(
    struct _uno_Mapping ** ppMapping,
    struct _uno_Environment * pFrom,
    struct _uno_Environment * pTo,
    rtl_uString * pAddPurpose )
    SAL_THROW_EXTERN_C();

/** Callback function pointer declaration to get a mapping.
    
    @param ppMapping	inout mapping
    @param pFrom		source environment
    @param pTo			destination environment
    @param pAddPurpose	additional purpose
*/
typedef void (SAL_CALL * uno_getMappingFunc)(
    struct _uno_Mapping ** ppMapping,
    struct _uno_Environment * pFrom,
    struct _uno_Environment * pTo,
    rtl_uString * pAddPurpose );

/** Registers a callback being called each time a mapping is demanded.
    
    @param pCallback	callback function
*/
void SAL_CALL uno_registerMappingCallback(
    uno_getMappingFunc pCallback )
    SAL_THROW_EXTERN_C();

/** Revokes a mapping callback registration.
    
    @param pCallback	callback function
*/
void SAL_CALL uno_revokeMappingCallback(
    uno_getMappingFunc pCallback )
    SAL_THROW_EXTERN_C();

/** Function pointer declaration to free a mapping.
    
    @param pMapping		mapping to be freed
*/
typedef void (SAL_CALL * uno_freeMappingFunc)( struct _uno_Mapping * pMapping );

/** Registers a mapping. A mapping registers itself on first acquire and revokes itself on last
    release. The given freeMapping function is called by the runtime to cleanup any resources.
    
    @param ppMapping	inout mapping to be registered
    @param freeMapping	called by runtime to delete mapping
    @param pFrom		source environment
    @param pTo			destination environment
    @param pAddPurpose	additional purpose string; defaults to 0
*/
void SAL_CALL uno_registerMapping(
    struct _uno_Mapping ** ppMapping, uno_freeMappingFunc freeMapping,
    struct _uno_Environment * pFrom, struct _uno_Environment * pTo, rtl_uString * pAddPurpose )
    SAL_THROW_EXTERN_C();

/** Revokes a mapping. A mapping registers itself on first acquire and revokes itself on last
    release.
    
    @param pMapping		mapping to be revoked
*/
void SAL_CALL uno_revokeMapping(
    struct _uno_Mapping * pMapping )
    SAL_THROW_EXTERN_C();

/** Gets an interface mapping from one language environment to another by corresponding environment
    type names.
    
    @param ppMapping	[inout] mapping; existing mapping will be released
    @param pFrom		source environment type name
    @param pTo			destination environment type name
                        (interfaces resulting in mapInterface() call can be used
                        in this language environment)
    @param pAddPurpose	additional purpose of mapping (e.g., protocolling); defaults to 0 (none)
*/
void SAL_CALL uno_getMappingByName(
    struct _uno_Mapping ** ppMapping,
    rtl_uString * pFrom,
    rtl_uString * pTo,
    rtl_uString * pAddPurpose )
    SAL_THROW_EXTERN_C();

/* symbol exported by each language binding library */
#define UNO_EXT_GETMAPPING "uno_ext_getMapping"

/** Function pointer declaration to get a mapping from a loaded bridge. Bridges export a function
    called uno_ext_getMapping() of this signature.
    
    @param ppMapping	[inout] mapping; existing mapping will be released
    @pFrom				source environment
    @pTo				destination environment
*/
typedef void (SAL_CALL * uno_ext_getMappingFunc)(
    struct _uno_Mapping ** ppMapping,
    struct _uno_Environment * pFrom,
    struct _uno_Environment * pTo );

#ifdef __cplusplus
}
#endif

#endif
