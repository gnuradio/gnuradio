/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_RUNTIME_BLOCK_GATEWAY_IMPL_H
#define INCLUDED_RUNTIME_BLOCK_GATEWAY_IMPL_H

#include <gnuradio/block_gateway.h>

namespace gr {

  /***********************************************************************
   * The gr::block gateway implementation class
   **********************************************************************/
  class block_gateway_impl : public block_gateway
  {
  public:
    block_gateway_impl(feval_ll *handler,
                       const std::string &name,
                       gr::io_signature::sptr in_sig,
                       gr::io_signature::sptr out_sig,
                       const block_gw_work_type work_type,
                       const unsigned factor);

    /*******************************************************************
     * Overloads for various scheduler-called functions
     ******************************************************************/
    void forecast(int noutput_items,
                  gr_vector_int &ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int &ninput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items);

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    int fixed_rate_noutput_to_ninput(int noutput_items);
    int fixed_rate_ninput_to_noutput(int ninput_items);

    bool start(void);
    bool stop(void);

    block_gw_message_type& block_message(void);

  private:
    feval_ll *_handler;
    block_gw_message_type _message;
    const block_gw_work_type _work_type;
    unsigned _decim, _interp;
  };

} /* namespace gr */

#endif /* INCLUDED_RUNTIME_BLOCK_GATEWAY_H */
