/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: RelationDlg.cxx,v $
 * $Revision: 1.27 $
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
//#ifndef _SVX_TABWIN_HXX
//#include "tabwin.hxx"
//#endif
#ifndef DBAUI_RELATIONDIALOG_HRC
#include "RelationDlg.hrc"
#endif
#ifndef DBAUI_RELATIONDIALOG_HXX
#include "RelationDlg.hxx"
#endif

#ifndef _WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif

#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif
#ifndef _DBU_DLG_HRC_
#include "dbu_dlg.hrc"
#endif
#ifndef _DBA_DBACCESS_HELPID_HRC_
#include "dbaccess_helpid.hrc"
#endif
#ifndef _COM_SUN_STAR_SDBC_KEYRULE_HPP_
#include <com/sun/star/sdbc/KeyRule.hpp>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef DBAUI_TOOLS_HXX
#include "UITools.hxx"
#endif
#ifndef DBAUI_JOINDESIGNVIEW_HXX
#include "JoinDesignView.hxx"
#endif
#ifndef DBAUI_JOINCONTROLLER_HXX
#include "JoinController.hxx"
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include <connectivity/dbexception.hxx>
#endif
#ifndef DBAUI_RTABLECONNECTIONDATA_HXX
#include "RTableConnectionData.hxx"
#endif
#ifndef DBAUI_RELATIONCONTROL_HXX
#include "RelationControl.hxx"
#endif

#include <algorithm>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;
using namespace ::dbaui;
using namespace ::dbtools;

//========================================================================
// class ORelationDialog
DBG_NAME(ORelationDialog)
//========================================================================
ORelationDialog::ORelationDialog( OJoinTableView* pParent,
                                 const TTableConnectionData::value_type& pConnectionData,
                                 BOOL bAllowTableSelect )
    :ModalDialog( pParent, ModuleRes(DLG_REL_PROPERTIES) )
    ,m_pTableMap(pParent->GetTabWinMap())

    ,aFL_CascUpd(           this, ModuleRes(FL_CASC_UPD) )
    ,aRB_NoCascUpd(			this, ModuleRes(RB_NO_CASC_UPD) )
    ,aRB_CascUpd(			this, ModuleRes(RB_CASC_UPD) )
    ,aRB_CascUpdNull(		this, ModuleRes(RB_CASC_UPD_NULL) )
    ,aRB_CascUpdDefault(	this, ModuleRes(RB_CASC_UPD_DEFAULT) )
    ,aFL_CascDel(           this, ModuleRes(FL_CASC_DEL) )
    ,aRB_NoCascDel(			this, ModuleRes(RB_NO_CASC_DEL) )
    ,aRB_CascDel(			this, ModuleRes(RB_CASC_DEL) )
    ,aRB_CascDelNull(		this, ModuleRes(RB_CASC_DEL_NULL) )
    ,aRB_CascDelDefault(	this, ModuleRes(RB_CASC_DEL_DEFAULT) )

    ,aPB_OK( this, ModuleRes( PB_OK ) )
    ,aPB_CANCEL( this, ModuleRes( PB_CANCEL ) )
    ,aPB_HELP( this, ModuleRes( PB_HELP ) )

    ,m_pOrigConnData( pConnectionData )
    ,m_bTriedOneUpdate(FALSE)
{
    DBG_CTOR(ORelationDialog,NULL);

    m_xConnection = pParent->getDesignView()->getController()->getConnection();

    //////////////////////////////////////////////////////////////////////
    // Connection kopieren
    m_pConnData.reset( static_cast<ORelationTableConnectionData*>(pConnectionData->NewInstance()) );
    m_pConnData->CopyFrom( *pConnectionData );

    Init(m_pConnData);
    m_pTableControl.reset( new OTableListBoxControl(this,ModuleRes(WND_CONTROL),m_pTableMap,this) );

    aPB_OK.SetClickHdl( LINK(this, ORelationDialog, OKClickHdl) );

    m_pTableControl->Init( m_pConnData );
    if ( bAllowTableSelect )
        m_pTableControl->fillListBoxes();
    else
        m_pTableControl->fillAndDisable(pConnectionData);

    m_pTableControl->lateInit();

    m_pTableControl->NotifyCellChange();

    FreeResource();
}

//------------------------------------------------------------------------
void ORelationDialog::Init(const TTableConnectionData::value_type& _pConnectionData)
{
    ORelationTableConnectionData* pConnData = static_cast<ORelationTableConnectionData*>(_pConnectionData.get());
    // Update Rules
    switch (pConnData->GetUpdateRules())
    {
    case KeyRule::NO_ACTION:
    case KeyRule::RESTRICT:
        aRB_NoCascUpd.Check( TRUE );
        break;

    case KeyRule::CASCADE:
        aRB_CascUpd.Check( TRUE );
        break;

    case KeyRule::SET_NULL:
        aRB_CascUpdNull.Check( TRUE );
        break;
    case KeyRule::SET_DEFAULT:
        aRB_CascUpdDefault.Check( TRUE );
        break;
    }

    // Delete Rules
    switch (pConnData->GetDeleteRules())
    {
    case KeyRule::NO_ACTION:
    case KeyRule::RESTRICT:
        aRB_NoCascDel.Check( TRUE );
        break;

    case KeyRule::CASCADE:
        aRB_CascDel.Check( TRUE );
        break;

    case KeyRule::SET_NULL:
        aRB_CascDelNull.Check( TRUE );
        break;
    case KeyRule::SET_DEFAULT:
        aRB_CascDelDefault.Check( TRUE );
        break;
    }
}

//------------------------------------------------------------------------
ORelationDialog::~ORelationDialog()
{
    DBG_DTOR(ORelationDialog,NULL);
}

//------------------------------------------------------------------------


//------------------------------------------------------------------------
IMPL_LINK( ORelationDialog, OKClickHdl, Button*, /*pButton*/ )
{
    //////////////////////////////////////////////////////////////////////
    // RadioButtons auslesen
    UINT16 nAttrib = 0;

    // Delete Rules
    if( aRB_NoCascDel.IsChecked() )
        nAttrib |= KeyRule::NO_ACTION;
    if( aRB_CascDel.IsChecked() )
        nAttrib |= KeyRule::CASCADE;
    if( aRB_CascDelNull.IsChecked() )
        nAttrib |= KeyRule::SET_NULL;
    if( aRB_CascDelDefault.IsChecked() )
        nAttrib |= KeyRule::SET_DEFAULT;

    ORelationTableConnectionData* pConnData = static_cast<ORelationTableConnectionData*>(m_pConnData.get());
    pConnData->SetDeleteRules( nAttrib );

    // Update Rules
    nAttrib = 0;
    if( aRB_NoCascUpd.IsChecked() )
        nAttrib |= KeyRule::NO_ACTION;
    if( aRB_CascUpd.IsChecked() )
        nAttrib |= KeyRule::CASCADE;
    if( aRB_CascUpdNull.IsChecked() )
        nAttrib |= KeyRule::SET_NULL;
    if( aRB_CascUpdDefault.IsChecked() )
        nAttrib |= KeyRule::SET_DEFAULT;
    pConnData->SetUpdateRules( nAttrib );

    m_pTableControl->SaveModified();

    //// wenn die ComboBoxen fuer die Tabellenauswahl enabled sind (Constructor mit bAllowTableSelect==TRUE), dann muss ich in die
    //// Connection auch die Tabellennamen stecken
    //m_pConnData->SetSourceWinName(m_pTableControl->getSourceWinName());
    //m_pConnData->SetDestWinName(m_pTableControl->getDestWinName());

    // try to create the relation
    try
    {
        ORelationTableConnectionData* pOrigConnData = static_cast<ORelationTableConnectionData*>(m_pOrigConnData.get());
        if (*pConnData != *pOrigConnData || pConnData->Update())
        {
            m_pOrigConnData->CopyFrom( *m_pConnData );
            EndDialog( RET_OK );
            return 0L;
        }
    }
    catch(const SQLException& e)
    {
        ::dbaui::showError(	SQLExceptionInfo(e),
                            this,
                            static_cast<OJoinTableView*>(GetParent())->getDesignView()->getController()->getORB());
    }
    catch(const Exception&)
    {
        //OSL_ENSURE(sal_False, "ORelationDialog, OKClickHdl: caught an exception!");
    }

    m_bTriedOneUpdate = TRUE;
    // this means that the original connection may be lost (if m_pConnData was not a newly created but an
    // existent conn to be modified), which we reflect by returning RET_NO (see ::Execute)

    // try again
    Init(m_pConnData);
    m_pTableControl->Init( m_pConnData );
    m_pTableControl->lateInit();

    return 0;
}


//------------------------------------------------------------------------
short ORelationDialog::Execute()
{
    short nResult = ModalDialog::Execute();
    if ((nResult != RET_OK) && m_bTriedOneUpdate)
        return RET_NO;

    return nResult;
}
// -----------------------------------------------------------------------------
TTableConnectionData::value_type ORelationDialog::getConnectionData() const
{
    return m_pConnData;
}
// -----------------------------------------------------------------------------
void ORelationDialog::setValid(sal_Bool _bValid)
{
    aPB_OK.Enable(_bValid);
}
// -----------------------------------------------------------------------------
void ORelationDialog::notifyConnectionChange()
{
    Init(m_pConnData);
}
// -----------------------------------------------------------------------------


