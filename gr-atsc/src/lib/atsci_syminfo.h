/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef _ATSC_SYMINFO_H_
#define _ATSC_SYMINFO_H_

namespace atsc {

  static const unsigned int SI_SEGMENT_NUM_MASK       = 0x1ff;
  static const unsigned int SI_FIELD_SYNC_SEGMENT_NUM = SI_SEGMENT_NUM_MASK;  // conceptually -1

  struct syminfo {
    unsigned int    symbol_num		: 10;	// 0..831
    unsigned int    segment_num		:  9;	// 0..311 and SI_FIELD_SYNC_SEGMENT_NUM
    unsigned int    field_num		:  1;	// 0..1
    unsigned int    valid		:  1;	// contents are valid
  };


  static inline bool
  tag_is_start_field_sync (syminfo tag)
  {
    return tag.symbol_num == 0 && tag.segment_num == SI_FIELD_SYNC_SEGMENT_NUM && tag.valid;
  }

  static inline bool
  tag_is_start_field_sync_1 (syminfo tag)
  {
    return tag_is_start_field_sync (tag) && tag.field_num == 0;
  }

  static inline bool
  tag_is_start_field_sync_2 (syminfo tag)
  {
    return tag_is_start_field_sync (tag) && tag.field_num == 1;
  }

}

#endif /* _ATSC_SYMINFO_H_ */
