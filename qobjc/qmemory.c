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
#include "qmemory.h"
#include "qtstack.h"
#include "qthreading.h"

#include <stdlib.h>
#include <memory.h>


Q_ERROR_MESSAGE(er0, "MEMORY: Virtual memory exhausted!\n");
Q_ERROR_MESSAGE(er1, "MEMORY: Trying set value to TLS through nil index!\n");
Q_ERROR_MESSAGE(er2, "MEMORY: Trying add object - %p to AutoreleasePool - %p!\n");


#define STACK_BYTE_COUNT    28

typedef struct _MemBlock {
    Q_ROBJ_HEADER
    char    memory[STACK_BYTE_COUNT];
} *MemBlock;

static void         _deallocAutoreleasePool(AutoreleasePool pool);
static void         _setCurrentThreadPool(AutoreleasePool pool);
static void         _freeDataForThreadPool();

static MemBlock     _allocMemBlock();
static void         _deallocMemBlock(MemBlock mb);

static const unsigned int kCntObjOnPage = 512;

Q_DECLARE_TSTACK(MemoryStack, MemBlock);

static unsigned long    _tlsIndex = 0;
static MemoryStack      _stackMem = nil;

Q_DEFINE_TSTACK(MemoryStack, MemBlock, _allocMemBlock);
Q_DEFINE_ARRAY(PoolPage, RObject, kCntObjOnPage)
Q_DEFINE_ARRAY(PageArray, PoolPage, 1)



void*
q_allocZone(size_t size) {
    void* res = malloc(size);
    if (!res) q_throwError(er0);
    q_zeroMemory(res, size);
    return res;
}

void*
q_reallocZone(void* ptr, size_t oldSize, size_t newSize) {
    void* res = realloc(ptr, newSize);
    if (!res) q_throwError(er0);
    q_zeroMemory((char*)res + oldSize, newSize - oldSize);
    return res;
}

void
q_freeZone(void* ptr) {
    if (ptr) free(ptr);
}

void
q_finalizeMemoryModule() {
    Q_RELEASE(_stackMem);
    _freeDataForThreadPool();
}

static MemBlock
_allocMemBlock() {
    MemBlock mb;
    Q_ALLOC(mb, struct _MemBlock, _deallocMemBlock);
    return mb;
}

static void
_deallocMemBlock(MemBlock mb) {
    q_freeZone(mb);
}

void*
q_retainMemBlock() {
    if (!_stackMem) _stackMem = q_allocMemoryStack();
    return q_retainSlotForMemoryStack(_stackMem)->memory;
}

void*
q_releaseMemBlock() {
    Q_ASSERT(_stackMem);
    return q_releaseSlotForMemoryStack(_stackMem)->memory;
}

void*
q_currentMemBlock() {
    Q_ASSERT(_stackMem);
    return q_currentSlotForMemoryStack(_stackMem)->memory;
}

void
q_addObjectToPool(RObject obj, AutoreleasePool pool) {
    PoolPage        lastPage;
    unsigned int    pageCnt, objCnt;

    if (!obj || !pool) q_throwError(er2, obj, pool);

    pageCnt = q_itemsCountInPageArray(pool->pages);
    lastPage = q_itemAtIndexInPageArray(pageCnt - 1, pool->pages);
    objCnt = q_itemsCountInPoolPage(lastPage);
    if (objCnt && !(objCnt % kCntObjOnPage)) {
        PoolPage page = q_allocPoolPage();
        q_addItemToPageArray(page, pool->pages);
        q_addItemToPoolPage(obj, page);
        Q_RELEASE(page);
    } else {
        q_addItemToPoolPage(obj, lastPage);
    }
    Q_RELEASE(obj);
}

AutoreleasePool
q_allocAutoreleasePool() {
    PoolPage page;
    AutoreleasePool curPool = q_currentThreadPool();
    AutoreleasePool pool;

    Q_ALLOC(pool, struct _AutoreleasePool, _deallocAutoreleasePool);
    pool->parentPool = curPool;
    pool->pages = q_allocPageArray();
    page = q_allocPoolPage();
    q_addItemToPageArray(page, pool->pages);
    Q_RELEASE(page);

    if (curPool) curPool->childPool = pool;
    pool->childPool = nil;

    _setCurrentThreadPool(pool);
    return pool;
}

static void
_deallocAutoreleasePool(AutoreleasePool pool) {
    Q_RELEASE(pool->childPool);
    Q_RELEASE(pool->pages);

    _setCurrentThreadPool(pool->parentPool);
    if (pool->parentPool) pool->parentPool->childPool = nil;
    q_freeZone(pool);
}

AutoreleasePool
q_currentThreadPool() {
    if (!_tlsIndex) {
        _tlsIndex = q_allocTls();
        Q_ASSERT(_tlsIndex);
        q_setValueToTls(nil, _tlsIndex);
    }
    return (AutoreleasePool)q_valueFromTls(_tlsIndex);
}

static void
_freeDataForThreadPool() {
    if (_tlsIndex) q_deallocTls(_tlsIndex);
}

static void
_setCurrentThreadPool(AutoreleasePool pool) {
    if (!_tlsIndex) q_throwError(er1);
    q_setValueToTls(pool, _tlsIndex);
}

void
q_zeroMemory(void* ptr, size_t sz) {
    memset(ptr, 0, sz);
}
