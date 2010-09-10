/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: OptimisticSet.cxx,v $
 * $Revision: 1.73 $
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
#ifndef PRIVATE_ROW_HXX
#define PRIVATE_ROW_HXX

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/sdbc/XRow.hpp>
#include "RowSetRow.hxx"

namespace dbaccess
{
    class OPrivateRow : public ::cppu::WeakImplHelper1< ::com::sun::star::sdbc::XRow>
    {
        ORowSetValueVector::Vector m_aRow;
        sal_Int32 m_nPos;
    public:
        OPrivateRow(const ORowSetValueVector::Vector& i_aRow) : m_aRow(i_aRow),m_nPos(0)
        {
        }
        virtual ::sal_Bool SAL_CALL wasNull(  ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getString( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::sal_Bool SAL_CALL getBoolean( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int8 SAL_CALL getByte( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int16 SAL_CALL getShort( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int32 SAL_CALL getInt( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::sal_Int64 SAL_CALL getLong( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual float SAL_CALL getFloat( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual double SAL_CALL getDouble( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL getBytes( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::util::Date SAL_CALL getDate( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::util::Time SAL_CALL getTime( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::util::DateTime SAL_CALL getTimestamp( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getBinaryStream( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL getCharacterStream( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any SAL_CALL getObject( ::sal_Int32 columnIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& typeMap ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRef > SAL_CALL getRef( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob > SAL_CALL getBlob( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XClob > SAL_CALL getClob( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XArray > SAL_CALL getArray( ::sal_Int32 columnIndex ) throw (::com::sun::star::sdbc::SQLException, ::com::sun::star::uno::RuntimeException);
    };
} // dbaccess
#endif // PRIVATE_ROW_HXX
