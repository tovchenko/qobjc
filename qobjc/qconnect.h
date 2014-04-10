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

#ifndef qobjc_qconnect_H
#define qobjc_qconnect_H

#include "qclass.h"


#define Q_DECLARE_CLASS(className)          typedef struct _Object className

#define Q_BEGIN_ROOT_CLASS(rootName)        { Class cls = q_allocClass(_T(#rootName)); q_registerRootClass(cls);
#define Q_BEGIN_CLASS(selfName, superName)  { Class cls = q_allocClass(_T(#selfName)); q_registerClass(cls, q_classByName(_T(#superName)));
#define Q_END_CLASS                           Q_RELEASE(cls); } return 0;

#define Q_BEGIN_IVAR_LIST                   { if (!q_ivarArray(cls)) { IvarArray iva = q_allocIvarArray();    \
                                                                       q_setIvarArray(cls, iva); Q_RELEASE(iva); }

#define Q_DEF_IVAR(name, type)              { Ivar ivar = q_allocIvar(_T(#name), #type); q_addItemToIvarArray(ivar, q_ivarArray(cls));    \
                                              Q_RELEASE(ivar); }

#define Q_BEGIN_INSTANCE_METHOD_LIST        { MethodList mlist = q_allocMethodList();    \
                                              if (!q_methodListArray(cls)) { MethodListArray mla = q_allocMethodListArray();    \
                                                                             q_setMethodListArray(cls, mla); Q_RELEASE(mla); }
#define Q_END_INSTANCE_METHOD_LIST          q_addItemToMethodListArray(mlist, q_methodListArray(cls)); Q_RELEASE(mlist); }

#define Q_BEGIN_CLASS_METHOD_LIST           { MethodList mlist = q_allocMethodList();    \
                                              if (!q_methodListArray(cls->isa)) { MethodListArray mla = q_allocMethodListArray();    \
                                                                                  q_setMethodListArray(cls->isa, mla); Q_RELEASE(mla); }
#define Q_END_CLASS_METHOD_LIST             q_addItemToMethodListArray(mlist, q_methodListArray(cls->isa)); Q_RELEASE(mlist); }

#define Q_DEF_METHOD(name, imp, types)      { SEL regSel = q_defineSelector(_T(#name));    \
                                              Method m = q_allocMethod(regSel, imp, #types);    \
                                              q_addItemToMethodList(m, mlist); Q_RELEASE(m); }

#define Q_BEGIN_PROTOCOL(protoName)         { Protocol* prt = q_send(q_send(Q_CLASS(Protocol), Q_SELECTOR(alloc)), Q_SELECTOR(initWithName:), _T(#protoName));    \
                                              q_send(Q_CLASS(Protocol), Q_SELECTOR(registerProtocol:), prt);

#define Q_END_PROTOCOL                      Q_RELEASE(prt); } return 0;

#define Q_INCLUDE_PROTOCOL_TO_PROTOCOL(protoName)    { q_send(prt, Q_SELECTOR(addProtocol:), Q_PROTOCOL(protoName)); }
#define Q_INCLUDE_PROTOCOL_TO_CLASS(protoName)       { q_addProtocolToClass(Q_PROTOCOL(protoName), cls); }

#define Q_BEGIN_INSTANCE_DESC               { BOOL descType = YES;
#define Q_BEGIN_CLASS_DESC                  { BOOL descType = NO;

#define Q_DEF_METHOD_DESC(op, types)        { SEL regSel = q_defineSelector(_T(#op));    \
                                              MethodDescription md = q_allocMethodDescription(regSel, #types);    \
                                              q_send(prt, Q_SELECTOR(addMethodDescription:forInstance:), md, descType);    \
                                              Q_RELEASE(md); }

#define Q_BEGIN_CATEGORY(clsName, ctgName)  { Class cls = Q_CLASS_OBJ(clsName);
#define Q_END_CATEGORY                      }; return 0; 

#define Q_END                               }


#define Q_CLASS(name)                       (id)q_classByName(_T(#name))
#define Q_CLASS_OBJ(name)                   q_classByName(_T(#name))
#define Q_SELECTOR(name)                    q_selectorByName(_T(#name))
#define Q_PROTOCOL(name)                    (Protocol*)q_send(Q_CLASS(Protocol), Q_SELECTOR(protocolByName:), _T(#name))

// instance method name
#define Q_IM_NAME(className, methodName)    q_imp_##_instance_##className##methodName
// class method name
#define Q_CM_NAME(className, methodName)    q_imp_##_class_##className##methodName
// method witch load class object
#define Q_M_NAME_LOAD(className)            q_load_##__##className()




void                    q_initializeEnvironment();
void                    q_finalizeEnvironment();

#endif // qobjc_qconnect_H
