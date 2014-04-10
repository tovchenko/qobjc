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

#ifndef qobjc_qarray_H
#define qobjc_qarray_H


#define Q_DECLARE_ARRAY(arrT, itemT)  typedef struct _##arrT {    \
                                          Q_ROBJ_HEADER    \
                                          unsigned int    count;    \
                                          unsigned int    nBloks;    \
                                          itemT*          items;    \
                                      } *arrT;    \
\
arrT              q_alloc##arrT();    \
void              _dealloc##arrT(arrT arr);    \
void              q_addItemTo##arrT(itemT item, arrT arr);    \
itemT             q_itemAtIndexIn##arrT(int index, arrT arr);    \
void              q_removeItemAtIndexIn##arrT(int index, arrT arr);    \
unsigned int      q_itemsCountIn##arrT(arrT arr)



// first variant array definition
#define Q_DEFINE_ARRAY(arrT, itemT, growNum)    \
\
arrT    \
q_alloc##arrT() {    \
    arrT arr;    \
    Q_ALLOC(arr, struct _##arrT, _dealloc##arrT);    \
    arr->count = 0;    \
    arr->nBloks = 0;    \
    arr->items = nil;    \
    return arr;    \
}    \
\
void    \
q_addItemTo##arrT(itemT item, arrT arr) {    \
    if (!item || !arr) q_throwError(_T("You want add object - %p to array - %p!\n"), item, arr);    \
    if (!(arr->nBloks % growNum))    \
        arr->items = (itemT*)q_reallocZone(arr->items, arr->nBloks * sizeof(itemT), (arr->nBloks + growNum) * sizeof(itemT));    \
    Q_RETAIN(item);    \
    if (arr->count >= arr->nBloks) ++arr->nBloks;    \
    arr->items[arr->count++] = item;    \
}    \
\
void    \
q_removeItemAtIndexIn##arrT(int index, arrT arr) {    \
    itemT it;    \
    if (!arr) q_throwError(_T("Can\'t remove object from nil array!\n"));    \
    it = q_itemAtIndexIn##arrT(index, arr);    \
    Q_RELEASE(it);    \
    memcpy(    \
        arr->items + index,    \
        arr->items + index + 1,    \
        (arr->count - index - 1) * sizeof(itemT));    \
    --arr->count;    \
}    \
\
Q_ARRAY_METHODS_DEFINITION(arrT, itemT, growNum)



// second variant array definition
#define Q_DEFINE_ARRAY_WITH_CALLBACKS(arrT, itemT, growNum, allocCallback, addRemoveCallback)    \
\
typedef void (*Alloc##arrT##Callback)(arrT*);    \
typedef void (*AddRemove##arrT##Callback)(itemT, arrT, BOOL direction);    \
\
static Alloc##arrT##Callback        _alloc##arrT##Callback = allocCallback;    \
static AddRemove##arrT##Callback    _addRemove##arrT##Callback = addRemoveCallback;    \
\
arrT    \
q_alloc##arrT() {    \
    arrT arr;    \
    Q_ALLOC(arr, struct _##arrT, _dealloc##arrT);    \
    arr->count = 0;    \
    arr->nBloks = 0;    \
    arr->items = nil;    \
    _alloc##arrT##Callback(&arr);    \
    return arr;    \
}    \
\
void    \
q_addItemTo##arrT(itemT item, arrT arr) {    \
    if (!item || !arr) q_throwError(_T("Adding object - %p to array - %p!\n"), item, arr);    \
    if (!(arr->nBloks % growNum))    \
        arr->items = (itemT*)q_reallocZone(arr->items, arr->nBloks * sizeof(itemT), (arr->nBloks + growNum) * sizeof(itemT));    \
    Q_RETAIN(item);    \
    if (arr->count >= arr->nBloks) ++arr->nBloks;    \
    arr->items[arr->count++] = item;    \
    _addRemove##arrT##Callback(item, arr, YES);    \
}    \
\
void    \
q_removeItemAtIndexIn##arrT(int index, arrT arr) {    \
    itemT it;    \
    if (!arr) q_throwError(_T("Can\'t remove object from nil array!\n"));    \
    it = q_itemAtIndexIn##arrT(index, arr);    \
    _addRemove##arrT##Callback(it, arr, NO);    \
    Q_RELEASE(it);    \
    memmove(    \
        arr->items + index,    \
        arr->items + index + 1,    \
        (arr->count - index - 1) * sizeof(itemT));    \
    --arr->count;    \
}    \
\
Q_ARRAY_METHODS_DEFINITION(arrT, itemT, growNum)



// base definition
#define Q_ARRAY_METHODS_DEFINITION(arrT, itemT, growNum)    \
\
void    \
_dealloc##arrT(arrT arr) {    \
    unsigned int i;    \
    if (!arr) q_throwError(_T("Can\'t dealloc nil array!\n"));    \
    for (i = 0; i < arr->count; ++i)    \
        Q_RELEASE(arr->items[i]);    \
    q_freeZone(arr->items);    \
    q_freeZone(arr);    \
}    \
\
itemT    \
q_itemAtIndexIn##arrT(int index, arrT arr) {    \
    if (((unsigned int)index) >= arr->count)    \
        q_throwError(_T("Index value grater than items count!\n"));    \
    return arr->items[index];    \
}    \
\
unsigned int    \
q_itemsCountIn##arrT(arrT arr) {    \
    if (!arr) q_throwError(_T("Can\'t return count of items from nil array!\n"));    \
    return arr->count;    \
}

#endif // qobjc_qarray_H