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


#ifndef qobjc_qtstack_H
#define qobjc_qtstack_H

// THREAD SAVE STACK

#define Q_DECLARE_TSTACK(stackT, itemT) typedef struct _##stackT {    \
                                            Q_ROBJ_HEADER    \
                                            unsigned long   specificKey;    \
                                        } *stackT;    \
\
                                        typedef struct _innerItem##stackT {    \
                                            struct _innerItem##stackT*  parent;    \
                                            struct _innerItem##stackT*  child;    \
                                            itemT                       data;    \
                                        } *innerItem##stackT;    \
\
stackT                  q_alloc##stackT();    \
void                    _dealloc##stackT(stackT st);    \
itemT                   q_retainSlotFor##stackT(stackT st);    \
itemT                   q_releaseSlotFor##stackT(stackT st);    \
itemT                   q_currentSlotFor##stackT(stackT st);



#define Q_DEFINE_TSTACK(stackT, itemT, allocDataCallback)    \
\
typedef itemT  (*AllocData##stackT##Callback)();    \
\
static AllocData##stackT##Callback  _allocData##stackT##Callback = allocDataCallback;    \
\
stackT    \
q_alloc##stackT() {    \
    stackT  st;    \
    Q_ALLOC(st, struct _##stackT, _dealloc##stackT);    \
    st->specificKey = q_allocTls();    \
    q_setValueToTls(nil, st->specificKey);    \
    return st;    \
}    \
\
void    \
_dealloc##stackT(stackT st) {    \
    innerItem##stackT curItem;    \
    Q_ASSERT(st);    \
    curItem = q_valueFromTls(st->specificKey);    \
    if (curItem) {    \
        innerItem##stackT it = curItem->child;    \
        while (it) {    \
            if (it->child) {    \
                it = it->child;    \
                Q_RELEASE(it->parent->data);    \
                q_freeZone(it->parent);    \
            } else {    \
                Q_RELEASE(it->data);    \
                q_freeZone(it);    \
                it = nil;    \
            }    \
        }    \
        while (curItem) {    \
            if (curItem->parent) {    \
                curItem = curItem->parent;    \
                Q_RELEASE(curItem->child->data);    \
                q_freeZone(curItem->child);    \
            } else {    \
                Q_RELEASE(curItem->data);    \
                q_freeZone(curItem);    \
                curItem = nil;    \
            }    \
        }    \
    }    \
    if (st->specificKey) q_deallocTls(st->specificKey);    \
    q_freeZone(st);    \
}    \
\
itemT    \
q_retainSlotFor##stackT(stackT st) {    \
    innerItem##stackT   curItem;    \
    if (!st) q_throwError(_T("TSTACK: Trying push data to not existing stack!\n"));    \
    curItem = q_valueFromTls(st->specificKey);    \
    if (curItem) {    \
        if (!curItem->child) {    \
            curItem->child = q_allocZone(sizeof(struct _innerItem##stackT));    \
            curItem->child->parent = curItem;    \
            curItem->child->data = (*_allocData##stackT##Callback)();    \
        }    \
        curItem = curItem->child;    \
    } else {    \
        curItem = q_allocZone(sizeof(struct _innerItem##stackT));    \
        curItem->data = (*_allocData##stackT##Callback)();    \
    }    \
    q_setValueToTls(curItem, st->specificKey);    \
    return curItem->data;    \
}    \
\
itemT    \
q_releaseSlotFor##stackT(stackT st) {    \
    innerItem##stackT curItem = q_valueFromTls(st->specificKey);    \
    if (curItem->parent)    \
        q_setValueToTls(curItem->parent, st->specificKey);    \
    return curItem->data;    \
}    \
\
itemT    \
q_currentSlotFor##stackT(stackT st) {    \
    innerItem##stackT curItem = q_valueFromTls(st->specificKey);    \
    return curItem->data;    \
}

#endif // qobjc_qtstack_H