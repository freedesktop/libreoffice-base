/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: WCopyTable.hxx,v $
 *
 *  $Revision: 1.25 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-05 17:01:38 $
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

#ifndef DBAUI_WIZ_COPYTABLEDIALOG_HXX
#define DBAUI_WIZ_COPYTABLEDIALOG_HXX

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSET_HPP_
#include <com/sun/star/sdbc/XResultSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSETMETADATA_HPP_
#include <com/sun/star/sdbc/XResultSetMetaData.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDATABASEMETADATA_HPP_
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif
#ifndef DBAUI_TYPEINFO_HXX
#include "TypeInfo.hxx"
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _SVT_WIZDLG_HXX
#include <svtools/wizdlg.hxx>
#endif
#ifndef DBAUI_DATABASEEXPORT_HXX
#include "DExport.hxx"
#endif
#ifndef DBAUI_WIZ_TABBPAGE_HXX
#include "WTabPage.hxx"
#endif
#ifndef DBAUI_FIELDDESCRIPTIONS_HXX
#include "FieldDescriptions.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XKEYSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XKeysSupplier.hpp>
#endif
#ifndef _SV_LSTBOX_HXX 
#include <vcl/lstbox.hxx>
#endif

#include <functional>

namespace dbaui
{

    typedef ::std::unary_function< ::rtl::OUString,bool> TColumnFindFunctorType;
    class TColumnFindFunctor : public TColumnFindFunctorType
    {
    public:
        virtual bool operator()(const ::rtl::OUString& _sColumnName) const = 0;
    };

    class TExportColumnFindFunctor : public TColumnFindFunctor
    {
        ODatabaseExport::TColumns* m_pColumns;
    public:
        TExportColumnFindFunctor(ODatabaseExport::TColumns* _pColumns)
        {
            m_pColumns = _pColumns;
        }
        inline bool operator()(const ::rtl::OUString& _sColumnName)	const
        {
            return m_pColumns->find(_sColumnName) != m_pColumns->end();
        }
    };

    class TMultiListBoxEntryFindFunctor : public TColumnFindFunctor
    {
        ::comphelper::TStringMixEqualFunctor m_aCase;
        ::std::vector< ::rtl::OUString>* m_pVector;
    public:
        TMultiListBoxEntryFindFunctor(::std::vector< ::rtl::OUString>* _pVector,
                                    const ::comphelper::TStringMixEqualFunctor& _aCase)
            :m_aCase(_aCase)
            ,m_pVector(_pVector)
        {
        }
        inline bool operator()(const ::rtl::OUString& _sColumnName)	const
        {
            return ::std::find_if(m_pVector->begin(),m_pVector->end(),
                ::std::bind2nd(m_aCase, _sColumnName)) != m_pVector->end();
        }
    };

    // ========================================================
    // ICopyTableSourceObject
    // ========================================================
    /** interface to an object to copy to another DB, using the OCopyTableWizard

        when the wizard is used to copy an object to another DB, it usually requires
        a sdbcx-level or sdb-level object (a css.sdbcx.Table or css.sdb.Query, that is).

        However, to also support copying tables from sdbc-level connections, we allow to
        work with the object name only. This implies some less features (like copying the
        UI settings of a table is not done), but still allows to copy definition and data.
    */
    class ICopyTableSourceObject
    {
    public:
        /// retrieves the fully qualified name of the object to copy
        virtual ::rtl::OUString     getQualifiedObjectName() const = 0;
        /// determines whether the object is a view
        virtual bool                isView() const = 0;
        /** copies the UI settings of the object to the given target object. Might be
            ignored by implementations which do not have Ui settings.
        */
        virtual void                copyUISettingsTo( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObject ) const = 0;
        /// retrieves the column names of the to-be-copied object
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                    getColumnNames() const = 0;
        /// retrieves the names of the primary keys of the to-be-copied object
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                    getPrimaryKeyColumnNames() const = 0;
        /// creates a OFieldDescription for the given column of the to-be-copied object
        virtual OFieldDescription*  createFieldDescription( const ::rtl::OUString& _rColumnName ) const = 0;
        /// returns the SELECT statement which can be used to retrieve the data of the to-be-copied object
        virtual ::rtl::OUString     getSelectStatement() const = 0;

        /** copies the filter and sorting
        *
        * \return 
        */
        virtual void                copyFilterAndSortingTo(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection,const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObject ) const = 0;

        /** returns the prepared statement which can be used to retrieve the data of the to-be-copied object

            The default implementation of this method will simply prepare a statement with the return value
            of ->getSelectStatement.
        */
        virtual ::utl::SharedUNOComponent< ::com::sun::star::sdbc::XPreparedStatement >
                                    getPreparedSelectStatement() const = 0;

        virtual ~ICopyTableSourceObject();
    };

    // ========================================================
    // ObjectCopySource
    // ========================================================
    class ObjectCopySource : public ICopyTableSourceObject
    {
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >         m_xConnection;
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >   m_xMetaData;
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >       m_xObject;
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >   m_xObjectPSI;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >    m_xObjectColumns;

    public:
        ObjectCopySource(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObject
        );

        // ICopyTableSourceObject overridables
        virtual ::rtl::OUString     getQualifiedObjectName() const;
        virtual bool                isView() const;
        virtual void                copyUISettingsTo( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObject ) const;
        virtual void                copyFilterAndSortingTo(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObject ) const;
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                    getColumnNames() const;
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                    getPrimaryKeyColumnNames() const;
        virtual OFieldDescription*  createFieldDescription( const ::rtl::OUString& _rColumnName ) const;
        virtual ::rtl::OUString     getSelectStatement() const;
        virtual ::utl::SharedUNOComponent< ::com::sun::star::sdbc::XPreparedStatement >
                                    getPreparedSelectStatement() const;
    };

    // ========================================================
    // NamedTableCopySource
    // ========================================================
    class NamedTableCopySource : public ICopyTableSourceObject
    {
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >         m_xConnection;
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData >   m_xMetaData;
        ::rtl::OUString                                                                 m_sTableName;
        ::rtl::OUString                                                                 m_sTableCatalog;
        ::rtl::OUString                                                                 m_sTableSchema;
        ::rtl::OUString                                                                 m_sTableBareName;
        ::std::vector< OFieldDescription >                                              m_aColumnInfo;
        ::utl::SharedUNOComponent< ::com::sun::star::sdbc::XPreparedStatement >         m_xStatement;

    public:
        NamedTableCopySource(
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
            const ::rtl::OUString& _rTableName
        );

        // ICopyTableSourceObject overridables
        virtual ::rtl::OUString     getQualifiedObjectName() const;
        virtual bool                isView() const;
        virtual void                copyUISettingsTo( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObject ) const;
        virtual void                copyFilterAndSortingTo(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection,const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxObject ) const;
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                    getColumnNames() const;
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                    getPrimaryKeyColumnNames() const;
        virtual OFieldDescription*  createFieldDescription( const ::rtl::OUString& _rColumnName ) const;
        virtual ::rtl::OUString     getSelectStatement() const;
        virtual ::utl::SharedUNOComponent< ::com::sun::star::sdbc::XPreparedStatement >
                                    getPreparedSelectStatement() const;

    private:
        void    impl_ensureColumnInfo_throw();
        ::utl::SharedUNOComponent< ::com::sun::star::sdbc::XPreparedStatement >
                impl_ensureStatement_throw();
    };

    // ========================================================
    // Wizard Dialog
    // ========================================================
    class OCopyTableWizard : public WizardDialog
    {
        friend class		OWizColumnSelect;
        friend class		OWizTypeSelect;
        friend class		OWizTypeSelectControl;
        friend class		OCopyTable;
        friend class		OWizNameMatching;

    public:
        DECLARE_STL_MAP(::rtl::OUString,::rtl::OUString,::comphelper::UStringMixLess,TNameMapping);

        enum Wizard_Button_Style
        {
            WIZARD_NEXT,
            WIZARD_PREV,
            WIZARD_FINISH,

            WIZARD_NONE
        };

    private:
        ODatabaseExport::TColumns		m_vDestColumns; // contains the columns
        ODatabaseExport::TColumnVector	m_aDestVec;		// the order to insert the columns
        ODatabaseExport::TColumns		m_vSourceColumns;
        ODatabaseExport::TColumnVector	m_vSourceVec;

        HelpButton              m_pbHelp;
        CancelButton			m_pbCancel;
        PushButton				m_pbPrev;
        PushButton				m_pbNext;
        OKButton				m_pbFinish;

        OTypeInfoMap				            m_aTypeInfo;
        ::std::vector<OTypeInfoMap::iterator>   m_aTypeInfoIndex;
        OTypeInfoMap				            m_aDestTypeInfo;
        ::std::vector<OTypeInfoMap::iterator>   m_aDestTypeInfoIndex;
        TNameMapping				            m_mNameMapping;

        ODatabaseExport::TPositions	            m_vColumnPos;
        ::std::vector<sal_Int32>	            m_vColumnTypes;

        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >			m_xDestConnection;

        const ICopyTableSourceObject&                                                   m_rSourceObject;

        ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter >	m_xFormatter;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory> m_xFactory;

        String					m_sTypeNames;		// these type names are the ones out of the resource file
        sal_uInt32				m_nPageCount;
        sal_Bool				m_bDeleteSourceColumns;
        bool                    m_bInterConnectionCopy;    // are we copying between different connections?

        ::com::sun::star::lang::Locale	m_aLocale;
        ::rtl::OUString					m_sName;	// for a table the name is composed
        ::rtl::OUString 				m_sSourceName;
        ::rtl::OUString					m_aKeyName;
        TOTypeInfoSP					m_pTypeInfo; // default type
        sal_Bool						m_bAddPKFirstTime;
        sal_Int16                       m_nOperation;
        Wizard_Button_Style	            m_ePressed;
        sal_Bool			            m_bCreatePrimaryKeyColumn;

    private:
        DECL_LINK( ImplPrevHdl	, PushButton* );
        DECL_LINK( ImplNextHdl	, PushButton* );
        DECL_LINK( ImplOKHdl	, OKButton* );
        DECL_LINK( ImplActivateHdl, WizardDialog* );
        sal_Bool CheckColumns(sal_Int32& _rnBreakPos);
        void loadData( const ICopyTableSourceObject& _rSourceObject,
                       ODatabaseExport::TColumns& _rColumns,
                       ODatabaseExport::TColumnVector& _rColVector );
        void construct();
        // need for table creation
        void appendColumns( ::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XColumnsSupplier>& _rxColSup, const ODatabaseExport::TColumnVector* _pVec, sal_Bool _bKeyColumns = sal_False ) const;
        void appendKey(::com::sun::star::uno::Reference< ::com::sun::star::sdbcx::XKeysSupplier>& _rxSup,const ODatabaseExport::TColumnVector* _pVec) const;
        // checks if the type is supported in the destination database
        sal_Bool supportsType(sal_Int32 _nDataType,sal_Int32& _rNewDataType);

        void    impl_loadSourceData();

    public:
        // used for copy tables or queries
        OCopyTableWizard(
            Window * pParent,
            const ::rtl::OUString& _rDefaultName,
            sal_Int16 _nOperation,
            const ICopyTableSourceObject&                                                           _rSourceObject,
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >&			_xSourceConnection,
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >&			_xConnection,
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB
        );

        // used for importing rtf/html sources
        OCopyTableWizard(
            Window* pParent,
            const ::rtl::OUString& _rDefaultName,
            sal_Int16 _nOperation,
            const ODatabaseExport::TColumns& _rDestColumns,
            const ODatabaseExport::TColumnVector& _rSourceColVec,
            const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xConnection,
            const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter >&	_xFormatter,
            TypeSelectionPageFactory _pTypeSelectionPageFactory,
            SvStream& _rTypeSelectionPageArg,
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rM
        );

        virtual ~OCopyTableWizard();

        virtual long		DeactivatePage();
        OKButton&			GetOKButton() { return m_pbFinish; }
        Wizard_Button_Style GetPressedButton() const { return m_ePressed; }
        void				EnableButton(Wizard_Button_Style eStyle,sal_Bool bEnable);
        void				AddWizardPage(OWizardPage* pPage); // Page wird von OCopyTableWizard gel�scht
        void				RemoveWizardPage(OWizardPage* pPage); // Page goes again to user
        void				CheckButtons(); // checks which button can be disabled, enabled

        // returns a vector where the position of a column and if the column is in the selection
        // when not the value is COLUMN_POSITION_NOT_FOUND == (sal_uInt32)-1
        ODatabaseExport::TPositions	GetColumnPositions()	const { return m_vColumnPos; }
        ::std::vector<sal_Int32>	GetColumnTypes()		const { return m_vColumnTypes; }

        void insertColumn(sal_Int32 _nPos,OFieldDescription* _pField);

        /** replaces a field description with another one. The name must not be known so far.
            @param	_nPos
                The pos inside the vector, 0 based.
            @param	_pField
                The field to set.
            @param	_sOldName
                The name of column to be replaced.
        */
        void replaceColumn(sal_Int32 _nPos,OFieldDescription* _pField,const ::rtl::OUString& _sOldName);

        /** returns whether a primary key should be created in the target database
        */
        sal_Bool		shouldCreatePrimaryKey() const;
        void            setCreatePrimaryKey( bool _bDoCreate, const ::rtl::OUString& _rSuggestedName );

        static bool     supportsPrimaryKey( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection );
        bool            supportsPrimaryKey() const { return supportsPrimaryKey( m_xDestConnection ); }

        static bool     supportsViews( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection );
        bool            supportsViews() const { return supportsViews( m_xDestConnection ); }

        /** returns the name of the primary key
            @return
                The name of the primary key.
        */
        ::rtl::OUString	getPrimaryKeyName() const { return m_aKeyName; }

        TOTypeInfoSP		getTypeInfo(sal_Int32 _nPos)		const { return m_aTypeInfoIndex[_nPos]->second; }
        const OTypeInfoMap*	getTypeInfo()						const { return &m_aTypeInfo; }

        TOTypeInfoSP		getDestTypeInfo(sal_Int32 _nPos)	const { return m_aDestTypeInfoIndex[_nPos]->second; }
        const OTypeInfoMap*	getDestTypeInfo()					const { return &m_aDestTypeInfo; }

        ::com::sun::star::lang::Locale	GetLocale() const { return m_aLocale; }
        ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter > GetFormatter() const { return m_xFormatter; }
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory> GetFactory() const { return m_xFactory; }

        const ODatabaseExport::TColumns*		getSourceColumns() const{ return &m_vSourceColumns; }
        const ODatabaseExport::TColumnVector*	getSrcVector() const	{ return &m_vSourceVec; }
        ODatabaseExport::TColumns*				getDestColumns()		{ return &m_vDestColumns; }
        const ODatabaseExport::TColumnVector*	getDestVector() const	{ return &m_aDestVec; }
        ::rtl::OUString	getName() const { return m_sName; }

        /** clears the dest vectors
        */
        void clearDestColumns();

        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createTable();
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > createView() const;
        sal_Int32 getMaxColumnNameLength() const;

        void setOperation( const sal_Int16 _nOperation );
        sal_Int16 getOperation() const;

        ::rtl::OUString convertColumnName(	const TColumnFindFunctor&	_rCmpFunctor,
                                            const ::rtl::OUString&	_sColumnName,
                                            const ::rtl::OUString&	_sExtraChars,
                                            sal_Int32				_nMaxNameLen);
        TOTypeInfoSP convertType(const TOTypeInfoSP&_pType,sal_Bool& _bNotConvert);

        ::rtl::OUString createUniqueName(const ::rtl::OUString& _sName);

        // displays a error message that a column type is not supported
        void showColumnTypeNotSupported(const ::rtl::OUString& _rColumnName);

        void removeColumnNameFromNameMap(const ::rtl::OUString& _sName);
    };
}

#endif // DBAUI_WIZ_COPYTABLEDIALOG_HXX
