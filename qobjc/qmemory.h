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

#ifndef qobjc_qmemory_H
#define qobjc_qmemory_H

#include "qarray.h"


typedef     void (*ReleaseCallBack)(void*);


#define Q_ROBJ_HEADER    unsigned int       _refCnt;    \
                       ReleaseCallBack      _release;

#define Q_RETAIN(obj)         ++(obj)->_refCnt
#define Q_RELEASE(obj)        if (obj && --(obj)->_refCnt == 0) { (*(obj)->_release)(obj); (obj) = nil; }
#define Q_AUTORELEASE(obj)    if (obj) q_addObjectToPool((RObject)obj, q_currentThreadPool())
#define Q_CHANGE_OBJECT(oldObj, newObj)   Q_RETAIN(newObj); Q_RELEASE(oldObj); (oldObj) = (newObj)

#define Q_ALLOC(obj, type, deallocCallBack)     obj = q_allocZone(sizeof(type));    \
                                                Q_RETAIN(obj);    \
                                                obj->_release = deallocCallBack

#define Q_ALLOC_BY_SIZE(obj, size, deallocCallBack)     obj = q_allocZone(size);    \
                                                        Q_RETAIN(obj);    \
                                                        obj->_release = deallocCallBack

typedef struct _RObject {
    Q_ROBJ_HEADER
} *RObject;


// generic memory management
void*                   q_allocZone(size_t size);
void*                   q_reallocZone(void* ptr, size_t oldSize, size_t newSize);
void                    q_freeZone(void* ptr);
void                    q_finalizeMemoryModule();
// memory blocks for stack storages
void*                   q_retainMemBlock();
void*                   q_releaseMemBlock();
void*                   q_currentMemBlock();


// autorelease functionality
Q_DECLARE_ARRAY(PoolPage, RObject);
Q_DECLARE_ARRAY(PageArray, PoolPage);

typedef struct _AutoreleasePool {
    Q_ROBJ_HEADER
    PageArray                   pages;
    struct _AutoreleasePool*    parentPool;
    struct _AutoreleasePool*    childPool;
} *AutoreleasePool;

void                    q_addObjectToPool(RObject obj, AutoreleasePool pool);
AutoreleasePool         q_allocAutoreleasePool();
AutoreleasePool         q_currentThreadPool();
// utilities for memory
void                    q_zeroMemory(void* ptr, size_t sz);


#endif // qobjc_qmemory_H