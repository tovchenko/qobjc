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
#include "qloading.h"
#include "qprivate.h"
#include "qclass.h"

Q_ERROR_MESSAGE(er0, "LOADER: SymbolHandle can\'t be created with nil name or nil callback!\n");
Q_ERROR_MESSAGE(er1, "LOADER: Can\'t load symbol for the nil name!\n");
Q_ERROR_MESSAGE(er2, "LOADER: Can\'t load not existing symbol!\n");
Q_ERROR_MESSAGE(er3, "LOADER: Can\'t return symbol table with undefined type!\n");

static void                 _deallocSymbolHandle(SymbolHandle sh);
static unsigned int         _hashFromSymbol(const SymbolHandle s);
static BOOL                 _isEqualSymbols(const SymbolHandle s1, const SymbolHandle s2);


Q_DEFINE_HASHTABLE(LoadSymbolTable, SymbolHandle, _hashFromSymbol, _isEqualSymbols, 30)
Q_DEFINE_ARRAY(LoadCategoryArray, SymbolHandle, 10)


static LoadSymbolTable      _classInfoTbl = nil;
static LoadSymbolTable      _protocolInfoTbl = nil;
static LoadCategoryArray    _categoryInfoArr = nil;


SymbolHandle
q_allocSymbolHandle(TCHAR* name, LoadSymbolCallback func) {
    SymbolHandle    sh;

    if (!name || !func) {
        q_throwError(er0);
        return nil;
    }
    Q_ALLOC(sh, struct _SymbolHandle, _deallocSymbolHandle);
    sh->name = q_allocZone((_tcslen(name) + 1) * sizeof(TCHAR));
    _tcscpy(sh->name, name);
    sh->loadFunc = func;

    return sh;
}

static void
_deallocSymbolHandle(SymbolHandle sh) {
    Q_ASSERT(sh);
    q_freeZone(sh->name);
    q_freeZone(sh);
}

LoadSymbolTable
q_infoTableBySymbolType(SymbolType type) {
    switch (type) {
        case Q_CLASS_TYPE:
            if (!_classInfoTbl) _classInfoTbl = q_allocLoadSymbolTable();
            return _classInfoTbl;
        case Q_PROTOCOL_TYPE:
            if (!_protocolInfoTbl) _protocolInfoTbl = q_allocLoadSymbolTable();
            return _protocolInfoTbl;
        default:
            q_throwError(er3);
            break;
    }
    return nil;
}

LoadCategoryArray
q_infoCategoryArray() {
    if (!_categoryInfoArr) _categoryInfoArr = q_allocLoadCategoryArray();
        return _categoryInfoArr;
}

void
q_loadSymbolByType(const TCHAR* name, SymbolType type) {
    struct _SymbolHandle query;
    SymbolHandle sh;

    if (!name) {
        q_throwError(er1);
        return;
    }
    query.name = (TCHAR*)name;
    sh = q_itemFromLoadSymbolTable(&query, q_infoTableBySymbolType(type));

    if (!sh) {
        q_throwError(er2);
        return;
    }
    (*sh->loadFunc)();
    // symbol was loaded, info about it not need
    q_removeItemFromLoadSymbolTable(sh, q_infoTableBySymbolType(type));
}

void
q_loadCategoriesForClass(const struct _Class* cls) {
    LoadCategoryArray ca = q_infoCategoryArray();
    int i, cnt;

    Q_ASSERT(cls);
    for (i = 0, cnt = q_itemsCountInLoadCategoryArray(ca); i < cnt; ++i) {
        SymbolHandle sh = q_itemAtIndexInLoadCategoryArray(i, ca);
        if (q_isEqualStrings(cls->name, sh->name)) {
            (*sh->loadFunc)();

            // category was loaded, info about it not need
            q_removeItemAtIndexInLoadCategoryArray(i, ca);
        }
    }
}

void
q_finalizeLoaderModule() {
    Q_RELEASE(_classInfoTbl);
    Q_RELEASE(_protocolInfoTbl);
    Q_RELEASE(_categoryInfoArr);
}

static unsigned int
_hashFromSymbol(const SymbolHandle s) {
    return q_hashFromString(s->name);
}

static BOOL
_isEqualSymbols(const SymbolHandle s1, const SymbolHandle s2) {
    return q_isEqualStrings(s1->name, s2->name);
}