/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: DbAdminImpl.cxx,v $
 * $Revision: 1.26.18.1 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_dbaccess.hxx"

#include "DbAdminImpl.hxx"
#include "dsmeta.hxx"

#ifndef _SFXPOOLITEM_HXX
#include <svtools/poolitem.hxx>
#endif
#ifndef _SFXITEMPOOL_HXX
#include <svtools/itempool.hxx>
#endif
#ifndef _SFXSTRITEM_HXX
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXINTITEM_HXX
#include <svtools/intitem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX
#include <svtools/eitem.hxx>
#endif
#ifndef _COMPHELPER_PROPERTY_HXX_
#include <comphelper/property.hxx>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif
#ifndef _SVTOOLS_LOGINDLG_HXX_
#include <svtools/logindlg.hxx>
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include <connectivity/dbexception.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_SQLCONTEXT_HPP_
#include <com/sun/star/sdb/SQLContext.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDRIVERACCESS_HPP_
#include <com/sun/star/sdbc/XDriverAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDRIVER_HPP_
#include <com/sun/star/sdbc/XDriver.hpp>
#endif
#ifndef DBAUI_DRIVERSETTINGS_HXX
#include "DriverSettings.hxx"
#endif
#ifndef _DBAUI_PROPERTYSETITEM_HXX_
#include "propertysetitem.hxx"
#endif
#ifndef _DBAUI_DATASOURCEITEMS_HXX_
#include "dsitems.hxx"
#endif
#ifndef DBAUI_ITEMSETHELPER_HXX
#include "IItemSetHelper.hxx"
#endif
#ifndef _DBU_DLG_HRC_
#include "dbu_dlg.hrc"
#endif
#ifndef DBACCESS_SHARED_DBUSTRINGS_HRC
#include "dbustrings.hrc"
#endif
#ifndef _VCL_STDTEXT_HXX
#include <vcl/stdtext.hxx>
#endif
#ifndef _SV_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SV_WAITOBJ_HXX 
#include <vcl/waitobj.hxx>
#endif
#ifndef _TYPELIB_TYPEDESCRIPTION_HXX_
#include <typelib/typedescription.hxx>
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif
#ifndef _DBAUI_STRINGLISTITEM_HXX_
#include "stringlistitem.hxx"
#endif
#ifndef _DBAUI_MODULE_DBU_HXX_
#include "moduledbu.hxx"
#endif
#ifndef DBAUI_TOOLS_HXX
#include "UITools.hxx"
#endif
#ifndef _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#include "dsnItem.hxx"

#include <algorithm>
#include <functional>
//.........................................................................
namespace dbaui
{
//.........................................................................
using namespace ::dbtools;
using namespace com::sun::star::uno;
using namespace com::sun::star;
using namespace com::sun::star::sdbc;
using namespace com::sun::star::sdb;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;
using namespace com::sun::star::container;
using namespace com::sun::star::frame;

//-------------------------------------------------------------------------
namespace
{
    sal_Bool implCheckItemType( SfxItemSet& _rSet, const USHORT _nId, const TypeId _nExpectedItemType )
    {
        sal_Bool bCorrectType = sal_False;

        SfxItemPool* pPool = _rSet.GetPool();
        DBG_ASSERT( pPool, "implCheckItemType: invalid item pool!" );
        if ( pPool )
        {
            const SfxPoolItem& rDefItem = pPool->GetDefaultItem( _nId );
            bCorrectType = rDefItem.IsA( _nExpectedItemType );
        }
        return bCorrectType;
    }

    void lcl_putProperty(const Reference< XPropertySet >& _rxSet, const ::rtl::OUString& _rName, const Any& _rValue)
    {
        try
        {
            if ( _rxSet.is() )
                _rxSet->setPropertyValue(_rName, _rValue);
        }
        catch(Exception&)
        {
    #ifdef DBG_UTIL
            ::rtl::OString sMessage("ODbAdminDialog::implTranslateProperty: could not set the property ");
            sMessage += ::rtl::OString(_rName.getStr(), _rName.getLength(), RTL_TEXTENCODING_ASCII_US);
            sMessage += ::rtl::OString("!");
            DBG_ERROR(sMessage.getStr());
    #endif
        }

    }

    String lcl_createHostWithPort(const SfxStringItem* _pHostName,const SfxInt32Item* _pPortNumber)
    {
        String sNewUrl;
                
        if ( _pHostName && _pHostName->GetValue().Len() )
            sNewUrl = _pHostName->GetValue();

        if ( _pPortNumber )
        {
            sNewUrl += String::CreateFromAscii(":");
            sNewUrl += String::CreateFromInt32(_pPortNumber->GetValue());
        }

        return sNewUrl;
    }
}

    //========================================================================
    //= ODbDataSourceAdministrationHelper
    //========================================================================
ODbDataSourceAdministrationHelper::ODbDataSourceAdministrationHelper(const Reference< XMultiServiceFactory >& _xORB,Window* _pParent,IItemSetHelper* _pItemSetHelper)
        : m_xORB(_xORB)
        , m_pParent(_pParent)
        , m_pItemSetHelper(_pItemSetHelper)
{
    /// initialize the property translation map
    // direct properties of a data source
    m_aDirectPropTranslator.insert(MapInt2String::value_type(DSID_CONNECTURL, PROPERTY_URL));
    m_aDirectPropTranslator.insert(MapInt2String::value_type(DSID_NAME, PROPERTY_NAME));
    m_aDirectPropTranslator.insert(MapInt2String::value_type(DSID_USER, PROPERTY_USER));
    m_aDirectPropTranslator.insert(MapInt2String::value_type(DSID_PASSWORD, PROPERTY_PASSWORD));
    m_aDirectPropTranslator.insert(MapInt2String::value_type(DSID_PASSWORDREQUIRED, PROPERTY_ISPASSWORDREQUIRED));
    m_aDirectPropTranslator.insert(MapInt2String::value_type(DSID_TABLEFILTER, PROPERTY_TABLEFILTER));
    m_aDirectPropTranslator.insert(MapInt2String::value_type(DSID_READONLY, PROPERTY_ISREADONLY));
    m_aDirectPropTranslator.insert(MapInt2String::value_type(DSID_SUPPRESSVERSIONCL, PROPERTY_SUPPRESSVERSIONCL));

    // implicit properties, to be found in the direct property "Info"
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_JDBCDRIVERCLASS, INFO_JDBCDRIVERCLASS));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_TEXTFILEEXTENSION, INFO_TEXTFILEEXTENSION));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CHARSET, INFO_CHARSET));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_TEXTFILEHEADER, INFO_TEXTFILEHEADER));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_FIELDDELIMITER, INFO_FIELDDELIMITER));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_TEXTDELIMITER, INFO_TEXTDELIMITER));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_DECIMALDELIMITER, INFO_DECIMALDELIMITER));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_THOUSANDSDELIMITER, INFO_THOUSANDSDELIMITER));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_SHOWDELETEDROWS, INFO_SHOWDELETEDROWS));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_ALLOWLONGTABLENAMES, INFO_ALLOWLONGTABLENAMES));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_ADDITIONALOPTIONS, INFO_ADDITIONALOPTIONS));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_SQL92CHECK, PROPERTY_ENABLESQL92CHECK));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_AUTOINCREMENTVALUE, PROPERTY_AUTOINCREMENTCREATION));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_AUTORETRIEVEVALUE, INFO_AUTORETRIEVEVALUE));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_AUTORETRIEVEENABLED, INFO_AUTORETRIEVEENABLED));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_APPEND_TABLE_ALIAS, INFO_APPEND_TABLE_ALIAS));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_AS_BEFORE_CORRNAME, INFO_AS_BEFORE_CORRELATION_NAME ) );
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CHECK_REQUIRED_FIELDS, INFO_FORMS_CHECK_REQUIRED_FIELDS ) );
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_PARAMETERNAMESUBST, INFO_PARAMETERNAMESUBST));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_IGNOREDRIVER_PRIV, INFO_IGNOREDRIVER_PRIV));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_BOOLEANCOMPARISON, PROPERTY_BOOLEANCOMPARISONMODE));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_ENABLEOUTERJOIN, PROPERTY_ENABLEOUTERJOIN));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CATALOG, PROPERTY_USECATALOGINSELECT));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_SCHEMA, PROPERTY_USESCHEMAINSELECT));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_INDEXAPPENDIX, ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("AddIndexAppendix"))));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_DOSLINEENDS, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "PreferDosLikeLineEnds" ) ) ) );
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_SOCKET, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LocalSocket" ) ) ) );

    // special settings for adabas
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_SHUTSERVICE, ::rtl::OUString::createFromAscii("ShutdownDatabase")));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_DATAINC, ::rtl::OUString::createFromAscii("DataCacheSizeIncrement")));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_CACHESIZE, ::rtl::OUString::createFromAscii("DataCacheSize")));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_CTRLUSER, ::rtl::OUString::createFromAscii("ControlUser")));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_CTRLPWD, ::rtl::OUString::createFromAscii("ControlPassword")));

    // extra settings for odbc
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_USECATALOG, INFO_USECATALOG));
    // extra settings for a ldap address book
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_LDAP_BASEDN, INFO_CONN_LDAP_BASEDN));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_LDAP_ROWCOUNT, INFO_CONN_LDAP_ROWCOUNT));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_CONN_LDAP_USESSL, ::rtl::OUString::createFromAscii("UseSSL")));
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_DOCUMENT_URL, PROPERTY_URL));

    // oracle
    m_aIndirectPropTranslator.insert(MapInt2String::value_type(DSID_IGNORECURRENCY, ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("IgnoreCurrency"))));

    try
    {
        m_xDatabaseContext = Reference< XNameAccess >(m_xORB->createInstance(SERVICE_SDB_DATABASECONTEXT), UNO_QUERY);
        m_xDynamicContext.set(m_xDatabaseContext,UNO_QUERY);
    }
    catch(Exception&)
    {
    }

    if ( !m_xDatabaseContext.is() )
    {
        ShowServiceNotAvailableError(_pParent->GetParent(), String(SERVICE_SDB_DATABASECONTEXT), sal_True);
    }

    DBG_ASSERT(m_xDynamicContext.is(), "ODbAdminDialog::ODbAdminDialog : no XNamingService interface !");
}
    //-------------------------------------------------------------------------
sal_Bool ODbDataSourceAdministrationHelper::getCurrentSettings(Sequence< PropertyValue >& _rDriverParam)
{
    DBG_ASSERT(m_pItemSetHelper->getOutputSet(), "ODbDataSourceAdministrationHelper::getCurrentSettings : not to be called without an example set!");
    if (!m_pItemSetHelper->getOutputSet())
        return sal_False;

    ::std::vector< PropertyValue > aReturn;
        // collecting this in a vector because it has a push_back, in opposite to sequences

    // user: DSID_USER -> "user"
    SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pUser, SfxStringItem, DSID_USER, sal_True);
    if (pUser && pUser->GetValue().Len())
        aReturn.push_back(
            PropertyValue(	::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("user")), 0,
                            makeAny(::rtl::OUString(pUser->GetValue())), PropertyState_DIRECT_VALUE));

    // check if the connection type requires a password
    if (hasAuthentication(*m_pItemSetHelper->getOutputSet()))
    {
        // password: DSID_PASSWORD -> "password"
        SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pPassword, SfxStringItem, DSID_PASSWORD, sal_True);
        String sPassword = pPassword ? pPassword->GetValue() : String();
        SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pPasswordRequired, SfxBoolItem, DSID_PASSWORDREQUIRED, sal_True);
        // if the set does not contain a password, but the item set says it requires one, ask the user
        if ((!pPassword || !pPassword->GetValue().Len()) && (pPasswordRequired && pPasswordRequired->GetValue()))
        {
            SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pName, SfxStringItem, DSID_NAME, sal_True);

            ::svt::LoginDialog aDlg(m_pParent,
                LF_NO_PATH | LF_NO_ACCOUNT | LF_NO_ERRORTEXT | LF_USERNAME_READONLY,
                String(), NULL);

            aDlg.SetName(pUser ? pUser->GetValue() : String());
            aDlg.ClearPassword();  // this will give the password field the focus

            String sName = pName ? pName->GetValue() : String();
            String sLoginRequest(ModuleRes(STR_ENTER_CONNECTION_PASSWORD));
            ::rtl::OUString sTemp = sName;
            sName = ::dbaui::getStrippedDatabaseName(NULL,sTemp);
            if ( sName.Len() )
                   sLoginRequest.SearchAndReplaceAscii("$name$", sName);
            else
            {
                sLoginRequest.SearchAndReplaceAscii("\"$name$\"", String());
                sLoginRequest.SearchAndReplaceAscii("$name$", String()); // just to be sure that in other languages the string will be deleted
            }
            aDlg.SetLoginRequestText(sLoginRequest);
            
            aDlg.SetSavePasswordText(ModuleRes(STR_REMEMBERPASSWORD_SESSION));
            aDlg.SetSavePassword(sal_True);

            sal_Int32 nResult = aDlg.Execute();
            if (nResult != RET_OK)
                return sal_False;

            sPassword = aDlg.GetPassword();
            if (aDlg.IsSavePassword())
                m_pItemSetHelper->getWriteOutputSet()->Put(SfxStringItem(DSID_PASSWORD, sPassword));
        }

        if (sPassword.Len())
            aReturn.push_back(
                PropertyValue(	::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("password")), 0,
                                makeAny(::rtl::OUString(sPassword)), PropertyState_DIRECT_VALUE));
    }

    if ( !aReturn.empty() )
        _rDriverParam = Sequence< PropertyValue >(&(*aReturn.begin()), aReturn.size());

    // append all the other stuff (charset etc.)
    fillDatasourceInfo(*m_pItemSetHelper->getOutputSet(), _rDriverParam);

    return sal_True;
}
//-------------------------------------------------------------------------
void ODbDataSourceAdministrationHelper::successfullyConnected()
{
    DBG_ASSERT(m_pItemSetHelper->getOutputSet(), "ODbDataSourceAdministrationHelper::successfullyConnected: not to be called without an example set!");
    if (!m_pItemSetHelper->getOutputSet())
        return;

    if (hasAuthentication(*m_pItemSetHelper->getOutputSet()))
    {
        SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pPassword, SfxStringItem, DSID_PASSWORD, sal_True);
        if (pPassword && (0 != pPassword->GetValue().Len()))
        {
            ::rtl::OUString sPassword = pPassword->GetValue();

            Reference< XPropertySet > xCurrentDatasource = getCurrentDataSource();
            lcl_putProperty(xCurrentDatasource,m_aDirectPropTranslator[DSID_PASSWORD], makeAny(sPassword));
        }
    }
}
//-------------------------------------------------------------------------
void ODbDataSourceAdministrationHelper::clearPassword()
{
    if (m_pItemSetHelper->getWriteOutputSet())
        m_pItemSetHelper->getWriteOutputSet()->ClearItem(DSID_PASSWORD);
}
// -----------------------------------------------------------------------------
::std::pair< Reference<XConnection>,sal_Bool> ODbDataSourceAdministrationHelper::createConnection()
{
    ::std::pair< Reference<XConnection>,sal_Bool> aRet;
    aRet.second = sal_False;
    Sequence< PropertyValue > aConnectionParams;
    if ( getCurrentSettings(aConnectionParams) )
    {
        // the current DSN
        // fill the table list with this connection information
        SQLExceptionInfo aErrorInfo;
        try
        {
            WaitObject aWaitCursor(m_pParent);
            aRet.first = getDriver()->connect(getConnectionURL(), aConnectionParams);
            aRet.second = sal_True;
        }
        catch (SQLContext& e) { aErrorInfo = SQLExceptionInfo(e); }
        catch (SQLWarning& e) { aErrorInfo = SQLExceptionInfo(e); }
        catch (SQLException& e) { aErrorInfo = SQLExceptionInfo(e); }

        showError(aErrorInfo,m_pParent,getORB());
    }
    if ( aRet.first.is() )
        successfullyConnected();// notify the admindlg to save the password

    return aRet;
}
// -----------------------------------------------------------------------------
Reference< XDriver > ODbDataSourceAdministrationHelper::getDriver()
{
    // get the global DriverManager
    Reference< XDriverAccess > xDriverManager;
    String sCurrentActionError = String(ModuleRes(STR_COULDNOTCREATE_DRIVERMANAGER));
        // in case an error occures
    sCurrentActionError.SearchAndReplaceAscii("#servicename#", (::rtl::OUString)SERVICE_SDBC_CONNECTIONPOOL);
    try
    {
        xDriverManager = Reference< XDriverAccess >(getORB()->createInstance(SERVICE_SDBC_CONNECTIONPOOL), UNO_QUERY);
        DBG_ASSERT(xDriverManager.is(), "ODbDataSourceAdministrationHelper::getDriver: could not instantiate the driver manager, or it does not provide the necessary interface!");
    }
    catch (Exception& e)
    {
        // wrap the exception into an SQLException
        SQLException aSQLWrapper(e.Message, getORB(), ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("S1000")), 0, Any());
        throw SQLException(sCurrentActionError, getORB(), ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("S1000")), 0, makeAny(aSQLWrapper));
    }
    if (!xDriverManager.is())
        throw SQLException(sCurrentActionError, getORB(), ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("S1000")), 0, Any());


    Reference< XDriver > xDriver = xDriverManager->getDriverByURL(getConnectionURL());
    if (!xDriver.is())
    {
        sCurrentActionError = String(ModuleRes(STR_NOREGISTEREDDRIVER));
        sCurrentActionError.SearchAndReplaceAscii("#connurl#", getConnectionURL());
        // will be caught and translated into an SQLContext exception
        throw SQLException(sCurrentActionError, getORB(), ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("S1000")), 0, Any());
    }
    return xDriver;
}

// -----------------------------------------------------------------------------
Reference< XPropertySet > ODbDataSourceAdministrationHelper::getCurrentDataSource()
{
    if ( !m_xDatasource.is() )
    {
        Reference<XInterface> xIn(m_aDataSourceOrName,UNO_QUERY);
        if ( !xIn.is() )
        {
            ::rtl::OUString sCurrentDatasource;
            m_aDataSourceOrName >>= sCurrentDatasource;
            OSL_ENSURE(sCurrentDatasource.getLength(),"No datasource name given!");
            try
            {
                if ( m_xDatabaseContext.is() )
                    m_xDatasource.set(m_xDatabaseContext->getByName(sCurrentDatasource),UNO_QUERY);
                xIn = m_xDatasource;
            }
            catch(const Exception&)
            {
            }
        }
        m_xModel.set(getDataSourceOrModel(xIn),UNO_QUERY);
        if ( m_xModel.is() )
            m_xDatasource.set(xIn,UNO_QUERY);
        else
        {
            m_xDatasource.set(getDataSourceOrModel(xIn),UNO_QUERY);
            m_xModel.set(xIn,UNO_QUERY);
        }
    }

    
    DBG_ASSERT(m_xDatasource.is(), "ODbDataSourceAdministrationHelper::getCurrentDataSource: no data source!");
    return m_xDatasource;
}
//-------------------------------------------------------------------------
::dbaccess::DATASOURCE_TYPE ODbDataSourceAdministrationHelper::getDatasourceType( const SfxItemSet& _rSet )
{
    SFX_ITEMSET_GET( _rSet, pConnectURL, SfxStringItem, DSID_CONNECTURL, sal_True );
    SFX_ITEMSET_GET( _rSet, pTypeCollection, DbuTypeCollectionItem, DSID_TYPECOLLECTION, sal_True );
    DBG_ASSERT( pConnectURL && pTypeCollection, "ODbDataSourceAdministrationHelper::getDatasourceType: invalid items in the source set!" );
    if ( !pConnectURL || !pTypeCollection )
        return  ::dbaccess::DST_UNKNOWN;

    String sConnectURL = pConnectURL->GetValue();
    ::dbaccess::ODsnTypeCollection* pCollection = pTypeCollection->getCollection();
    DBG_ASSERT( pCollection, "ODbDataSourceAdministrationHelper::getDatasourceType: invalid type collection!" );
    if ( !pCollection )
        return  ::dbaccess::DST_UNKNOWN;

    return pCollection->getType( sConnectURL );
}

//-------------------------------------------------------------------------
sal_Bool ODbDataSourceAdministrationHelper::hasAuthentication(const SfxItemSet& _rSet) const
{
    return DataSourceMetaData::getAuthentication( getDatasourceType( _rSet ) ) != AuthNone;
}
// -----------------------------------------------------------------------------
String ODbDataSourceAdministrationHelper::getConnectionURL() const
{
    String sNewUrl;

    ::dbaccess::DATASOURCE_TYPE eType = getDatasourceType(*m_pItemSetHelper->getOutputSet());

    SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pUrlItem, SfxStringItem, DSID_CONNECTURL, sal_True);
    SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pTypeCollection, DbuTypeCollectionItem, DSID_TYPECOLLECTION, sal_True);

    OSL_ENSURE(pUrlItem,"Connection URL is NULL. -> GPF!");
    DBG_ASSERT(pTypeCollection, "ODbDataSourceAdministrationHelper::getDatasourceType: invalid items in the source set!");
    ::dbaccess::ODsnTypeCollection* pCollection = pTypeCollection->getCollection();
    DBG_ASSERT(pCollection, "ODbDataSourceAdministrationHelper::getDatasourceType: invalid type collection!");

    

    switch( eType )
    {
        case  ::dbaccess::DST_DBASE:
        case  ::dbaccess::DST_FLAT:
        case  ::dbaccess::DST_CALC:
            break;
        case  ::dbaccess::DST_ADABAS:
            {
                SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pHostName, SfxStringItem, DSID_CONN_HOSTNAME, sal_True);
                sNewUrl = lcl_createHostWithPort(pHostName,NULL);
                String sUrl = pCollection->cutPrefix(pUrlItem->GetValue());
                if ( sUrl.GetTokenCount(':') == 1 )
                    sNewUrl += String::CreateFromAscii(":");

                sNewUrl += sUrl;
            }
            break;
        case  ::dbaccess::DST_MSACCESS:
        case  ::dbaccess::DST_MSACCESS_2007:
            {
                ::rtl::OUString sFileName = pCollection->cutPrefix(pUrlItem->GetValue());
                ::rtl::OUString sNewFileName;
                if ( ::osl::FileBase::getSystemPathFromFileURL( sFileName, sNewFileName ) == ::osl::FileBase::E_None )
                {
                    sNewUrl += String(sNewFileName);
                }
            }
            break;
        case  ::dbaccess::DST_MYSQL_NATIVE:
        case  ::dbaccess::DST_MYSQL_JDBC:
            {
                SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pHostName, SfxStringItem, DSID_CONN_HOSTNAME, sal_True);
                SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pPortNumber, SfxInt32Item, DSID_MYSQL_PORTNUMBER, sal_True);
                SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pDatabaseName, SfxStringItem, DSID_DATABASENAME, sal_True);
                sNewUrl = lcl_createHostWithPort(pHostName,pPortNumber);
                String sDatabaseName = pDatabaseName ? pDatabaseName->GetValue() : String();
                if ( !sDatabaseName.Len() && pUrlItem )
                    sDatabaseName = pCollection->cutPrefix( pUrlItem->GetValue() );
                    // TODO: what's that? Why is the database name transported via the URL Item?
                    // Huh? Anybody there?
                    // OJ: It is needed when the connection properties are changed. There the URL is used for every type.

                if ( sDatabaseName.Len() )
                {
                    sNewUrl += String::CreateFromAscii("/");
                    sNewUrl += sDatabaseName;
                }
            }
            break;
        case  ::dbaccess::DST_ORACLE_JDBC:
            {
                SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pHostName, SfxStringItem, DSID_CONN_HOSTNAME, sal_True);
                SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pPortNumber, SfxInt32Item, DSID_ORACLE_PORTNUMBER, sal_True);
                SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pDatabaseName, SfxStringItem, DSID_DATABASENAME, sal_True);
                if ( pHostName && pHostName->GetValue().Len() )
                {
                    sNewUrl = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("@"));
                    sNewUrl += lcl_createHostWithPort(pHostName,pPortNumber);
                    String sDatabaseName = pDatabaseName ? pDatabaseName->GetValue() : String();
                    if ( !sDatabaseName.Len() && pUrlItem )
                        sDatabaseName = pCollection->cutPrefix( pUrlItem->GetValue() );
                    if ( sDatabaseName.Len() )
                    {
                        sNewUrl += String::CreateFromAscii(":");
                        sNewUrl += sDatabaseName;
                    }
                }
                else
                { // here someone entered a JDBC url which looks like oracle, so we have to use the url property

                }
            }
            break;
        case  ::dbaccess::DST_LDAP:
            {
                //	SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pHostName, SfxStringItem, DSID_CONN_HOSTNAME, sal_True);
                SFX_ITEMSET_GET(*m_pItemSetHelper->getOutputSet(), pPortNumber, SfxInt32Item, DSID_CONN_LDAP_PORTNUMBER, sal_True);
                sNewUrl = pCollection->cutPrefix(pUrlItem->GetValue());
                sNewUrl += lcl_createHostWithPort(NULL,pPortNumber);
            }
            break;
        case  ::dbaccess::DST_JDBC:
            // run through
        default:
            break;
    }
    if ( sNewUrl.Len() )
    {
        String sUrl = pCollection->getDatasourcePrefix(eType);
        sUrl += sNewUrl;
        sNewUrl = sUrl;
    }
    else
        sNewUrl = pUrlItem->GetValue();

    return sNewUrl;
}
//-------------------------------------------------------------------------
struct PropertyValueLess
{
    bool operator() (const PropertyValue& x, const PropertyValue& y) const
        { return x.Name < y.Name ? true : false; }		// construct prevents a MSVC6 warning
};
DECLARE_STL_SET( PropertyValue, PropertyValueLess, PropertyValueSet);

//........................................................................
void ODbDataSourceAdministrationHelper::translateProperties(const Reference< XPropertySet >& _rxSource, SfxItemSet& _rDest)
{
    ::rtl::OUString sNewConnectURL, sName, sUid, sPwd;
    Sequence< ::rtl::OUString > aTableFitler;

    if (_rxSource.is())
    {
        for	(	ConstMapInt2StringIterator aDirect = m_aDirectPropTranslator.begin();
                aDirect != m_aDirectPropTranslator.end();
                ++aDirect
            )
        {
            // get the property value
            Any aValue;
            try
            {
                aValue = _rxSource->getPropertyValue(aDirect->second);
            }
            catch(Exception&)
            {
#ifdef DBG_UTIL
                ::rtl::OString aMessage("ODbDataSourceAdministrationHelper::translateProperties: could not extract the property ");
                aMessage += ::rtl::OString(aDirect->second.getStr(), aDirect->second.getLength(), RTL_TEXTENCODING_ASCII_US);
                aMessage += ::rtl::OString("!");
                DBG_ERROR(aMessage.getStr());
#endif
            }
            // transfer it into an item
            implTranslateProperty(_rDest, aDirect->first, aValue);
        }

        // get the additional informations
        Sequence< PropertyValue > aAdditionalInfo;
        try
        {
            _rxSource->getPropertyValue(PROPERTY_INFO) >>= aAdditionalInfo;
        }
        catch(Exception&) { }

        // collect the names of the additional settings
        const PropertyValue* pAdditionalInfo = aAdditionalInfo.getConstArray();
        PropertyValueSet aInfos;
        for (sal_Int32 i=0; i<aAdditionalInfo.getLength(); ++i, ++pAdditionalInfo)
        {
            if (0 == pAdditionalInfo->Name.compareToAscii("JDBCDRV"))
            {	// compatibility
                PropertyValue aCompatibility(*pAdditionalInfo);
                aCompatibility.Name = ::rtl::OUString::createFromAscii("JavaDriverClass");
                aInfos.insert(aCompatibility);
            }
            else
                aInfos.insert(*pAdditionalInfo);
        }

        // go through all known translations and check if we have such a setting
        PropertyValue aSearchFor;
        for	(	ConstMapInt2StringIterator aIndirect = m_aIndirectPropTranslator.begin();
                aIndirect != m_aIndirectPropTranslator.end();
                ++aIndirect
            )
        {
            aSearchFor.Name = aIndirect->second;
            ConstPropertyValueSetIterator aInfoPos = aInfos.find(aSearchFor);
            if (aInfos.end() != aInfoPos)
                // the property is contained in the info sequence
                // -> transfer it into an item
                implTranslateProperty(_rDest, aIndirect->first, aInfoPos->Value);
        }

        convertUrl(_rDest);
    }

    try
    {
        _rDest.Put(OPropertySetItem(DSID_DATASOURCE_UNO, _rxSource));
        Reference<XStorable> xStore(getDataSourceOrModel(_rxSource),UNO_QUERY);
        _rDest.Put(SfxBoolItem(DSID_READONLY, !xStore.is() || xStore->isReadonly() ));
    }
    catch(Exception&)
    {
        OSL_ENSURE(0,"IsReadOnly throws an exception!");
    }
}

//-------------------------------------------------------------------------
void ODbDataSourceAdministrationHelper::translateProperties(const SfxItemSet& _rSource, const Reference< XPropertySet >& _rxDest)
{
    DBG_ASSERT(_rxDest.is(), "ODbDataSourceAdministrationHelper::translateProperties: invalid property set!");
    if (!_rxDest.is())
        return;

    // the property set info
    Reference< XPropertySetInfo > xInfo;
    try { xInfo = _rxDest->getPropertySetInfo(); }
    catch(Exception&) { }
    
    const ::rtl::OUString sUrlProp(RTL_CONSTASCII_USTRINGPARAM("URL"));
    // -----------------------------
    // transfer the direct properties
    for	(	ConstMapInt2StringIterator aDirect = m_aDirectPropTranslator.begin();
            aDirect != m_aDirectPropTranslator.end();
            ++aDirect
        )
    {
        const SfxPoolItem* pCurrentItem = _rSource.GetItem((USHORT)aDirect->first);
        if (pCurrentItem)
        {
            sal_Int16 nAttributes = PropertyAttribute::READONLY;
            if (xInfo.is())
            {
                try { nAttributes = xInfo->getPropertyByName(aDirect->second).Attributes; }
                catch(Exception&) { }
            }
            if ((nAttributes & PropertyAttribute::READONLY) == 0)
            {
                if ( sUrlProp == aDirect->second )
                {
                    Any aValue(makeAny(::rtl::OUString(getConnectionURL())));
                    //	aValue <<= ::rtl::OUString();
                    lcl_putProperty(_rxDest, aDirect->second,aValue);
                }
                else
                    implTranslateProperty(_rxDest, aDirect->second, pCurrentItem);
            }
        }
    }

    // -------------------------------
    // now for the indirect properties

    Sequence< PropertyValue > aInfo;
    // the original properties
    try
    {
        _rxDest->getPropertyValue(PROPERTY_INFO) >>= aInfo;
    }
    catch(Exception&) { }

    // overwrite and extend them
    fillDatasourceInfo(_rSource, aInfo);
    // and propagate the (newly composed) sequence to the set
    lcl_putProperty(_rxDest,PROPERTY_INFO, makeAny(aInfo));
}


//-------------------------------------------------------------------------
void ODbDataSourceAdministrationHelper::fillDatasourceInfo(const SfxItemSet& _rSource, Sequence< ::com::sun::star::beans::PropertyValue >& _rInfo)
{
    // within the current "Info" sequence, replace the ones we can examine from the item set
    // (we don't just fill a completely new sequence with our own items, but we preserve any properties unknown to
    // us)

    // first determine which of all the items are relevant for the data source (depends on the connection url)
    ::dbaccess::DATASOURCE_TYPE eType = getDatasourceType(_rSource);
    ::std::vector< sal_Int32> aDetailIds;
    ODriversSettings::getSupportedIndirectSettings(eType,aDetailIds);

    // collect the translated property values for the relevant items
    PropertyValueSet aRelevantSettings;
    ConstMapInt2StringIterator aTranslation;
    ::std::vector< sal_Int32>::iterator aDetailsEnd = aDetailIds.end();
    for (::std::vector< sal_Int32>::iterator aIter = aDetailIds.begin();aIter != aDetailsEnd ; ++aIter)
    {
        const SfxPoolItem* pCurrent = _rSource.GetItem((USHORT)*aIter);
        aTranslation = m_aIndirectPropTranslator.find(*aIter);
        if ( pCurrent && (m_aIndirectPropTranslator.end() != aTranslation) )
        {
            if ( aTranslation->second == INFO_CHARSET )
            {
                ::rtl::OUString sCharSet;
                implTranslateProperty(pCurrent) >>= sCharSet;
                if ( sCharSet.getLength() )
                    aRelevantSettings.insert(PropertyValue(aTranslation->second, 0, makeAny(sCharSet), PropertyState_DIRECT_VALUE));
            }
            else
                aRelevantSettings.insert(PropertyValue(aTranslation->second, 0, implTranslateProperty(pCurrent), PropertyState_DIRECT_VALUE));
        }
    }

    // settings to preserve
    MapInt2String	aPreservedSettings;

    // now aRelevantSettings contains all the property values relevant for the current data source type,
    // check the original sequence if it already contains any of these values (which have to be overwritten, then)
    PropertyValue* pInfo = _rInfo.getArray();
    PropertyValue aSearchFor;
    sal_Int32 nObsoleteSetting = -1;
    sal_Int32 nCount = _rInfo.getLength();
    for (sal_Int32 i = 0; i < nCount; ++i, ++pInfo)
    {
        aSearchFor.Name = pInfo->Name;
        PropertyValueSetIterator aOverwrittenSetting = aRelevantSettings.find(aSearchFor);
        if (aRelevantSettings.end() != aOverwrittenSetting)
        {	// the setting was present in the original sequence, and it is to be overwritten -> replace it
            if ( !::comphelper::compare(pInfo->Value,aOverwrittenSetting->Value) )
                *pInfo = *aOverwrittenSetting;
            aRelevantSettings.erase(aOverwrittenSetting);
        }
        else if (0 == pInfo->Name.compareToAscii("JDBCDRV"))
        {	// this is a compatibility setting, remove it from the sequence (it's replaced by JavaDriverClass)
            nObsoleteSetting = i;
        }
        else
            aPreservedSettings[i] = pInfo->Name;
    }
    if (-1 != nObsoleteSetting)
        ::comphelper::removeElementAt(_rInfo, nObsoleteSetting);

    if ( !aPreservedSettings.empty() )
    {	// check if there are settings which
        // * are known as indirect properties
        // * but not relevant for the current data source type
        // These settings have to be removed: If they're not relevant, we have no UI for changing them.
        // 25.06.2001 - 88004/87182 - frank.schoenheit@sun.com

        // for this, we need a string-controlled quick access to m_aIndirectPropTranslator
        StringSet aIndirectProps;
        ::std::transform(m_aIndirectPropTranslator.begin(),
                         m_aIndirectPropTranslator.end(),
                         ::std::insert_iterator<StringSet>(aIndirectProps,aIndirectProps.begin()),
                         ::std::select2nd<MapInt2String::value_type>());

        // now check the to-be-preserved props
        ::std::vector< sal_Int32 > aRemoveIndexes;
        sal_Int32 nPositionCorrector = 0;
        ConstMapInt2StringIterator aPreservedEnd = aPreservedSettings.end();
        for	(	ConstMapInt2StringIterator aPreserved = aPreservedSettings.begin();
                aPreserved != aPreservedEnd;
                ++aPreserved
            )
        {
            if (aIndirectProps.end() != aIndirectProps.find(aPreserved->second))
            {
#ifdef DBG_UTIL
                const ::rtl::OUString sName = aPreserved->second;
#endif
                aRemoveIndexes.push_back(aPreserved->first - nPositionCorrector);
                ++nPositionCorrector;
            }
        }
        // now finally remove all such props
        ::std::vector< sal_Int32 >::const_iterator aRemoveEnd = aRemoveIndexes.end();
        for (	::std::vector< sal_Int32 >::const_iterator aRemoveIndex = aRemoveIndexes.begin();
                aRemoveIndex != aRemoveEnd;
                ++aRemoveIndex
            )
            ::comphelper::removeElementAt(_rInfo, *aRemoveIndex);
#ifdef DBG_UTIL
        const PropertyValue* pWhatsLeft = _rInfo.getConstArray();
        const PropertyValue* pWhatsLeftEnd = pWhatsLeft + _rInfo.getLength();
        for (; pWhatsLeft != pWhatsLeftEnd; ++pWhatsLeft)
        {
            ::rtl::OUString sLookAtIt = pWhatsLeft->Name;
        }
#endif
    }

    // here we have a special entry for types from oracle
    if ( eType ==  ::dbaccess::DST_ORACLE_JDBC )
    {
        Sequence< Any > aTypeSettings;
        static const ::rtl::OUString s_sCondition(RTL_CONSTASCII_USTRINGPARAM("Column(2) = "));
        static const ::rtl::OUString s_sValue(RTL_CONSTASCII_USTRINGPARAM("Column(6) = PRECISION"));
        static const sal_Int32 pTypes[] = { -5, -4, -3, -2, -1, 1, 2, 12};
        aTypeSettings.realloc((sizeof(pTypes)/sizeof(pTypes[0])) * 2);
        Any* pCondIter = aTypeSettings.getArray();
        const Any* pCondEnd  = pCondIter + aTypeSettings.getLength();
        for(const sal_Int32* pType = pTypes;pCondIter != pCondEnd;++pCondIter,++pType)
        {
            *pCondIter <<= (s_sCondition + ::rtl::OUString::valueOf(*pType));
            ++pCondIter;
            *pCondIter <<= s_sValue;
        }
        aRelevantSettings.insert(PropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TypeInfoSettings")), 0, makeAny(aTypeSettings), PropertyState_DIRECT_VALUE));
    }

    // check which values are still left ('cause they were not present in the original sequence, but are to be set)
    if ( !aRelevantSettings.empty() )
    {
        sal_Int32 nOldLength = _rInfo.getLength();
        _rInfo.realloc(nOldLength + aRelevantSettings.size());
        PropertyValue* pAppendValues = _rInfo.getArray() + nOldLength;
        ConstPropertyValueSetIterator aRelevantEnd = aRelevantSettings.end();
        for	(	ConstPropertyValueSetIterator aLoop = aRelevantSettings.begin();
                aLoop != aRelevantEnd;
                ++aLoop, ++pAppendValues
            )
        {
            if ( aLoop->Name == INFO_CHARSET )
            {
                ::rtl::OUString sCharSet;
                aLoop->Value >>= sCharSet;
                if ( sCharSet.getLength() )
                    *pAppendValues = *aLoop;
            }
            else
                *pAppendValues = *aLoop;
        }
    }
}
//-------------------------------------------------------------------------
Any ODbDataSourceAdministrationHelper::implTranslateProperty(const SfxPoolItem* _pItem)
{
    // translate the SfxPoolItem
    Any aValue;
    if (_pItem->ISA(SfxStringItem))
        aValue <<= ::rtl::OUString(PTR_CAST(SfxStringItem, _pItem)->GetValue().GetBuffer());
    else if (_pItem->ISA(SfxBoolItem))
        aValue <<= PTR_CAST(SfxBoolItem, _pItem)->GetValue();
    else if (_pItem->ISA(SfxInt32Item))
        aValue <<= PTR_CAST(SfxInt32Item, _pItem)->GetValue();
    else if (_pItem->ISA(OStringListItem))
        aValue <<= PTR_CAST(OStringListItem, _pItem)->getList();
    else
    {
        DBG_ERROR("ODbDataSourceAdministrationHelper::implTranslateProperty: unsupported item type!");
        return aValue;
    }

    return aValue;
}
//-------------------------------------------------------------------------
void ODbDataSourceAdministrationHelper::implTranslateProperty(const Reference< XPropertySet >& _rxSet, const ::rtl::OUString& _rName, const SfxPoolItem* _pItem)
{
    Any aValue = implTranslateProperty(_pItem);
    lcl_putProperty(_rxSet, _rName,aValue);
}
#ifdef DBG_UTIL
//-------------------------------------------------------------------------
::rtl::OString ODbDataSourceAdministrationHelper::translatePropertyId( sal_Int32 _nId )
{
    ::rtl::OUString aString;

    MapInt2String::const_iterator aPos = m_aDirectPropTranslator.find( _nId );
    if ( m_aDirectPropTranslator.end() != aPos )
    {
        aString = aPos->second;
    }
    else
    {
        MapInt2String::const_iterator indirectPos = m_aIndirectPropTranslator.find( _nId );
        if ( m_aIndirectPropTranslator.end() != indirectPos )
            aString = indirectPos->second;
    }

    ::rtl::OString aReturn( aString.getStr(), aString.getLength(), RTL_TEXTENCODING_ASCII_US );
    return aReturn;
}
#endif

//-------------------------------------------------------------------------
void ODbDataSourceAdministrationHelper::implTranslateProperty( SfxItemSet& _rSet, sal_Int32  _nId, const Any& _rValue )
{
    USHORT nId = (USHORT)_nId;
    switch (_rValue.getValueType().getTypeClass())
    {
        case TypeClass_STRING:
            if ( implCheckItemType( _rSet, nId, SfxStringItem::StaticType() ) )
            {
                ::rtl::OUString sValue;
                _rValue >>= sValue;
                _rSet.Put(SfxStringItem(nId, sValue.getStr()));
            }
            else {
                DBG_ERROR(
                    (	::rtl::OString( "ODbDataSourceAdministrationHelper::implTranslateProperty: invalid property value (" )
                    +=	::rtl::OString( translatePropertyId( _nId ) )
                    +=	::rtl::OString( " should be no string)!" )
                    ).getStr()
                );
            }
            break;

        case TypeClass_BOOLEAN:
            if ( implCheckItemType( _rSet, nId, SfxBoolItem::StaticType() ) )
            {
                sal_Bool bVal = sal_False;
                _rValue >>= bVal;
                _rSet.Put(SfxBoolItem(nId, bVal));
            }
            else {
                DBG_ERROR(
                    (	::rtl::OString( "ODbDataSourceAdministrationHelper::implTranslateProperty: invalid property value (" )
                    +=	::rtl::OString( translatePropertyId( _nId ) )
                    +=	::rtl::OString( " should be no boolean)!" )
                    ).getStr()
                );
            }
            break;

        case TypeClass_LONG:
            if ( implCheckItemType( _rSet, nId, SfxInt32Item::StaticType() ) )
            {
                sal_Int32 nValue = 0;
                _rValue >>= nValue;
                _rSet.Put( SfxInt32Item( nId, nValue ) );
            }
            else {
                DBG_ERROR(
                    (	::rtl::OString( "ODbDataSourceAdministrationHelper::implTranslateProperty: invalid property value (" )
                    +=	::rtl::OString( translatePropertyId( _nId ) )
                    +=	::rtl::OString( " should be no int)!" )
                    ).getStr()
                );
            }
            break;

        case TypeClass_SEQUENCE:
            if ( implCheckItemType( _rSet, nId, OStringListItem::StaticType() ) )
            {
                // determine the element type
                TypeDescription aTD(_rValue.getValueType());
                typelib_IndirectTypeDescription* pSequenceTD =
                    reinterpret_cast< typelib_IndirectTypeDescription* >(aTD.get());
                DBG_ASSERT(pSequenceTD && pSequenceTD->pType, "ODbDataSourceAdministrationHelper::implTranslateProperty: invalid sequence type!");

                Type aElementType(pSequenceTD->pType);
                switch (aElementType.getTypeClass())
                {
                    case TypeClass_STRING:
                    {
                        Sequence< ::rtl::OUString > aStringList;
                        _rValue >>= aStringList;
                        _rSet.Put(OStringListItem(nId, aStringList));
                    }
                    break;
                    default:
                        DBG_ERROR("ODbDataSourceAdministrationHelper::implTranslateProperty: unsupported property value type!");
                }
            }
            else {
                DBG_ERROR(
                    (	::rtl::OString( "ODbDataSourceAdministrationHelper::implTranslateProperty: invalid property value (" )
                    +=	::rtl::OString( translatePropertyId( _nId ) )
                    +=	::rtl::OString( " should be no string sequence)!" )
                    ).getStr()
                );
            }
            break;

        case TypeClass_VOID:
            _rSet.ClearItem(nId);
            break;

        default:
            DBG_ERROR("ODbDataSourceAdministrationHelper::implTranslateProperty: unsupported property value type!");
    }
}


String ODbDataSourceAdministrationHelper::getDocumentUrl(SfxItemSet& _rDest)
{
    SFX_ITEMSET_GET(_rDest, pUrlItem, SfxStringItem, DSID_DOCUMENT_URL, sal_True);
    OSL_ENSURE(pUrlItem,"Document URL is NULL. -> GPF!");
    return pUrlItem->GetValue();
}


// -----------------------------------------------------------------------------
void ODbDataSourceAdministrationHelper::convertUrl(SfxItemSet& _rDest)
{
    ::dbaccess::DATASOURCE_TYPE eType = getDatasourceType(_rDest);

    SFX_ITEMSET_GET(_rDest, pUrlItem, SfxStringItem, DSID_CONNECTURL, sal_True);
    SFX_ITEMSET_GET(_rDest, pTypeCollection, DbuTypeCollectionItem, DSID_TYPECOLLECTION, sal_True);

    OSL_ENSURE(pUrlItem,"Connection URL is NULL. -> GPF!");
    DBG_ASSERT(pTypeCollection, "ODbAdminDialog::getDatasourceType: invalid items in the source set!");
    ::dbaccess::ODsnTypeCollection* pCollection = pTypeCollection->getCollection();
    DBG_ASSERT(pCollection, "ODbAdminDialog::getDatasourceType: invalid type collection!");

    USHORT nPortNumberId	= 0;
    sal_Int32 nPortNumber	= -1;
    String sNewHostName;
    String sUrl = pCollection->cutPrefix(pUrlItem->GetValue());
    String sUrlPart;

    pCollection->extractHostNamePort(pUrlItem->GetValue(),sUrlPart,sNewHostName,nPortNumber);

    switch( eType )
    {
        case  ::dbaccess::DST_MYSQL_NATIVE:
        case  ::dbaccess::DST_MYSQL_JDBC:
            nPortNumberId = DSID_MYSQL_PORTNUMBER;
            break;
        case  ::dbaccess::DST_ORACLE_JDBC:
            nPortNumberId = DSID_ORACLE_PORTNUMBER;
            break;
        case  ::dbaccess::DST_LDAP:
            nPortNumberId = DSID_CONN_LDAP_PORTNUMBER;
            break;
        default:
            break;
    }

    if ( sUrlPart.Len() )
    {
        String sNewUrl = pCollection->getDatasourcePrefix(eType);
        sNewUrl += sUrlPart;
        _rDest.Put(SfxStringItem(DSID_CONNECTURL, sNewUrl));
    }

    if ( sNewHostName.Len() )
        _rDest.Put(SfxStringItem(DSID_CONN_HOSTNAME, sNewHostName));

    if ( nPortNumber != -1 && nPortNumberId != 0 )
        _rDest.Put(SfxInt32Item(nPortNumberId, nPortNumber));

}
// -----------------------------------------------------------------------------
sal_Bool ODbDataSourceAdministrationHelper::saveChanges(const SfxItemSet& _rSource)
{
    // put the remembered settings into the property set
    Reference<XPropertySet> xDatasource = getCurrentDataSource();
    if ( !xDatasource.is() )
        return sal_False;

    translateProperties(_rSource,xDatasource );

    return sal_True;
}
// -----------------------------------------------------------------------------
void ODbDataSourceAdministrationHelper::setDataSourceOrName( const Any& _rDataSourceOrName ) 
{ 
    DBG_ASSERT( !m_aDataSourceOrName.hasValue(), "ODbDataSourceAdministrationHelper::setDataSourceOrName: already have one!" );
        // hmm. We could reset m_xDatasource/m_xModel, probably, and continue working
    m_aDataSourceOrName = _rDataSourceOrName;
}
//=========================================================================
//= DbuTypeCollectionItem
//=========================================================================
TYPEINIT1(DbuTypeCollectionItem, SfxPoolItem);
//-------------------------------------------------------------------------
DbuTypeCollectionItem::DbuTypeCollectionItem(sal_Int16 _nWhich, ::dbaccess::ODsnTypeCollection* _pCollection)
    :SfxPoolItem(_nWhich)
    ,m_pCollection(_pCollection)
{
}

//-------------------------------------------------------------------------
DbuTypeCollectionItem::DbuTypeCollectionItem(const DbuTypeCollectionItem& _rSource)
    :SfxPoolItem(_rSource)
    ,m_pCollection(_rSource.getCollection())
{
}

//-------------------------------------------------------------------------
int DbuTypeCollectionItem::operator==(const SfxPoolItem& _rItem) const
{
    DbuTypeCollectionItem* pCompare = PTR_CAST(DbuTypeCollectionItem, &_rItem);
    return pCompare && (pCompare->getCollection() == getCollection());
}

//-------------------------------------------------------------------------
SfxPoolItem* DbuTypeCollectionItem::Clone(SfxItemPool* /*_pPool*/) const
{
    return new DbuTypeCollectionItem(*this);
}

//.........................................................................
}	// namespace dbaui
//.........................................................................



