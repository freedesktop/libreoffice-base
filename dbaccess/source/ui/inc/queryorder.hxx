/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: queryorder.hxx,v $
 *
 *  $Revision: 1.8 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 16:01:35 $
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
#ifndef DBAUI_QUERYORDER_HXX
#define DBAUI_QUERYORDER_HXX

#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif

#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif

#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif

#define DOG_ROWS	3

namespace rtl
{
    class OUString;
}
namespace com
{
    namespace sun
    {
        namespace star
        {
            namespace sdb
            {
                class XSingleSelectQueryComposer;
            }
            namespace sdbc
            {
                class XConnection;
            }
            namespace container
            {
                class XNameAccess;
            }
            namespace beans
            {
                struct PropertyValue;
                class XPropertySet;
            }
        }
    }
}

//==================================================================
// DlgOrderCrit
//==================================================================
namespace dbaui
{
    class DlgOrderCrit : public ModalDialog
    {
    protected:
        ListBox			aLB_ORDERFIELD1;
        ListBox			aLB_ORDERVALUE1;
        ListBox			aLB_ORDERFIELD2;
        ListBox			aLB_ORDERVALUE2;
        ListBox			aLB_ORDERFIELD3;
        ListBox			aLB_ORDERVALUE3;
        FixedText		aFT_ORDERFIELD;
        FixedText		aFT_ORDERAFTER1;
        FixedText		aFT_ORDERAFTER2;
        FixedText		aFT_ORDEROPER;
        FixedText		aFT_ORDERDIR;
        OKButton		aBT_OK;
        CancelButton	aBT_CANCEL;
        HelpButton		aBT_HELP;
        FixedLine       aFL_ORDER;
        String			aSTR_NOENTRY;
        ::rtl::OUString	m_sOrgOrder;

        ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSingleSelectQueryComposer> m_xQueryComposer;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess>	m_xColumns;
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection>		m_xConnection;


        ListBox*		arrLbFields[DOG_ROWS];
        ListBox*		arrLbValues[DOG_ROWS];

        DECL_LINK( FieldListSelectHdl, ListBox * );
        void			EnableLines();

    public:
        DlgOrderCrit(	Window * pParent,
                        const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection>& _rxConnection,
                        const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSingleSelectQueryComposer>& _rxComposer,
                        const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess>& _rxCols);

                        ~DlgOrderCrit();
        void			BuildOrderPart();

        ::rtl::OUString	GetOrderList( ) const;
        void			SetOrderList( const String& _rOrderList );
        ::rtl::OUString	GetOrignalOrder() const { return m_sOrgOrder; }
    };
}
#endif // DBAUI_QUERYORDER_HXX


