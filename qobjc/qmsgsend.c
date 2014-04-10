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
#include "qmsgsend.h"
#include "qencoding.h"

#include <stdarg.h>
#include <limits.h>


Q_ERROR_MESSAGE(er0, "SEND: Not found message - \"%s\" in class - \"%s\"!\n");
Q_ERROR_MESSAGE(er1, "SEND: Can\'t send message to super class of Object class - it nil!\n");
const TCHAR* const  kSendStError = _T("SEND: Trying send message with nil pointer on data-struct!\n");


static const char*  _reverseArgTypeStr(const char* types);

static const int        kHalfDoubleSize = sizeof(double) - sizeof(float);
static char             _reverseTypes[UCHAR_MAX];


IMP
q_lookupImp(id obj, SEL op, char** outTypes) {
    Method  m;

    Q_ASSERT(obj->isa);
    if (!(m = q_lookupMethodInClassHierarchy(op, obj->isa))) q_throwError(er0, op, obj->isa->name);
    if (outTypes) *outTypes = m->types;
    return m->imp;
}

IMP
q_lookupImpSuper(id obj, SEL op, char** outTypes) {
    Method  m;
    
    Q_ASSERT(obj->isa);
    if (!obj->isa->super)   q_throwError(er1);
    if (!(m = q_lookupMethodInClassHierarchy(op, obj->isa->super))) q_throwError(er0, op, obj->isa->super->name);
    if (outTypes) *outTypes = m->types;
    return m->imp;
}

int
q_correctFloatValues(void* ptr, const char* types) {
    static struct _Method   queryMethod = { 0, nil, nil, nil, nil };

    unsigned int    curSize = 0;
    unsigned int    argsSize = 0;
    unsigned int    shiftCnt = 0;
    const char*     reverseTypes = _reverseArgTypeStr(types);
    char*           reverseArgPtr = nil;
        
    queryMethod.types = (char*)types;
    q_sizeOfMethodAllArgTypesEncode(&queryMethod, &argsSize);
    reverseArgPtr = (char*)ptr + argsSize;

    while (*reverseTypes) {
        // 'float' argument need correct put onto stack memory,
        // but 'char', 'unsigned char', 'short', 'unsigned short'
        // not need specialy put, because this types already passed as 'int'
        if (*Q_ARG_TYPE(reverseTypes) == Q_C_FLT) {
            char*   val;
            curSize += sizeof(double);
            val = reverseArgPtr - curSize + kHalfDoubleSize + shiftCnt;
            *(float*)(val) = (float)*(double*)(val - kHalfDoubleSize);
            memmove((char*)ptr + kHalfDoubleSize, ptr, argsSize - curSize);
            (char*)ptr += kHalfDoubleSize;
            shiftCnt += kHalfDoubleSize;
        } else {
            curSize += q_sizeOfTypeEncode(Q_ARG_TYPE(reverseTypes));
        }
        reverseTypes = q_skipTypeSpecEncode(Q_ARG_TYPE(reverseTypes));
    }
    return shiftCnt;
}

int
q_retSize(char* signatyre) {
    return q_sizeOfTypeEncode(Q_ARG_TYPE(signatyre));
}

static const char*
_reverseArgTypeStr(const char* types) {
    unsigned int    index = UCHAR_MAX;
    unsigned int    counter = 0;
    unsigned int    i;

    _reverseTypes[index--] = '\0';
    types = q_skipTypeSpecEncode(Q_ARG_TYPE(types));
    while (*types) {
        for (counter = 0; *(++types) != '+' && *types; ++counter); // empty loop
        for (i = 0; i < counter + 1; ++i)
            _reverseTypes[index--] = *(types - i - 1);
    }
    return _reverseTypes + index + 1;
}
