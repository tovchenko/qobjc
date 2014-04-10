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
#include "../qthreading.h"

#ifndef WIN_PLATFORM
    #error  Unable to compile file thr_win32.c - this is not windows platform!
#endif // WIN_PLATFORM

#include <windows.h>



Q_ERROR_MESSAGE(er0, "THREADING: TlsAlloc failed!");
Q_ERROR_MESSAGE(er1, "THREADING: TlsGetValue error!");
Q_ERROR_MESSAGE(er2, "THREADING: TlsSetValue error!");
Q_ERROR_MESSAGE(er4, "THREADING: TlsFree error!");


unsigned long
q_allocTls() {
    unsigned long index = TlsAlloc();
    if (index == TLS_OUT_OF_INDEXES)
        q_throwError(er0);
    return index;
}

void*
q_valueFromTls(unsigned long index) {
    void* retVal = TlsGetValue(index);
    if ((!retVal) && (GetLastError() != ERROR_SUCCESS))
        q_throwError(er1);
    return retVal;
}

void
q_setValueToTls(void* val, unsigned long index) {
    if (!TlsSetValue(index, val))
        q_throwError(er2);
}

void
q_deallocTls(unsigned long index) {
    if (!TlsFree(index)) q_throwError(er4);
}