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
#include "encodeTest.h"


// {StrTestSimple=ii}
struct StrTestSimple {
    int a;
    int b;
};


// {StrTestAlign=cci}
struct StrTestAlign {
    char    a;
    char    b;
    int     i;
};

// {StrTestAlign2=icc}
struct StrTestAlign2 {
    int     i;
    char    a;
    char    b;
};

// {StrBit=b5cb2isb1lb12s}
struct StrBit {
    char a:5;
    int  b:2;
    short   h;
    long c:1;
    short d:12;
};

// {StrNo=cisls}
struct StrNo {
    char a;
    int  b;
    short h;
    long c;
    short d;
};

// {TwiceStrTest={StrTestAlign=cci}i^{StrTestAlign2=icc}}
struct TwiceStrTest {
    struct StrTestAlign   s1;
    int                   j;
    struct StrTestAlign2* s2;
};

// (UnionTest=id{StrTestAlign=cci})
union UnionTest {
    int                 a;
    double              d;
    struct StrTestAlign s2;
};

// {SimpleS=i{StrBit=b5cb2isb1lb12s}d}
struct SimpleS {
    int           k;
    struct StrBit bt;
    double        d;
};

// \"var\"{StrComplexSimple={SimpleS=i{StrBit=b5cb2isb1lb12s}d}@#cCsSiIlLfd^i*{TwiceStrTest={StrTestAlign=c\"name\"ci}i\"pointr\"^{StrTestAlign2=icc}}[14i][131(UnionTest=id{StrTestAlign=cci})]}
struct StrComplexSimple {
    struct SimpleS      gg;
    id                  a;
    Class               b;
    char                c;
    unsigned char       d;
    short               e;
    unsigned short      i;
    int                 j;
    unsigned int        k;
    long                l;
    unsigned long       n;
    float               f;
    double              v;
    int*                p;
    char*               t;
    struct TwiceStrTest s1;
    int                 pi[14];
    union UnionTest     ut[131];
};



void
runEncodeTest() {
    sizeEncodeTest();
    methodTypeEncodeTest((SEL)0x1);
}

void
sizeEncodeTest() {
    size_t realSize = sizeof(struct StrComplexSimple);
    size_t encodeSize = q_sizeOfTypeEncode("\"var\"{StrComplexSimple={SimpleS=i{StrBit=b5cb2isb1lb12s}d}@#cCsSiIlLfd^i*{TwiceStrTest={StrTestAlign=c\"name\"ci}i\"pointr\"^{StrTestAlign2=icc}}[14i][131(UnionTest=id{StrTestAlign=cci})]}");
    if (realSize != encodeSize) testFailure(_T("realSize != encodeSize!!!"));

    realSize = sizeof(struct StrBit);
    encodeSize = q_sizeOfTypeEncode("{StrBit=b5cb2isb1lb12s}");
    if (realSize != encodeSize) testFailure(_T("Not work bit fields!"));
}

void
methodTypeEncodeTest(SEL sel) {
    struct _Method m;
    size_t ret_size = 0;
    size_t all_size = 0;

    m.imp = (IMP)0x1; // some address, not important, this test value
    m.sel = sel;
    m.types = "+\"retval\"{StrTestAlign=cci}+\"param1\"{TwiceStrTest={StrTestAlign=cci}i^{StrTestAlign2=icc}}+\"param2\"d";

    ret_size = q_sizeOfMethodRetTypeEncode(&m);
    all_size = q_sizeOfMethodAllArgTypesEncode(&m, nil);

    if (ret_size != sizeof(struct StrTestAlign))
        testFailure(_T("Method type encoding don't work!"));
    if (all_size != sizeof(struct TwiceStrTest) + sizeof(double) + sizeof(id) + sizeof(SEL))
        testFailure(_T("Method type encoding don't work!"));
}
