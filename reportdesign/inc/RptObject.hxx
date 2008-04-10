/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: RptObject.hxx,v $
 * $Revision: 1.6 $
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
#ifndef _REPORT_RPTUIOBJ_HXX
#define _REPORT_RPTUIOBJ_HXX

#include <svx/svdoole2.hxx>
#include <svx/svdouno.hxx>


#include <comphelper/processfactory.hxx>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/report/XReportComponent.hpp>
#include <com/sun/star/report/XSection.hpp>
#include <svx/svdocirc.hxx>
#include <svx/svdogrp.hxx>
#include <svx/svdoashp.hxx>
#include <comphelper/stl_types.hxx>
#include <comphelper/implementationreference.hxx>
#include "dllapi.h"

namespace rptui
{
typedef ::std::multimap< sal_Int16, ::rtl::OUString, ::std::less< sal_Int16 > > IndexToNameMap;
    enum DlgEdHintKind
    {
        RPTUI_HINT_UNKNOWN,
        RPTUI_HINT_WINDOWSCROLLED,
        RPTUI_HINT_LAYERCHANGED,
        RPTUI_HINT_OBJORDERCHANGED,
        RPTUI_HINT_SELECTIONCHANGED
    };

    class OUnoObject;
    class REPORTDESIGN_DLLPUBLIC DlgEdHint: public SfxHint
    {
    private:
        DlgEdHintKind	eHintKind;
        OUnoObject*		pDlgEdObj;

        DlgEdHint(DlgEdHint&);
        void operator =(DlgEdHint&);
    public:
        TYPEINFO();
        DlgEdHint( DlgEdHintKind eHint );
        DlgEdHint( DlgEdHintKind eHint, OUnoObject* pObj );
        virtual ~DlgEdHint();

        inline DlgEdHintKind	GetKind() const	{ return eHintKind; }
        inline OUnoObject*		GetObject() const { return pDlgEdObj; }
    };


class OReportPage;
class OPropertyMediator;

class REPORTDESIGN_DLLPUBLIC OObjectBase
{
public:
    typedef ::comphelper::ImplementationReference<OPropertyMediator,::com::sun::star::beans::XPropertyChangeListener> TMediator;

protected:
    mutable TMediator                                                                           m_xMediator;
    mutable ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener> m_xPropertyChangeListener;
    //mutable ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener> 
    mutable ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>		m_xReportComponent;
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener>	        m_xContainerListener;
    ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection>				        m_xSection;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >                       m_xKeepShapeAlive;
    ::rtl::OUString m_sComponentName;
    sal_Bool		m_bIsListening;

    OObjectBase(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent);
    OObjectBase(const ::rtl::OUString& _sComponentName);

    virtual ~OObjectBase();	
    
    inline sal_Bool	isListening() const { return m_bIsListening; }

    void SetPropsFromRect(const Rectangle& _rRect);
    void PositionAndSizeChange( const ::com::sun::star::beans::PropertyChangeEvent& evt );

    virtual void SetSnapRectImpl(const Rectangle& _rRect) = 0;
    virtual SdrPage* GetImplPage() const = 0;
    virtual void SetObjectItemHelper(const SfxPoolItem& rItem);
    sal_Bool IsInside(const Rectangle& _rRect,const Point& rPnt,USHORT nTol) const;

    /** called by instances of derived classes to implement their overloading of getUnoShape
    */
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
            getUnoShapeOf( SdrObject& _rSdrObject );

private:
    static void    ensureSdrObjectOwnership(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxShape );

public:
    void StartListening();
    void EndListening(sal_Bool bRemoveListener = sal_True);
    // PropertyChangeListener
    virtual void _propertyChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw(::com::sun::star::uno::RuntimeException);

    // ContainerListener
    void _elementInserted( const ::com::sun::star::container::ContainerEvent& Event ) throw(::com::sun::star::uno::RuntimeException);
    void _elementReplaced( const ::com::sun::star::container::ContainerEvent& Event ) throw(::com::sun::star::uno::RuntimeException);
    void _elementRemoved( const ::com::sun::star::container::ContainerEvent& Event ) throw(::com::sun::star::uno::RuntimeException);

    sal_Bool		supportsService( const ::rtl::OUString& _sServiceName ) const;	

    ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent> getReportComponent() const;
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet> getAwtComponent();
    inline void setOldParent(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection>& _xSection) { m_xSection = _xSection; }
    inline ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection> getOldParent() const { return m_xSection;}
    ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection> getSection() const;
    inline const ::rtl::OUString getServiceName() const { return m_sComponentName; }

    /** releases the reference to our UNO shape (m_xKeepShapeAlive)
    */
    void    releaseUnoShape() { m_xKeepShapeAlive.clear(); }

    static SdrObject* createObject(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent);
    static sal_uInt16 getObjectType(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent);
};
//============================================================================
// OCustomShape
//============================================================================
class REPORTDESIGN_DLLPUBLIC OCustomShape: public SdrObjCustomShape , public OObjectBase
{
    friend class OReportPage;
    friend class DlgEdFactory;

public:
    static OCustomShape* Create( const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent )
    {
        return new OCustomShape( _xComponent );
    }

protected:
    OCustomShape(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent);
    OCustomShape(const ::rtl::OUString& _sComponentName);

    virtual void NbcMove( const Size& rSize );
    virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
    virtual void NbcSetLogicRect(const Rectangle& rRect);
    virtual FASTBOOL EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd);

    virtual void SetSnapRectImpl(const Rectangle& _rRect);
    virtual SdrPage* GetImplPage() const;
    void SetObjectItemHelper(const SfxPoolItem& rItem);

public:
    TYPEINFO();

    virtual ~OCustomShape();
    
    virtual sal_Int32	GetStep() const;

    virtual SdrObject* CheckHit(const Point& rPnt,USHORT nTol,const SetOfByte*) const;
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet> getAwtComponent();

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getUnoShape();
};

//============================================================================
// OOle2Obj
//============================================================================
class REPORTDESIGN_DLLPUBLIC OOle2Obj: public SdrOle2Obj , public OObjectBase
{
    friend class OReportPage;
    friend class DlgEdFactory;

public:
    static OOle2Obj* Create( const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent )
    {
        return new OOle2Obj( _xComponent );
    }
    OOle2Obj(const ::rtl::OUString& _sComponentName,const svt::EmbeddedObjectRef& rNewObjRef, const String& rNewObjName, const Rectangle& rNewRect, FASTBOOL bFrame_=FALSE);

protected:
    OOle2Obj(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent);
    OOle2Obj(const ::rtl::OUString& _sComponentName);
    

    virtual void NbcMove( const Size& rSize );
    virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
    virtual void NbcSetLogicRect(const Rectangle& rRect);
    virtual FASTBOOL EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd);

    virtual void SetSnapRectImpl(const Rectangle& _rRect);
    virtual SdrPage* GetImplPage() const;

public:
    TYPEINFO();

    virtual ~OOle2Obj();
    
    virtual sal_Int32	GetStep() const;

    virtual SdrObject* CheckHit(const Point& rPnt,USHORT nTol,const SetOfByte*) const;
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet> getAwtComponent();

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getUnoShape();
};

//============================================================================
// OUnoObject
//============================================================================
class REPORTDESIGN_DLLPUBLIC OUnoObject: public SdrUnoObj , public OObjectBase
{
    friend class OReportPage;
    friend class OObjectBase;
    friend class DlgEdFactory;

    sal_uInt16   m_nObjectType;
protected:
    OUnoObject(	 const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent
                ,const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& _xControlModel
                ,sal_uInt16   _nObjectType);
    OUnoObject(const ::rtl::OUString& _sComponentName
                ,const ::rtl::OUString& rModelName
                ,sal_uInt16   _nObjectType);
    OUnoObject(	 const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent>& _xComponent
                ,const ::rtl::OUString& rModelName
                ,sal_uInt16   _nObjectType);

    virtual ~OUnoObject();
    
    virtual void NbcMove( const Size& rSize );
    virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
    virtual void NbcSetLogicRect(const Rectangle& rRect);
    virtual FASTBOOL EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd);

    virtual void SetSnapRectImpl(const Rectangle& _rRect);
    virtual SdrPage* GetImplPage() const;

public:
    TYPEINFO();

    virtual sal_Int32	GetStep() const;
    
    virtual SdrObject* CheckHit(const Point& rPnt,USHORT nTol,const SetOfByte*) const;
    virtual void _propertyChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw(::com::sun::star::uno::RuntimeException);

    /** creates the m_xMediator when it doesn't already exist.
        @param	_bReverse	when set to <TRUE/> then the properties from the uno control will be copied into report control
    */
    void CreateMediator(sal_Bool _bReverse = sal_False);

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet> getAwtComponent();

    inline sal_uInt16 getObjectId() const { return m_nObjectType; }

    static ::rtl::OUString GetDefaultName(const OUnoObject* _pObj);

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getUnoShape();
};

//============================================================================
} // rptui
//============================================================================
#endif // _REPORT_RPTUIOBJ_HXX

