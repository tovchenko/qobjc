/*
    This file is part of the q-objc runtime library.
    Copyright (C) 2008-2009 Taras Tovchenko Victorovich <doctorset@gmail.com>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "classTest.h"


Q_ERROR_MESSAGE(tm1, "Don\'t work \"selectorByName()\"!\n");
Q_ERROR_MESSAGE(tm2, "Don\'t work \"isMappedSelector()\"!\n");
Q_ERROR_MESSAGE(tm3, "Don\'t work \"nameBySelector()\"!\n");
Q_ERROR_MESSAGE(tm4, "Don\'t work \"classByName()\"!\n");
Q_ERROR_MESSAGE(tm5, "Can\'t find method in lookupMethod... functions!\n");


void
runClassTest() {
    AutoreleasePool pool = q_allocAutoreleasePool();

    simpleCreationTest();
    selectorTest();
    // TODO: need enable when will write Object class
    // registerClassTest();

    Q_RELEASE(pool);
}

void
simpleCreationTest() {
    Class   cls1 = q_allocClass(_T("CustomCls1"));
    Ivar    ivar1 = q_allocIvar(_T("_variable1"), "i");
    SEL     sel = q_allocSelector(_T("firstSel"));
    Method  meth1 = q_allocMethod(sel, (IMP)0x1, "v+i+^d");


    
    Q_RELEASE(meth1);
    Q_RELEASE(sel);
    Q_RELEASE(ivar1);
    Q_RELEASE(cls1);
}

void
selectorTest() {
    const TCHAR* tName;
    SEL tSel;
    SEL s1 = q_allocSelector(_T("addSomeObject:"));
    SEL s2 = q_allocSelector(_T("buttonPush"));
    SEL s3 = q_allocSelector(_T("someSel"));

    q_registerSelector(s2);
    q_registerSelector(s3);

    tSel = q_selectorByName(_T("addSomeObject:"));
    if (tSel) testFailure(tm1);

    tSel = q_selectorByName(_T("buttonPush"));
    if (!tSel) testFailure(tm1);

    if (q_isMappedSelector(s1) || !q_isMappedSelector(s3)) testFailure(tm2);

    tName = q_nameBySelector(s1);
    if (tName) testFailure(tm3);

    tName = q_nameBySelector(s2);
    if (_tcscmp(tName, _T("buttonPush"))) testFailure(tm3);

    Q_RELEASE(s1);
    Q_RELEASE(s2);
    Q_RELEASE(s3);
}

void
registerClassTest() {
    Class   tst;
    Class   notReg = q_allocClass(_T("NotRegister"));
    Class   baseCls = q_allocClass(_T("Base"));
    Class   deriv1Cls = q_allocClass(_T("Der1"));
    Class   deriv2Cls = q_allocClass(_T("Der2"));
    Class   secondDer = q_allocClass(_T("SecLevel"));

    q_registerClass(baseCls, Q_CLASS_OBJ(Object));
    q_registerClass(deriv1Cls, baseCls);
    q_registerClass(deriv2Cls, baseCls);
    q_registerClass(secondDer, deriv2Cls);

    tst = q_classByName(_T("NotRegister"));
    if (tst) testFailure(tm4);
    tst = q_classByName(_T("Der1"));
    if (!tst) testFailure(tm4);
    tst = q_classByName(_T("SecLevel"));
    if (!tst) testFailure(tm4);

    methodsLookupTest(baseCls, deriv2Cls, secondDer);

    Q_RELEASE(secondDer);
    Q_RELEASE(deriv2Cls);
    Q_RELEASE(deriv1Cls);
    Q_RELEASE(baseCls);
    Q_RELEASE(notReg);
}

void
methodsLookupTest(Class cls1, Class cls2, Class cls3) {
    SEL sel1 = q_allocSelector(_T("Sel1"));
    SEL sel2 = q_allocSelector(_T("Sel2"));
    SEL sel3 = q_allocSelector(_T("Sel3"));
    
    q_registerSelector(sel1);
    q_registerSelector(sel2);
    q_registerSelector(sel3);

    {
        Method tst;
        Method m1 = q_allocMethod(sel1, (IMP)0x1, "+v");
        Method m2 = q_allocMethod(sel2, (IMP)0x1, "+v");
        Method m3 = q_allocMethod(sel3, (IMP)0x1, "+v");
        MethodListArray mla1 = q_allocMethodListArray();
        MethodListArray mla2 = q_allocMethodListArray();
        MethodListArray mla3 = q_allocMethodListArray();
        MethodList  mlist1 = q_allocMethodList();
        MethodList  mlist2 = q_allocMethodList();
        MethodList  mlist3 = q_allocMethodList();

        q_addItemToMethodListArray(mlist1, mla1);
        q_addItemToMethodListArray(mlist2, mla2);
        q_addItemToMethodListArray(mlist3, mla3);

        q_addItemToMethodList(m1, mlist1);
        q_addItemToMethodList(m2, mlist2);
        q_addItemToMethodList(m3, mlist3);

        q_setMethodListArray(cls1, mla1);
        q_setMethodListArray(cls2, mla2);
        // setMethodListArray(cls3, mla3);

        tst = q_lookupMethodInMethodList(sel2, mlist2);
        if (!tst || _tcscmp(tst->sel->name, _T("Sel2"))) testFailure(tm5);
        tst = q_lookupMethodInMethodListArray(sel2, mla2);
        if (!tst || _tcscmp(tst->sel->name, _T("Sel2"))) testFailure(tm5);
        tst = q_lookupMethodInClassHierarchy(sel1, cls3);
        if (!tst || _tcscmp(tst->sel->name, _T("Sel1"))) testFailure(tm5);


        Q_RELEASE(mlist1);
        Q_RELEASE(mlist2);
        Q_RELEASE(mlist3);
        Q_RELEASE(mla1);
        Q_RELEASE(mla2);
        Q_RELEASE(mla3);
        Q_RELEASE(m1);
        Q_RELEASE(m2);
        Q_RELEASE(m3);
    }

    Q_RELEASE(sel1);
    Q_RELEASE(sel2);
    Q_RELEASE(sel3);
}