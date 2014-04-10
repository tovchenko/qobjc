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
#include "qprotocol.h"
#include "qmsgsend.h"
#include "qprivate.h"
#include "qloading.h"

#include "string.h"


Q_ERROR_MESSAGE(er0, "PROTOCOL: Protocol \"%s\" already exists!\n");
Q_ERROR_MESSAGE(er1, "PROTOCOL: You can\'t use \"init\" method for protocol object, use \"initWithName:\"!\n");
Q_ERROR_MESSAGE(er3, "PROTOCOL: Protocol that you want add to protocol array already added!\n");

static void             _deallocMethodDescription(MethodDescription md);
static ProtocolTable    _protocolTable();
static unsigned int     _hashFromProtocol(const Protocol* prt);
static BOOL             _isEqualProtocols(const Protocol* prt1, const Protocol* prt2);


static Class            _protocolClass = nil;
static ProtocolTable    _protoTable = nil;

Q_DEFINE_HASHTABLE(ProtocolTable, Protocol*, _hashFromProtocol, _isEqualProtocols, 20)
Q_DEFINE_ARRAY(DescriptionArray, MethodDescription, 10)
Q_DEFINE_ARRAY(ProtocolArray, Protocol*, 3)


void
q_loadProtocolClass() {
    Q_M_NAME_LOAD(Protocol);
    _protocolClass = Q_CLASS_OBJ(Protocol);
    Q_RETAIN(_protocolClass);
    q_loadCategoriesForClass(_protocolClass);
}

void
q_unloadProtocolClass() {
    Q_RELEASE(_protoTable);
    Q_RELEASE(_protocolClass);
}


id
Q_IM_NAME(Protocol, initWithName)(id self, SEL _cmd, const TCHAR* name) {
    if (self = q_sendSuper(self, Q_SELECTOR(init))) {
        TCHAR* nameVal;

        nameVal = q_allocZone((_tcslen(name) + 1) * sizeof(TCHAR));
        _tcscpy(nameVal, name);
        q_setValueToIvar(self, _T("_name"), &nameVal);
    }
    return self;
}

id
Q_IM_NAME(Protocol, init)(id self, SEL _cmd) {
    q_throwError(er1);
    Q_RELEASE(self);
    return nil;
}

void
Q_IM_NAME(Protocol, dealloc)(id self, SEL _cmd) {
    TCHAR**             val = nil;
    ProtocolArray*      protoList = nil;
    DescriptionArray*   descList = nil;

    q_getValueFromIvar(self, _T("_protocols"), (void**)&protoList);
    Q_RELEASE(*protoList);

    q_getValueFromIvar(self, _T("_instanceMethods"), (void**)&descList);
    Q_RELEASE(*descList);

    q_getValueFromIvar(self, _T("_classMethods"), (void**)&descList);
    Q_RELEASE(*descList);

    q_getValueFromIvar(self, _T("_name"), (void**)&val);
    q_freeZone(*val);
}

TCHAR*
Q_IM_NAME(Protocol, name)(id self, SEL _cmd) {
    TCHAR**    val = nil;
    q_getValueFromIvar(self, _T("_name"), (void**)&val);
    return *val;
}

BOOL
Q_IM_NAME(Protocol, conformsTo)(id self, SEL _cmd, Protocol* prtObj) {
    ProtocolArray*  protoList = nil;

    if (!prtObj) return NO;
    if (self == prtObj) return YES;
    
    q_getValueFromIvar(self, _T("_protocols"), (void**)&protoList);
    if (*protoList) {
        int i, cnt;
        for (i = 0, cnt = q_itemsCountInProtocolArray(*protoList); i < cnt; ++i) {
            if (q_send(
                    q_itemAtIndexInProtocolArray(i, *protoList),
                    Q_SELECTOR(conformsTo:),
                    prtObj))
                return YES;
        }
    }
    return NO;
}

void
Q_IM_NAME(Protocol, addProtocol)(id self, SEL _cmd, Protocol* prtObj) {
    ProtocolArray*  protoList = nil;
    int i, cnt;

    if (!prtObj) return;

    q_getValueFromIvar(self, _T("_protocols"), (void**)&protoList);
    if (!(*protoList)) *protoList = q_allocProtocolArray();

#ifdef _DEBUG
    for (i = 0, cnt = q_itemsCountInProtocolArray(*protoList); i < cnt; ++i) {
        if (q_itemAtIndexInProtocolArray(i, *protoList) == prtObj) {
            q_throwError(er3);
            return;
        }
    }
#endif
    q_addItemToProtocolArray(prtObj, *protoList);
}

void
Q_IM_NAME(Protocol, addMethodDescriptionForInstance)(
    id self, SEL _cmd, MethodDescription md, BOOL forInstance) {

    DescriptionArray*   descRef;

    if (forInstance) {
        q_getValueFromIvar(self, _T("_instanceMethods"), (void**)&descRef);
    } else {
        q_getValueFromIvar(self, _T("_classMethods"), (void**)&descRef);
    }
    if (!(*descRef)) *descRef = q_allocDescriptionArray();

    q_addItemToDescriptionArray(md, *descRef);
}

void
Q_CM_NAME(Protocol, registerProtocol)(id self, SEL _cmd, Protocol* prtObj) {
    if (!prtObj) return;
    if (q_itemFromProtocolTable(prtObj, _protocolTable()))
        q_throwError(er0, q_send(prtObj, Q_SELECTOR(name)));

    q_addItemToProtocolTable(prtObj, _protocolTable());
}

Protocol*
Q_CM_NAME(Protocol, protocolByName)(id self, SEL _cmd, const TCHAR* name) {
    Protocol*   query = q_send(q_send(Q_CLASS(Protocol), Q_SELECTOR(alloc)), Q_SELECTOR(initWithName:), name);
    Protocol*   result = q_itemFromProtocolTable(query, _protocolTable());

    if (!result) {
        q_loadSymbolByType(name, Q_PROTOCOL_TYPE);
        result = q_itemFromProtocolTable(query, _protocolTable());
        Q_ASSERT(result);
    }
    Q_RELEASE(query);

    return result;
}

MethodDescription
q_allocMethodDescription(SEL op, char* types) {
    MethodDescription md;
    Q_ALLOC(md, struct _MethodDescription, _deallocMethodDescription);

    md->types = q_allocZone((strlen(types) + 1) * sizeof(char));
    strcpy(md->types, types);
    Q_RETAIN(op);
    md->sel = op;

    return md;
}

static void
_deallocMethodDescription(MethodDescription md) {
    q_freeZone(md->types);
    Q_RELEASE(md->sel);
    q_freeZone(md);
}

static ProtocolTable
_protocolTable() {
    if (!_protoTable)
        _protoTable = q_allocProtocolTable();
    return _protoTable;
}

static unsigned int
_hashFromProtocol(const Protocol* prt) {
    TCHAR** str = nil;
    q_getValueFromIvar((id)prt, _T("_name"), (void**)&str);
    return q_hashFromString(*str);
}

static BOOL
_isEqualProtocols(const Protocol* prt1, const Protocol* prt2) {
    TCHAR**  n1 = nil;
    TCHAR**  n2 = nil;
    q_getValueFromIvar((id)prt1, _T("_name"), (void**)&n1);
    q_getValueFromIvar((id)prt2, _T("_name"), (void**)&n2);
    return q_isEqualStrings(*n1, *n2);
}