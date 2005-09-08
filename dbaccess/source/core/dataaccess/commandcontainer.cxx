/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: commandcontainer.cxx,v $
 *
 *  $Revision: 1.11 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 10:43:21 $
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

#ifndef _DBA_COREDATAACCESS_COMMANDCONTAINER_HXX_
#include "commandcontainer.hxx"
#endif
#ifndef _DBA_COREDATAACESS_COMMANDDEFINITION_HXX_
#include "commanddefinition.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef DBACCESS_SHARED_DBASTRINGS_HRC
#include "dbastrings.hrc"
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::ucb;
using namespace ::osl;
using namespace ::comphelper;
using namespace ::cppu;

//........................................................................
namespace dbaccess
{
//........................................................................

//==========================================================================
//= OCommandContainer
//==========================================================================
DBG_NAME(OCommandContainer)
//--------------------------------------------------------------------------
OCommandContainer::OCommandContainer( const Reference< ::com::sun::star::lang::XMultiServiceFactory >& _xORB
                                     ,const Reference< XInterface >&	_xParentContainer
                                     ,const TContentPtr& _pImpl
                                     ,sal_Bool _bTables
                                     )									 
    :ODefinitionContainer(_xORB,_xParentContainer,_pImpl)
    ,m_bTables(_bTables)
{
    DBG_CTOR(OCommandContainer, NULL);
}
//--------------------------------------------------------------------------
OCommandContainer::~OCommandContainer()
{
    DBG_DTOR(OCommandContainer, NULL);
}
// -----------------------------------------------------------------------------
IMPLEMENT_FORWARD_XINTERFACE2( OCommandContainer,ODefinitionContainer,OCommandContainer_BASE)
IMPLEMENT_TYPEPROVIDER2(OCommandContainer,ODefinitionContainer,OCommandContainer_BASE);
//--------------------------------------------------------------------------
Reference< XContent > OCommandContainer::createObject( const ::rtl::OUString& _rName)
{
    ODefinitionContainer_Impl* pItem = static_cast<ODefinitionContainer_Impl*>(m_pImpl.get());
    OSL_ENSURE(pItem->m_aDocumentMap.find(_rName) != pItem->m_aDocumentMap.end() ," Invalid entry in map!");
    if ( m_bTables )
        return new OComponentDefinition(*this, _rName,m_xORB,pItem->m_aDocumentMap.find(_rName)->second,m_bTables);
    return new OCommandDefinition(*this, _rName,m_xORB,pItem->m_aDocumentMap.find(_rName)->second);
}
// -----------------------------------------------------------------------------
Reference< XInterface > SAL_CALL OCommandContainer::createInstanceWithArguments(const Sequence< Any >& aArguments ) throw (Exception, RuntimeException)
{
    return createInstance( );
}
// -----------------------------------------------------------------------------
Reference< XInterface > SAL_CALL OCommandContainer::createInstance( ) throw (Exception, RuntimeException)
{
    return m_xORB->createInstance(m_bTables ? SERVICE_SDB_TABLEDEFINITION : SERVICE_SDB_COMMAND_DEFINITION);
}
//........................................................................
}	// namespace dbaccess
//........................................................................

