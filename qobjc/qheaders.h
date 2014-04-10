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

#ifndef qobjc_qheaders_H
#define qobjc_qheaders_H


#ifdef _MSC_VER
    #define WIN_PLATFORM
#else
    #define MAC_PLATFORM
#endif // _MSC_VER


#ifdef WIN_PLATFORM
    #ifndef DECLARE_DLL_SYMBOLS
        #define DECLARE_DLL_SYMBOLS 1
    #endif // DECLARE_DLL_SYMBOLS
    #if DECLARE_DLL_SYMBOLS
        #ifdef QOBJC_EXPORTS
            #define Q_API __declspec(dllexport)
        #else
            #if defined(__cplusplus)
                #define Q_API extern "C"   __declspec(dllimport)
            #else
                #define Q_API __declspec(dllimport)
            #endif // __cplusplus
        #endif // ODRUNTIME_EXPORTS
    #else
        #define Q_API
    #endif // DECLARE_DLL_SYMBOLS
#endif // WIN_PLATFORM


#ifndef nil
    #define nil     0
#endif
#ifndef Nil
    #define Nil     nil
#endif

typedef int BOOL;
#ifndef YES
    #define YES     1
#endif
#ifndef NO
    #define NO      0
#endif


#endif // qobjc_qheaders_H