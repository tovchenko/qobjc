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
#include "qselector.h"
#include "qprivate.h"


Q_ERROR_MESSAGE(er0, "Selector module not initialized!\n");
Q_ERROR_MESSAGE(er1, "Selector name is nil!\n");
Q_ERROR_MESSAGE(er2, "Selector object is nil!\n");

static void            _deallocSelector(SEL s);
static BOOL            _isEqualSelectors(const SEL s1, const SEL s2);
static unsigned int    _hashFromSelector(const SEL s);

static SelectorTable    _selectorTbl = nil;

Q_DEFINE_HASHTABLE(SelectorTable, SEL, _hashFromSelector, _isEqualSelectors, 100)



SEL
q_allocSelector(const TCHAR* name) {
    SEL sel;
    Q_ALLOC(sel, struct _SEL, _deallocSelector);
    sel->name = q_allocZone((_tcslen(name) + 1) * sizeof(TCHAR));
    _tcscpy(sel->name, name);

    return sel;
}

static void
_deallocSelector(SEL s) {
    q_freeZone(s->name);
    q_freeZone(s);
}

SEL
q_defineSelector(const TCHAR* name) {
    SEL regSel = nil;
    if (!name) {
        q_throwError(er1);
        return nil;
    }
    if (!(regSel = q_selectorByName(name))) {
        SEL sel = q_allocSelector(name);
        regSel = q_registerSelector(sel);
        Q_RELEASE(sel);
    }
    return regSel;
}

SEL
q_registerSelector(const SEL sel) {
    SEL querySel = nil;
    if (!_selectorTbl) {
        q_throwError(er0);
        return nil;
    }
    if (querySel = q_itemFromSelectorTable(sel, _selectorTbl))
        return querySel;

    q_addItemToSelectorTable(sel, _selectorTbl);
    return sel;
}

SEL
q_selectorByName(const TCHAR* name) {
    SEL keySel, resSel;
    if (!_selectorTbl) {
        q_throwError(er0);
        return nil;
    }
    if (!name) {
        q_throwError(er1);
        return nil;
    }

    keySel = q_allocSelector(name);
    resSel = q_itemFromSelectorTable(keySel, _selectorTbl);
    Q_RELEASE(keySel);

    return resSel;
}

const TCHAR*
q_nameBySelector(const SEL sel) {
    if (!_selectorTbl) {
        q_throwError(er0);
        return nil;
    }
    if (!sel) {
        q_throwError(er2);
        return nil;
    }
    if (q_isMappedSelector(sel))
        return sel->name;
    return nil;
}

BOOL
q_isMappedSelector(const SEL sel) {
    if (!_selectorTbl) {
        q_throwError(er0);
        return NO;
    }
    if (!sel) {
        q_throwError(er2);
        return NO;
    }
    if (q_itemFromSelectorTable(sel, _selectorTbl))
        return YES;
    return NO;
}

void
q_initializeSelectorModule() {
    if (_selectorTbl) return;
    _selectorTbl = q_allocSelectorTable();
}

void
q_finalizeSelectorModule() {
    Q_RELEASE(_selectorTbl);
}

static BOOL
_isEqualSelectors(const SEL s1, const SEL s2) {
    return q_isEqualStrings(s1->name, s2->name);
}

static unsigned int
_hashFromSelector(const SEL s) {
    return q_hashFromString(s->name);
}