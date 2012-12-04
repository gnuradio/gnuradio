/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CTRLPORT_PROBE_C_H
#define INCLUDED_CTRLPORT_PROBE_C_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <rpcregisterhelpers.h>
#include <boost/thread/shared_mutex.hpp>

class gr_ctrlport_probe_c;
typedef boost::shared_ptr<gr_ctrlport_probe_c> gr_ctrlport_probe_c_sptr;

GR_CORE_API gr_ctrlport_probe_c_sptr
gr_make_ctrlport_probe_c(const std::string &id, const std::string &desc);

class GR_CORE_API gr_ctrlport_probe_c : public gr_sync_block
{
 private:
  friend GR_CORE_API gr_ctrlport_probe_c_sptr gr_make_ctrlport_probe_c
    (const std::string &id, const std::string &desc);

  gr_ctrlport_probe_c(const std::string &id, const std::string &desc);

  boost::shared_mutex ptrlock;

  const gr_complex* d_ptr;
  size_t d_ptrLen;

  rpcbasic_register_get<gr_ctrlport_probe_c, std::vector<std::complex<float> >  > d_const_rpc;

 public:
  ~gr_ctrlport_probe_c();

  std::vector<gr_complex> get();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_CTRLPORT_GR_CTRLPORT_PROBE_C_H */

