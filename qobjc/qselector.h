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

#ifndef qobjc_qselector_H
#define qobjc_qselector_H

#include "qhashtable.h"


typedef struct _SEL {
    Q_ROBJ_HEADER
    TCHAR*  name;
} *SEL;

Q_DECLARE_HASHTABLE(SelectorTable, SEL);



SEL                     q_allocSelector(const TCHAR* name);
SEL                     q_defineSelector(const TCHAR* name);
// work with registers selectors
SEL                     q_registerSelector(const SEL sel);
SEL                     q_selectorByName(const TCHAR* name);
const TCHAR*            q_nameBySelector(const SEL sel);
BOOL                    q_isMappedSelector(const SEL sel);
// selector module
void                    q_initializeSelectorModule();
void                    q_finalizeSelectorModule();

#endif // qobjc_qselector_H
