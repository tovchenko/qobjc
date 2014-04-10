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

#ifndef qobjc_qhashtable_H
#define qobjc_qhashtable_H

#define Q_DECLARE_HASHTABLE(tblT, itemT)    typedef HashTable tblT;    \
\
tblT                  q_alloc##tblT();    \
BOOL                  q_addItemTo##tblT(itemT item, tblT table);    \
BOOL                  q_removeItemFrom##tblT(itemT itemKey, tblT table);    \
itemT                 q_itemFrom##tblT(itemT itemKey, tblT table);    \
unsigned int          q_itemsCountIn##tblT(tblT table);    \
struct _HashState     q_initStateFor##tblT(tblT table);    \
BOOL                  q_nextStateFor##tblT(HashState state, tblT table, itemT* item)


#define Q_DEFINE_HASHTABLE(tblT, itemT, hashF, isEqualF, capacity)    \
tblT    \
q_alloc##tblT() {    \
    return (tblT)q_allocHashTable((HashCallback)hashF, (IsEqualCallback)isEqualF, capacity);    \
}    \
\
BOOL    \
q_addItemTo##tblT(itemT item, tblT table) {    \
    return q_addItemToHashTable((RObject)item, (HashTable)table);    \
}    \
\
BOOL    \
q_removeItemFrom##tblT(itemT itemKey, tblT table) {    \
    return q_removeItemFromHashTable((RObject)itemKey, (HashTable)table);    \
}    \
\
itemT    \
q_itemFrom##tblT(itemT itemKey, tblT table) {    \
    return (itemT)q_itemFromHashTable((RObject)itemKey, (HashTable)table);    \
}    \
\
unsigned int    \
q_itemsCountIn##tblT(tblT table) {    \
    return q_itemsCountInHashTable((HashTable)table);    \
}    \
\
struct _HashState    \
q_initStateFor##tblT(tblT table) {    \
    return q_initStateForHashTable((HashTable)table);    \
}    \
\
BOOL    \
q_nextStateFor##tblT(HashState state, tblT table, itemT* item) {    \
    return q_nextStateForHashTable(state, (HashTable)table, (RObject*)item);    \
}




typedef unsigned int    (*HashCallback)(const RObject);
typedef BOOL            (*IsEqualCallback)(const RObject, const RObject);


typedef struct _HashState {
    unsigned int i;
    unsigned int j;
} *HashState;


typedef struct _HashTable {
    Q_ROBJ_HEADER
    HashCallback         hashFunc;
    IsEqualCallback      isEqualFunc;
    unsigned int         count;
    unsigned int         nbBuckets;
    void*                buckets;
} *HashTable;


HashTable             q_allocHashTable(
                        HashCallback hashFunc,
                        IsEqualCallback isEqualFunc,
                        unsigned int capacity);
// add remove items
BOOL                  q_addItemToHashTable(RObject item, HashTable table);
BOOL                  q_removeItemFromHashTable(RObject itemKey, HashTable table);
// get item
RObject               q_itemFromHashTable(const RObject itemKey, HashTable table);
unsigned int          q_itemsCountInHashTable(const HashTable table);
// hash state
struct _HashState     q_initStateForHashTable(const HashTable table);
BOOL                  q_nextStateForHashTable(HashState state, const HashTable table, RObject* item);

#endif // qobjc_qhashtable_H
