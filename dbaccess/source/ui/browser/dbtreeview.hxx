/*************************************************************************
 *
 *  $RCSfile: dbtreeview.hxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: oj $ $Date: 2002-05-02 07:14:29 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the License); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an AS IS basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef DBACCESS_UI_DBTREEVIEW_HXX
#define DBACCESS_UI_DBTREEVIEW_HXX

#ifndef _SV_WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

class SvTreeListBox;
namespace dbaui
{
    class DBTreeListBox;
    class DBTreeListModel;
    // ------------------
    // - DBTreeView -
    // ------------------

    class DBTreeView : public Window
    {
    private:
        DBTreeListBox*	m_pTreeListBox;
    protected:
        // window overridables
        virtual void Resize();
    public:						
            
        DBTreeView( Window* pParent,
                const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB,
                WinBits nBits );
        ~DBTreeView();

        /** sets a handler which is called when an list box entry is to be expanded.
            <p>When calling the link, the parameter is an SvLBoxEntry marking the entry to be expanded.
            </p>
        */
        void	SetPreExpandHandler(const Link& _rHdl);
        /// gets the currently set NodeExpansionHandler
        Link	GetPreExpandHandler() const;

        void	setCutHandler(const Link& _rHdl);
        Link	getCutHandler() const;
        void	setCopyHandler(const Link& _rHdl);
        Link	getCopyHandler() const;
        void	setPasteHandler(const Link& _rHdl);
        Link	getPasteHandler() const;
        void	setDeleteHandler(const Link& _rHdl);
        Link	getDeleteHandler() const;
        void	setEditingHandler(const Link& _rHdl);
        Link	getEditingHandler() const;
        void	setEditedHandler(const Link& _rHdl);
        Link	getEditedHandler() const;


        DBTreeListModel*	getModel() const;
        void				setModel(DBTreeListModel* _pTreeModel);
        void				setSelectHdl(const Link& _rHdl);

        DBTreeListBox*		getListBox() const;

        virtual void GetFocus();
    };
}

#endif // DBACCESS_UI_DBTREEVIEW_HXX

