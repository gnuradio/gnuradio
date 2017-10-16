/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_FREEDV_TX_SS_IMPL_H
#define INCLUDED_VOCODER_FREEDV_TX_SS_IMPL_H

#include <gnuradio/vocoder/freedv_tx_ss.h>

extern "C" {
  struct freedv_tx_callback_state {
    char  tx_str[80];
    char *ptx_str;
    int calls;
  };
  char get_next_tx_char(void *callback_state);
  void get_next_proto(void *callback_state,char *proto_bits);
  void datarx(void *callback_state, unsigned char *packet, size_t size);
  void datatx(void *callback_state, unsigned char *packet, size_t *size);
}

namespace gr {
  namespace vocoder {

    class freedv_tx_ss_impl : public freedv_tx_ss
    {
    private:
      short *d_speech_in;
      short *d_mod_out;
      struct freedv_tx_callback_state d_cb_state;
      struct freedv *d_freedv;
      int d_mode;
      float d_squelch_thresh;
      int d_speech_samples;
      int d_nom_modem_samples;
      int d_use_codectx;
      int d_use_datatx;
      std::string d_msg_text;
      struct CODEC2 *d_c2;

    public:
      freedv_tx_ss_impl(int mode, const std::string txt_msg);
      ~freedv_tx_ss_impl();

      // Where all the action really happens
      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_FREEDV_TX_SS_IMPL_H */
