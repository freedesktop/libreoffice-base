/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: CIndexes.hxx,v $
 * $Revision: 1.10 $
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

#ifndef DBACCESS_INDEXES_HXX_
#define DBACCESS_INDEXES_HXX_

#ifndef CONNECTIVITY_INDEXESHELPER_HXX
#include "connectivity/TIndexes.hxx"
#endif

namespace dbaccess
{
    class OIndexes : public connectivity::OIndexesHelper
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > m_xIndexes;
    protected:
        virtual connectivity::sdbcx::ObjectType createObject(const ::rtl::OUString& _rName);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createDescriptor();
        virtual connectivity::sdbcx::ObjectType appendObject( const ::rtl::OUString& _rForName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor );
        virtual void dropObject(sal_Int32 _nPos,const ::rtl::OUString _sElementName);
    public:
        OIndexes(connectivity::OTableHelper* _pTable,
                 ::osl::Mutex& _rMutex,
                 const ::std::vector< ::rtl::OUString> &_rVector,
                 const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& _rxIndexes
                 ) : connectivity::OIndexesHelper(_pTable,_rMutex,_rVector)
            ,m_xIndexes(_rxIndexes)
        {}

        virtual void SAL_CALL disposing(void);
    };
}

#endif // DBACCESS_INDEXES_HXX_
