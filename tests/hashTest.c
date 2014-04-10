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
#include "hashTest.h"


static unsigned int             classHashTbl(const Class cls);
static BOOL                     classEqualTbl(const Class cls1, const Class cls2);
static unsigned int             hash_code_from_string(const TCHAR* str);
static BOOL                     string_is_equal(const TCHAR* str1, const TCHAR* str2);



static unsigned int
hash_code_from_string(const TCHAR* str) {
    unsigned int hash = 0;

    for (;;) {
        unsigned int a = *str++;
        if (0 == a) break;
        hash += (hash << 8) + a;
    }
    return hash;
}

static BOOL
string_is_equal(const TCHAR* str1, const TCHAR* str2) {
    TCHAR c1, c2;
    for ( ; (c1 = *str1) == (c2 = *str2); str1++, str2++)
        if (c1 == '\0')
            return YES;
    return NO;
}

static unsigned int
classHashTbl(const Class cls) {
    unsigned int h;

    if (!cls || !cls->name)
        return 0;
    h = hash_code_from_string(cls->name); 
    return h;
}

static BOOL
classEqualTbl(const Class cls1, const Class cls2) {
    return string_is_equal(cls1->name, cls2->name);
}


// start hash table test

Q_DECLARE_HASHTABLE(MyTable, Class);
Q_DEFINE_HASHTABLE(MyTable, Class, classHashTbl, classEqualTbl, 2)


void
runHashTest() {
/*
    HashTable   tbl = q_allocHashTable((HashCallback)classHashTbl, (IsEqualCallback)classEqualTbl, 0);

    Class cls1 = allocClass(_T("MYClass1"));
    Class cls2 = allocClass(_T("MYClass2"));
    Class cls3 = allocClass(_T("MYClass3"));


    q_addItemToHashTable((RObject)cls1, tbl);
    q_addItemToHashTable((RObject)cls2, tbl);
    q_addItemToHashTable((RObject)cls3, tbl);

    q_removeItemFromHashTable((RObject)cls2, tbl);


    Q_RELEASE(cls3);
    Q_RELEASE(cls1);
    Q_RELEASE(cls2);
    Q_RELEASE(tbl);
*/
    ownTableTest();
}

void
ownTableTest() {
    Class c;
    MyTable tbl = q_allocMyTable();

    Class cls1 = q_allocClass(_T("SecondClass1"));
    Class cls2 = q_allocClass(_T("SecondClass2"));
    Class cls3 = q_allocClass(_T("SecondClass3"));

    q_addItemToMyTable(cls2, tbl);
    q_addItemToMyTable(cls1, tbl);
    q_addItemToMyTable(cls3, tbl);

    q_removeItemFromMyTable(cls1, tbl);

    c = q_itemFromMyTable(cls1, tbl);
    if (c) testFailure(_T("Remove from hash table don\'t work!\n"));

    {
        Class sCls;
        struct _HashState state = q_initStateForMyTable(tbl);
        q_nextStateForMyTable(&state, tbl, &sCls);
        if (sCls != cls3 && sCls != cls2)
            testFailure(_T("NextState function in hash table don\'t work!\n"));
    }

    Q_RELEASE(cls1);
    Q_RELEASE(cls2);
    Q_RELEASE(cls3);
    Q_RELEASE(tbl);
}