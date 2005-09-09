/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xml_cdim.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 12:00:19 $
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

#include "xml_cdim.hxx"

const char ComponentDescriptionImpl::C_sTagDescription[]
                                        =   "COMPONENTDESCRIPTION";
const char ComponentDescriptionImpl::C_sStatus[]
                                        =   "Status";
const char * ComponentDescriptionImpl::C_sSubTags[ComponentDescription::tag_MAX]
                                        = {	"None",
                                            "Name",
                                            "Description",
                                            "ModuleName",
                                            "LoaderName",
                                            "SupportedService",
                                            "ProjectBuildDependency",
                                            "RuntimeModuleDependency",
                                            "ServiceDependency",
                                            "Language",
                                            C_sStatus,
                                            "Type"
                                          };

ComponentDescriptionImpl::ComponentDescriptionImpl()
//	:	aTags
{
    const int i_max = tag_MAX;
    aTags.reserve(i_max);

    for (int i = 0; i < i_max; ++i)
    {
        aTags.push_back( new ValueList(E_Tag(i)) );
    }  // end for
}

ComponentDescriptionImpl::~ComponentDescriptionImpl()
{
    for ( std::vector< ValueList* >::iterator aIter = aTags.begin();
          aIter != aTags.end();
          ++aIter )
    {
        delete *aIter;
    }
}

inline void
GetStatusValue( ByteString & o_sValue, const ByteString & i_sStatusTag )
{
    // o_sValue is always == "" at the beginning.

    const char * pStatusValue = strchr(i_sStatusTag.GetBuffer(), '"');
    if (pStatusValue == 0)
        return;
    pStatusValue++;
    const char * pStatusValueEnd = strrchr(pStatusValue,'"');
    if (pStatusValueEnd == 0 || pStatusValueEnd - pStatusValue < 1)
        return ;

    ByteString sValue(pStatusValue, pStatusValueEnd - pStatusValue);
    o_sValue = sValue;
}

ComponentDescriptionImpl::ValueList *
ComponentDescriptionImpl::GetBeginTag( ByteString &	 o_sValue,
                                       const char *& io_pStartOfTag ) const
{
    o_sValue = "";

    const char * pCurTextEnd = strchr(io_pStartOfTag,'>');
    if ( 0 == pCurTextEnd )
        return 0;

    if ( ComponentDescriptionImpl::CheckEndOfDescription(io_pStartOfTag) )
        return 0;

    ByteString sTag(io_pStartOfTag + 1, pCurTextEnd - io_pStartOfTag - 1);
    io_pStartOfTag += sTag.Len() + 2;

    // Special case <Status ... >
    if ( strnicmp(C_sStatus, sTag.GetBuffer(), (sizeof C_sStatus) - 1 ) == 0 )
    {
        GetStatusValue(o_sValue,sTag);
        return aTags[tag_Status];
    }

    // Regular seeking for matching data list:
    for ( INT32 i = 0; i < tag_MAX; i++ )
    {
        if ( 0 == stricmp(sTag.GetBuffer(), C_sSubTags[i]) )
            return aTags[i];
    }	// end for

    return 0;
}

const std::vector< ByteString > &
ComponentDescriptionImpl::DataOf( ComponentDescriptionImpl::E_Tag i_eTag ) const
{
    if (0 < i_eTag && i_eTag < tag_MAX)
        return *aTags[i_eTag];
    else
        return ValueList::Null_();
}

ByteString
ComponentDescriptionImpl::DatumOf( ComponentDescriptionImpl::E_Tag i_eTag ) const
{
    if (0 < i_eTag && i_eTag < tag_MAX)
    {
        ValueList & rValues = *aTags[i_eTag];
        if (rValues.size() > 0)
            return rValues[0];
    }
    return "";
}

void
ComponentDescriptionImpl::ParseUntilStartOfDescription( const char * & io_pBufferPosition )
{
    for ( const char * pSearch = strchr(io_pBufferPosition,'<');
          pSearch != 0;
          pSearch = strchr(pSearch+1,'<') )
    {
        if ( pSearch != io_pBufferPosition
             && 0 == strnicmp(pSearch+1,C_sTagDescription, strlen(C_sTagDescription))
             && *(pSearch + strlen(C_sTagDescription) + 1) == '>' )
        {
            io_pBufferPosition = pSearch + strlen(C_sTagDescription) + 2;
            return;
        }
    }	// end for

    io_pBufferPosition = 0;
}

BOOL
ComponentDescriptionImpl::ValueList::MatchesEndTag( const char * i_pTextPosition ) const
{
    return strnicmp( i_pTextPosition+2, C_sSubTags[eTag], strlen(C_sSubTags[eTag]) ) == 0
           && strncmp(i_pTextPosition,"</",2) == 0
           && *(i_pTextPosition + 2 + strlen(C_sSubTags[eTag]) ) == '>';
}

INT32
ComponentDescriptionImpl::ValueList::EndTagLength() const
{
    return strlen(C_sSubTags[eTag]) + 3;
}


const ComponentDescriptionImpl::ValueList &
ComponentDescriptionImpl::ValueList::Null_()
{
    static const ValueList aNull_(ComponentDescription::tag_None);
    return aNull_;
}


