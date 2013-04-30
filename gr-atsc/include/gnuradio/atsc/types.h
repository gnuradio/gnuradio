/* -*- c++ -*- */
/*
 * Copyright 2001,2006 Free Software Foundation, Inc.
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

#ifndef _ATSC_TYPES_H_
#define _ATSC_TYPES_H_

#include <gnuradio/atsc/consts.h>
#include <cstring>
#include <cassert>


/*!
 * \brief pipeline info that flows with data
 *
 * Not all modules need all the info
 */
class plinfo {
public:
  plinfo () : _flags (0), _segno (0) { }

  // accessors

  bool field_sync1_p () const { return (_flags & fl_field_sync1) != 0; }
  bool field_sync2_p () const { return (_flags & fl_field_sync2) != 0; }
  bool field_sync_p ()  const { return field_sync1_p () || field_sync2_p (); }

  bool regular_seg_p () const { return (_flags & fl_regular_seg) != 0; }

  bool in_field1_p ()   const { return (_flags & fl_field2) == 0; }
  bool in_field2_p ()   const { return (_flags & fl_field2) != 0; }

  bool first_regular_seg_p () const { return (_flags & fl_first_regular_seg) != 0; }

  bool transport_error_p ()   const { return (_flags & fl_transport_error) != 0; }

  unsigned int segno ()	const { return _segno; }
  unsigned int flags () const { return _flags; }

  // setters

  void set_field_sync1 ()
  {
    _segno = 0;
    _flags = fl_field_sync1;
  }

  void set_field_sync2 ()
  {
    _segno = 0;
    _flags = fl_field_sync2 | fl_field2;
  }

  void set_regular_seg (bool field2, int segno)
  {
    assert (0 <= segno && segno < ATSC_DSEGS_PER_FIELD);
    _segno = segno;
    _flags = fl_regular_seg;
    if (segno == 0)
      _flags |= fl_first_regular_seg;
    if (segno >= ATSC_DSEGS_PER_FIELD)
      _flags |= fl_transport_error;
    if (field2)
      _flags |= fl_field2;
  }

  void set_transport_error (bool error){
    if (error)
      _flags |= fl_transport_error;
    else
      _flags &= ~fl_transport_error;
  }

  // overload equality operator
  bool operator== (const plinfo &other) const {
    return (_flags == other._flags && _segno == other._segno);
  }

  bool operator!= (const plinfo &other) const {
    return !(_flags == other._flags && _segno == other._segno);
  }

  /*!
   * Set \p OUT such that it reflects a \p NSEGS_OF_DELAY
   * pipeline delay from \p IN.
   */
  static void delay (plinfo &out, const plinfo &in, int nsegs_of_delay);

  /*!
   * confirm that \p X is plausible
   */
  static void sanity_check (const plinfo &in);


protected:
  unsigned short	_flags;		// bitmask
  unsigned short	_segno;		// segment number [0,311]

  // these three are mutually exclusive
  //     This is a regular data segment.
  static const int	fl_regular_seg		= 0x0001;
  //	 This is a field sync segment, for 1st half of a field.
  static const int	fl_field_sync1		= 0x0002;
  //	 This is a field sync segment, for 2nd half of a field.
  static const int	fl_field_sync2		= 0x0004;

  // This bit is on ONLY when fl_regular_seg is set AND when this is
  // the first regular data segment AFTER a field sync segment.  This
  // segment causes various processing modules to reset.
  static const int	fl_first_regular_seg 	= 0x0008;

  // which field are we in?
  static const int	fl_field2		= 0x0010;	// else field 1

  // This bit is set when Reed-Solomon decoding detects an error that it
  // can't correct.  Note that other error detection (e.g. Viterbi) do not
  // set it, since Reed-Solomon will correct many of those.  This bit is
  // then copied into the final Transport Stream packet so that MPEG
  // software can see that the 188-byte data segment has been corrupted.
  static const int	fl_transport_error	= 0x0020;
};




class atsc_mpeg_packet {
 public:
  static const int NPAD  = 68;
  unsigned char	data[ATSC_MPEG_DATA_LENGTH + 1];	// first byte is sync
  unsigned char _pad_[NPAD];				// pad to power of 2 (256)

  // overload equality operator
  bool operator== (const atsc_mpeg_packet &other) const {
    return std::memcmp (data, other.data, sizeof (data)) == 0;
  };

  bool operator!= (const atsc_mpeg_packet &other) const {
    return !(std::memcmp (data, other.data, sizeof (data)) == 0);
  };
};

class atsc_mpeg_packet_no_sync {
 public:
  static const int NPAD = 65;
  plinfo	pli;
  unsigned char	data[ATSC_MPEG_DATA_LENGTH];
  unsigned char _pad_[NPAD];				// pad to power of 2 (256)

  // overload equality operator
  bool operator== (const atsc_mpeg_packet_no_sync &other) const {
    return std::memcmp (data, other.data, sizeof (data)) == 0;
  }

  bool operator!= (const atsc_mpeg_packet_no_sync &other) const {
    return !(std::memcmp (data, other.data, sizeof (data)) == 0);
  }
};

class atsc_mpeg_packet_rs_encoded {
 public:
  static const int NPAD = 45;
  plinfo	pli;
  unsigned char	data[ATSC_MPEG_RS_ENCODED_LENGTH];
  unsigned char _pad_[NPAD];				// pad to power of 2 (256)

  // overload equality operator
  bool operator== (const atsc_mpeg_packet_rs_encoded &other) const {
    return std::memcmp (data, other.data, sizeof (data)) == 0;
  }

  bool operator!= (const atsc_mpeg_packet_rs_encoded &other) const {
    return !(std::memcmp (data, other.data, sizeof (data)) == 0);
  }
};


//! contains 832 3 bit symbols.  The low 3 bits in the byte hold the symbol.

class atsc_data_segment {
 public:
  static const int NPAD = 188;
  plinfo	pli;
  unsigned char	data[ATSC_DATA_SEGMENT_LENGTH];
  unsigned char _pad_[NPAD];				// pad to power of 2 (1024)

  // overload equality operator
  bool operator== (const atsc_data_segment &other) const {
    return std::memcmp (data, other.data, sizeof (data)) == 0;
  }

  bool operator!= (const atsc_data_segment &other) const {
    return !(std::memcmp (data, other.data, sizeof (data)) == 0);
  }
};

/*!
 * Contains 832 bipolar floating point symbols.
 * Nominal values are +/- {1, 3, 5, 7}.
 * This data type represents the input to the viterbi decoder.
 */

class atsc_soft_data_segment {
 public:
  static const int NPAD = 764;
  plinfo	pli;
  float		data[ATSC_DATA_SEGMENT_LENGTH];
  unsigned char _pad_[NPAD];			// pad to power of 2 (4096)

  // overload equality operator
  bool operator== (const atsc_data_segment &other) const {
    return std::memcmp (data, other.data, sizeof (data)) == 0;
  }

  bool operator!= (const atsc_data_segment &other) const {
    return !(std::memcmp (data, other.data, sizeof (data)) == 0);
  }
};


#endif /* _ATSC_TYPES_H_ */
