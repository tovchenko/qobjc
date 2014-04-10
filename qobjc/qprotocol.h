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

#ifndef qobjc_qprotocol_H
#define qobjc_qprotocol_H

#include "qconnect.h"


Q_DECLARE_CLASS(Protocol);

typedef struct _MethodDescription {
    Q_ROBJ_HEADER
    SEL     sel;
    char*   types;
} *MethodDescription;

MethodDescription           q_allocMethodDescription(SEL op, char* types);


Q_DECLARE_HASHTABLE(ProtocolTable, Protocol*);
Q_DECLARE_ARRAY(DescriptionArray, MethodDescription); // {_DescriptionArray=I^vII^{_MethodDescription=I^v:*}}
Q_DECLARE_ARRAY(ProtocolArray, Protocol*); // {_ProtocolArray=I^vII@}

// instance methods
id              Q_IM_NAME(Protocol, initWithName)(id self, SEL _cmd, const TCHAR* name);
id              Q_IM_NAME(Protocol, init)(id self, SEL _cmd);
void            Q_IM_NAME(Protocol, dealloc)(id self, SEL _cmd);
TCHAR*          Q_IM_NAME(Protocol, name)(id self, SEL _cmd);
BOOL            Q_IM_NAME(Protocol, conformsTo)(id self, SEL _cmd, Protocol* prtObj);
void            Q_IM_NAME(Protocol, addProtocol)(id self, SEL _cmd, Protocol* prtObj);
void            Q_IM_NAME(Protocol, addMethodDescriptionForInstance)(id self, SEL _cmd, MethodDescription md, BOOL forInstance);

// class methods
void            Q_CM_NAME(Protocol, registerProtocol)(id self, SEL _cmd, Protocol* prtObj);
Protocol*       Q_CM_NAME(Protocol, protocolByName)(id self, SEL _cmd, const TCHAR* name);

static int
Q_M_NAME_LOAD(Protocol) {
    Q_BEGIN_CLASS(Protocol, Object)
        Q_BEGIN_IVAR_LIST
            Q_DEF_IVAR(_name, *)
            Q_DEF_IVAR(_protocols, ^{_ProtocolArray=I^vII@})
            Q_DEF_IVAR(_instanceMethods, ^{_DescriptionArray=I^vII^{_MethodDescription=I^v:*}})
            Q_DEF_IVAR(_classMethods, ^{_DescriptionArray=I^vII^{_MethodDescription=I^v:*}})
        Q_END
        Q_BEGIN_INSTANCE_METHOD_LIST
            Q_DEF_METHOD(initWithName:, Q_IM_NAME(Protocol, initWithName), +@+*)
            Q_DEF_METHOD(init, Q_IM_NAME(Protocol, init), +@)
            Q_DEF_METHOD(dealloc, Q_IM_NAME(Protocol, dealloc), +v)
            Q_DEF_METHOD(name, Q_IM_NAME(Protocol, name), +*)
            Q_DEF_METHOD(conformsTo:, Q_IM_NAME(Protocol, conformsTo), +i+@)
            Q_DEF_METHOD(addProtocol:, Q_IM_NAME(Protocol, addProtocol), +v+@)
            Q_DEF_METHOD(
                addMethodDescription:forInstance:,
                Q_IM_NAME(Protocol, addMethodDescriptionForInstance),
                +v+^{_MethodDescription=I^v:*}+i)
        Q_END_INSTANCE_METHOD_LIST
        Q_BEGIN_CLASS_METHOD_LIST
            Q_DEF_METHOD(registerProtocol:, Q_CM_NAME(Protocol, registerProtocol), +v+@)
            Q_DEF_METHOD(protocolByName:, Q_CM_NAME(Protocol, protocolByName), +@+*)
        Q_END_CLASS_METHOD_LIST
    Q_END_CLASS
}

void        q_loadProtocolClass();
void        q_unloadProtocolClass();

#endif // qobjc_qprotocol_H