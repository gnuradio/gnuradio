/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TAG_INFO_H
#define INCLUDED_GR_TAG_INFO_H

#include <gruel/pmt.h>

namespace gr_tags {

  enum {
    TAG_NITEM_REF = 0,
    TAG_SRCID_REF,
    TAG_KEY_REF,
    TAG_VALUE_REF
  };

  extern const pmt::pmt_t key_time;
  extern const pmt::pmt_t key_sample_rate;
  extern const pmt::pmt_t key_frequency;
  extern const pmt::pmt_t key_rssi;
  extern const pmt::pmt_t key_gain;

  /*! 
   * \brief Returns the item \p tag occurred at (as a uint64_t)
   */
  static inline uint64_t
  get_nitems(const pmt::pmt_t &tag) {
    return pmt::pmt_to_uint64(pmt::pmt_tuple_ref(tag, TAG_NITEM_REF));
  }
  
  /*! 
   * \brief Returns the source ID of \p tag (as a PMT)
   */
  static inline pmt::pmt_t
  get_srcid(const pmt::pmt_t &tag) {
    return pmt::pmt_tuple_ref(tag, TAG_SRCID_REF);
  }
  
  /*! 
   * \brief Returns the key of \p tag (as a PMT symbol)
   */
  static inline pmt::pmt_t
  get_key(const pmt::pmt_t &tag) {
    return pmt::pmt_tuple_ref(tag, TAG_KEY_REF);
  }

  /*! 
   * \brief Returns the value of \p tag (as a PMT)
   */
  static inline pmt::pmt_t
  get_value(const pmt::pmt_t &tag) {
    return pmt::pmt_tuple_ref(tag, TAG_VALUE_REF);
  }

  /*! 
   * \brief Comparison function to test which tag, \p x or \p y, came first in time
   */
  static inline bool
  nitems_compare(pmt::pmt_t x, pmt::pmt_t y) {
    return get_nitems(x) < get_nitems(y);
  }

}; /* namespace tags */

#endif /* GR_TAG_INFO */
