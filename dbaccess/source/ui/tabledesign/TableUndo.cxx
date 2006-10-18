/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: TableUndo.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: ihi $ $Date: 2006-10-18 13:33:08 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_dbaccess.hxx"
#ifndef DBAUI_TABLEUNDO_HXX
#include "TableUndo.hxx"
#endif
#ifndef _DBAUI_MODULE_DBU_HXX_
#include "moduledbu.hxx"
#endif
#ifndef _DBU_TBL_HRC_
#include "dbu_tbl.hrc"
#endif
#ifndef DBAUI_TABLEEDITORCONTROL_HXX
#include "TEditControl.hxx"
#endif
#ifndef DBAUI_TABLEROW_HXX
#include "TableRow.hxx"
#endif
#ifndef DBACCESS_UI_BROWSER_ID_HXX
#include "browserids.hxx"
#endif
#ifndef DBUI_TABLECONTROLLER_HXX
#include "TableController.hxx"
#endif
#ifndef DBAUI_TABLEDESIGNVIEW_HXX
#include "TableDesignView.hxx"
#endif
#ifndef DBAUI_FIELDDESCRIPTIONS_HXX
#include "FieldDescriptions.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

using namespace dbaui;
#if SUPD > 636
using namespace ::svt;
#endif

TYPEINIT1( OCommentUndoAction,          SfxUndoAction );
TYPEINIT1( OTableDesignUndoAct,         OCommentUndoAction );
TYPEINIT1( OTableEditorUndoAct,         OTableDesignUndoAct );
TYPEINIT1( OTableDesignCellUndoAct,     OTableDesignUndoAct );
TYPEINIT1( OTableEditorTypeSelUndoAct,  OTableEditorUndoAct );
TYPEINIT1( OTableEditorDelUndoAct,      OTableEditorUndoAct );
TYPEINIT1( OTableEditorInsUndoAct,      OTableEditorUndoAct );
TYPEINIT1( OTableEditorInsNewUndoAct,   OTableEditorUndoAct );
TYPEINIT1( OPrimKeyUndoAct,             OTableEditorUndoAct );

//==============================================================================
// class OTableDesignUndoAct
//==============================================================================
DBG_NAME(OTableDesignUndoAct);
OTableDesignUndoAct::OTableDesignUndoAct( OTableRowView* pOwner,USHORT nCommentID ) : OCommentUndoAction(nCommentID)
    ,m_pTabDgnCtrl(  pOwner )
{
    DBG_CTOR(OTableDesignUndoAct,NULL);
    m_pTabDgnCtrl->m_nCurUndoActId++;
}

//-------------------------------------------------------------------------
OTableDesignUndoAct::~OTableDesignUndoAct()
{
    DBG_DTOR(OTableDesignUndoAct,NULL);
}

//-------------------------------------------------------------------------
void OTableDesignUndoAct::Undo()
{
    m_pTabDgnCtrl->m_nCurUndoActId--;

    //////////////////////////////////////////////////////////////////////
    // Wenn erstes Undo zurueckgenommen wurde, ist Doc nicht modifiziert worden
    if( m_pTabDgnCtrl->m_nCurUndoActId == 0 )
    {
        m_pTabDgnCtrl->GetView()->getController()->setModified(sal_False);
        m_pTabDgnCtrl->GetView()->getController()->InvalidateFeature(SID_SAVEDOC);
    }
}

//-------------------------------------------------------------------------
void OTableDesignUndoAct::Redo()
{
    m_pTabDgnCtrl->m_nCurUndoActId++;

    //////////////////////////////////////////////////////////////////////
    // Wenn Redo fuer erste Undo-Action, muss Modified-Flag wieder gesetzt werden
    if( m_pTabDgnCtrl->m_nCurUndoActId > 0 )
    {
        m_pTabDgnCtrl->GetView()->getController()->setModified(sal_True);
        m_pTabDgnCtrl->GetView()->getController()->InvalidateFeature(SID_SAVEDOC);
    }
}
//==============================================================================
// class OTableDesignCellUndoAct
//==============================================================================
DBG_NAME(OTableDesignCellUndoAct);
OTableDesignCellUndoAct::OTableDesignCellUndoAct( OTableRowView* pOwner, long nRowID, USHORT nColumn ) :
     OTableDesignUndoAct( pOwner ,STR_TABED_UNDO_CELLMODIFIED)
    ,m_nCol( nColumn )
    ,m_nRow( nRowID )
{
    DBG_CTOR(OTableDesignCellUndoAct,NULL);
    //////////////////////////////////////////////////////////////////////
    // Text an der Position (m_nRow, m_nCol) auslesen
    m_sOldText = m_pTabDgnCtrl->GetCellData( m_nRow, m_nCol );
}

//-------------------------------------------------------------------------
OTableDesignCellUndoAct::~OTableDesignCellUndoAct()
{
    DBG_DTOR(OTableDesignCellUndoAct,NULL);
}

//-------------------------------------------------------------------------
void OTableDesignCellUndoAct::Undo()
{
    //////////////////////////////////////////////////////////////////////
    // Neuen Text der alten Zelle speichern und alten wieder einsetzen
    m_pTabDgnCtrl->ActivateCell( m_nRow, m_nCol );
    m_sNewText = m_pTabDgnCtrl->GetCellData( m_nRow, m_nCol );
    m_pTabDgnCtrl->SetCellData( m_nRow, m_nCol, m_sOldText );
    //////////////////////////////////////////////////////////////////////
    // Wenn erstes Undo zurueckgenommen wurde, ist Zelle nicht mehr modifiziert
    if (m_pTabDgnCtrl->GetCurUndoActId() == 1)
    {
        CellControllerRef xController = m_pTabDgnCtrl->Controller();
        if ( xController.Is() )
            xController->ClearModified();
        m_pTabDgnCtrl->GetView()->getController()->setModified(sal_False);

    }

    OTableDesignUndoAct::Undo();
}

//-------------------------------------------------------------------------
void OTableDesignCellUndoAct::Redo()
{
    //////////////////////////////////////////////////////////////////////
    // Neuen Text wieder einseten
    m_pTabDgnCtrl->ActivateCell( m_nRow, m_nCol );
    m_pTabDgnCtrl->SetCellData( m_nRow, m_nCol, m_sNewText );

    OTableDesignUndoAct::Redo();
}

//==============================================================================
// class OTableEditorUndoAct
//==============================================================================
DBG_NAME(OTableEditorUndoAct);
OTableEditorUndoAct::OTableEditorUndoAct( OTableEditorCtrl* pOwner,USHORT _nCommentID ) :
     OTableDesignUndoAct(  pOwner ,_nCommentID)
    ,pTabEdCtrl(pOwner)
{
    DBG_CTOR(OTableEditorUndoAct,NULL);
}

//-------------------------------------------------------------------------
OTableEditorUndoAct::~OTableEditorUndoAct()
{
    DBG_DTOR(OTableEditorUndoAct,NULL);
}

//==============================================================================
// class OTableEditorTypeSelUndoAct
//==============================================================================
DBG_NAME(OTableEditorTypeSelUndoAct);
OTableEditorTypeSelUndoAct::OTableEditorTypeSelUndoAct( OTableEditorCtrl* pOwner, long nRowID, USHORT nColumn, const TOTypeInfoSP& _pOldType )
    :OTableEditorUndoAct( pOwner ,STR_TABED_UNDO_TYPE_CHANGED)
    ,m_nCol( nColumn )
    ,m_nRow( nRowID )
    ,m_pOldType( _pOldType )
{
    DBG_CTOR(OTableEditorTypeSelUndoAct,NULL);
}

//-------------------------------------------------------------------------
OTableEditorTypeSelUndoAct::~OTableEditorTypeSelUndoAct()
{
    DBG_DTOR(OTableEditorTypeSelUndoAct,NULL);
}

//-------------------------------------------------------------------------
void OTableEditorTypeSelUndoAct::Undo()
{
    //////////////////////////////////////////////////////////////////////
    // Typ zuruecksetzen
    OFieldDescription* pFieldDesc = pTabEdCtrl->GetFieldDescr(m_nRow);
    if(pFieldDesc)
        m_pNewType = pFieldDesc->getTypeInfo();
    else
        m_pNewType = TOTypeInfoSP();
    pTabEdCtrl->SetCellData(m_nRow,m_nCol,m_pOldType);
    pTabEdCtrl->SwitchType( m_pOldType );

    OTableEditorUndoAct::Undo();
}

//-------------------------------------------------------------------------
void OTableEditorTypeSelUndoAct::Redo()
{
    //////////////////////////////////////////////////////////////////////
    // Neuen Typ
    pTabEdCtrl->GoToRowColumnId( m_nRow ,m_nCol);
    pTabEdCtrl->SetCellData(m_nRow,m_nCol,m_pNewType);

    OTableEditorUndoAct::Redo();
}

//==============================================================================
// class OTableEditorDelUndoAct
//==============================================================================
DBG_NAME(OTableEditorDelUndoAct);
OTableEditorDelUndoAct::OTableEditorDelUndoAct( OTableEditorCtrl* pOwner) :
     OTableEditorUndoAct( pOwner ,STR_TABED_UNDO_ROWDELETED)
{
    DBG_CTOR(OTableEditorDelUndoAct,NULL);
    //////////////////////////////////////////////////////////////////////
    // DeletedRowList fuellen
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pOriginalRows = pOwner->GetRowList();
    long nIndex = pOwner->FirstSelectedRow();
     ::boost::shared_ptr<OTableRow>  pOriginalRow;
     ::boost::shared_ptr<OTableRow>  pNewRow;

    while( nIndex >= 0 )
    {
        pOriginalRow = (*pOriginalRows)[nIndex];
        pNewRow.reset(new OTableRow( *pOriginalRow, nIndex ));
        m_aDeletedRows.push_back( pNewRow);

        nIndex = pOwner->NextSelectedRow();
    }
}

//-------------------------------------------------------------------------
OTableEditorDelUndoAct::~OTableEditorDelUndoAct()
{
    DBG_DTOR(OTableEditorDelUndoAct,NULL);
    m_aDeletedRows.clear();	
}

//-------------------------------------------------------------------------
void OTableEditorDelUndoAct::Undo()
{
    //////////////////////////////////////////////////////////////////////
    // Geloeschte Zeilen wieder einfuegen
    ULONG nPos;
    ::std::vector< ::boost::shared_ptr<OTableRow> >::iterator aIter = m_aDeletedRows.begin();
    
     ::boost::shared_ptr<OTableRow>  pNewOrigRow;
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pOriginalRows = pTabEdCtrl->GetRowList();

    for(;aIter != m_aDeletedRows.end();++aIter)
    {
        pNewOrigRow.reset(new OTableRow( **aIter ));
        nPos = (*aIter)->GetPos();
        pOriginalRows->insert( pOriginalRows->begin()+nPos,pNewOrigRow);
    }

    pTabEdCtrl->DisplayData(pTabEdCtrl->GetCurRow());
    pTabEdCtrl->Invalidate();
    OTableEditorUndoAct::Undo();
}

//-------------------------------------------------------------------------
void OTableEditorDelUndoAct::Redo()
{
    //////////////////////////////////////////////////////////////////////
    // Zeilen wieder loeschen
    ULONG nPos;
    ::std::vector< ::boost::shared_ptr<OTableRow> >::iterator aIter = m_aDeletedRows.begin();
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pOriginalRows = pTabEdCtrl->GetRowList();

    for(;aIter != m_aDeletedRows.end();++aIter)
    {
        nPos = (*aIter)->GetPos();
        pOriginalRows->erase( pOriginalRows->begin()+nPos );
    }

    pTabEdCtrl->DisplayData(pTabEdCtrl->GetCurRow());
    pTabEdCtrl->Invalidate();
    OTableEditorUndoAct::Redo();
}

//-------------------------------------------------------------------------
//==============================================================================
// class OTableEditorInsUndoAct
//==============================================================================
DBG_NAME(OTableEditorInsUndoAct);
OTableEditorInsUndoAct::OTableEditorInsUndoAct( OTableEditorCtrl* pOwner, 
                                               long nInsertPosition ,
                                               const ::std::vector<  ::boost::shared_ptr<OTableRow> >& _vInsertedRows) 
    :OTableEditorUndoAct( pOwner,STR_TABED_UNDO_ROWINSERTED )
    ,m_vInsertedRows(_vInsertedRows)
    ,m_nInsPos( nInsertPosition )
{
    DBG_CTOR(OTableEditorInsUndoAct,NULL);
}

//-------------------------------------------------------------------------
OTableEditorInsUndoAct::~OTableEditorInsUndoAct()
{
    DBG_DTOR(OTableEditorInsUndoAct,NULL);
    m_vInsertedRows.clear();	
}

//-------------------------------------------------------------------------
void OTableEditorInsUndoAct::Undo()
{
    //////////////////////////////////////////////////////////////////////
    // Eingefuegte Zeilen wieder loeschen
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pOriginalRows = pTabEdCtrl->GetRowList();
    for( long i=(m_nInsPos+m_vInsertedRows.size()-1); i>(m_nInsPos-1); i-- )
    {
        pOriginalRows->erase(pOriginalRows->begin()+i);
    }

    pTabEdCtrl->RowRemoved( m_nInsPos, m_vInsertedRows.size(), TRUE );
    pTabEdCtrl->InvalidateHandleColumn();

    OTableEditorUndoAct::Undo();
}

//-------------------------------------------------------------------------
void OTableEditorInsUndoAct::Redo()
{
    //////////////////////////////////////////////////////////////////////
    // Zeilen wieder einfuegen
    long nInsertRow = m_nInsPos;
     ::boost::shared_ptr<OTableRow>  pRow;
    ::std::vector< ::boost::shared_ptr<OTableRow> >::iterator aIter = m_vInsertedRows.begin();
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pRowList = pTabEdCtrl->GetRowList();
    for(;aIter != m_vInsertedRows.end();++aIter)
    {
        pRow.reset(new OTableRow( **aIter ));
        pRowList->insert( pRowList->begin()+nInsertRow ,pRow );
        nInsertRow++;
    }

    pTabEdCtrl->RowInserted( m_nInsPos, m_vInsertedRows.size(), TRUE );
    pTabEdCtrl->InvalidateHandleColumn();

    OTableEditorUndoAct::Redo();
}

//==============================================================================
// class OTableEditorInsNewUndoAct
//==============================================================================
DBG_NAME(OTableEditorInsNewUndoAct);
OTableEditorInsNewUndoAct::OTableEditorInsNewUndoAct( OTableEditorCtrl* pOwner, long nInsertPosition, long nInsertedRows ) :
     OTableEditorUndoAct( pOwner ,STR_TABED_UNDO_NEWROWINSERTED)
    ,m_nInsPos( nInsertPosition )
    ,m_nInsRows( nInsertedRows )
{
    DBG_CTOR(OTableEditorInsNewUndoAct,NULL);
}

//-------------------------------------------------------------------------
OTableEditorInsNewUndoAct::~OTableEditorInsNewUndoAct()
{
    DBG_DTOR(OTableEditorInsNewUndoAct,NULL);
}

//-------------------------------------------------------------------------
void OTableEditorInsNewUndoAct::Undo()
{
    //////////////////////////////////////////////////////////////////////
    // Eingefuegte Zeilen wieder loeschen
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pOriginalRows = pTabEdCtrl->GetRowList();

    for( long i=(m_nInsPos+m_nInsRows-1); i>(m_nInsPos-1); i-- )
    {
        pOriginalRows->erase(pOriginalRows->begin()+i);
    }

    pTabEdCtrl->RowRemoved( m_nInsPos, m_nInsRows, TRUE );
    pTabEdCtrl->InvalidateHandleColumn();

    OTableEditorUndoAct::Undo();
}

//-------------------------------------------------------------------------
void OTableEditorInsNewUndoAct::Redo()
{
    //////////////////////////////////////////////////////////////////////
    // Zeilen wieder einfuegen
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pRowList = pTabEdCtrl->GetRowList();

    for( long i=m_nInsPos; i<(m_nInsPos+m_nInsRows); i++ )
        pRowList->insert( pRowList->begin()+i,::boost::shared_ptr<OTableRow>(new OTableRow()) );

    pTabEdCtrl->RowInserted( m_nInsPos, m_nInsRows, TRUE );
    pTabEdCtrl->InvalidateHandleColumn();

    OTableEditorUndoAct::Redo();
}

//-------------------------------------------------------------------------
//========================================================================
// class OPrimKeyUndoAct
//========================================================================
DBG_NAME(OPrimKeyUndoAct);
//-------------------------------------------------------------------------
OPrimKeyUndoAct::OPrimKeyUndoAct( OTableEditorCtrl* pOwner, MultiSelection aDeletedKeys, MultiSelection aInsertedKeys) :
     OTableEditorUndoAct( pOwner ,STR_TABLEDESIGN_UNDO_PRIMKEY)
    ,m_aDelKeys( aDeletedKeys )
    ,m_aInsKeys( aInsertedKeys )
    ,m_pEditorCtrl( pOwner )
{
    DBG_CTOR(OPrimKeyUndoAct,NULL);
}

//-------------------------------------------------------------------------
OPrimKeyUndoAct::~OPrimKeyUndoAct()
{
    DBG_DTOR(OPrimKeyUndoAct,NULL);
}

//-------------------------------------------------------------------------
void OPrimKeyUndoAct::Undo()
{
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pRowList = pTabEdCtrl->GetRowList();
    ::boost::shared_ptr<OTableRow>  pRow;
    long nIndex;

    //////////////////////////////////////////////////////////////////////
    // Die eingefuegten Keys loeschen
    for( nIndex = m_aInsKeys.FirstSelected(); nIndex != (long)SFX_ENDOFSELECTION; nIndex=m_aInsKeys.NextSelected() )
    {
        OSL_ENSURE(nIndex <= static_cast<long>(pRowList->size()),"Index for undo isn't valid!");
        pRow = (*pRowList)[nIndex];
        pRow->SetPrimaryKey( FALSE );
    }

    //////////////////////////////////////////////////////////////////////
    // Die geloeschten Keys herstellen
    for( nIndex = m_aDelKeys.FirstSelected(); nIndex != (long)SFX_ENDOFSELECTION; nIndex=m_aDelKeys.NextSelected() )
    {
        OSL_ENSURE(nIndex <= static_cast<long>(pRowList->size()),"Index for undo isn't valid!");
        pRow = (*pRowList)[nIndex];
        pRow->SetPrimaryKey( TRUE );
    }

    m_pEditorCtrl->InvalidateHandleColumn();
    OTableEditorUndoAct::Undo();
}

//-------------------------------------------------------------------------
void OPrimKeyUndoAct::Redo()
{
    ::std::vector< ::boost::shared_ptr<OTableRow> >* pRowList = pTabEdCtrl->GetRowList();
    long nIndex;

    //////////////////////////////////////////////////////////////////////
    // Die geloeschten Keys loeschen
    for( nIndex = m_aDelKeys.FirstSelected(); nIndex != (long)SFX_ENDOFSELECTION; nIndex=m_aDelKeys.NextSelected() )
        (*pRowList)[nIndex]->SetPrimaryKey( FALSE );

    //////////////////////////////////////////////////////////////////////
    // Die eingefuegten Keys herstellen
    for( nIndex = m_aInsKeys.FirstSelected(); nIndex != (long)SFX_ENDOFSELECTION; nIndex=m_aInsKeys.NextSelected() )
        (*pRowList)[nIndex]->SetPrimaryKey( TRUE );

    m_pEditorCtrl->InvalidateHandleColumn();
    OTableEditorUndoAct::Redo();
}




