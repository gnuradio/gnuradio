
//
// Copyright 2011 Free Software Foundation, Inc.
//
// This file is part of GNU Radio
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INCLUDED_PMT_SERIAL_TAGS_H
#define INCLUDED_PMT_SERIAL_TAGS_H

enum pst_tags {
    PST_TRUE = 0x00,
    PST_FALSE = 0x01,
    PST_SYMBOL = 0x02,
    PST_INT32 = 0x03,
    PST_DOUBLE = 0x04,
    PST_COMPLEX = 0x05,
    PST_NULL = 0x06,
    PST_PAIR = 0x07,
    PST_VECTOR = 0x08,
    PST_DICT = 0x09,
    PST_UNIFORM_VECTOR = 0x0a,
    PST_UINT64 = 0x0b,
    PST_TUPLE = 0x0c,
    PST_INT64 = 0x0d,
    UVI_ENDIAN_MASK = 0x80,
    UVI_SUBTYPE_MASK = 0x7f,
    UVI_LITTLE_ENDIAN = 0x00,
    UVI_BIG_ENDIAN = 0x80,
    UVI_U8 = 0x00,
    UVI_S8 = 0x01,
    UVI_U16 = 0x02,
    UVI_S16 = 0x03,
    UVI_U32 = 0x04,
    UVI_S32 = 0x05,
    UVI_U64 = 0x06,
    UVI_S64 = 0x07,
    UVI_F32 = 0x08,
    UVI_F64 = 0x09,
    UVI_C32 = 0x0a,
    UVI_C64 = 0x0b,
    PST_COMMENT = 0x3b,
    PST_COMMENT_END = 0x0a
};
#endif /* INCLUDED_PMT_SERIAL_TAGS_H */
