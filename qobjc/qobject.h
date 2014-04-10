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

#ifndef qobjc_qobject_H
#define qobjc_qobject_H

#include "qconnect.h"


Q_DECLARE_CLASS(Object);

// instance methods
id      Q_IM_NAME(Object, init)(id self, SEL _cmd);
void    Q_IM_NAME(Object, dealloc)(id self, SEL _cmd);
Class   Q_IM_NAME(Object, class)(id self, SEL _cmd);
BOOL    Q_IM_NAME(Object, conformsTo)(id self, SEL _cmd, Protocol* prtObj);

// class methods
id      Q_CM_NAME(Object, alloc)(id self, SEL _cmd);
id      Q_CM_NAME(Object, new)(id self, SEL _cmd);
Class   Q_CM_NAME(Object, class)(id self, SEL _cmd);
BOOL    Q_CM_NAME(Object, conformsTo)(id self, SEL _cmd, Protocol* prtObj);


static int
Q_M_NAME_LOAD(Object) {
    Q_BEGIN_ROOT_CLASS(Object)
        Q_BEGIN_INSTANCE_METHOD_LIST
            Q_DEF_METHOD(init, Q_IM_NAME(Object, init), +@)
            Q_DEF_METHOD(dealloc, Q_IM_NAME(Object, dealloc), +v)
            Q_DEF_METHOD(class, Q_IM_NAME(Object, class), +#)
            Q_DEF_METHOD(conformsTo:, Q_IM_NAME(Object, conformsTo), +i)
        Q_END_INSTANCE_METHOD_LIST
        Q_BEGIN_CLASS_METHOD_LIST
            Q_DEF_METHOD(alloc, Q_CM_NAME(Object, alloc), +@)
            Q_DEF_METHOD(new, Q_CM_NAME(Object, new), +@)
            Q_DEF_METHOD(class, Q_CM_NAME(Object, class), +#)
            Q_DEF_METHOD(conformsTo:, Q_CM_NAME(Object, conformsTo), +i)
        Q_END_CLASS_METHOD_LIST
    Q_END_CLASS
}


void        q_loadRootClass();
void        q_unloadRootClass();


#endif // qobjc_qobject_H
