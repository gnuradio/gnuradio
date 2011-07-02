
//
// Copyright 2011 Free Software Foundation, Inc.
//
// This file is part of GNU Radio
//
// GNU Radio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// GNU Radio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//
// THIS FILE IS MACHINE GENERATED FROM gen-serial-tags.py. DO NOT EDIT BY HAND.
// See pmt-serial-tags.scm for additional commentary.
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
