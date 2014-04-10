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
#include "qobject.h"
#include "qmsgsend.h"
#include "qprotocol.h"
#include "qloading.h"


static int      _loadDeallocClass();
static id       _allocObject(Class cls);
static void     _deallocObject(id obj);


static Class    _objectClass = nil;
static Class    _deallocClass = nil;


void
q_loadRootClass() {
    Q_M_NAME_LOAD(Object);
    _loadDeallocClass();

    _objectClass = Q_CLASS_OBJ(Object);
    Q_RETAIN(_objectClass);
    q_loadCategoriesForClass(_objectClass);

    _deallocClass = Q_CLASS_OBJ(__deallocClass);
    Q_RETAIN(_deallocClass);
}

void
q_unloadRootClass() {
    Q_RELEASE(_objectClass);
    Q_RELEASE(_deallocClass);
}

static int
_loadDeallocClass() {
    Q_BEGIN_CLASS(__deallocClass, Object)
        Q_BEGIN_INSTANCE_METHOD_LIST
            Q_DEF_METHOD(dealloc, Q_IM_NAME(Object, dealloc), +v)
        Q_END_INSTANCE_METHOD_LIST
    Q_END_CLASS
}

id
Q_IM_NAME(Object, init)(id self, SEL _cmd) {
    return self;
}

void
Q_IM_NAME(Object, dealloc)(id self, SEL _cmd) {
} // this important: this function will be stay empty!

Class
Q_IM_NAME(Object, class)(id self, SEL _cmd) {
    return self->isa;
}

BOOL
Q_IM_NAME(Object, conformsTo)(id self, SEL _cmd, Protocol* prtObj) {
    if (!prtObj) return NO;
    return (BOOL)q_send((id)self->isa, Q_SELECTOR(conformsTo:), prtObj);
}

id
Q_CM_NAME(Object, alloc)(id self, SEL _cmd) {
    return _allocObject((Class)self);
}

id
Q_CM_NAME(Object, new)(id self, SEL _cmd) {
    return q_send(q_send(self, Q_SELECTOR(alloc)), Q_SELECTOR(init));
}

Class
Q_CM_NAME(Object, class)(id self, SEL _cmd) {
    return (Class)self;
}

BOOL
Q_CM_NAME(Object, conformsTo)(id self, SEL _cmd, Protocol* prtObj) {
    Class   tmpCls;
    for (tmpCls = (Class)self; tmpCls; tmpCls = tmpCls->super) {
        unsigned int i, cnt;
        ProtocolArray protoList = tmpCls->protocols;

        if (!protoList) continue;
        for (i = 0, cnt = q_itemsCountInProtocolArray(protoList); i < cnt; ++i)
            if (q_send(q_itemAtIndexInProtocolArray(i, protoList), Q_SELECTOR(conformsTo:), prtObj))
                return YES;
    }
    return NO;
}

static id
_allocObject(Class cls) {
    struct _Object  tmp;
    id              new_obj;

    tmp.isa = cls;
    Q_ALLOC_BY_SIZE(new_obj, q_sizeOfObject(&tmp), _deallocObject);
    new_obj->isa = cls;
    return new_obj;
}

static void
_deallocObject(id obj) {
    do {
        Class tmp = obj->isa->super;
        obj->isa->super = _deallocClass;
        q_send(obj, Q_SELECTOR(dealloc));
        obj->isa->super = tmp;
    } while (obj->isa = obj->isa->super);

    q_freeZone(obj);
}
