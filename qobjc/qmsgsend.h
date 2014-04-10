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

#ifndef qobjc_qmsgsend_H
#define qobjc_qmsgsend_H

#include "qclass.h"


IMP                     q_lookupImp(id obj, SEL op, char** outTypes);
IMP                     q_lookupImpSuper(id obj, SEL op, char** outTypes);
int                     q_correctFloatValues(void* ptr, const char* types);
int                     q_retSize(char* signatyre);

void*                   q_send(id obj, SEL op, ...);
double                  q_sendFp(id obj, SEL op, ...);
void                    q_sendSt(void* outSt, id obj, SEL op, ...);

void*                   q_sendSuper(id obj, SEL op, ...);
double                  q_sendFpSuper(id obj, SEL op, ...);
void                    q_sendStSuper(void* outSt, id obj, SEL op, ...);



#endif // qobjc_qmsgsend_H