/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: dbadmin.hxx,v $
 * $Revision: 1.36.68.1 $
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

#ifndef _DBAUI_DBADMIN_HXX_
#define _DBAUI_DBADMIN_HXX_

#ifndef _SFXTABDLG_HXX
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _DBAUI_DSNTYPES_HXX_
#include "dsntypes.hxx"
#endif
#ifndef DBAUI_ITEMSETHELPER_HXX
#include "IItemSetHelper.hxx"
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#include <memory>

FORWARD_DECLARE_INTERFACE(beans,XPropertySet)
FORWARD_DECLARE_INTERFACE(sdbc,XConnection)
FORWARD_DECLARE_INTERFACE(lang,XMultiServiceFactory)

//.........................................................................
namespace dbaui
{
//.........................................................................

//=========================================================================
//= ODbAdminDialog
//=========================================================================
class OGeneralPage;
class ODbDataSourceAdministrationHelper;
/** tab dialog for administrating the office wide registered data sources
*/
class ODbAdminDialog : public SfxTabDialog , public IItemSetHelper, public IDatabaseSettingsDialog
{
private:
    typedef ::std::stack< sal_Int32 > PageStack;
    PageStack				m_aCurrentDetailPages;	// ids of all currently enabled (type-dependent) detail pages

    ::std::auto_ptr<ODbDataSourceAdministrationHelper>	m_pImpl;

    sal_Bool				m_bResetting : 1;	/// sal_True while we're resetting the pages
    sal_Bool				m_bApplied : 1;		/// sal_True if any changes have been applied while the dialog was executing
    sal_Bool				m_bUIEnabled : 1;	/// <TRUE/> if the UI is enabled, false otherwise. Cannot be switched back to <TRUE/>, once it is <FALSE/>

public:
    /** ctor. The itemset given should have been created by <method>createItemSet</method> and should be destroyed
        after the dialog has been destroyed
    */
    ODbAdminDialog(Window* pParent,
        SfxItemSet* _pItems,
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
        );
    virtual ~ODbAdminDialog();

    /** create and return an item set for use with the dialog.
        @param		_pTypeCollection		pointer to an <type>ODatasourceMap</type>. May be NULL, in this case
                                            the pool will not contain a typecollection default.
    */
    static SfxItemSet*	createItemSet(SfxItemSet*& _rpSet, SfxItemPool*& _rpPool, SfxPoolItem**& _rppDefaults, ::dbaccess::ODsnTypeCollection* _pTypeCollection);
    /** destroy and item set / item pool / pool defaults previously created by <method>createItemSet</method>
    */
    static void			destroyItemSet(SfxItemSet*& _rpSet, SfxItemPool*& _rpPool, SfxPoolItem**& _rppDefaults);

    /** selects the DataSource
        @param	_rName
            The name of the data source
    */
    void selectDataSource(const ::com::sun::star::uno::Any& _aDataSourceName);

    virtual const SfxItemSet* getOutputSet() const;
    virtual SfxItemSet* getWriteOutputSet();

    // forwards to ODbDataSourceAdministrationHelper
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > getORB() const;
    virtual ::std::pair< ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >,sal_Bool> createConnection();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDriver >	getDriver();
    virtual ::dbaccess::DATASOURCE_TYPE getDatasourceType(const SfxItemSet& _rSet) const;
    virtual void clearPassword();
    virtual sal_Bool saveDatasource();
    virtual void setTitle(const ::rtl::OUString& _sTitle);
    virtual void enableConfirmSettings( bool _bEnable );

protected:
    // adds a new detail page and remove all the old ones
    void addDetailPage(USHORT _nPageId,USHORT _nTextId,CreateTabPage pCreateFunc);
    // removes all detail pages
    void removeDetailPages();

    virtual void PageCreated(USHORT _nId, SfxTabPage& _rPage);
    virtual short Ok();

protected:
    inline sal_Bool	isUIEnabled() const { return m_bUIEnabled; }
    inline void		disabledUI() { m_bUIEnabled = sal_False; }

    /// select a datasource with a given name, adjust the item set accordingly, and everything like that ..
    void implSelectDatasource(const ::com::sun::star::uno::Any& _aDataSourceName);
    /// reset the tag pages according to m_sCurrentDatasource and <arg>_rxDatasource</arg>
    void resetPages(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxDatasource);

    enum ApplyResult
    {
        AR_LEAVE_MODIFIED,		// somthing was modified and has successfully been committed
        AR_LEAVE_UNCHANGED,		// no changes were made
        AR_KEEP					// don't leave the page (e.g. because an error occured)
    };
    /** apply all changes made
    */
    ApplyResult	implApplyChanges();
};

//.........................................................................
}	// namespace dbaui
//.........................................................................

#endif // _DBAUI_DBADMIN_HXX_

