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
#include "qclass.h"
#include "qprivate.h"
#include "qencoding.h"
#include "qprotocol.h"
#include "qloading.h"

#include <string.h>


Q_ERROR_MESSAGE(er0, "CLASS: Ivar can\'t be created with nil name or type!\n");
Q_ERROR_MESSAGE(er1, "CLASS: Method can\'t be created with nil SEL or types!\n");
Q_ERROR_MESSAGE(er2, "CLASS: Class can\'t be created with nil or \"\" name!\n");
Q_ERROR_MESSAGE(er3, "CLASS: Can\'t register nil class or super class!\n");
Q_ERROR_MESSAGE(er4, "CLASS: Class \"%s\" already exists!\n");
Q_ERROR_MESSAGE(er5, "CLASS: Super class \"%s\" dosn't exists!\n");
Q_ERROR_MESSAGE(er6, "CLASS: Class \"%s\" can\'t be root class, root class already defined!\n");
Q_ERROR_MESSAGE(er7, "CLASS: Class table is nil, need initialize class module!\n");
Q_ERROR_MESSAGE(er8, "CLASS: Can\'t find method for nil selector or nil methodList!\n");
Q_ERROR_MESSAGE(er9, "CLASS: Can\'t find methodList for nil selector or nil methodListArray!\n");
Q_ERROR_MESSAGE(er10, "CLASS: Can\'t find methodList for nil selector or nil start class!\n");
Q_ERROR_MESSAGE(er11, "CLASS: Can\'t find ivar for nil name or nil ivarList!\n");
Q_ERROR_MESSAGE(er12, "CLASS: Can\'t find ivar for nil name or nil start class!\n");
Q_ERROR_MESSAGE(er13, "CLASS: Used nil object or nil name or nil value for Ivar!\n");
Q_ERROR_MESSAGE(er14, "CLASS: Can\'t work with nil object!\n");
Q_ERROR_MESSAGE(er15, "CLASS: Static zone in a class is nil!\n");
Q_ERROR_MESSAGE(er16, "CLASS: Used nil protocol object or nil class!\n");
Q_ERROR_MESSAGE(er17, "CLASS: Protocol that you want add to protocol array already added!\n");


static void             _deallocIvar(Ivar ivar);
static void             _newIvarArrayCallback(IvarArray* iva);
static void             _addRemoveIvarCallback(Ivar ivar, IvarArray iva, BOOL direction);
static void             _deallocMethod(Method method);
static void             _deallocClass(Class cls);
static unsigned int     _hashFromClass(const Class cls);
static BOOL             _isEqualClasses(const Class cls1, const Class cls2);

static ClassTable   _classTbl = nil;

Q_DEFINE_ARRAY_WITH_CALLBACKS(IvarArray, Ivar, 5, _newIvarArrayCallback, _addRemoveIvarCallback)
Q_DEFINE_ARRAY(MethodList, Method, 10)
Q_DEFINE_ARRAY(MethodListArray, MethodList, 1)
Q_DEFINE_HASHTABLE(ClassTable, Class, _hashFromClass, _isEqualClasses, 20)

#define DATA_FIELDS_SIZE(ivarArr)    *(int*)((char*)ivarArr + sizeof(struct _IvarArray))


Ivar
q_allocIvar(const TCHAR* name, const char* type) {
    Ivar    ivar;

    if (!name || !type) {
        q_throwError(er0);
        return nil;
    }
    Q_ALLOC(ivar, struct _Ivar, _deallocIvar);

    ivar->name = q_allocZone((_tcslen(name) + 1) * sizeof(TCHAR));
    _tcscpy(ivar->name, name);
    ivar->type = q_allocZone((strlen(type) + 1) * sizeof(char));
    strcpy(ivar->type, type);

    return ivar;
}

static void
_deallocIvar(Ivar ivar) {
    Q_ASSERT(ivar);

    q_freeZone(ivar->name);
    q_freeZone(ivar->type);
    q_freeZone(ivar);
}

void
q_setValueToIvar(id obj, const TCHAR* name, void* val) {
    Ivar    ivar;
    int     baseOffset;

    if (!obj || !name || !val) {
        q_throwError(er13);
        return;
    }
    if (ivar = q_lookupIvarInClassHierarchy(name, obj->isa, &baseOffset))
        memcpy(((char*)obj) + baseOffset + ivar->offset,
            val, q_sizeOfTypeEncode(ivar->type));
}

void
q_getValueFromIvar(id obj, const TCHAR* name, void** val) {
    Ivar    ivar;
    int     baseOffset;

    if (!obj || !name || !val) {
        q_throwError(er13);
        return;
    }
    if (ivar = q_lookupIvarInClassHierarchy(name, obj->isa, &baseOffset))
        *val = ((char*)obj) + baseOffset + ivar->offset;
}

static void
_newIvarArrayCallback(IvarArray* iva) {
    *iva = q_reallocZone(
        *iva,
        sizeof(struct _IvarArray),
        sizeof(struct _IvarArray) + sizeof(int));
}

static void
_addRemoveIvarCallback(Ivar ivar, IvarArray iva, BOOL direction) {
    if (direction) {
        ivar->offset = DATA_FIELDS_SIZE(iva);
        DATA_FIELDS_SIZE(iva) += q_sizeOfTypeEncode(ivar->type);
    } else {
        DATA_FIELDS_SIZE(iva) -= q_sizeOfTypeEncode(ivar->type);
    }
}

Method
q_allocMethod(SEL sel, IMP imp, const char* types) {
    Method  method;

    if (!sel || !types) {
        q_throwError(er1);
        return nil;
    }
    Q_ALLOC(method, struct _Method, _deallocMethod);

    Q_RETAIN(sel);
    method->sel = sel;
    method->imp = imp;
    method->types = q_allocZone((strlen(types) + 1) * sizeof(char));
    strcpy(method->types, types);

    return method;
}

static void
_deallocMethod(Method method) {
    Q_ASSERT(method);

    Q_RELEASE(method->sel);
    q_freeZone(method->types);
    q_freeZone(method);
}

Class
q_allocClass(const TCHAR* name) {
    Class   newCls;
    Class   metaCls;

    if (!name) {
        q_throwError(er2);
        return nil;
    }
    Q_ALLOC(newCls, struct _Class, _deallocClass);
    Q_ALLOC(metaCls, struct _Class, _deallocClass);

    newCls->isa = metaCls;

    newCls->name = q_allocZone((_tcslen(name) + 1) * sizeof(TCHAR));
    _tcscpy(newCls->name, name);
    metaCls->name = newCls->name;

    metaCls->info |= CLS_META;

    return newCls;
}

static void
_deallocClass(Class cls) {
    Q_ASSERT(cls);

    if (!(cls->info & CLS_META)) {
        q_freeZone(cls->name);
        Q_RELEASE(cls->isa);
    }
    Q_RELEASE(cls->protocols);
    Q_RELEASE(cls->ivars);
    Q_RELEASE(cls->methodLists);
    q_freeZone(cls);
}

void
q_setMethodListArray(Class cls, MethodListArray mla) {
    Q_ASSERT(cls);
    Q_ASSERT(mla);

    if (mla == cls->methodLists) return;
    Q_CHANGE_OBJECT(cls->methodLists, mla);
}

MethodListArray
q_methodListArray(const Class cls) {
    return cls->methodLists;
}

void
q_setIvarArray(Class cls, IvarArray iva) {
    Q_ASSERT(cls);
    Q_ASSERT(iva);

    if (iva == cls->ivars) return;
    Q_CHANGE_OBJECT(cls->ivars, iva);
}

IvarArray
q_ivarArray(const Class cls) {
    return cls->ivars;
}

void
q_addProtocolToClass(Protocol* prtObj, const Class cls) {
    unsigned int i, cnt;

    if (!prtObj || !cls) {
        q_throwError(er16);
        return;
    }
    if (!cls->protocols) cls->protocols = q_allocProtocolArray();

#ifdef _DEBUG
    for (i = 0, cnt = q_itemsCountInProtocolArray(cls->protocols); i < cnt; ++i) {
        if (q_itemAtIndexInProtocolArray(i, cls->protocols) == prtObj) {
            q_throwError(er17);
            return;
        }
    }
#endif
    q_addItemToProtocolArray(prtObj, cls->protocols);
}

int
q_sizeOfObject(const id obj) {
    int     result = 0;
    Class   c;

    if (!obj) {
        q_throwError(er14);
        return 0;
    }
    c = obj->isa;
    if (!c) return sizeof(struct _Object);

    if (c->info & CLS_META) {
        return sizeof(struct _Class);
    } else {
        do {
            if (q_ivarArray(c))
                result += DATA_FIELDS_SIZE(q_ivarArray(c));
        } while (c = c->super);
        result += sizeof(struct _Object);
    }
    return result;
}

void
q_registerClass(Class newCls, Class superCls) {
    Class   rootCls;
    Class   metaCls;

    if (!newCls || !superCls) { q_throwError(er3); return; }
    if (!_classTbl) { q_throwError(er7); return; }
    if (q_itemFromClassTable(newCls, _classTbl)) { q_throwError(er4, newCls->name); return; }
    if (!q_itemFromClassTable(superCls, _classTbl)) { q_throwError(er5, superCls->name); return; }

    metaCls = newCls->isa;

    rootCls = superCls;
    while (rootCls->super) rootCls = rootCls->super;

    newCls->super = superCls;
    metaCls->super = superCls->isa;
    metaCls->isa = rootCls->isa;

    q_addItemToClassTable(newCls, _classTbl);
}

void
q_registerRootClass(Class rootCls) {
    Class   metaCls;

    if (!rootCls) { 
        q_throwError(er3);
        return;
    }
    if (!_classTbl) {
        _classTbl = q_allocClassTable();
    }
    if (q_itemsCountInClassTable(_classTbl)) {
        q_throwError(er6, rootCls->name);
        return;
    }
    metaCls = rootCls->isa;

    rootCls->super = nil;
    metaCls->super = rootCls;
    metaCls->isa = metaCls;

    q_addItemToClassTable(rootCls, _classTbl);
}

Class
q_classByName(const TCHAR* name) {
    struct _Class query;
    Class   result;

    if (!name) {
        q_throwError(er14);
        return nil;
    }
    query.name = (TCHAR*)name;
    result = q_itemFromClassTable(&query, _classTbl);
    if (!result) {
        q_loadSymbolByType(name, Q_CLASS_TYPE);
        result = q_itemFromClassTable(&query, _classTbl);
        Q_ASSERT(result);
        q_loadCategoriesForClass(result);
    }
    return result;
}

Ivar
q_lookupIvarInIvarArray(const TCHAR* name, const IvarArray iva) {
    unsigned int i, cnt;

    if (!name || !iva) {
        q_throwError(er11);
        return nil;
    }
    for (i = 0, cnt = q_itemsCountInIvarArray(iva); i < cnt; ++i) {
        Ivar v = q_itemAtIndexInIvarArray(i, iva);
        if (!_tcscmp(v->name, name)) return v;
    }
    return nil;
}

Ivar
q_lookupIvarInClassHierarchy(const TCHAR* name, const Class startCls, int* baseOffset) {
    Class cls = startCls;

    if (!name || !startCls) {
        q_throwError(er12);
        return nil;
    }
    do {
        Ivar        v;
        IvarArray   iva = q_ivarArray(cls);

        if (iva && (v = q_lookupIvarInIvarArray(name, iva))) {
            struct _Object  tmp = { 0, nil, cls->super };
            if (baseOffset) *baseOffset = q_sizeOfObject(&tmp);
            return v;
        }
    } while (cls = cls->super);

    return nil;
}

Method
q_lookupMethodInMethodList(const SEL sel, const MethodList mlist) {
    unsigned int    i, cnt;

    if (!sel || !mlist) {
        q_throwError(er8);
        return nil;
    }
    for (i = 0, cnt = q_itemsCountInMethodList(mlist); i < cnt; ++i) {
        Method m = q_itemAtIndexInMethodList(i, mlist);
        if (m->sel == sel) return m;
    }
    return nil;
}

Method
q_lookupMethodInMethodListArray(const SEL sel, const MethodListArray mla) {
    unsigned int        i;

    if (!sel || !mla) {
        q_throwError(er9);
        return nil;
    }
    for (i = q_itemsCountInMethodListArray(mla); i > 0; --i) {
        MethodList mlist = q_itemAtIndexInMethodListArray(i - 1, mla);
        Method m;
        if (m = q_lookupMethodInMethodList(sel, mlist)) return m;
    }
    return nil;
}

Method
q_lookupMethodInClassHierarchy(const SEL sel, const Class startCls) {
    Class cls = startCls;

    if (!sel || !startCls) {
        q_throwError(er10);
        return nil;
    }
    do {
        Method             m;
        MethodListArray    mla = q_methodListArray(cls);

        if (mla && (m = q_lookupMethodInMethodListArray(sel, mla)))
            return m;
    } while (cls = cls->super);

    return nil;
}

void
q_finalizeClassModule() {
    Q_RELEASE(_classTbl);
}

static unsigned int
_hashFromClass(const Class cls) {
    return q_hashFromString(cls->name);
}

static BOOL
_isEqualClasses(const Class cls1, const Class cls2) {
    return q_isEqualStrings(cls1->name, cls2->name);
}