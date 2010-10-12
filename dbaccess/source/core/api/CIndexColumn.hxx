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

#ifndef DBACCESS_INDEXCOLUMN_HXX_
#define DBACCESS_INDEXCOLUMN_HXX_

#include "adabas/BColumn.hxx"

namespace connectivity
{
    namespace adabas
    {
        class OIndexColumn : public OAdabasColumn
        {
        protected:
            sal_Bool	m_IsAscending;
        public:
            OIndexColumn();
            OIndexColumn(	sal_Bool _IsAscending,
                            const ::rtl::OUString& _Name,
                            const ::rtl::OUString& _TypeName,
                            const ::rtl::OUString& _DefaultValue,
                            sal_Int32		_IsNullable,
                            sal_Int32		_Precision,
                            sal_Int32		_Scale,
                            sal_Int32		_Type,
                            sal_Bool		_IsAutoIncrement);

            virtual void construct();
        };
    }
}
#endif // DBACCESS_INDEXCOLUMN_HXX_
