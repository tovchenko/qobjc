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
#include "msgsendTest.h"

Q_ERROR_MESSAGE(tm1, "Send message functionality error!\n");
Q_ERROR_MESSAGE(tm2, "Ivar lookup functionality error!\n");
Q_ERROR_MESSAGE(tm3, "Calculate of object size error!\n");

const int kTstVal = 39;

struct TestStr1 {
    int a;
    int b;
};

struct TestStr2 {
    double c;
    float f;
    struct TestStr1 st;
    int i;
};

static void             _firstMethod(id obj, SEL _cmd);
static void             _secondMethod(id obj, SEL _cmd, int x);
static int              _thirdMethod(id obj, SEL _cmd, double d);
static struct TestStr1  _fouthMethod(id obj, SEL _cmd);
static struct TestStr2  _fifthMethod(id obj, SEL _cmd);
static float            _sixthMethod(id obj, SEL _cmd, float z0, double d, float z1, float z2, int i);

static void     _getClasses(Class* cls1, Class* cls2, Class* cls3, Class* cls4);
static void     _getMethods(Method* m1, Method* m2, Method* m3, Method* m4, Method* m5, Method* m6);
static void     _getIvars(Ivar* iv1, Ivar* iv2, Ivar* iv3, Ivar* iv4);


void
runMsgSendTest() {
    AutoreleasePool pool = q_allocAutoreleasePool();

    msgSend1Test();

    Q_RELEASE(pool);
}

void
msgSend1Test() {
    Class   baseCls, derivCls1, derivCls2, derivClsDeriv;
    Method  m1, m2, m3, m4, m5, m6;
    Ivar    iv1, iv2, iv3, iv4, tstIv;
    id obj = q_allocZone(sizeof(struct _Object) + sizeof(int) + sizeof(struct TestStr1));
    SEL sel;

    _getClasses(&baseCls, &derivCls1, &derivCls2, &derivClsDeriv);
    _getMethods(&m1, &m2, &m3, &m4, &m5, &m6);
    _getIvars(&iv1, &iv2, &iv3, &iv4);

    {
        IvarArray iva1 = q_allocIvarArray();
        IvarArray iva2 = q_allocIvarArray();
        IvarArray iva3 = q_allocIvarArray();

        q_setIvarArray(baseCls, iva1);
        q_setIvarArray(baseCls->isa, iva2);
        q_setIvarArray(derivCls2, iva3);

        Q_RELEASE(iva1);
        Q_RELEASE(iva2);
        Q_RELEASE(iva3);
    }

    q_addItemToIvarArray(iv1, q_ivarArray(baseCls));
    q_addItemToIvarArray(iv2, q_ivarArray(baseCls->isa));
    q_addItemToIvarArray(iv3, q_ivarArray(baseCls));
    q_addItemToIvarArray(iv4, q_ivarArray(derivCls2));

    tstIv = q_lookupIvarInIvarArray(_T("_variable1"), q_ivarArray(baseCls));
    if (!tstIv || _tcscmp(_T("_variable1"), tstIv->name)) testFailure(tm2);
    tstIv = q_lookupIvarInClassHierarchy(_T("_variable1"), derivClsDeriv, nil);
    if (!tstIv || _tcscmp(_T("_variable1"), tstIv->name)) testFailure(tm2);
    tstIv = q_lookupIvarInClassHierarchy(_T("_myFlt"), derivClsDeriv->isa, nil);
    if (!tstIv || _tcscmp(_T("_myFlt"), tstIv->name)) testFailure(tm2);

    q_addItemToMethodList(m1, q_itemAtIndexInMethodListArray(0, q_methodListArray(baseCls)));
    q_addItemToMethodList(m3, q_itemAtIndexInMethodListArray(2, q_methodListArray(baseCls)));
    q_addItemToMethodList(m2, q_itemAtIndexInMethodListArray(0, q_methodListArray(baseCls)));
    q_addItemToMethodList(m4, q_itemAtIndexInMethodListArray(0, q_methodListArray(derivClsDeriv)));
    q_addItemToMethodList(m5, q_itemAtIndexInMethodListArray(0, q_methodListArray(derivCls2)));
    q_addItemToMethodList(m6, q_itemAtIndexInMethodListArray(1, q_methodListArray(derivCls2)));

    obj->isa = derivClsDeriv;

    {
        int ss = q_sizeOfObject(obj);
        int d2s = q_sizeOfObject((id)derivCls2); 
        int bs = q_sizeOfObject((id)baseCls);
        int     ival = 7;
        float   fval = 15.2f;
        struct TestStr1 st1 = { 54, 33 };

        if (ss != 32) testFailure(tm3);
        if (d2s != sizeof(struct _Class) || bs != sizeof(struct _Class))
            testFailure(tm3);

        q_setValueToIvar(obj, _T("_variable1"), &ival);
        q_setValueToIvar(obj, _T("_myStruct"), &st1);
        // setValueToIvar((id)obj->isa, _T("_myFlt"), &fval);
    }

    sel = Q_SELECTOR(firstMethod:);
    q_send(obj, sel);

    sel = Q_SELECTOR(thirdMethod:);
    if ((int)q_send(obj, sel, 5.0) != (kTstVal + 5)) testFailure(tm1);

    sel = Q_SELECTOR(secondMethod:);
    q_send(obj, sel, kTstVal);

    {
        struct TestStr1 st1;
        struct TestStr2 st2;

        sel = Q_SELECTOR(fouthMethod:);
        q_sendSt(&st1, obj, sel);
        if (st1.a != 56 || st1.b != 65) testFailure(tm1);

        sel = Q_SELECTOR(fifthMethod:);
        q_sendSt(&st2, obj, sel);
        if (st2.c != 6.0 || st2.f != 5.0f || st2.i != 8 || st2.st.a != 34 || st2.st.b != 43)
            testFailure(tm1);
    }

    {
        int                 ival2;
        int*                pI;
        struct TestStr1*    pSt1;
        // float   fval2;
        // float*  pF;

        q_getValueFromIvar(obj, _T("_variable1"), &pI);
        q_getValueFromIvar(obj, _T("_myStruct"), &pSt1);
        //getValueFromIvar((id)obj->isa, _T("_myFlt"), &pF);

        ival2 = *pI;
        // fval2 = *pF;

        if (ival2 != 7 || !(pSt1->a == 54 && pSt1->b == 33)) testFailure(tm2);
    }


    q_freeZone(obj);
}

static void
_getClasses(Class* cls1, Class* cls2, Class* cls3, Class* cls4) {
    *cls1 = q_allocClass(_T("BaseClass"));
    *cls2 = q_allocClass(_T("Deriv1"));
    *cls3 = q_allocClass(_T("Deriv2"));
    *cls4 = q_allocClass(_T("DerivDeriv"));

    {
        MethodListArray mla1 = q_allocMethodListArray();
        MethodListArray mla2 = q_allocMethodListArray();
        MethodListArray mla3 = q_allocMethodListArray();
        MethodListArray mla4 = q_allocMethodListArray();

        q_setMethodListArray(*cls1, mla1);
        q_setMethodListArray(*cls2, mla3);
        q_setMethodListArray(*cls2, mla2);
        q_setMethodListArray(*cls3, mla3);
        q_setMethodListArray(*cls4, mla4);

        {
            MethodList ml11 = q_allocMethodList();
            MethodList ml12 = q_allocMethodList();
            MethodList ml13 = q_allocMethodList();
            MethodList ml21 = q_allocMethodList();
            MethodList ml31 = q_allocMethodList();
            MethodList ml32 = q_allocMethodList();
            MethodList ml41 = q_allocMethodList();

            q_addItemToMethodListArray(ml11, mla1);
            q_addItemToMethodListArray(ml12, mla1);
            q_addItemToMethodListArray(ml13, mla1);
            q_addItemToMethodListArray(ml21, mla2);
            q_addItemToMethodListArray(ml31, mla3);
            q_addItemToMethodListArray(ml32, mla3);
            q_addItemToMethodListArray(ml41, mla4);

            Q_RELEASE(ml11);
            Q_RELEASE(ml12);
            Q_RELEASE(ml13);
            Q_RELEASE(ml21);
            Q_RELEASE(ml31);
            Q_RELEASE(ml32);
            Q_RELEASE(ml41);
        }

        Q_RELEASE(mla1);
        Q_RELEASE(mla2);
        Q_RELEASE(mla3);
        Q_RELEASE(mla4);
    }

    q_registerClass(*cls1, Q_CLASS_OBJ(Object));
    q_registerClass(*cls2, *cls1);
    q_registerClass(*cls3, *cls1);
    q_registerClass(*cls4, *cls3);

    Q_AUTORELEASE(*cls1);
    Q_AUTORELEASE(*cls2);
    Q_AUTORELEASE(*cls3);
    Q_AUTORELEASE(*cls4);
}

static void
_getMethods(Method* m1, Method* m2, Method* m3, Method* m4, Method* m5, Method* m6) {
    SEL s1 = q_allocSelector(_T("firstMethod:"));
    SEL s2 = q_allocSelector(_T("secondMethod:"));
    SEL s3 = q_allocSelector(_T("thirdMethod:"));
    SEL s4 = q_allocSelector(_T("fouthMethod:"));
    SEL s5 = q_allocSelector(_T("fifthMethod:"));
    SEL s6 = q_allocSelector(_T("six"));

    *m1 = q_allocMethod(s1, _firstMethod, "+v");
    *m2 = q_allocMethod(s2, _secondMethod, "+v+i");
    *m3 = q_allocMethod(s3, _thirdMethod, "+i+d");
    *m4 = q_allocMethod(s4, _fouthMethod, "+{TestStr1=ii}");
    *m5 = q_allocMethod(s5, _fifthMethod, "+{TestStr2=df{TestStr1=ii}i}");
    *m6 = q_allocMethod(s6, _sixthMethod, "+f+f+d+f+f+i");

    q_registerSelector(s1);
    q_registerSelector(s2);
    q_registerSelector(s3);
    q_registerSelector(s4);
    q_registerSelector(s5);
    q_registerSelector(s6);

    Q_RELEASE(s1);
    Q_RELEASE(s2);
    Q_RELEASE(s3);
    Q_RELEASE(s4);
    Q_RELEASE(s5);
    Q_RELEASE(s6);

    Q_AUTORELEASE(*m1);
    Q_AUTORELEASE(*m2);
    Q_AUTORELEASE(*m3);
    Q_AUTORELEASE(*m4);
    Q_AUTORELEASE(*m5);
    Q_AUTORELEASE(*m6);
}

static void
_getIvars(Ivar* iv1, Ivar* iv2, Ivar* iv3, Ivar* iv4) {
    *iv1 = q_allocIvar(_T("_variable1"), "i");
    *iv2 = q_allocIvar(_T("_myFlt"), "f");
    *iv3 = q_allocIvar(_T("_myStruct"), "{TestStr1=ii}");
    *iv4 = q_allocIvar(_T("_var2"), "d");

    Q_AUTORELEASE(*iv1);
    Q_AUTORELEASE(*iv2);
    Q_AUTORELEASE(*iv3);
    Q_AUTORELEASE(*iv4);
}




// test methods

static void
_firstMethod(id obj, SEL _cmd) {
    float val;
    testLog(_T("Invoke 1-st method!\n"));

    val = (float)q_sendFp(obj, Q_SELECTOR(six), 12.4f, 5.6, 13.5f, 14.6f, 4);
    if (val != 23.07f) testFailure(tm1);
}

static void
_secondMethod(id obj, SEL _cmd, int x) {
    testLog(_T("Invoke 2-nd method!\n"));
}

static int
_thirdMethod(id obj, SEL _cmd, double d) {
    testLog(_T("Invoke 3-rd method!\n"));
    return (kTstVal + (int)d);
}

static struct TestStr1
_fouthMethod(id obj, SEL _cmd) {
    struct TestStr1 st1 = { 56, 65 };
    testLog(_T("Invoke 4-th method!\n"));
    return st1;
}

static struct TestStr2
_fifthMethod(id obj, SEL _cmd) {
    struct TestStr1 st1 = { 34, 43 };
    struct TestStr2 st2;

    testLog(_T("Invoke 5-th method!\n"));

    st2.c = 6.0;
    st2.f = 5.0f;
    st2.i = 8;
    st2.st = st1;

    return st2;
}

static float
_sixthMethod(id obj, SEL _cmd, float z0, double d, float z1, float z2, int i) {
    double a = 5.0;
    float v = 5.67f;

    return (float)a + v + z0;
}