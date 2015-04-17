/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/digital/@BASE_NAME@.h>

namespace gr {
  namespace digital {

    class @IMPL_NAME@ : public @BASE_NAME@
    {
    protected:
      enum state_t {STATE_WAITING, STATE_PREPAD, STATE_RAMPUP,
                    STATE_COPY, STATE_RAMPDOWN, STATE_POSTPAD};

    private:
      const std::vector<@O_TYPE@> d_up_flank;
      const std::vector<@O_TYPE@> d_down_flank;
      const int d_nprepad;
      const int d_npostpad;
      const bool d_insert_phasing;
      const pmt::pmt_t d_length_tag_key;
      std::vector<@O_TYPE@> d_up_phasing;
      std::vector<@O_TYPE@> d_down_phasing;
      uint64_t d_nremaining;
      state_t d_state;

      void enter_waiting() { d_state = STATE_WAITING; }
      void enter_prepad() { d_state = STATE_PREPAD; }
      void enter_rampup() { d_state = STATE_RAMPUP; }
      void enter_copy() { d_state = STATE_COPY; }
      void enter_rampdown() { d_state = STATE_RAMPDOWN; }
      void enter_postpad() { d_state = STATE_POSTPAD; }
      void add_length_tag(int offset);
      void propagate_tag(tag_t &tag, int offset);

    public:
      @IMPL_NAME@(const std::vector<@O_TYPE@> &taps, int pre_padding,
                  int post_padding, bool insert_phasing,
                  const std::string &length_tag_name);
      ~@IMPL_NAME@();

      void forecast(int noutput_items,
                    gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
      int pre_padding() const { return d_nprepad; }
      int post_padding() const { return d_npostpad; }
      int prefix_length() const { return d_nprepad +
                                         d_up_flank.size(); }
      int suffix_length() const { return d_npostpad +
                                         d_down_flank.size(); }
    };

  } // namespace digital
} // namespace gr

#endif /* @GUARD_NAME@ */
