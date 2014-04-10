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
#include "qencoding.h"

#include <ctype.h>
#include <stdlib.h>


Q_ERROR_MESSAGE(er0, "ENCODING: Bad array type - \"%s\"!\n");
Q_ERROR_MESSAGE(er1, "ENCODING: Undefined type \"%s\"!\n");
Q_ERROR_MESSAGE(er2, "ENCODING: Is not struct type - \"%s\"!\n");
Q_ERROR_MESSAGE(er3, "ENCODING: Wrong method!\n");
Q_ERROR_MESSAGE(er4, "ENCODING: Type - \"%s\" is not argument type!\n");

struct _BitsPerUnit { char a; };
static const unsigned int kBitsPerUnit = 8;
#define STRUCT_SIZE_BOUNDARY    (kBitsPerUnit * sizeof(struct _BitsPerUnit))

#define _MAX(a, b)       (((a) > (b)) ? (a) : (b))
#define _ROUND(v, a)     (a) * (((v) + (a) - 1) / (a))


const char*
q_skipVarNameEncode(const char* type) {
    if (*type == '"')
        for (type++; *type++ != '"';); // empty loop
    return type;
}

const char*
q_skipVarQualifiersEncode(const char* type) {
    // TODO: need to implement
    return type;
}

const char*
q_skipTypeSpecEncode(const char* type) {
    type = q_skipVarNameEncode(type);
    type = q_skipVarQualifiersEncode(type);

    switch (*type) {
    case Q_C_ID:
    case Q_C_CLASS:
    case Q_C_SEL:
    case Q_C_CHR:
    case Q_C_UCHR:
    case Q_C_SHT:
    case Q_C_USHT:
    case Q_C_INT:
    case Q_C_UINT:
    case Q_C_LNG:
    case Q_C_ULNG:
    case Q_C_FLT:
    case Q_C_DBL:
    case Q_C_VOID:
    case Q_C_CHARPTR:
    case Q_C_UNDEF:
        return ++type;
    case Q_C_ARY_B:
        while (isdigit(*++type)); // empty loop
        type = q_skipTypeSpecEncode(type);
        if (*type == Q_C_ARY_E)
            return ++type;
        else q_throwError(er0, type);
    case Q_C_BFLD:
        // skip size of bits
        while (isdigit(*++type)); // empty loop
        return ++type;
    case Q_C_STRUCT_B:
        while (*type != Q_C_STRUCT_E && *type++ != '='); // empty loop
        while (*type != Q_C_STRUCT_E)
            type = q_skipTypeSpecEncode(type);
        return ++type;
    case Q_C_UNION_B:
        while (*type != Q_C_UNION_E && *type++ != '='); // empty loop
        while (*type != Q_C_UNION_E)
            type = q_skipTypeSpecEncode(type);
        return ++type;
    case Q_C_PTR:
        // go to type at pointer
        return q_skipTypeSpecEncode(++type);
    default:
        q_throwError(er1, type);
    }
    return 0;
}

int
q_sizeOfTypeEncode(const char* type) {
    type = q_skipVarNameEncode(type);
    type = q_skipVarQualifiersEncode(type);

    switch (*type) {
    case Q_C_ID:
        return sizeof(id);
    case Q_C_CLASS:
        return sizeof(Class);
    case Q_C_SEL:
        return sizeof(SEL);
    case Q_C_CHR:
        return sizeof(char);
    case Q_C_UCHR:
        return sizeof(unsigned char);
    case Q_C_SHT:
        return sizeof(short);
    case Q_C_USHT:
        return sizeof(unsigned short);
    case Q_C_INT:
        return sizeof(int);
    case Q_C_UINT:
        return sizeof(unsigned int);
    case Q_C_LNG:
        return sizeof(long);
    case Q_C_ULNG:
        return sizeof(unsigned long);
    case Q_C_FLT:
        return sizeof(float);
    case Q_C_DBL:
        return sizeof(double);
    case Q_C_VOID:
        return 0;
    case Q_C_PTR:
    case Q_C_CHARPTR:
        return sizeof(char*);
    case Q_C_ARY_B: {
        int len = atoi(type + 1);
        while (isdigit(*++type)); // empty loop
        return len * q_sizeOfTypeEncode(type);
    }
    case Q_C_BFLD:
        return atoi(type + 1) / kBitsPerUnit;
    case Q_C_STRUCT_B: {
        struct _StructLayout    layout;
        unsigned int            size;

        q_layoutStructBeginEncode(type, &layout);
        while (q_layoutStructNextEncode(&layout)); // empty loop
        q_layoutStructEndEncode(&layout, &size, nil);
        return size;
    }
    case Q_C_UNION_B: {
        int max_size = 0;
        while (*type != Q_C_UNION_E && *type++ != '='); // empty loop
        while (*type != Q_C_UNION_E) {
            type = q_skipVarNameEncode(type);
            max_size = _MAX(max_size, q_sizeOfTypeEncode(type));
            type = q_skipTypeSpecEncode(type);
        }
        return max_size;
    }
    default:
        q_throwError(er1, type);
    }
    return 0;
}

int
q_alignOfTypeEncode(const char* type) {
    type = q_skipVarNameEncode(type);

    switch (*type) {
    case Q_C_ID:
        return __alignof(id);
    case Q_C_CLASS:
        return __alignof(Class);
    case Q_C_SEL:
        return __alignof(SEL);
    case Q_C_CHR:
        return __alignof(char);
    case Q_C_UCHR:
        return __alignof(unsigned char);
    case Q_C_SHT:
        return __alignof(short);
    case Q_C_USHT:
        return __alignof(unsigned short);
    case Q_C_INT:
        return __alignof(int);
    case Q_C_UINT:
        return __alignof(unsigned int);
    case Q_C_LNG:
        return __alignof(long);
    case Q_C_ULNG:
        return __alignof(unsigned long);
    case Q_C_FLT:
        return __alignof(float);
    case Q_C_DBL:
        return __alignof(double);
    case Q_C_VOID:
        return 0;
    case Q_C_PTR:
    case Q_C_CHARPTR:
        return __alignof(char*);
    case Q_C_ARY_B:
        while (isdigit(*++type)); // empty loop
        return q_alignOfTypeEncode(type);
    case Q_C_STRUCT_B: {
        struct _StructLayout    layout;
        unsigned int            align;

        q_layoutStructBeginEncode(type, &layout);
        while (q_layoutStructNextEncode(&layout)); // empty loop
        q_layoutStructEndEncode(&layout, nil, &align);
        return align;
    }
    case Q_C_UNION_B: {
        int maxaling = 0;
        while (*type != Q_C_UNION_E && *type++ != '='); // empty loop
        while (*type != Q_C_UNION_E) {
            type = q_skipVarNameEncode(type);
            maxaling = _MAX(maxaling, q_alignOfTypeEncode(type));
            type = q_skipTypeSpecEncode(type);
        }
        return maxaling;
    }
    default:
        q_throwError(er1, type);
    }
    return 0;
}

void
q_layoutStructBeginEncode(const char* type, StructLayout layout) {
    const char* ntype;

    if (*type++ != Q_C_STRUCT_B) q_throwError(er2, type);

    ntype = type;
    while (*ntype != Q_C_STRUCT_E && *ntype != Q_C_STRUCT_B && *ntype != Q_C_UNION_B && *ntype++ != '='); // empty loop
    if (*(ntype - 1) == '=')
        type = ntype;

    layout->cur_type = type;
    layout->prev_type = nil;
    layout->offset = 0;
    layout->align = _MAX(kBitsPerUnit, STRUCT_SIZE_BOUNDARY);
}

BOOL
q_layoutStructNextEncode(StructLayout layout) {
    unsigned int    desired_align;
    const char*     type;

    // add size of previous layout
    if (layout->prev_type) {
        type = q_skipVarQualifiersEncode(layout->prev_type);

        if (*type == Q_C_BFLD) while (isdigit(*++type)); // empty loop
        layout->offset += q_sizeOfTypeEncode(type) * kBitsPerUnit;
    }

    if (*layout->cur_type == Q_C_STRUCT_E) return NO;

    layout->cur_type = q_skipVarNameEncode(layout->cur_type);
    type = q_skipVarQualifiersEncode(layout->cur_type);

    if (*type == Q_C_BFLD) while (isdigit(*++type)); // empty loop

    desired_align = q_alignOfTypeEncode(type) * kBitsPerUnit;
    layout->align = _MAX(layout->align, desired_align);

    if (layout->offset % desired_align != 0)
        // skip space before this field
        // bump the cumulative size to multiple of field alignment
        layout->offset = _ROUND(layout->offset, desired_align);

    layout->prev_type = layout->cur_type;
    layout->cur_type = q_skipTypeSpecEncode(layout->cur_type);
    return YES;
}

void
q_layoutStructEndEncode(StructLayout layout, int* size, unsigned int* align) {
    if (layout->cur_type && *layout->cur_type == Q_C_STRUCT_E) {
        layout->align = _MAX(1, layout->align);
        layout->offset = _ROUND(layout->offset, layout->align);
        layout->cur_type = nil;
    }

    if (size) *size = layout->offset / kBitsPerUnit;
    if (align) *align = layout->align / kBitsPerUnit;
}

const char*
q_firstArgTypeOfMethodEncode(Method m) {
    if (m) return m->types;
    q_throwError(er3);
    return nil;
}

BOOL
q_nextArgTypeOfMethodEncode(const char** iter) {
    if (Q_IS_ARG_TYPE(*iter)) {
        *iter = q_skipTypeSpecEncode(Q_ARG_TYPE(*iter));
        if (**iter != '\0')
            return YES;
        return NO;
    }
    q_throwError(er4, *iter);
    return NO;
}

int
q_sizeOfMethodRetTypeEncode(Method m) {
    return q_sizeOfTypeEncode(Q_ARG_TYPE(q_firstArgTypeOfMethodEncode(m)));
}

int
q_sizeOfMethodAllArgTypesEncode(Method m, int* sizeWithCorrection) {
    int           size = 0;
    const char*   iter = nil;

    iter = q_firstArgTypeOfMethodEncode(m);
    while (q_nextArgTypeOfMethodEncode(&iter)) {
        int realSize = q_sizeOfTypeEncode(Q_ARG_TYPE(iter));
        size += realSize;
        if (sizeWithCorrection)
            if (*Q_ARG_TYPE(iter) == Q_C_FLT) {
                *sizeWithCorrection += sizeof(double);
            } else if ((*Q_ARG_TYPE(iter) == Q_C_CHR) ||
                       (*Q_ARG_TYPE(iter) == Q_C_UCHR) ||
                       (*Q_ARG_TYPE(iter) == Q_C_SHT) ||
                       (*Q_ARG_TYPE(iter) == Q_C_USHT)) {
                *sizeWithCorrection += sizeof(int);
            } else {
                *sizeWithCorrection += realSize;
            }
    }
    size += sizeof(id);
    size += sizeof(SEL);
    if (sizeWithCorrection) {
        *sizeWithCorrection += sizeof(id);
        *sizeWithCorrection += sizeof(SEL);
    }
    return size;
}