 #########################################################################
 #
 #  $RCSfile: makefile.mk,v $
 #
 #  $Revision: 1.5 $
 #
 #  last change: $Author: fs $ $Date: 2000-10-26 18:07:11 $
 #
 #  The Contents of this file are made available subject to the terms of
 #  either of the following licenses
 #
 #         - GNU Lesser General Public License Version 2.1
 #         - Sun Industry Standards Source License Version 1.1
 #
 #  Sun Microsystems Inc., October, 2000
 #
 #  GNU Lesser General Public License Version 2.1
 #  =============================================
 #  Copyright 2000 by Sun Microsystems, Inc.
 #  901 San Antonio Road, Palo Alto, CA 94303, USA
 #
 #  This library is free software; you can redistribute it and/or
 #  modify it under the terms of the GNU Lesser General Public
 #  License version 2.1, as published by the Free Software Foundation.
 #
 #  This library is distributed in the hope that it will be useful,
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 #  Lesser General Public License for more details.
 #
 #  You should have received a copy of the GNU Lesser General Public
 #  License along with this library; if not, write to the Free Software
 #  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 #  MA  02111-1307  USA
 #
 #
 #  Sun Industry Standards Source License Version 1.1
 #  =================================================
 #  The contents of this file are subject to the Sun Industry Standards
 #  Source License Version 1.1 (the "License"); You may not use this file
 #  except in compliance with the License. You may obtain a copy of the
 #  License at http://www.openoffice.org/license.html.
 #
 #  Software provided under this License is provided on an "AS IS" basis,
 #  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
 #  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 #  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 #  See the License for the specific provisions governing your rights and
 #  obligations concerning the Software.
 #
 #  The Initial Developer of the Original Code is: Sun Microsystems, Inc..
 #
 #  Copyright: 2000 by Sun Microsystems, Inc.
 #
 #  All Rights Reserved.
 #
 #  Contributor(s): _______________________________________
 #
 #
 ########################################################################/

PRJ=..$/..$/..
PRJINC=$(PRJ)$/source
PRJNAME=dbaccess
TARGET=uidlg

ENABLE_EXCEPTIONS=TRUE

# --- Settings ----------------------------------

.INCLUDE : settings.mk

# --- Files -------------------------------------

# ... resource files ............................

BMP_IN=$(PRJ)$/res

SRCFILES =	\
        paramdialog.src	\
        dsselect.src	\
        dbadmin.src	\
        dbfindex.src	\
        sqlmessage.src

# ... object files ............................

SLOFILES=	\
        $(SLO)$/paramdialog.obj	\
        $(SLO)$/odbcconfig.obj	\
        $(SLO)$/dsselect.obj	\
        $(SLO)$/dbadmin.obj	\
        $(SLO)$/adminpages.obj	\
        $(SLO)$/sqlmessage.obj	\
        $(SLO)$/dbfindex.obj

# --- Targets ----------------------------------

.INCLUDE : target.mk


 #########################################################################
 # history:
 #	$Log: not supported by cvs2svn $
 #	Revision 1.4  2000/10/25 13:01:00  fs
 #	moved three files to ..\uno
 #	
 #	Revision 1.3  2000/10/24 13:03:00  fs
 #	some new file - added functionality for browsing system data sources
 #	
 #	Revision 1.2  2000/10/09 12:39:29  fs
 #	some (a lot of) new imlpementations - still under development
 #	
 #	Revision 1.1  2000/10/05 10:06:31  fs
 #	initial checkin
 #	
 #
 #	Revision 1.0 20.09.00 10:58:55  fs
 ########################################################################/

