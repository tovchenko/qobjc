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
#include "qprivate.h"



BOOL
q_isEqualStrings(const TCHAR* str1, const TCHAR* str2) {
    TCHAR c1, c2;
    for (; (c1 = *str1) == (c2 = *str2); ++str1, ++str2)
        if (c1 == '\0') return YES;
    return NO;
}

unsigned int
q_hashFromString(const TCHAR* str) {
    unsigned int hash = 0;

    for (;;) {
        unsigned int a = *str++;
        if (0 == a) break;
        hash += (hash << 8) + a;
    }
    return hash;
}