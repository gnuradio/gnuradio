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
      enum state_t {STATE_WAIT, STATE_PREPAD, STATE_RAMPUP,
                    STATE_COPY, STATE_RAMPDOWN, STATE_POSTPAD};

    private:
      const std::vector<@O_TYPE@> d_up_ramp;
      const std::vector<@O_TYPE@> d_down_ramp;
      const int d_nprepad;
      const int d_npostpad;
      const bool d_insert_phasing;
      const pmt::pmt_t d_length_tag_key;
      std::vector<@O_TYPE@> d_up_phasing;
      std::vector<@O_TYPE@> d_down_phasing;
      int d_ncopy;
      int d_limit;
      int d_index;
      uint64_t d_length_tag_offset;
      bool d_finished;
      state_t d_state;

      void write_padding(@O_TYPE@ *&dst, int &nwritten, int nspace);
      void copy_items(@O_TYPE@ *&dst, const @I_TYPE@ *&src, int &nwritten,
                      int &nread, int nspace);
      void apply_ramp(@O_TYPE@ *&dst, const @I_TYPE@ *&src, int &nwritten,
                      int &nread, int nspace);
      void add_length_tag(int offset);
      void propagate_tags(int in_offset, int out_offset, int count, bool skip=true);
      void enter_wait();
      void enter_prepad();
      void enter_rampup();
      void enter_copy();
      void enter_rampdown();
      void enter_postpad();

    public:
      @IMPL_NAME@(const std::vector<@O_TYPE@> &taps, int pre_padding,
                  int post_padding, bool insert_phasing,
                  const std::string &length_tag_name);
      ~@IMPL_NAME@();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
      int pre_padding() const { return d_nprepad; }
      int post_padding() const { return d_npostpad; }
      int prefix_length() const;
      int suffix_length() const;
    };

  } // namespace digital
} // namespace gr

#endif /* @GUARD_NAME@ */
