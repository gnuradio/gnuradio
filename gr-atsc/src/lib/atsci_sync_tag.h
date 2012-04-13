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
#ifndef _ATSC_SYNC_TAG_H_
#define _ATSC_SYNC_TAG_H_

/*
 * Constants used to communicate in the second stream passed between
 * GrAtscFSCorrelator, GrAtscEqualizer and GrAtscFieldSyncMux.  The
 * second stream is sample-for-sample parallel with the streaming
 * floating point in the first stream.  The second stream provides
 * information about alignment boundaries.
 *
 * These are in floating point because the current implementation
 * requires that for a given module, all input streams share the same
 * type and all output streams share the same type.  We'd use unsigned
 * char otherwise.
 */

namespace atsc_sync_tag {

  // Nothing special
  static const float NORMAL = 0.0;

  // The corresponding symbol is the first symbol of the
  // data segment sync sequence { +5, -5, -5, +5 }
  static const float START_SEG_SYNC = 1.0;

  // The corresponding symbol is the first symbol of the
  // field sync 1 PN511 pattern.
  static const float START_FIELD_SYNC_1 = 2.0;

  // The corresponding symbol is the first symbol of the
  // field sync 2 PN511 pattern.
  static const float START_FIELD_SYNC_2 = 3.0;
};

#endif /* _ATSC_SYNC_TAG_H_ */
