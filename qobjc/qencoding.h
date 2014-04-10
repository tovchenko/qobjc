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

#ifndef qobjc_qencoding_H
#define qobjc_qencoding_H

#include "qclass.h"


// skip sign '+' than div type arguments string for method
#define Q_ARG_TYPE(type_)             ((type_) + 1)
#define Q_IS_ARG_TYPE(type_)          ((*type_) == '+')


/************************************************************************/
/* Bit Field:   b'size''type'   b12i       but VC2008 not use the       */
/*                                         number of bits, it use only  */
/*                                         information about field type */
/************************************************************************/


// encode symbols for types
#define Q_C_ID       '@' // id
#define Q_C_CLASS    '#' // Class
#define Q_C_SEL      ':' // SEL
#define Q_C_CHR      'c' // char
#define Q_C_UCHR     'C' // unsigned char
#define Q_C_SHT      's' // short
#define Q_C_USHT     'S' // unsigned short
#define Q_C_INT      'i' // int
#define Q_C_UINT     'I' // unsigned int
#define Q_C_LNG      'l' // long
#define Q_C_ULNG     'L' // unsigned long
#define Q_C_FLT      'f' // float
#define Q_C_DBL      'd' // double
#define Q_C_BFLD     'b' // bit field
#define Q_C_VOID     'v' // void
#define Q_C_UNDEF    '?' // undefined
#define Q_C_PTR      '^' // pointer
#define Q_C_CHARPTR  '*' // pointer to char array
#define Q_C_ARY_B    '[' // array begin
#define Q_C_ARY_E    ']' // array end
#define Q_C_UNION_B  '(' // union begin
#define Q_C_UNION_E  ')' // union end
#define Q_C_STRUCT_B '{' // struct begin
#define Q_C_STRUCT_E '}' // struct end


typedef struct _StructLayout {
    const char*    cur_type;
    const char*    prev_type;
    int            offset;
    unsigned int   align;
} *StructLayout;

// basic skip operations
const char*               q_skipVarNameEncode(const char* type);
const char*               q_skipVarQualifiersEncode(const char* type);
const char*               q_skipTypeSpecEncode(const char* type);
// size and align of type
int                       q_sizeOfTypeEncode(const char* type);
int                       q_alignOfTypeEncode(const char* type);
// work with struct layouts
void                      q_layoutStructBeginEncode(const char* type, StructLayout layout);
BOOL                      q_layoutStructNextEncode(StructLayout layout);
void                      q_layoutStructEndEncode(StructLayout layout, int* size, unsigned int* align);
// method signature encoding
const char*               q_firstArgTypeOfMethodEncode(Method m);
BOOL                      q_nextArgTypeOfMethodEncode(const char** iter);
int                       q_sizeOfMethodRetTypeEncode(Method m);
int                       q_sizeOfMethodAllArgTypesEncode(Method m, int* sizeWithCorrection);

#endif // qobjc_qencoding_H
