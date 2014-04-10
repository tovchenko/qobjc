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

#ifndef qobjc_qloading_H
#define qobjc_qloading_H

#include "qhashtable.h"


#define Q_PUT_LOAD_MODULE                   static int q_loadModule();    \
                                            static int (*_loadModulePtr)() = q_loadModule;

#define Q_BEGIN_MODULE                      static int q_loadModule() {    \
                                                if (_loadModulePtr) {    \
                                                    static int (*_loadModulePtr)() = nil;

#define Q_SYMBOL_INFO(name, callback, type)       { SymbolHandle sh = q_allocSymbolHandle(_T(#name), callback);    \
                                                    if (type == Q_CATEGORY_TYPE)    \
                                                        q_addItemToLoadCategoryArray(sh, q_infoCategoryArray());    \
                                                    else    \
                                                        q_addItemToLoadSymbolTable(sh, q_infoTableBySymbolType(type));    \
                                                    Q_RELEASE(sh); }

#define Q_END_MODULE                            }    \
                                                return 0;    \
                                            }


struct _Class;

#define Q_CATEGORY_TYPE     5

typedef enum _SymbolType {
    Q_CLASS_TYPE,
    Q_PROTOCOL_TYPE
} SymbolType;

typedef int (*LoadSymbolCallback)();

typedef struct _SymbolHandle {
    Q_ROBJ_HEADER
    TCHAR*              name;
    LoadSymbolCallback  loadFunc;
} *SymbolHandle;

Q_DECLARE_HASHTABLE(LoadSymbolTable, SymbolHandle);
Q_DECLARE_ARRAY(LoadCategoryArray, SymbolHandle);


SymbolHandle            q_allocSymbolHandle(TCHAR* name, LoadSymbolCallback func);

LoadSymbolTable         q_infoTableBySymbolType(SymbolType type);
LoadCategoryArray       q_infoCategoryArray();

void                    q_loadSymbolByType(const TCHAR* name, SymbolType type);
void                    q_loadCategoriesForClass(const struct _Class* cls);

void                    q_finalizeLoaderModule();

#endif // qobjc_qloading_H
