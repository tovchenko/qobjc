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
#include "miscTest.h"

Q_ERROR_MESSAGE(tm1, "Don\'t work remove items in array!\n");

typedef struct _MyType {
    Q_ROBJ_HEADER
    int height;
    int width;
} *MyType;


Q_DECLARE_ARRAY(ArrType, MyType);
Q_DEFINE_ARRAY(ArrType, MyType, 2)


void
clear(MyType obj) {
    free(obj);
}

MyType
allocMyType() {
    MyType obj;

    Q_ALLOC(obj, struct _MyType, clear);
    return obj;
}

void
runMiscTest() {

    AutoreleasePool pool = q_allocAutoreleasePool();

    MyType obj1 = allocMyType();
    MyType obj2 = allocMyType();
    MyType obj3 = allocMyType();
    MyType obj4 = allocMyType();
    ArrType myArr = q_allocArrType();
    MyType tst = nil;

    obj2->height = 25;
    obj3->height = 30;
    obj4->height = 45;

    q_addItemToArrType(obj1, myArr);
    q_addItemToArrType(obj2, myArr);
    q_addItemToArrType(obj3, myArr);
    q_addItemToArrType(obj4, myArr);
    Q_RELEASE(obj1);
    Q_RELEASE(obj2);
    Q_RELEASE(obj3);
    Q_RELEASE(obj4);

    tst = q_itemAtIndexInArrType(1, myArr);
    tst = q_itemAtIndexInArrType(2, myArr);
    tst = q_itemAtIndexInArrType(3, myArr);

    q_removeItemAtIndexInArrType(1, myArr);

    tst = q_itemAtIndexInArrType(1, myArr);
    if (tst->height != 30) testFailure(tm1);
    tst = q_itemAtIndexInArrType(2, myArr);
    if (tst->height != 45) testFailure(tm1);

    Q_AUTORELEASE(myArr);

    {
        int h = q_itemAtIndexInArrType(1, myArr)->height;
        int vv = 0;
        ++vv;
    }

    {
        unsigned int i;
        for (i = 0; i < 1000; ++i) {
            MyType obj = allocMyType();
            Q_AUTORELEASE(obj);
        }
    }


    Q_AUTORELEASE(pool);

    {
        int al = 9;
        ++al;
    }
}
