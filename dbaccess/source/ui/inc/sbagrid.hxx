/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef _SBA_GRID_HXX
#define _SBA_GRID_HXX

#include <svx/fmgridcl.hxx>

#include <svx/fmgridif.hxx>

#include <com/sun/star/sdb/XSQLQueryComposer.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/util/URL.hpp>
#include <comphelper/uno3.hxx>
#include <comphelper/stl_types.hxx>
#include "sbamultiplex.hxx"
#include <svx/dataaccessdescriptor.hxx>
#include <queue>

class SvNumberFormatter;

namespace dbaui
{
    //------------------------------------------------------------------
    struct SbaURLCompare : public ::std::binary_function< ::com::sun::star::util::URL, ::com::sun::star::util::URL, bool>
    {
        bool operator() (const ::com::sun::star::util::URL& x, const ::com::sun::star::util::URL& y) const {return x.Complete == y.Complete ? true : false;}
    };

    struct SbaURLHash
    {
        sal_Int32 operator() (const ::com::sun::star::util::URL& x) const {return x.Complete.hashCode();}
    };
    //==================================================================
    // SbaXGridControl
    //==================================================================

    //	class ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > ;
    FORWARD_DECLARE_INTERFACE(lang,XMultiServiceFactory)

    class SbaXStatusMultiplexer;
    class SbaXGridControl
                :public FmXGridControl
                ,public ::com::sun::star::frame::XDispatch
    {
        DECLARE_STL_MAP(::com::sun::star::util::URL, SbaXStatusMultiplexer*, SbaURLCompare,StatusMultiplexerArray);
        StatusMultiplexerArray		m_aStatusMultiplexer;

    public:
        SbaXGridControl(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&);
        virtual ~SbaXGridControl();

        // UNO
        DECLARE_UNO3_DEFAULTS(SbaXGridControl, FmXGridControl);
        virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType) throw (::com::sun::star::uno::RuntimeException);

        // XTypeProvider
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  ) throw (::com::sun::star::uno::RuntimeException);

        // ::com::sun::star::lang::XServiceInfo
        ::rtl::OUString SAL_CALL getImplementationName() throw();
        virtual ::comphelper::StringSequence SAL_CALL getSupportedServiceNames() throw();
        // need by registration
        static ::rtl::OUString getImplementationName_Static() throw( ::com::sun::star::uno::RuntimeException );
        static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void) throw( ::com::sun::star::uno::RuntimeException );
        static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                SAL_CALL Create(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&);

        // ::com::sun::star::frame::XDispatch
        virtual void SAL_CALL dispatch(const ::com::sun::star::util::URL& aURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL addStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl, const ::com::sun::star::util::URL& aURL) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl, const ::com::sun::star::util::URL& aURL) throw( ::com::sun::star::uno::RuntimeException );

        // ::com::sun::star::lang::XComponent
        virtual void SAL_CALL dispose(void) throw( ::com::sun::star::uno::RuntimeException );

        virtual void SAL_CALL createPeer(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit > & rToolkit, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer > & rParentPeer) throw( ::com::sun::star::uno::RuntimeException );

    protected:
        virtual FmXGridPeer*	imp_CreatePeer(Window* pParent);
    };

    //==================================================================
    // SbaXGridPeer
    //==================================================================

    class SbaXGridPeer
                :public FmXGridPeer
                ,public ::com::sun::star::frame::XDispatch
    {
        ::cppu::OMultiTypeInterfaceContainerHelperVar< ::com::sun::star::util::URL,SbaURLHash , SbaURLCompare>	m_aStatusListeners;

    public:
        SbaXGridPeer(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&);
        virtual ~SbaXGridPeer();

        // UNO
        virtual void SAL_CALL  acquire() throw() { FmXGridPeer::acquire(); }
        virtual void SAL_CALL release() throw() { FmXGridPeer::release(); }
        virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType) throw (::com::sun::star::uno::RuntimeException);

        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException);

        static const ::com::sun::star::uno::Sequence< sal_Int8 >& getUnoTunnelId();
        sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 > & rId ) throw (::com::sun::star::uno::RuntimeException);
        static SbaXGridPeer* getImplementation(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>& _rxIFace);

        // ::com::sun::star::frame::XDispatch
        virtual void SAL_CALL dispatch(const ::com::sun::star::util::URL& aURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArgs) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL addStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl, const ::com::sun::star::util::URL& aURL) throw( ::com::sun::star::uno::RuntimeException );
        virtual void SAL_CALL removeStatusListener(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl, const ::com::sun::star::util::URL& aURL) throw( ::com::sun::star::uno::RuntimeException );

        // ::com::sun::star::frame::XDispatchProvider
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  SAL_CALL queryDispatch(const ::com::sun::star::util::URL& aURL, const ::rtl::OUString& aTargetFrameName, sal_Int32 nSearchFlags) throw( ::com::sun::star::uno::RuntimeException );

        // ::com::sun::star::lang::XComponent
        virtual void SAL_CALL dispose(void) throw( ::com::sun::star::uno::RuntimeException );

    protected:
        virtual FmGridControl*	imp_CreateControl(Window* pParent, WinBits nStyle);
#if defined(_MSC_VER) && (_MSC_VER >= 1310 )
        typedef ::com::sun::star::frame::XStatusListener xstlist_type;
        typedef ::com::sun::star::uno::Reference< xstlist_type > xlistener_type;
        void NotifyStatusChanged(const ::com::sun::star::util::URL& aUrl, const xlistener_type & xControl = xlistener_type() );
#else
        void NotifyStatusChanged(const ::com::sun::star::util::URL& aUrl, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xControl = ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > ());
#endif // # _MSC_VER>=1310

    private:
        // for asny execution of XDispatch::dispatch
        struct DispatchArgs
        {
            ::com::sun::star::util::URL													aURL;
            ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >	aArgs;
        };
        ::std::queue< DispatchArgs >	m_aDispatchArgs;
        DECL_LINK( OnDispatchEvent, void* );

        // for dynamic states of our 4 dispatchable URLs
        enum DispatchType
        {
            dtBrowserAttribs,
            dtRowHeight,
            dtColumnAttribs,
            dtColumnWidth,

            dtUnknown
        };
        DispatchType classifyDispatchURL( const ::com::sun::star::util::URL& _rURL );

        DECLARE_STL_STDKEY_MAP( DispatchType, sal_Bool, MapDispatchToBool );
        MapDispatchToBool	m_aDispatchStates;
    };

    //==================================================================
    // SbaGridHeader
    //==================================================================

    class SbaGridHeader
                :public FmGridHeader
                ,public DragSourceHelper
    {
    public:
        SbaGridHeader(BrowseBox* pParent, WinBits nWinBits = WB_STDHEADERBAR | WB_DRAG);

    protected:

        // FmGridHeader overridables
        virtual void	PreExecuteColumnContextMenu(sal_uInt16 nColId, PopupMenu& rMenu);
        virtual	void	PostExecuteColumnContextMenu(sal_uInt16 nColId, const PopupMenu& rMenu, sal_uInt16 nExecutionResult);

    private:
        // DragSourceHelper overridables
        virtual void StartDrag( sal_Int8 _nAction, const Point& _rPosPixel );

        // Window overridables
        virtual void MouseButtonDown( const MouseEvent& rMEvt );

        sal_Bool ImplStartColumnDrag(sal_Int8 _nAction, const Point& _rMousePos);

    private:
        using FmGridHeader::StartDrag;
    };

    // =========================================================================
    // interfaces for communication between the vcl grid control and a controller
    // -------------------------------------------------------------------------
    class SbaGridListener
    {
    public:
        virtual void RowChanged()		= 0;
        virtual void ColumnChanged()	= 0;
        virtual void SelectionChanged()	= 0;
        virtual void CellActivated()	= 0;
        virtual void CellDeactivated()	= 0;
        virtual void BeforeDrop()		= 0;
        virtual void AfterDrop()		= 0;
    };

    //==================================================================
    // SbaGridControl
    //==================================================================
    //------------------------------------------------------------------
    class UnoDataBrowserController;
    class SbaGridControl : public FmGridControl
    {
        friend class SbaGridHeader;
        friend class SbaXGridPeer;

    // Attributes
    protected:
        ::svx::ODataAccessDescriptor	m_aDataDescriptor;
        SbaGridListener*	            m_pMasterListener;

        sal_Int32			            m_nAsyncDropEvent;

        sal_uInt16                          m_nCurrentActionColId;
            // ui actions (e.g. a context menu) may be performed on columns which aren't the current one
            // and aren't selected, so we have to track this column id

        sal_Bool	                    m_bActivatingForDrop;
    // Attribute Access
    public:
        sal_uInt16	GetCurrentActionColumn() const	{ return m_nCurrentActionColId; }

    public:
        SbaGridControl(::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >,Window* pParent, FmXGridPeer* _pPeer, WinBits nBits = WB_TABSTOP);
        virtual ~SbaGridControl();

        virtual void Command( const CommandEvent& rCEvt );
        virtual void Select();

        void SetMasterListener(SbaGridListener* pListener)	{ m_pMasterListener = pListener; }

        virtual void ActivateCell(long nRow, sal_uInt16	nCol, sal_Bool bSetCellFocus = sal_True);
        virtual void DeactivateCell(sal_Bool bUpdate = sal_True);
        void ActivateCell() { FmGridControl::ActivateCell(); }

        sal_Bool IsAllSelected() const { return (GetSelectRowCount() == GetRowCount()) && (GetRowCount() > 0); }

        HeaderBar* GetHeaderBar() const { return FmGridControl::GetHeaderBar(); }

        /** return the description of the specified object.
            @param	eObjType
                The type to ask for
            @param	_nPosition
                The position of a tablecell (index position), header bar  colum/row cell
            @return
                The description of the specified object.
        */
        virtual ::rtl::OUString GetAccessibleObjectDescription( ::svt::AccessibleBrowseBoxObjType eObjType,sal_Int32 _nPosition = -1) const;

        virtual void DeleteSelectedRows();
        /** copies the currently selected rows to the clipboard
            @precond
                at least one row is selected
        */
        void CopySelectedRowsToClipboard();


    protected:
        // DragSourceHelper overridables
        virtual void StartDrag( sal_Int8 _nAction, const Point& _rPosPixel );

        // BrowseBox overridables
        virtual void	CursorMoved();
        virtual sal_Int8 AcceptDrop( const BrowserAcceptDropEvent& rEvt );
        virtual sal_Int8 ExecuteDrop( const BrowserExecuteDropEvent& rEvt );
        virtual void	MouseButtonDown( const BrowserMouseEvent& rMEvt);

        // EditBrowseBox overridables
        virtual BrowserHeader* imp_CreateHeaderBar(BrowseBox* pParent);
        virtual ::svt::CellController* GetController(long nRow, sal_uInt16 nCol);

        // DbGridControl overridables
        virtual void PreExecuteRowContextMenu(sal_uInt16 nRow, PopupMenu& rMenu);
        virtual void PostExecuteRowContextMenu(sal_uInt16 nRow, const PopupMenu& rMenu, sal_uInt16 nExecutionResult);

        // DbGridControl overridables
        virtual void onRowChange();
        virtual void onColumnChange();

        // my own overridables
        virtual void BeforeDrop();
        virtual void AfterDrop();

        // get a fields property set from a model pos
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  getField(sal_uInt16 nModelPos);

        // get my data source
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  getDataSource() const;

        // drag events
        virtual void DoColumnDrag(sal_uInt16 nColumnPos);
        virtual void DoRowDrag(sal_Int16 nRowPos);
        virtual void DoFieldDrag(sal_uInt16 nColumnPos, sal_Int16 nRowPos);

        void SetBrowserAttrs();
        void SetColWidth(sal_uInt16 nColId);
        void SetRowHeight();
        void SetColAttrs(sal_uInt16 nColId);

        SvNumberFormatter* GetDatasourceFormatter();

        DECL_LINK(AsynchDropEvent, void*);

    private:
        sal_Bool	IsReadOnlyDB() const;
        void implTransferSelectedRows( sal_Int16 nRowPos, bool _bTrueIfClipboardFalseIfDrag );

    private:
        using FmGridControl::AcceptDrop;
        using FmGridControl::ExecuteDrop;
        using FmGridControl::MouseButtonDown;
    };
}
#endif // _SBA_GRID_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
