/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: core_resource.hxx,v $
 * $Revision: 1.3 $
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
#ifndef REPORTDESIGN_CORE_RESOURCE_HXX_
#define REPORTDESIGN_CORE_RESOURCE_HXX_


#include <rtl/ustring.hxx>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

class SimpleResMgr;
//.........................................................................
namespace reportdesign
{

#define RPT_RESSTRING(id,_rM) ResourceManager::loadString(id,_rM)

    //==================================================================
    //= ResourceManager
    //= handling ressources within the DBA-Core library
    //==================================================================
    class ResourceManager
    {
        static SimpleResMgr*	m_pImpl;

    private:
        // no instantiation allowed
        ResourceManager() { }
        ~ResourceManager() { }

        // we'll instantiate one static member of the following class, which, in it's dtor,
        // ensures that m_pImpl will be deleted
        class EnsureDelete
        {
        public:
            EnsureDelete() { }
            ~EnsureDelete();
        };
        friend class EnsureDelete;

    protected:
        static void ensureImplExists(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiComponentFactory >& _rM);

    public:
        /** loads the string with the specified resource id from the FormLayer resource file
        */
        static ::rtl::OUString loadString(sal_uInt16 _nResId,const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiComponentFactory >& _rM);
    };

//.........................................................................
}
//.........................................................................

#endif // REPORTDESIGN_CORE_RESOURCE_HXX_

