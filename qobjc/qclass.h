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

#ifndef qobjc_qclass_H
#define qobjc_qclass_H

#include "qselector.h"


// object declaration
#define Q_OBJECT_HEADER   struct _Class*    isa;

typedef struct _Object {
    Q_ROBJ_HEADER
    Q_OBJECT_HEADER
} *id;


// instances variable definitions
typedef struct _Ivar {
    Q_ROBJ_HEADER
    TCHAR*          name;
    char*           type;
    unsigned int    offset;
} *Ivar;

Q_DECLARE_ARRAY(IvarArray, Ivar);

Ivar                    q_allocIvar(const TCHAR* name, const char* type);

// set, get value
void                    q_setValueToIvar(id obj, const TCHAR* name, void* val);
void                    q_getValueFromIvar(id obj, const TCHAR* name, void** val);


// method definitions
typedef void*   IMP;

typedef struct _Method {
    Q_ROBJ_HEADER
    SEL      sel;
    char*    types;
    IMP      imp;
} *Method;

Q_DECLARE_ARRAY(MethodList, Method);
Q_DECLARE_ARRAY(MethodListArray, MethodList);

Method                  q_allocMethod(SEL sel, IMP imp, const char* types);


// class definitions
struct _ProtocolArray;
typedef struct _Object Protocol;

typedef struct _Class {
    Q_ROBJ_HEADER
    Q_OBJECT_HEADER
    struct _Class*          super;
    TCHAR*                  name;
    unsigned long           info;
    IvarArray               ivars;
    MethodListArray         methodLists;
    struct _ProtocolArray*  protocols;
} *Class;

#define CLS_META    0x2L

Q_DECLARE_HASHTABLE(ClassTable, Class);

// generic operations
Class                   q_allocClass(const TCHAR* name);
void                    q_setMethodListArray(Class cls, MethodListArray mla);
MethodListArray         q_methodListArray(const Class cls);
void                    q_setIvarArray(Class cls, IvarArray iva);
IvarArray               q_ivarArray(const Class cls);
// protocol operations
void                    q_addProtocolToClass(Protocol* prtObj, const Class cls);
// object operations
int                     q_sizeOfObject(const id obj);
// register operations
void                    q_registerClass(Class newCls, Class superCls);
void                    q_registerRootClass(Class rootCls);
Class                   q_classByName(const TCHAR* name);
// lookup ivars
Ivar                    q_lookupIvarInIvarArray(const TCHAR* name, const IvarArray iva);
Ivar                    q_lookupIvarInClassHierarchy(const TCHAR* name, const Class startCls, int* baseOffset);
// lookup methods
Method                  q_lookupMethodInMethodList(const SEL sel, const MethodList mlist);
Method                  q_lookupMethodInMethodListArray(const SEL sel, const MethodListArray mla);
Method                  q_lookupMethodInClassHierarchy(const SEL sel, const Class startCls);
// class module
void                    q_finalizeClassModule();


#endif // qobjc_qclass_H


