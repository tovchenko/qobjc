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

#ifndef qobjc_stdafx_H
#define qobjc_stdafx_H


#include "qheaders.h"

#ifdef WIN_PLATFORM
    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
    #define _CRT_SECURE_NO_WARNINGS
    #include <tchar.h>
    #include "targetver.h"
#endif // WIN_PLATFORM


#include "qerror.h"
#include "qmemory.h"


#endif // qobjc_stdafx_H
