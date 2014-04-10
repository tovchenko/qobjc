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
#include "qhashtable.h"

#include <memory.h>


Q_ERROR_MESSAGE(er0, "HASHTABLE: Can\'t allocate table with nil hash or nil isEqual callback!\n");

typedef union _OneOrMany {
    RObject   one;
    RObject*  many;
} *OneOrMany;

typedef struct _HashBucket {
    unsigned int        count; 
    union _OneOrMany    elements;
} *HashBucket;


#define	BUCKETOF(table, item) (((HashBucket)table->buckets) + \
    ((*table->hashFunc)(item) % table->nbBuckets))

#define ISEQUAL(table, item1, item2) ((item1 == item2) || \
    (*table->isEqualFunc)(item1, item2))


static unsigned int log2 (unsigned int x) { return (x < 2) ? 0 : log2 (x >> 1) + 1; }
static unsigned int exp2m1 (unsigned int x) { return (1 << x) - 1; }

static void         _deallocHashTable(HashTable table);
static void         _reHashTable(HashTable table);
static void         _capacityReHashTable(HashTable table, unsigned int capacity);
static void         _releaseBucketPairs(struct _HashBucket bucket);
static void         _releaseBucketsInHashTable(HashTable table);



HashTable
q_allocHashTable(HashCallback hashFunc,
               IsEqualCallback isEqualFunc,
               unsigned int capacity) {
    HashTable   table;

    if (!hashFunc || !isEqualFunc) q_throwError(er0);

    Q_ALLOC(table, struct _HashTable, _deallocHashTable);
    table->hashFunc = hashFunc;
    table->isEqualFunc = isEqualFunc;
    table->count = 0;
    table->nbBuckets = exp2m1(log2(capacity) + 1);
    table->buckets = q_allocZone(table->nbBuckets * sizeof(struct _HashBucket));
    return table;
}

static void
_deallocHashTable(HashTable table) {
    _releaseBucketsInHashTable(table);
    q_freeZone(table->buckets);
    q_freeZone(table);
}

BOOL
q_addItemToHashTable(RObject item, HashTable table) {
    HashBucket      bucket = BUCKETOF(table, item);
    unsigned int    j = bucket->count;
    RObject*        newEl = nil;
    RObject*        pairs = nil;

    if (!j) {
        ++bucket->count;
        Q_CHANGE_OBJECT(bucket->elements.one, item);
        ++table->count;
        return NO;
    }

    if (j == 1) {
        if (ISEQUAL(table, item, bucket->elements.one)) {
            Q_CHANGE_OBJECT(bucket->elements.one, item);
            return YES;
        }

        newEl = q_allocZone(2 * sizeof(RObject));
        newEl[1] = bucket->elements.one;
        Q_RETAIN(item);
        newEl[0] = item;

        ++bucket->count;
        bucket->elements.many = newEl;
        ++table->count;

        if (table->count > table->nbBuckets) _reHashTable(table);
        return NO;
    }

    pairs = bucket->elements.many;
    while (j--)
        if (ISEQUAL(table, item, *pairs)) {
            Q_CHANGE_OBJECT(*pairs, item);
            return YES;
        }

    ++pairs;
    newEl = q_allocZone((bucket->count + 1) * sizeof(RObject));
    if (bucket->count)
        memcpy(newEl + 1, bucket->elements.many, bucket->count * sizeof(RObject));
    Q_RETAIN(item);
    newEl[0] = item;
    q_freeZone(bucket->elements.many);
    ++bucket->count;
    bucket->elements.many = newEl;
    ++table->count;

    if (table->count > table->nbBuckets) _reHashTable(table);
    return NO;
}

BOOL
q_removeItemFromHashTable(RObject itemKey, HashTable table) {
    HashBucket      bucket = BUCKETOF(table, itemKey);
    unsigned int    j = bucket->count;
    RObject*        pairs = nil;
    RObject*        newEl = nil;

    if (!j) return NO;

    if (j == 1) {
        if (!ISEQUAL(table, itemKey, bucket->elements.one)) return nil;

        --table->count;
        --bucket->count;
        Q_RELEASE(bucket->elements.one);
        return YES;
    }

    pairs = bucket->elements.many;
    if (j == 2) {
        if (ISEQUAL(table, itemKey, pairs[0])) {
            bucket->elements.one = pairs[1];
            Q_RELEASE(pairs[0]);
        } else if (ISEQUAL(table, itemKey, pairs[1])) {
            bucket->elements.one = pairs[0];
            Q_RELEASE(pairs[1]);
        } else {
            return NO;
        }

        q_freeZone(pairs);
        --table->count;
        --bucket->count;
        return YES;
    }

    while (j--) {
        if (ISEQUAL(table, itemKey, *pairs)) {
            Q_RELEASE(*pairs);
            newEl = (bucket->count - 1)
                ? q_allocZone((bucket->count - 1) * sizeof(RObject))
                : nil;
            if (bucket->count != j) memcpy(newEl, bucket->elements.many,
                                           sizeof(RObject) * (bucket->count - j - 1));
            if (j) memcpy(newEl + bucket->count - j - 1,
                          bucket->elements.many + bucket->count - j,
                          sizeof(RObject) * j);
            q_freeZone(bucket->elements.many);
            --table->count;
            --bucket->count;
            bucket->elements.many = newEl;
            return YES;
        }
        ++pairs;
    }
    return NO;
}

RObject
q_itemFromHashTable(const RObject itemKey, HashTable table) {
    HashBucket      bucket = BUCKETOF(table, itemKey);
    unsigned int    j = bucket->count;
    RObject*        pairs = nil;

    if (!j) return nil;

    if (j == 1)
        return ISEQUAL(table, itemKey, bucket->elements.one)
            ? bucket->elements.one
            : nil;

    pairs = bucket->elements.many;
    while (j--) {
        if (ISEQUAL(table, itemKey, *pairs))
            return *pairs;
        ++pairs;
    }
    return nil;
}

unsigned int
q_itemsCountInHashTable(const HashTable table) {
    if (table) return table->count;
    return 0;
}

struct _HashState
q_initStateForHashTable(const HashTable table) {
    struct _HashState state;
    state.i = table->nbBuckets;
    state.j = 0;
    return state;
}

BOOL
q_nextStateForHashTable(HashState state, const HashTable table, RObject* item) {
    HashBucket buckets = table->buckets;

    while (state->j == 0) {
        if (state->i == 0) return NO;
        --state->i;
        state->j = buckets[state->i].count;
    }

    --state->j;
    buckets += state->i;
    *item = (buckets->count == 1)
        ? buckets->elements.one
        : buckets->elements.many[state->j];
    return YES;
}

static void
_reHashTable(HashTable table) {
    _capacityReHashTable(table, table->nbBuckets * 2 + 1);
}

static void
_capacityReHashTable(HashTable table, unsigned int capacity) {
    HashTable           old = q_allocHashTable(table->hashFunc, table->isEqualFunc, 1);
    struct _HashState   state;
    RObject             aux = nil;

    old->count = table->count;
    old->nbBuckets = table->nbBuckets;
    q_freeZone(old->buckets);
    old->buckets = table->buckets;

    table->nbBuckets = capacity;
    table->count = 0;
    table->buckets = q_allocZone(table->nbBuckets * sizeof(struct _HashBucket));

    state = q_initStateForHashTable(old);
    while (q_nextStateForHashTable(&state, old, &aux)) q_addItemToHashTable(aux, table);

    Q_RELEASE(old);
}

static void
_releaseBucketPairs(struct _HashBucket bucket) {
    unsigned int    j = bucket.count;
    RObject*        pairs = nil;

    if (j == 1) {
        Q_RELEASE(bucket.elements.one);
        return;
    }

    pairs = bucket.elements.many;
    while (j--) {
        Q_RELEASE(*pairs);
        ++pairs;
    }
    q_freeZone(bucket.elements.many);
}

static void
_releaseBucketsInHashTable(HashTable table) {
    unsigned int i = table->nbBuckets;
    HashBucket   buckets = table->buckets;

    while (i--) {
        if (buckets->count) {
            _releaseBucketPairs(*buckets);
            buckets->count = 0;
            buckets->elements.one = nil;
        }
        ++buckets;
    }
}
