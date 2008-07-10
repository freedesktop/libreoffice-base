/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: xmlFormatCondition.hxx,v $
 * $Revision: 1.5 $
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
#ifndef RPT_XMLFORMATCONDITION_HXX
#define RPT_XMLFORMATCONDITION_HXX

#include <xmloff/xmlictxt.hxx>
#include <com/sun/star/report/XFormatCondition.hpp>

namespace rptxml
{
    class ORptFilter;
    class OXMLFormatCondition : public SvXMLImportContext
    {
        ORptFilter& m_rImport;
        ::rtl::OUString m_sStyleName;
        ::com::sun::star::uno::Reference< ::com::sun::star::report::XFormatCondition >	m_xComponent;
        OXMLFormatCondition(const OXMLFormatCondition&);
        void operator =(const OXMLFormatCondition&);
    public:

        OXMLFormatCondition( ORptFilter& rImport, sal_uInt16 nPrfx,
                    const ::rtl::OUString& rLName,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList
                    ,const ::com::sun::star::uno::Reference< ::com::sun::star::report::XFormatCondition >& _xComponent
                    );
        virtual ~OXMLFormatCondition();
        virtual void EndElement();
    };
// -----------------------------------------------------------------------------
} // namespace rptxml
// -----------------------------------------------------------------------------

#endif // RPT_XMLFORMATTEDFIELD_HXX
