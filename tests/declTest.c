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
#include "declTest.h"

#include <stdarg.h>


#pragma data_seg(".CRT$XIU")
Q_PUT_LOAD_MODULE
#pragma data_seg()
Q_BEGIN_MODULE
    Q_SYMBOL_INFO(ThirdC, thirdClass, Q_CLASS_TYPE)
    Q_SYMBOL_INFO(MyDecl, firstClass, Q_CLASS_TYPE)
    Q_SYMBOL_INFO(QMyProto, loadProtocol, Q_PROTOCOL_TYPE)
    Q_SYMBOL_INFO(QCoping, loadProtocol2, Q_PROTOCOL_TYPE)
    Q_SYMBOL_INFO(QMyProto2, loadProtocol3, Q_PROTOCOL_TYPE)
    Q_SYMBOL_INFO(QTestProto, loadProtocol4, Q_PROTOCOL_TYPE)
    Q_SYMBOL_INFO(QSecondProto, loadProtocol5, Q_PROTOCOL_TYPE)
    Q_SYMBOL_INFO(QCoding, loadProtocol6, Q_PROTOCOL_TYPE)
    Q_SYMBOL_INFO(MyDecl, loadCategory1, Q_CATEGORY_TYPE)
    // Q_SYMBOL_INFO(Object, loadTestCategory, Q_CATEGORY_TYPE)
Q_END_MODULE


Q_ERROR_MESSAGE(tm1, "DECLTEST: Don\'t work class ivars!\n");
Q_ERROR_MESSAGE(tm2, "DECLTEST: Don\'t send message!\n");
Q_ERROR_MESSAGE(tm3, "DECLTEST: Don\'t work protocols functionality!\n");

Q_DECLARE_CLASS(ThirdC);

struct SType {
    float   x;
    float   y;
    double  z;
};

struct TwoInt {
    int i;
    int j;
};

struct OneInt {
    int i;
};

static      int             _simpleMeth(id self, SEL _cmd, int val);
static      float           _calcAreaXY(id self, SEL _cmd, float x, float y, double d, float x1, float y1);
static      id              _allocObj(id self, SEL _cmd);
static      struct TwoInt   _secM(id self, SEL _cmd, char val, float f1, double d, float f2);
static      struct OneInt   _secInt(id self, SEL _cmd,  float f1, double d, float f2);
static      struct SType    _secDiff(id self, SEL _cmd,  float f1, double d, float f2);
static      void            _dealloc1(id self, SEL _cmd);
static      void            _dealloc2(id self, SEL _cmd);
static      void            _superTest1(id self, SEL _cmd);
static      void            _superTest2(id self, SEL _cmd);
static      int             _variableParams(id self, SEL _cmd, int a, float b, float c, double d, float e, ...);
static      float           _calcAreaXYCategory(id self, SEL _cmd, float x, float y, double d, float x1, float y1);
static      id              _customAlloc();



void
runDeclTest() {
    int val = nil;
    // firstClass();
    secondClass();
    thirdClass();

    workWithClasses();
    baseProtocolTest();
    memoryTest();
}

int
loadProtocol() {
    Q_BEGIN_PROTOCOL(QMyProto)
    Q_END_PROTOCOL
}

int
loadProtocol2() {
    Q_BEGIN_PROTOCOL(QCoping)
        Q_BEGIN_INSTANCE_DESC
            Q_DEF_METHOD_DESC(init, +@)
            Q_DEF_METHOD_DESC(alloc, +@)
        Q_END
        Q_BEGIN_CLASS_DESC
            Q_DEF_METHOD_DESC(initWithName:, +@+*)
        Q_END
   Q_END_PROTOCOL
}

int
loadProtocol3() {
    Q_BEGIN_PROTOCOL(QMyProto2)
        Q_INCLUDE_PROTOCOL_TO_PROTOCOL(QMyProto)
    Q_END_PROTOCOL
}

int
loadProtocol4() {
    Q_BEGIN_PROTOCOL(QTestProto)
    Q_END_PROTOCOL
}

int
loadProtocol5() {
    Q_BEGIN_PROTOCOL(QSecondProto)
    Q_END_PROTOCOL
}

int
loadProtocol6() {
    Q_BEGIN_PROTOCOL(QCoding)
        Q_INCLUDE_PROTOCOL_TO_PROTOCOL(QCoping)
        Q_BEGIN_INSTANCE_DESC
            Q_DEF_METHOD_DESC(secM, +{TwoInt=ii}+c+f+d+f)
            Q_DEF_METHOD_DESC(allocObject, +v)
            Q_DEF_METHOD_DESC(secDiff, +{SType=ffd}+f+d+f)
        Q_END
        Q_BEGIN_CLASS_DESC
            Q_DEF_METHOD_DESC(registerProtocol:, +v+@)
        Q_END
    Q_END_PROTOCOL
}

int
firstClass() {
    Q_BEGIN_CLASS(MyDecl, BaseClass)
        Q_INCLUDE_PROTOCOL_TO_CLASS(QMyProto2)
        Q_BEGIN_IVAR_LIST
            Q_DEF_IVAR(_someInt, i)
            Q_DEF_IVAR(_someDouble, d)
            Q_DEF_IVAR(_someFloat, f)
        Q_END
        Q_BEGIN_INSTANCE_METHOD_LIST
            Q_DEF_METHOD(calculateAreaX:Y:, _calcAreaXY, +f+f+f+d+f+f)
            Q_DEF_METHOD(dealloc, _dealloc1, +v)
        Q_END_INSTANCE_METHOD_LIST
    Q_END_CLASS
}

int
secondClass() {
    Q_BEGIN_CLASS(SecLevel, MyDecl)
        Q_BEGIN_IVAR_LIST
            Q_DEF_IVAR(_setUI, I);
            Q_DEF_IVAR(_secS, {SType=ffd})
            Q_DEF_IVAR(_selec, :)
        Q_END
        Q_BEGIN_CLASS_METHOD_LIST
            Q_DEF_METHOD(allocObject, _allocObj, +v)
            Q_DEF_METHOD(secM, _secM, +{TwoInt=ii}+c+f+d+f)
            Q_DEF_METHOD(secInt, _secInt, +{OneInt=i}+f+d+f)
            Q_DEF_METHOD(secDiff, _secDiff, +{SType=ffd}+f+d+f)
        Q_END_CLASS_METHOD_LIST
        Q_BEGIN_INSTANCE_METHOD_LIST
            // Q_DEF_METHOD(calculateAreaX:Y:, _calcAreaXY, +f+f+f+d+f+f)
            Q_DEF_METHOD(varParams, _variableParams, +i+i+f+f+d+f)
            Q_DEF_METHOD(superTest, _superTest1, +v)
            Q_DEF_METHOD(simpleMeth:, _simpleMeth, +v+i)
            Q_DEF_METHOD(dealloc, _dealloc2, +v)
        Q_END_INSTANCE_METHOD_LIST
    Q_END_CLASS
}

int
loadCategory1() {
    Q_BEGIN_CATEGORY(MyDecl, Private)
        Q_BEGIN_INSTANCE_METHOD_LIST
            Q_DEF_METHOD(calculateAreaX:Y:, _calcAreaXYCategory, +f+f+f+d+f+f)
        Q_END_INSTANCE_METHOD_LIST
    Q_END_CATEGORY
}

int
loadTestCategory() {
    Q_BEGIN_CATEGORY(Object, Private)
        Q_BEGIN_CLASS_METHOD_LIST
            Q_DEF_METHOD(alloc, _customAlloc, +@)
        Q_END_CLASS_METHOD_LIST
    Q_END_CATEGORY
}

int
thirdClass() {
    Q_BEGIN_CLASS(ThirdC, SecLevel)
        Q_INCLUDE_PROTOCOL_TO_CLASS(QTestProto)
        Q_INCLUDE_PROTOCOL_TO_CLASS(QSecondProto)
        Q_BEGIN_IVAR_LIST
            Q_DEF_IVAR(_count, i)
            Q_DEF_IVAR(_size, f)
        Q_END
        Q_BEGIN_INSTANCE_METHOD_LIST
            Q_DEF_METHOD(superTest, _superTest2, +v)
        Q_END_INSTANCE_METHOD_LIST
    Q_END_CLASS  
}

void
declProtocols() {
    {
        BOOL isConform = (BOOL)q_send(Q_PROTOCOL(QCoding), Q_SELECTOR(conformsTo:), Q_PROTOCOL(QCoping));
        if (!isConform) testFailure(tm3);
        isConform = (BOOL)q_send(Q_PROTOCOL(QCoping), Q_SELECTOR(conformsTo:), Q_PROTOCOL(QCoding));
        if (isConform) testFailure(tm3);
    }
    q_addProtocolToClass(Q_PROTOCOL(QCoping), Q_CLASS_OBJ(SecLevel));

    {
        id obj = q_send(q_send(Q_CLASS(ThirdC), Q_SELECTOR(alloc)), Q_SELECTOR(init));

        BOOL isConform = (BOOL)q_send(Q_CLASS(ThirdC), Q_SELECTOR(conformsTo:), Q_PROTOCOL(QSecondProto));
        if (!isConform) testFailure(tm3);

        isConform = (BOOL)q_send(obj, Q_SELECTOR(conformsTo:), Q_PROTOCOL(QMyProto));
        if (!isConform) testFailure(tm3);

        Q_RELEASE(obj);
    }
}

void
baseProtocolTest() {
    MethodDescription md1;
    int tst = NO;
    Protocol*   prt1 = q_send(q_send(Q_CLASS(Protocol), Q_SELECTOR(alloc)), Q_SELECTOR(initWithName:), _T("FirstProto"));
    Protocol*   prt2 = q_send(q_send(Q_CLASS(Protocol), Q_SELECTOR(alloc)), Q_SELECTOR(initWithName:), _T("SecondProto"));
    Protocol*   prt3 = q_send(q_send(Q_CLASS(Protocol), Q_SELECTOR(alloc)), Q_SELECTOR(initWithName:), _T("ThirdProto"));
    Protocol*   prt4 = q_send(q_send(Q_CLASS(Protocol), Q_SELECTOR(alloc)), Q_SELECTOR(initWithName:), _T("ForthProto"));

    q_send(Q_CLASS(Protocol), Q_SELECTOR(registerProtocol:), prt1); Q_RELEASE(prt1);
    q_send(Q_CLASS(Protocol), Q_SELECTOR(registerProtocol:), prt2); Q_RELEASE(prt2);
    q_send(Q_CLASS(Protocol), Q_SELECTOR(registerProtocol:), prt3); Q_RELEASE(prt3);
    q_send(Q_CLASS(Protocol), Q_SELECTOR(registerProtocol:), prt4); Q_RELEASE(prt4);



    md1 = q_allocMethodDescription(Q_SELECTOR(init), "+@");
    q_send(prt1, Q_SELECTOR(addMethodDescription:forInstance:), md1, YES);
    Q_RELEASE(md1);

    md1 = q_allocMethodDescription(Q_SELECTOR(alloc), "+@");
    q_send(prt1, Q_SELECTOR(addMethodDescription:forInstance:), md1, NO);
    Q_RELEASE(md1);



    q_send(prt1, Q_SELECTOR(addProtocol:), prt2);
    q_send(prt2, Q_SELECTOR(addProtocol:), prt3);
    q_send(prt3, Q_SELECTOR(addProtocol:), prt4);

    tst = (int)q_send(prt1, Q_SELECTOR(conformsTo:), prt4);
    if (!tst) testFailure(tm3);

    declProtocols();
}

void
workWithClasses() {
    ThirdC*     obj = nil;
    Class cls3 = q_classByName(_T("ThirdC"));
    id obj3 = q_allocZone(sizeof(struct _Object) + sizeof(int) + sizeof(double)
        + sizeof(float) + sizeof(unsigned int) + sizeof(struct SType) + sizeof(SEL)
        + sizeof(int) + sizeof(float) + sizeof(int) + sizeof(struct TwoInt));
    obj3->isa = cls3;

    ivarsTest(obj3);
    methodTest(obj3);

 /*   {
        int i;
        double* val1, *val2, *val3, *val4, *val5, tst;
        val1 = q_retainMemBlock();
        *val1 = 23.4;
        val2 = q_retainMemBlock();
        *val2 = 5.6;
        val3 = q_retainMemBlock();
        *val3 = 3.3;
        val4 = q_retainMemBlock();
        *val4 = 98.2;
        val5 = q_retainMemBlock();
        *val5 = 44.0;

        for (i = 0; i < 3; ++i)
            tst = *(double*)q_releaseMemBlock();
        val1 = q_retainMemBlock();
        *val1 = 12.1;
        val2 = q_retainMemBlock();
        *val2 = 13.1;

        for (i = 0; i < 4; ++i)
            tst = *(double*)q_releaseMemBlock();
    }
*/
    q_freeZone(obj3);
}

void
ivarsTest(id obj) {
    unsigned int    iVal = 34, *tstI = nil;
    float           fVal = 44.5f, *tstF = nil;
    SEL*            tstS = nil;
    SEL             sVal = q_allocSelector(_T("MySel"));

    q_setValueToIvar(obj, _T("_setUI"), &iVal);
    q_setValueToIvar(obj, _T("_size"), &fVal);
    q_setValueToIvar(obj, _T("_selec"), &sVal);

    q_getValueFromIvar(obj, _T("_setUI"), &tstI);
    if (*tstI != iVal) testFailure(tm1);
    q_getValueFromIvar(obj, _T("_size"), &tstF);
    if (*tstF != fVal) testFailure(tm1);
    q_getValueFromIvar(obj, _T("_selec"), (void**)&tstS);
    if (*tstS != sVal) testFailure(tm1);

    Q_RELEASE(sVal);
}

void
methodTest(id obj) {
    id tmp = nil;
    int itst = 0;
    struct TwoInt st;

    for (itst = 0; itst < 1000; ++itst)
        q_send(obj, Q_SELECTOR(simpleMeth:), 10);


    tmp = q_send(Q_CLASS(ThirdC), Q_SELECTOR(allocObject));
    q_freeZone(tmp);

    for (itst = 0; itst < 10; ++itst) {
        q_sendFp(obj, Q_SELECTOR(calculateAreaX:Y:), 16.6f, 173.3f, 20.1, 12.3f, 13.3f);
    }

    q_send(obj, Q_SELECTOR(superTest));
    q_sendSuper(obj, Q_SELECTOR(superTest));

    q_send(obj, Q_SELECTOR(varParams), 12, 13.1f, 14.2f, 15.3, 16.4f, 69.145);

    q_sendSt(&st, Q_CLASS(ThirdC), Q_SELECTOR(secM));
    if (st.i != 22 || st.j != 33) testFailure(tm2);
}


static int
_simpleMeth(id self, SEL _cmd, int val) {
    static int cnt = 0;
    float area = 0;
    int itst = 0;
    struct TwoInt st;
    struct OneInt si;
    struct SType  sd;
    if (!val) return 0;

    itst = (int)q_send(self, Q_SELECTOR(simpleMeth:), val - 1);
    q_sendSt(&st, Q_CLASS(ThirdC), Q_SELECTOR(secM), 250, 12.3f, 16.9, 13.2f);
    q_sendSt(&si, Q_CLASS(ThirdC), Q_SELECTOR(secInt), 12.3f, 16.9, 13.2f);
    q_sendSt(&sd, Q_CLASS(ThirdC), Q_SELECTOR(secDiff), 12.3f, 16.9, 13.2f);

    // if (val != 68.3f) testFailure(tm2);
    testLog(_T("DECL: Invoke simple method !\n"));
    return 0;
/*
    // area = (float)msgSendFP(self, Q_SELECTOR(calculateAreaX:Y:), 54.34f, 2.571f);
    q_sendFp(self, Q_SELECTOR(calculateAreaX:Y:), 54.34f, 2.571f);
    if (area != 54.34f * 2.571f) testFailure(tm2);
*/
}

static float
_calcAreaXY(id self, SEL _cmd, float x, float y, double d, float x1, float y1) {
    if (x != 16.6f || y != 173.3f || d != 20.1 || x1 != 12.3f || y1 != 13.3f) testFailure(tm2);
    testLog(_T("DECL: Invoke calculateAreaX:Y: method !\n"));
    return x * y;
}

static id
_allocObj(id self, SEL _cmd) {
    id obj = q_allocZone(sizeof(struct _Object) + sizeof(int));
    obj->isa = (Class)self;
    testLog(_T("DECL: allocObject method !\n"));

    return obj;
}

static struct TwoInt
_secM(id self, SEL _cmd, char val, float f1, double d, float f2) {
    struct TwoInt st = { 22, 33 };
    testLog(_T("DECL: Invoke secM method !\n"));

    return st;
}

static struct OneInt
_secInt(id self, SEL _cmd, float f1, double d, float f2) {
    struct OneInt st = { 22 };
    return st;
}

static struct SType
_secDiff(id self, SEL _cmd, float f1, double d, float f2) {
    struct SType st = { 4.5f, 34.1f, 56.7 };
    return st;
}

void
memoryTest() {
    id obj = q_send(Q_CLASS(ThirdC), Q_SELECTOR(new));

    Q_RELEASE(obj);
}

static void
_dealloc1(id self, SEL _cmd) {
    testLog(_T("Call dealloc-1 !\n"));
}

static void
_dealloc2(id self, SEL _cmd) {
    testLog(_T("Call dealloc-2 !\n"));
}

static void
_superTest1(id self, SEL _cmd) {
    testLog(_T("Call super-1 !\n"));
}

static void
_superTest2(id self, SEL _cmd) {
    testLog(_T("Call super-2 !\n"));
}

static int
_variableParams(id self, SEL _cmd, int a, float b, float c, double d, float e, ...) {
    double val = 0;
    va_list vlist;
    va_start(vlist, e);

    val = va_arg(vlist, double);
    if (val != 69.145) testFailure(tm2);

    va_end(vlist);
    return 0;
}

static float
_calcAreaXYCategory(id self, SEL _cmd, float x, float y, double d, float x1, float y1) {
    testLog(_T("Call _calcAreaXY from Category!\n"));
    return 0.0f;
}

static id
_customAlloc() {
    return nil;
}