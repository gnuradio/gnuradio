/*
 * Copyright 2011-2013 Free Software Foundation, Inc.
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

#include "block_gateway_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <boost/bind.hpp>

namespace gr {

  /***********************************************************************
   * Helper routines
   **********************************************************************/
  template <typename OutType, typename InType>
  void
  copy_pointers(OutType &out, const InType &in)
  {
    out.resize(in.size());
    for(size_t i = 0; i < in.size(); i++) {
      out[i] = (void *)(in[i]);
    }
  }


  block_gateway::sptr
  block_gateway::make(feval_ll *handler,
                      const std::string &name,
                      gr::io_signature::sptr in_sig,
                      gr::io_signature::sptr out_sig,
                      const block_gw_work_type work_type,
                      const unsigned factor)
  {
    return block_gateway::sptr
      (new block_gateway_impl(handler, name, in_sig, out_sig,
                              work_type, factor));
  }

  block_gateway_impl::block_gateway_impl(feval_ll *handler,
                                         const std::string &name,
                                         gr::io_signature::sptr in_sig,
                                         gr::io_signature::sptr out_sig,
                                         const block_gw_work_type work_type,
                                         const unsigned factor)
    : block(name, in_sig, out_sig),
      _handler(handler),
      _work_type(work_type)
  {
    switch(_work_type) {
    case GR_BLOCK_GW_WORK_GENERAL:
      _decim = 1; //not relevant, but set anyway
      _interp = 1; //not relevant, but set anyway
      break;

    case GR_BLOCK_GW_WORK_SYNC:
      _decim = 1;
      _interp = 1;
      this->set_fixed_rate(true);
      break;

    case GR_BLOCK_GW_WORK_DECIM:
      _decim = factor;
      _interp = 1;
      break;

    case GR_BLOCK_GW_WORK_INTERP:
      _decim = 1;
      _interp = factor;
      this->set_output_multiple(_interp);
      break;
    }
  }

  void
  block_gateway_impl::forecast(int noutput_items,
                               gr_vector_int &ninput_items_required)
  {
    switch(_work_type) {
    case GR_BLOCK_GW_WORK_GENERAL:
      _message.action = block_gw_message_type::ACTION_FORECAST;
      _message.forecast_args_noutput_items = noutput_items;
      _message.forecast_args_ninput_items_required = ninput_items_required;
      _handler->calleval(0);
      ninput_items_required = _message.forecast_args_ninput_items_required;
      return;

    default:
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = fixed_rate_noutput_to_ninput(noutput_items);
      return;
    }
  }

  int
  block_gateway_impl::general_work(int noutput_items,
                                   gr_vector_int &ninput_items,
                                   gr_vector_const_void_star &input_items,
                                   gr_vector_void_star &output_items)
  {
    switch(_work_type) {
    case GR_BLOCK_GW_WORK_GENERAL:
      _message.action = block_gw_message_type::ACTION_GENERAL_WORK;
      _message.general_work_args_noutput_items = noutput_items;
      _message.general_work_args_ninput_items = ninput_items;
      copy_pointers(_message.general_work_args_input_items, input_items);
      _message.general_work_args_output_items = output_items;
      _handler->calleval(0);
      return _message.general_work_args_return_value;

    default:
      int r = work (noutput_items, input_items, output_items);
      if(r > 0)
        consume_each(r*_decim/_interp);
      return r;
    }
  }

  int
  block_gateway_impl::work(int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
  {
    _message.action = block_gw_message_type::ACTION_WORK;
    _message.work_args_ninput_items = fixed_rate_noutput_to_ninput(noutput_items);
    if(_message.work_args_ninput_items == 0)
      return -1;
    _message.work_args_noutput_items = noutput_items;
    copy_pointers(_message.work_args_input_items, input_items);
    _message.work_args_output_items = output_items;
    _handler->calleval(0);
    return _message.work_args_return_value;
  }

  int
  block_gateway_impl::fixed_rate_noutput_to_ninput(int noutput_items)
  {
    return (noutput_items*_decim/_interp) + history() - 1;
  }

  int
  block_gateway_impl::fixed_rate_ninput_to_noutput(int ninput_items)
  {
    return std::max(0, ninput_items - (int)history() + 1)*_interp/_decim;
  }

  bool
  block_gateway_impl::start(void)
  {
    _message.action = block_gw_message_type::ACTION_START;
    _handler->calleval(0);
    return _message.start_args_return_value;
  }

  bool
  block_gateway_impl::stop(void)
  {
    _message.action = block_gw_message_type::ACTION_STOP;
    _handler->calleval(0);
    return _message.stop_args_return_value;
  }

  block_gw_message_type&
  block_gateway_impl::block_message(void)
  {
    return _message;
  }

} /* namespace gr */
