/*************************************************************************
 *
 *  $RCSfile: marktree.hxx,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: oj $ $Date: 2002-04-29 08:13:00 $
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
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _DBAUI_MARKTREE_HXX_
#define _DBAUI_MARKTREE_HXX_

#ifndef _SVTREEBOX_HXX
#include <svtools/svtreebx.hxx>
#endif

//.........................................................................
namespace dbaui
{
//.........................................................................

//========================================================================
//= OMarkableTreeListBox
//========================================================================
/** a tree list box where all entries can be marked (with a checkbox) and
    unmarked. In addition, inner nodes know a third state which applies
    if some, but not all of their descendants are marked.
*/
class OMarkableTreeListBox : public SvTreeListBox
{
    SvLBoxButtonData*	m_pCheckButton;
    Link				m_aCheckButtonHandler;

public:
    OMarkableTreeListBox( Window* pParent, WinBits nWinStyle=0 );
    OMarkableTreeListBox( Window* pParent, const ResId& rResId );
    ~OMarkableTreeListBox();

    virtual void    KeyInput( const KeyEvent& rKEvt );
    virtual void	CheckButtonHdl();
    void			CheckButtons();		// make the button states consistent (bottom-up)
    SvLBoxEntry*	GetEntryPosByName(const String& aName,SvLBoxEntry* pStart=NULL) const;

    /// the handler given is called whenever the check state of one or more items changed
    void SetCheckHandler(const Link& _rHdl) { m_aCheckButtonHandler = _rHdl; }

    /** call when HiContrast change. 
    */
    virtual void notifyHiContrastChanged();

protected:
    virtual void Paint(const Rectangle& _rRect);
    virtual void checkedButton_noBroadcast(SvLBoxEntry* _pEntry);

    SvButtonState	implDetermineState(SvLBoxEntry* _pEntry);
        // determines the check state of the given entry, by analyzing the states of all descendants

private:
    void InitButtonData();
};

//.........................................................................
}	// namespace dbaui
//.........................................................................

#endif // _DBAUI_MARKTREE_HXX_

/*************************************************************************
 * history:
 *	$Log: not supported by cvs2svn $
 *	Revision 1.4  2001/04/27 08:09:23  fs
 *	+implDeterminedState - needed to correctly implement CheckButtons
 *	
 *	Revision 1.3  2001/01/30 08:28:00  fs
 *	+checkedButton_noBroadcast
 *	
 *	Revision 1.2  2000/10/09 12:41:31  fs
 *	use a different font when painting a disabled control
 *	
 *	Revision 1.1  2000/10/05 10:08:43  fs
 *	initial checkin
 *	
 *
 *	Revision 1.0 28.09.00 13:19:31  fs
 ************************************************************************/

