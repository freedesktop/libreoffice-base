/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: linkeddocuments.cxx,v $
 * $Revision: 1.31.24.2 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_dbaccess.hxx"

#ifndef _DBAUI_LINKEDDOCUMENTS_HXX_
#include "linkeddocuments.hxx"
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#include <tools/diagnose_ex.h>
#include <unotools/confignode.hxx>
#ifndef DBACCESS_SHARED_DBUSTRINGS_HRC
#include "dbustrings.hrc"
#endif
#include <comphelper/classids.hxx>
#ifndef COMPHELPER_NAMEDVALUECOLLECTION_HXX
#include <comphelper/namedvaluecollection.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_ 
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_ 
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMESEARCHFLAG_HPP_
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDPROCESSOR_HPP_
#include <com/sun/star/ucb/XCommandProcessor.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_OPENCOMMANDARGUMENT2_HPP_
#include <com/sun/star/ucb/OpenCommandArgument2.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_OPENMODE_HPP_
#include <com/sun/star/ucb/OpenMode.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XJOBEXECUTOR_HPP_
#include <com/sun/star/task/XJobExecutor.hpp>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <cppuhelper/extract.hxx>
#endif
#ifndef _COMPHELPER_TYPES_HXX_ 
#include <comphelper/types.hxx>
#endif
#ifndef _SV_MSGBOX_HXX 
#include <vcl/msgbox.hxx>
#endif
#ifndef _UCBHELPER_CONTENT_HXX 
#include <ucbhelper/content.hxx>
#endif
#ifndef _DBU_MISC_HRC_
#include "dbu_misc.hrc"
#endif
#ifndef SVTOOLS_FILENOTATION_HXX_
#include <svl/filenotation.hxx>
#endif
#ifndef DBACCESS_UI_BROWSER_ID_HXX
#include "browserids.hxx"
#endif
#ifndef _SFXNEW_HXX 
#include <sfx2/new.hxx>
#endif
#ifndef _SVTOOLS_TEMPLDLG_HXX 
#include <svtools/templdlg.hxx>
#endif
#ifndef _DBAUI_MODULE_DBU_HXX_
#include "moduledbu.hxx"
#endif
// -----------------
// for calling basic
#ifndef _SFXAPP_HXX 
#include <sfx2/app.hxx>
#endif
#ifndef _SBXCLASS_HXX 
#include <basic/sbx.hxx>
#endif
#ifndef _SB_SBUNO_HXX 
#include <basic/sbuno.hxx>
#endif
#ifndef _SFX_MACROCONF_HXX 
#include <sfx2/macrconf.hxx>
#endif
#ifndef _EHDL_HXX 
#include <svtools/ehdl.hxx>
#endif
#ifndef _SVX_DATACCESSDESCRIPTOR_HXX_ 
#include <svx/dataaccessdescriptor.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMECONTAINER_HPP_
#include <com/sun/star/container/XHierarchicalNameContainer.hpp>
#endif
#ifndef _SV_WAITOBJ_HXX
#include <vcl/waitobj.hxx>
#endif
#ifndef _COMPHELPER_MIMECONFIGHELPER_HXX_
#include <comphelper/mimeconfighelper.hxx>
#endif

#include <cppuhelper/exc_hlp.hxx>
#include <connectivity/dbtools.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <com/sun/star/io/WrongFormatException.hpp>
#include "com/sun/star/sdb/RowSetVetoException.hpp"

//......................................................................
namespace dbaui
{
//......................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::container;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::frame;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::util;
    using namespace ::com::sun::star::ucb;
    using namespace ::com::sun::star::sdbc;
    using namespace ::com::sun::star::task;
    using namespace ::svt;

    namespace
    {
        Sequence< sal_Int8 > lcl_GetSequenceClassID( sal_uInt32 n1, sal_uInt16 n2, sal_uInt16 n3,
                                                    sal_uInt8 b8, sal_uInt8 b9, sal_uInt8 b10, sal_uInt8 b11,
                                                    sal_uInt8 b12, sal_uInt8 b13, sal_uInt8 b14, sal_uInt8 b15 )
        {
            Sequence< sal_Int8 > aResult( 16 );
            aResult[0] = static_cast<sal_Int8>(n1 >> 24);
            aResult[1] = static_cast<sal_Int8>(( n1 << 8 ) >> 24);
            aResult[2] = static_cast<sal_Int8>(( n1 << 16 ) >> 24);
            aResult[3] = static_cast<sal_Int8>(( n1 << 24 ) >> 24);
            aResult[4] = static_cast<sal_Int8>(n2 >> 8);
            aResult[5] = static_cast<sal_Int8>(( n2 << 8 ) >> 8);
            aResult[6] = static_cast<sal_Int8>(n3 >> 8);
            aResult[7] = static_cast<sal_Int8>(( n3 << 8 ) >> 8);
            aResult[8] = b8;
            aResult[9] = b9;
            aResult[10] = b10;
            aResult[11] = b11;
            aResult[12] = b12;
            aResult[13] = b13;
            aResult[14] = b14;
            aResult[15] = b15;

            return aResult;
        }
    }


    //==================================================================
    //= OLinkedDocumentsAccess
    //==================================================================
    DBG_NAME(OLinkedDocumentsAccess)	
    //------------------------------------------------------------------
    OLinkedDocumentsAccess::OLinkedDocumentsAccess(Window* _pDialogParent
                                                    , const Reference< XFrame >& _rxParentFrame
                                                    , const Reference< XMultiServiceFactory >& _rxORB
                                                    , const Reference< XNameAccess >& _rxContainer
                                                    , const Reference< XConnection>& _xConnection
                                                    , const ::rtl::OUString& _sDataSourceName
                                                    )
        :m_xORB(_rxORB)
        ,m_xDocumentContainer(_rxContainer)
        ,m_xConnection(_xConnection)
        ,m_xParentFrame(_rxParentFrame)
        ,m_pDialogParent(_pDialogParent)
        ,m_sDataSourceName(_sDataSourceName)
    {
        DBG_CTOR(OLinkedDocumentsAccess,NULL);		
        OSL_ENSURE(m_xORB.is(), "OLinkedDocumentsAccess::OLinkedDocumentsAccess: invalid service factory!");
        OSL_ENSURE(m_pDialogParent, "OLinkedDocumentsAccess::OLinkedDocumentsAccess: really need a dialog parent!");
    }
    //------------------------------------------------------------------
    OLinkedDocumentsAccess::~OLinkedDocumentsAccess()
    {
        DBG_DTOR(OLinkedDocumentsAccess,NULL);		
    }
    //------------------------------------------------------------------
    Reference< XComponent> OLinkedDocumentsAccess::impl_open( const ::rtl::OUString& _rLinkName, Reference< XComponent >& _xDefinition,
        ElementOpenMode _eOpenMode, const ::comphelper::NamedValueCollection& _rAdditionalArgs )
    {
        Reference< XComponent> xRet;
        OSL_ENSURE(m_xDocumentContainer.is(), "OLinkedDocumentsAccess::OLinkedDocumentsAccess: invalid document container!");
        Reference< XComponentLoader > xComponentLoader(m_xDocumentContainer,UNO_QUERY);
        if ( !xComponentLoader.is() )
            return xRet;

        WaitObject aWaitCursor( m_pDialogParent );

        ::comphelper::NamedValueCollection aArguments;
        ::rtl::OUString sOpenMode;
        switch ( _eOpenMode )
        {
            case E_OPEN_NORMAL:
                sOpenMode = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "open" ) );
                break;

            case E_OPEN_FOR_MAIL:
                aArguments.put( "Hidden", true );
                // fall through

            case E_OPEN_DESIGN:
                sOpenMode = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "openDesign" ) );
                break;

            default:
                OSL_ENSURE( false, "OLinkedDocumentsAccess::implOpen: invalid open mode!" );
                break;
        }
        aArguments.put( "OpenMode", sOpenMode );

        aArguments.put( (::rtl::OUString)PROPERTY_ACTIVE_CONNECTION, m_xConnection );
        try
        {	
            Reference<XHierarchicalNameContainer> xHier(m_xDocumentContainer,UNO_QUERY);
            if ( xHier.is() && xHier->hasByHierarchicalName(_rLinkName) )
            {
                _xDefinition.set(xHier->getByHierarchicalName(_rLinkName),UNO_QUERY);
            }

            aArguments.merge( _rAdditionalArgs, true );

            xRet = xComponentLoader->loadComponentFromURL( _rLinkName, ::rtl::OUString(), 0, aArguments.getPropertyValues() );
        }
        catch(Exception& e) 
        {
            (void)e;
            throw;
        }

        return xRet;
    }
    //------------------------------------------------------------------
    Reference< XComponent> OLinkedDocumentsAccess::impl_newWithPilot( const char* _pWizardService,
        Reference< XComponent >& _xDefinition, const sal_Int32 _nCommandType, const ::rtl::OUString& _rObjectName )
    {
        Reference< XComponent> xRet;
        try
        {
            ::svx::ODataAccessDescriptor aDesc;
            aDesc.setDataSource(m_sDataSourceName);
            if ( _rObjectName.getLength() && ( _nCommandType != -1 ) )
            {
                aDesc[::svx::daCommandType] <<= _nCommandType;
                aDesc[::svx::daCommand] <<= _rObjectName;
            }
            if ( m_xConnection.is() )
                aDesc[::svx::daConnection] <<= m_xConnection;

            Sequence<Any> aSeq = aDesc.createAnySequence();
            const sal_Int32 nLength = aSeq.getLength();
            aSeq.realloc(nLength + 1 );
            PropertyValue aVal;
            aVal.Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ParentFrame"));
            aVal.Value <<= m_xParentFrame;
            aSeq[nLength] <<= aVal;
            
            Reference< XJobExecutor > xFormWizard;
            {
                WaitObject aWaitCursor( m_pDialogParent );
                xFormWizard.set(m_xORB->createInstanceWithArguments(::rtl::OUString::createFromAscii(_pWizardService),aSeq),UNO_QUERY);
            }
            if ( xFormWizard.is() )
            {
                xFormWizard->trigger(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("start")));
                Reference<XPropertySet> xProp(xFormWizard,UNO_QUERY);
                if ( xProp.is() )
                {
                    Reference<XPropertySetInfo> xInfo = xProp->getPropertySetInfo();
                    if ( xInfo->hasPropertyByName(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Document"))) )
                    {
                        _xDefinition.set(xProp->getPropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DocumentDefinition"))),UNO_QUERY);
                        xRet.set(xProp->getPropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Document"))),UNO_QUERY);
                    }
                }
                xFormWizard->trigger(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("end")));
                ::comphelper::disposeComponent(xFormWizard);
            }
        }
        catch(const Exception& e)
        {
            (void) e;
            OSL_ENSURE(sal_False, "OLinkedDocumentsAccess::newWithPilot: caught an exception while loading the object!");
        }
        return xRet;
    }
    //------------------------------------------------------------------
    Reference< XComponent> OLinkedDocumentsAccess::newFormWithPilot(Reference< XComponent >& _xDefinition,const sal_Int32 _nCommandType,const ::rtl::OUString& _rObjectName)
    {
        return impl_newWithPilot( "com.sun.star.wizards.form.CallFormWizard", _xDefinition, _nCommandType, _rObjectName );
    }

    //------------------------------------------------------------------
    Reference< XComponent> OLinkedDocumentsAccess::newReportWithPilot( Reference< XComponent >& _xDefinition, const sal_Int32 _nCommandType, const ::rtl::OUString& _rObjectName )
    {
        return impl_newWithPilot( "com.sun.star.wizards.report.CallReportWizard", _xDefinition, _nCommandType, _rObjectName );
    }
    //------------------------------------------------------------------
    Reference< XComponent> OLinkedDocumentsAccess::newTableWithPilot()
    {
        Reference< XComponent > xDefinition;
        return impl_newWithPilot( "com.sun.star.wizards.table.CallTableWizard", xDefinition, -1, ::rtl::OUString() );
    }    
    //------------------------------------------------------------------
    Reference< XComponent> OLinkedDocumentsAccess::newQueryWithPilot()
    {
        Reference< XComponent > xDefinition;
        return impl_newWithPilot( "com.sun.star.wizards.query.CallQueryWizard", xDefinition, -1, ::rtl::OUString() );
    }
    //------------------------------------------------------------------
    Reference< XComponent > OLinkedDocumentsAccess::newDocument( sal_Int32 _nNewFormId, Reference< XComponent >& _xDefinition )
    {
        OSL_ENSURE(m_xDocumentContainer.is(), "OLinkedDocumentsAccess::OLinkedDocumentsAccess: invalid document container!");
        // determine the URL to use for the new document
        Sequence<sal_Int8> aClassId;
        switch (_nNewFormId)
        {
            case ID_FORM_NEW_TEXT:
                aClassId = lcl_GetSequenceClassID(SO3_SW_CLASSID);
                OSL_ENSURE(aClassId == comphelper::MimeConfigurationHelper::GetSequenceClassID(SO3_SW_CLASSID),"Not equal");
                break;

            case ID_FORM_NEW_CALC:
                aClassId = lcl_GetSequenceClassID(SO3_SC_CLASSID);
                break;

            case ID_FORM_NEW_IMPRESS:
                aClassId = lcl_GetSequenceClassID(SO3_SIMPRESS_CLASSID);
                break;

            case ID_REPORT_NEW_TEXT:
                aClassId = comphelper::MimeConfigurationHelper::GetSequenceClassID(SO3_RPT_CLASSID_90);
                break;

            case SID_DB_FORM_NEW_PILOT:
            default:
                OSL_ENSURE(sal_False, "OLinkedDocumentsAccess::newForm: pleas use newFormWithPilot!");
                return Reference< XComponent >();

        }
        // load the document as template
        Reference< XComponent > xNewDocument;
        try
        {	// get the desktop object
        
            Reference<XMultiServiceFactory> xORB(m_xDocumentContainer,UNO_QUERY);
            if ( xORB.is() )
            {
                Sequence< Any > aArguments(2);

                PropertyValue aValue;

                aValue.Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ClassID"));
                aValue.Value <<= aClassId;
                aArguments[0] <<= aValue;

                aValue.Name = PROPERTY_ACTIVE_CONNECTION;
                aValue.Value <<= m_xConnection;
                aArguments[1] <<= aValue;

                Reference<XCommandProcessor> xContent(xORB->createInstanceWithArguments(SERVICE_SDB_DOCUMENTDEFINITION,aArguments),UNO_QUERY);
                if ( xContent.is() )
                {
                    _xDefinition.set(xContent,UNO_QUERY);
                    Command aCommand;
                    aCommand.Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("openDesign"));
                    OpenCommandArgument2 aOpenCommand;
                    aOpenCommand.Mode = OpenMode::DOCUMENT;
                    aCommand.Argument <<= aOpenCommand;
                    WaitObject aWaitCursor( m_pDialogParent );
                    xNewDocument.set(xContent->execute(aCommand,xContent->createCommandIdentifier(),Reference< XCommandEnvironment >()),UNO_QUERY);
                }
            }
        }
        catch(const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }

        return xNewDocument;
    }

    //------------------------------------------------------------------
    Reference< XComponent > OLinkedDocumentsAccess::open( const ::rtl::OUString& _rLinkName, Reference< XComponent >& _xDefinition,
        ElementOpenMode _eOpenMode, const ::comphelper::NamedValueCollection& _rAdditionalArgs )
    {	
        dbtools::SQLExceptionInfo aInfo;
        Reference< XComponent > xRet;
        try 
        {
            xRet = impl_open( _rLinkName, _xDefinition, _eOpenMode, _rAdditionalArgs );
            if ( !xRet.is() )
            {
                String sMessage = String(ModuleRes(STR_COULDNOTOPEN_LINKEDDOC));
                sMessage.SearchAndReplaceAscii("$file$",_rLinkName);

                com::sun::star::sdbc::SQLException aSQLException;
                aSQLException.Message = sMessage;
                // aSQLException.Context = e.Context;
                aInfo = dbtools::SQLExceptionInfo(aSQLException);
            }
            return xRet;
        }
        catch (com::sun::star::io::WrongFormatException e)
        { 
            com::sun::star::sdbc::SQLException aSQLException;
            aSQLException.Message = e.Message;
            aSQLException.Context = e.Context;
            aInfo = dbtools::SQLExceptionInfo(aSQLException);

            // more like a hack, insert an empty message
            String sText( ModuleRes( RID_STR_EXTENSION_NOT_PRESENT ) );
            sText.SearchAndReplaceAscii("$file$",_rLinkName);
            aInfo.prepend(sText);

            String sMessage = String(ModuleRes(STR_COULDNOTOPEN_LINKEDDOC));
            sMessage.SearchAndReplaceAscii("$file$",_rLinkName);
            aInfo.prepend(sMessage);            
        }
        catch(Exception& e) 
        {
            Any aAny = ::cppu::getCaughtException();
            com::sun::star::sdbc::SQLException a;
            if ( !(aAny >>= a) || (a.ErrorCode != dbtools::ParameterInteractionCancelled) )
            {
                com::sun::star::sdbc::SQLException aSQLException;
                aSQLException.Message = e.Message;
                aSQLException.Context = e.Context;
                aInfo = dbtools::SQLExceptionInfo(aSQLException);
                
                // more like a hack, insert an empty message
                aInfo.prepend(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(" \n")));
                
                String sMessage = String(ModuleRes(STR_COULDNOTOPEN_LINKEDDOC));
                sMessage.SearchAndReplaceAscii("$file$",_rLinkName);
                aInfo.prepend(sMessage);
            }
        }
        if (aInfo.isValid())
        {
            showError(aInfo, VCLUnoHelper::GetInterface(m_pDialogParent), m_xORB );
        }
        return xRet;
    }
    

//......................................................................
}	// namespace dbaui
//......................................................................

