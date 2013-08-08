/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CTRLPORT_PROBE_C_IMPL_H
#define INCLUDED_CTRLPORT_PROBE_C_IMPL_H

#include <gnuradio/blocks/ctrlport_probe_c.h>
#include <gnuradio/rpcregisterhelpers.h>
#include <boost/thread/shared_mutex.hpp>

namespace gr {
  namespace blocks {

    class ctrlport_probe_c_impl : public ctrlport_probe_c
    {
    private:
      boost::shared_mutex ptrlock;

      std::string d_id;
      std::string d_desc;
      const gr_complex* d_ptr;
      size_t d_ptrLen;

    public:
      ctrlport_probe_c_impl(const std::string &id, const std::string &desc);
      ~ctrlport_probe_c_impl();

      void setup_rpc();

      std::vector<gr_complex> get();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_GR_CTRLPORT_PROBE_C_IMPL_H */

