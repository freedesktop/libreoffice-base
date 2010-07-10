/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#ifndef DBAUI_DLGSAVE_HXX
#include "dlgsave.hxx"
#endif
#ifndef DBAUI_DLGSAVE_HRC
#include "dlgsave.hrc"
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _DBU_DLG_HRC_
#include "dbu_dlg.hrc"
#endif
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _DBAUI_SQLMESSAGE_HXX_
#include "sqlmessage.hxx"
#endif
#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include <connectivity/dbtools.hxx>
#endif
#ifndef DBAUI_TOOLS_HXX
#include "UITools.hxx"
#endif
#ifndef _DBA_DBACCESS_HELPID_HRC_
#include "dbaccess_helpid.hrc"
#endif
#ifndef DBAUI_SQLNAMEEDIT_HXX
#include "SqlNameEdit.hxx"
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_ 
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XHIERARCHICALNAMEACCESS_HPP_
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDATABASEMETADATA_HPP_
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#endif
#ifndef _DBAUI_MODULE_DBU_HXX_
#include "moduledbu.hxx"
#endif
#ifndef DBACCESS_SOURCE_UI_INC_OBJECTNAMECHECK_HXX
#include "objectnamecheck.hxx"
#endif
#ifndef TOOLS_DIAGNOSE_EX_H
#include <tools/diagnose_ex.h>
#endif


using namespace dbaui;
using namespace dbtools;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdbc;
namespace dbaui
{
class OSaveAsDlgImpl
{
public:
    FixedText			m_aDescription;
    FixedText			m_aCatalogLbl;
    OSQLNameComboBox	m_aCatalog;
    FixedText			m_aSchemaLbl;
    OSQLNameComboBox	m_aSchema;
    FixedText			m_aLabel;
    OSQLNameEdit		m_aTitle;
    OKButton			m_aPB_OK;
    CancelButton		m_aPB_CANCEL;
    HelpButton			m_aPB_HELP;
    String				m_aQryLabel;
    String				m_sTblLabel;
    rtl::OUString       m_sCatalog;
    rtl::OUString       m_sSchema;	
    String				m_aName;
    const IObjectNameCheck&
                        m_rObjectNameCheck;
    String				m_sParentURL;
    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData>			m_xMetaData;
    sal_Int32			m_nType;
    sal_Int32			m_nFlags;

    OSaveAsDlgImpl(	Window * pParent,const sal_Int32& _rType,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection>& _xConnection,
                    const String& rDefault,
                    const IObjectNameCheck& _rObjectNameCheck,
                    sal_Int32 _nFlags);
    OSaveAsDlgImpl(	Window * pParent,
                    const String& rDefault,
                    const IObjectNameCheck& _rObjectNameCheck,
                    sal_Int32 _nFlags);
};
// -----------------------------------------------------------------------------
} // dbaui
// -----------------------------------------------------------------------------
OSaveAsDlgImpl::OSaveAsDlgImpl( Window * _pParent,
                        const sal_Int32& _rType,
                        const Reference< XConnection>& _xConnection,
                        const String& rDefault,
                        const IObjectNameCheck& _rObjectNameCheck,
                        sal_Int32 _nFlags)
    :m_aDescription(_pParent, ModuleRes (FT_DESCRIPTION))
    ,m_aCatalogLbl(_pParent, ModuleRes (FT_CATALOG))
    ,m_aCatalog(_pParent, ModuleRes (ET_CATALOG), ::rtl::OUString())
    ,m_aSchemaLbl(_pParent, ModuleRes (FT_SCHEMA))
    ,m_aSchema(_pParent, ModuleRes (ET_SCHEMA), ::rtl::OUString())
    ,m_aLabel(_pParent, ModuleRes (FT_TITLE))
    ,m_aTitle(_pParent, ModuleRes (ET_TITLE), ::rtl::OUString())
    ,m_aPB_OK(_pParent, ModuleRes( PB_OK ) )
    ,m_aPB_CANCEL(_pParent, ModuleRes( PB_CANCEL ))
    ,m_aPB_HELP(_pParent, ModuleRes( PB_HELP))
    ,m_aQryLabel(ModuleRes(STR_QRY_LABEL))
    ,m_sTblLabel(ModuleRes(STR_TBL_LABEL))
    ,m_aName(rDefault)
    ,m_rObjectNameCheck( _rObjectNameCheck )
             ,m_nType(_rType)
             ,m_nFlags(_nFlags)
{
    if ( _xConnection.is() )
        m_xMetaData = _xConnection->getMetaData();

    if ( m_xMetaData.is() )
    {
        ::rtl::OUString sExtraNameChars( m_xMetaData->getExtraNameCharacters() );
        m_aCatalog.setAllowedChars( sExtraNameChars );
        m_aSchema.setAllowedChars( sExtraNameChars );
        m_aTitle.setAllowedChars( sExtraNameChars );
    }

    m_aCatalog.SetDropDownLineCount( 10 );
    m_aSchema.SetDropDownLineCount( 10 );
}
// -----------------------------------------------------------------------------
OSaveAsDlgImpl::OSaveAsDlgImpl( Window * _pParent,
                        const String& rDefault,
                        const IObjectNameCheck& _rObjectNameCheck,
                        sal_Int32 _nFlags)
             :m_aDescription(_pParent, ModuleRes (FT_DESCRIPTION))
             ,m_aCatalogLbl(_pParent, ModuleRes (FT_CATALOG))
             ,m_aCatalog(_pParent, ModuleRes (ET_CATALOG))
             ,m_aSchemaLbl(_pParent, ModuleRes (FT_SCHEMA))
             ,m_aSchema(_pParent, ModuleRes (ET_SCHEMA))
             ,m_aLabel(_pParent, ModuleRes (FT_TITLE))
             ,m_aTitle(_pParent, ModuleRes (ET_TITLE))
             ,m_aPB_OK(_pParent, ModuleRes( PB_OK ) )
             ,m_aPB_CANCEL(_pParent, ModuleRes( PB_CANCEL ))
             ,m_aPB_HELP(_pParent, ModuleRes( PB_HELP))
             ,m_aQryLabel(ModuleRes(STR_QRY_LABEL))
             ,m_sTblLabel(ModuleRes(STR_TBL_LABEL))
             ,m_aName(rDefault)
             ,m_rObjectNameCheck( _rObjectNameCheck )
             ,m_nType(CommandType::COMMAND)
             ,m_nFlags(_nFlags)
{
    m_aCatalog.SetDropDownLineCount( 10 );
    m_aSchema.SetDropDownLineCount( 10 );
}

// -----------------------------------------------------------------------------
using namespace ::com::sun::star::lang;

//==================================================================
namespace
{
    typedef Reference< XResultSet > (SAL_CALL XDatabaseMetaData::*FGetMetaStrings)();

    void lcl_fillComboList( ComboBox& _rList, const Reference< XConnection >& _rxConnection,
        FGetMetaStrings _GetAll, const ::rtl::OUString& _rCurrent )
    {
        try
        {
            Reference< XDatabaseMetaData > xMetaData( _rxConnection->getMetaData(), UNO_QUERY_THROW );

            Reference< XResultSet > xRes = (xMetaData.get()->*_GetAll)();
            Reference< XRow > xRow( xRes, UNO_QUERY_THROW );
            ::rtl::OUString sValue;
            while ( xRes->next() )
            {
                sValue = xRow->getString( 1 );
                if ( !xRow->wasNull() )
                    _rList.InsertEntry( sValue );
            }

            USHORT nPos = _rList.GetEntryPos( String( _rCurrent ) );
            if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
                _rList.SelectEntryPos( nPos );
            else
                _rList.SelectEntryPos( 0 );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
}

//==================================================================
OSaveAsDlg::OSaveAsDlg( Window * pParent,
                        const sal_Int32& _rType,
                        const Reference< XMultiServiceFactory >& _rxORB,
                        const Reference< XConnection>& _xConnection,
                        const String& rDefault,
                        const IObjectNameCheck& _rObjectNameCheck,
                        sal_Int32 _nFlags)
    :ModalDialog( pParent, ModuleRes(DLG_SAVE_AS))
    ,m_xORB( _rxORB )
{
    m_pImpl = new OSaveAsDlgImpl(this,_rType,_xConnection,rDefault,_rObjectNameCheck,_nFlags);

    switch (_rType)
    {
        case CommandType::QUERY:
            implInitOnlyTitle(m_pImpl->m_aQryLabel);
            break;

        case CommandType::TABLE:
            OSL_ENSURE( m_pImpl->m_xMetaData.is(), "OSaveAsDlg::OSaveAsDlg: no meta data for entering table names: this will crash!" );
            {
                m_pImpl->m_aLabel.SetText(m_pImpl->m_sTblLabel);
                Point aPos(m_pImpl->m_aPB_OK.GetPosPixel());
                if(m_pImpl->m_xMetaData.is() && !m_pImpl->m_xMetaData->supportsCatalogsInTableDefinitions())
                {
                    m_pImpl->m_aCatalogLbl.Hide();
                    m_pImpl->m_aCatalog.Hide();

                    aPos = m_pImpl->m_aLabel.GetPosPixel();

                    m_pImpl->m_aLabel.SetPosPixel(m_pImpl->m_aSchemaLbl.GetPosPixel());
                    m_pImpl->m_aTitle.SetPosPixel(m_pImpl->m_aSchema.GetPosPixel());

                    m_pImpl->m_aSchemaLbl.SetPosPixel(m_pImpl->m_aCatalogLbl.GetPosPixel());
                    m_pImpl->m_aSchema.SetPosPixel(m_pImpl->m_aCatalog.GetPosPixel());
                }
                else
                {
                    // now fill the catalogs
                    lcl_fillComboList( m_pImpl->m_aCatalog, _xConnection,
                        &XDatabaseMetaData::getCatalogs, _xConnection->getCatalog() );
                }

                if ( !m_pImpl->m_xMetaData->supportsSchemasInTableDefinitions())
                {
                    m_pImpl->m_aSchemaLbl.Hide();
                    m_pImpl->m_aSchema.Hide();

                    aPos = m_pImpl->m_aLabel.GetPosPixel();

                    m_pImpl->m_aLabel.SetPosPixel(m_pImpl->m_aSchemaLbl.GetPosPixel());
                    m_pImpl->m_aTitle.SetPosPixel(m_pImpl->m_aSchema.GetPosPixel());
                }
                else
                {
                    lcl_fillComboList( m_pImpl->m_aSchema, _xConnection,
                        &XDatabaseMetaData::getSchemas, m_pImpl->m_xMetaData->getUserName() );
                }

                OSL_ENSURE(m_pImpl->m_xMetaData.is(),"The metadata can not be null!");
                if(m_pImpl->m_aName.Search('.') != STRING_NOTFOUND)
                {
                    ::rtl::OUString sCatalog,sSchema,sTable;
                    ::dbtools::qualifiedNameComponents(m_pImpl->m_xMetaData,
                                                        m_pImpl->m_aName,
                                                        sCatalog, 
                                                        sSchema, 
                                                        sTable,
                                                        ::dbtools::eInDataManipulation);

                    USHORT nPos = m_pImpl->m_aCatalog.GetEntryPos(String(sCatalog));
                    if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
                        m_pImpl->m_aCatalog.SelectEntryPos(nPos);
                    
                    if ( sSchema.getLength() )
                    {
                        nPos = m_pImpl->m_aSchema.GetEntryPos(String(sSchema));
                        if ( nPos != COMBOBOX_ENTRY_NOTFOUND )
                            m_pImpl->m_aSchema.SelectEntryPos(nPos);
                    }
                    m_pImpl->m_aTitle.SetText(sTable);
                }
                else
                    m_pImpl->m_aTitle.SetText(m_pImpl->m_aName);

                m_pImpl->m_aPB_OK.SetPosPixel(Point(m_pImpl->m_aPB_OK.GetPosPixel().X(),aPos.Y()));
                m_pImpl->m_aPB_CANCEL.SetPosPixel(Point(m_pImpl->m_aPB_CANCEL.GetPosPixel().X(),aPos.Y()));
                m_pImpl->m_aPB_HELP.SetPosPixel(Point(m_pImpl->m_aPB_HELP.GetPosPixel().X(),aPos.Y()));

                USHORT nLength =  m_pImpl->m_xMetaData.is() ? static_cast<USHORT>(m_pImpl->m_xMetaData->getMaxTableNameLength()) : 0;
                nLength = nLength ? nLength : EDIT_NOLIMIT;

                m_pImpl->m_aTitle.SetMaxTextLen(nLength);
                m_pImpl->m_aSchema.SetMaxTextLen(nLength);
                m_pImpl->m_aCatalog.SetMaxTextLen(nLength);

                sal_Bool bCheck = _xConnection.is() && isSQL92CheckEnabled(_xConnection);
                m_pImpl->m_aTitle.setCheck(bCheck); // enable non valid sql chars as well
                m_pImpl->m_aSchema.setCheck(bCheck); // enable non valid sql chars as well
                m_pImpl->m_aCatalog.setCheck(bCheck); // enable non valid sql chars as well
                
                Size aSize = GetSizePixel();
                aSize.Height() =
                    aPos.Y() + m_pImpl->m_aPB_OK.GetSizePixel().Height() + m_pImpl->m_aTitle.GetSizePixel().Height() / 2;
                SetSizePixel(aSize);
            }
            break;

        default:
            OSL_ENSURE( false, "OSaveAsDlg::OSaveAsDlg: Type not supported yet!" );
    }

    implInit();
}
// -----------------------------------------------------------------------------
OSaveAsDlg::OSaveAsDlg( Window * pParent,
                        const Reference< XMultiServiceFactory >& _rxORB,
                        const String& rDefault,
                        const String& _sLabel,
                        const IObjectNameCheck& _rObjectNameCheck,
                        sal_Int32 _nFlags)
             :ModalDialog( pParent, ModuleRes(DLG_SAVE_AS))
             ,m_xORB( _rxORB )
{
    m_pImpl = new OSaveAsDlgImpl(this,rDefault,_rObjectNameCheck,_nFlags);
    implInitOnlyTitle(_sLabel);
    implInit();	
}
// -----------------------------------------------------------------------------
OSaveAsDlg::~OSaveAsDlg()
{
    DELETEZ(m_pImpl);
}
// -----------------------------------------------------------------------------
IMPL_LINK(OSaveAsDlg, ButtonClickHdl, Button *, pButton)
{
    if (pButton == &m_pImpl->m_aPB_OK)
    {
        m_pImpl->m_aName = m_pImpl->m_aTitle.GetText();

        ::rtl::OUString sNameToCheck( m_pImpl->m_aName );

        if ( m_pImpl->m_nType == CommandType::TABLE )
        {
            sNameToCheck = ::dbtools::composeTableName(
                m_pImpl->m_xMetaData,
                getCatalog(),
                getSchema(),
                sNameToCheck,
                sal_False,  // no quoting
                ::dbtools::eInDataManipulation
            );
        }

        SQLExceptionInfo aNameError;
        if ( m_pImpl->m_rObjectNameCheck.isNameValid( sNameToCheck, aNameError ) )
            EndDialog( RET_OK );

        showError( aNameError, this, m_xORB );
        m_pImpl->m_aTitle.GrabFocus();
    }
    return 0;
}
// -----------------------------------------------------------------------------

IMPL_LINK(OSaveAsDlg, EditModifyHdl, Edit *, pEdit )
{
    if (pEdit == &m_pImpl->m_aTitle)
        m_pImpl->m_aPB_OK.Enable(0 != m_pImpl->m_aTitle.GetText().Len());
    return 0;
}
// -----------------------------------------------------------------------------
void OSaveAsDlg::implInitOnlyTitle(const String& _rLabel)
{
    m_pImpl->m_aLabel.SetText(_rLabel);
    m_pImpl->m_aCatalogLbl.Hide();
    m_pImpl->m_aCatalog.Hide();
    m_pImpl->m_aSchemaLbl.Hide();
    m_pImpl->m_aSchema.Hide();

    Point aPos(m_pImpl->m_aSchemaLbl.GetPosPixel());
    m_pImpl->m_aLabel.SetPosPixel(m_pImpl->m_aCatalogLbl.GetPosPixel());
    m_pImpl->m_aTitle.SetPosPixel(m_pImpl->m_aCatalog.GetPosPixel());

    m_pImpl->m_aPB_OK.SetPosPixel(Point(m_pImpl->m_aPB_OK.GetPosPixel().X(),aPos.Y()));
    m_pImpl->m_aPB_CANCEL.SetPosPixel(Point(m_pImpl->m_aPB_CANCEL.GetPosPixel().X(),aPos.Y()));
    m_pImpl->m_aPB_HELP.SetPosPixel(Point(m_pImpl->m_aPB_HELP.GetPosPixel().X(),aPos.Y()));

    sal_Int32 nNewHeight =
        aPos.Y() + m_pImpl->m_aPB_OK.GetSizePixel().Height() + m_pImpl->m_aTitle.GetSizePixel().Height() / 2;

    SetSizePixel(Size(GetSizePixel().Width(), nNewHeight));

    m_pImpl->m_aTitle.SetText(m_pImpl->m_aName);
    m_pImpl->m_aTitle.setCheck(sal_False); // enable non valid sql chars as well
}
// -----------------------------------------------------------------------------
void OSaveAsDlg::implInit()
{
    if ( 0 == ( m_pImpl->m_nFlags & SAD_ADDITIONAL_DESCRIPTION ) )
    {
        // hide the description window
        m_pImpl->m_aDescription.Hide();

        // the number of pixels we have to move the other controls
        sal_Int32 nMoveUp = m_pImpl->m_aCatalog.GetPosPixel().Y() - m_pImpl->m_aDescription.GetPosPixel().Y();

        // loop to all controls and move them ...
        for	(	Window* pChildControl = GetWindow( WINDOW_FIRSTCHILD );
                pChildControl;
                pChildControl= pChildControl->GetWindow( WINDOW_NEXT )
            )
        {
            if ( &m_pImpl->m_aDescription != pChildControl )
            {
                Point aPos = pChildControl->GetPosPixel();
                aPos.Y() -= nMoveUp;
                pChildControl->SetPosPixel(aPos);
            }
        }

        // change our own size accordingly
        Size aSize = GetSizePixel();
        aSize.Height() -= nMoveUp;
        SetSizePixel(aSize);
    }

    if ( SAD_TITLE_PASTE_AS == ( m_pImpl->m_nFlags & SAD_TITLE_PASTE_AS ) )
        SetText( String( ModuleRes( STR_TITLE_PASTE_AS ) ) );
    else if ( SAD_TITLE_RENAME == ( m_pImpl->m_nFlags & SAD_TITLE_RENAME ) )
    {
        SetText( String( ModuleRes( STR_TITLE_RENAME ) ) );
        m_pImpl->m_aTitle.SetHelpId(HID_DLG_RENAME);
    }
    
    m_pImpl->m_aPB_OK.SetClickHdl(LINK(this,OSaveAsDlg,ButtonClickHdl));
    m_pImpl->m_aTitle.SetModifyHdl(LINK(this,OSaveAsDlg,EditModifyHdl));
    m_pImpl->m_aTitle.GrabFocus();
    FreeResource();
}
// -----------------------------------------------------------------------------
String OSaveAsDlg::getName() const		{ return m_pImpl->m_aName; }
String OSaveAsDlg::getCatalog() const	{ return m_pImpl->m_aCatalog.IsVisible() ? m_pImpl->m_aCatalog.GetText() : String(); }
String OSaveAsDlg::getSchema() const	{ return m_pImpl->m_aSchema.IsVisible() ? m_pImpl->m_aSchema.GetText() : String(); }

