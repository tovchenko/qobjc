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
#include "qerror.h"

#include <stdarg.h>
#include <stdlib.h>



static ErrorHandlerType _customErrorHandler = nil;



void
q_throwError(const TCHAR* const message, ...) {
    va_list paramList;
    va_start(paramList, message);
    q_throwErrorWithObjectAndVAList(nil, message, paramList);
    va_end(paramList);
}

void
q_throwErrorWithObject(void* obj, const TCHAR* const message, ...) {
    va_list paramList;
    va_start(paramList, message);
    q_throwErrorWithObjectAndVAList(obj, message, paramList);
    va_end(paramList);
}

void
q_throwErrorWithObjectAndVAList(void* obj, const TCHAR* const message, va_list traits) {
    if (_customErrorHandler) {
        if ((*_customErrorHandler)(obj, message, traits)) return;
    } else {
        _vtprintf(message, traits);
        abort();
    }
}

void
q_setErrorHandler(ErrorHandlerType erHandler) {
    _customErrorHandler = erHandler;
}
