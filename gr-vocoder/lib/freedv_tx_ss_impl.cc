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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "freedv_tx_ss_impl.h"

#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>

extern "C" {
  char
  get_next_tx_char(void *callback_state)
  {
    char c;
    struct freedv_tx_callback_state* pstate;

    pstate = (struct freedv_tx_callback_state*)callback_state;
    c = *pstate->ptx_str++;

    if (*pstate->ptx_str == 0) {
      pstate->ptx_str = pstate->tx_str;
      c = 0x0d; // FreeDV uses Carriage Return termination
    }

    return c;
  }
}

namespace gr {
  namespace vocoder {

    freedv_tx_ss::sptr
    freedv_tx_ss::make(int mode, const std::string msg_txt)
    {
      return gnuradio::get_initial_sptr
        (new freedv_tx_ss_impl(mode, msg_txt));
    }

    freedv_tx_ss_impl::freedv_tx_ss_impl(int mode, const std::string msg_txt)
      : sync_block("vocoder_freedv_tx_ss",
                   io_signature::make(1, 1, sizeof(short)),
                   io_signature::make(1, 1, sizeof(short))),
        d_mode(mode), d_msg_text(msg_txt)
    {
      if((d_freedv = freedv_open(mode)) == NULL)
        throw std::runtime_error("freedv_tx_ss_impl: freedv_open failed");
      snprintf(d_cb_state.tx_str,79,"%s",d_msg_text.c_str());
      d_cb_state.ptx_str = d_cb_state.tx_str;
      freedv_set_callback_txt(d_freedv, NULL, get_next_tx_char, (void *) &d_cb_state);
      d_nom_modem_samples = freedv_get_n_nom_modem_samples(d_freedv);
      set_output_multiple(d_nom_modem_samples);
    }

    freedv_tx_ss_impl::~freedv_tx_ss_impl()
    {
      freedv_close(d_freedv);
    }

    int
    freedv_tx_ss_impl::work(int noutput_items,
                            gr_vector_const_void_star &input_items,
                            gr_vector_void_star &output_items)
    {
      short *in = (short*)input_items[0];
      short *out = (short*)output_items[0];
      int i;

      for(i=0;i<(noutput_items/d_nom_modem_samples);i++)
        freedv_tx(d_freedv, &(out[i*d_nom_modem_samples]), &(in[i*d_nom_modem_samples]));
      return noutput_items;
    }

  } /* namespace vocoder */
} /* namespace gr */
