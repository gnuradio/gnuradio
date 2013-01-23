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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_ctrlport_probe2_c.h>
#include <gr_io_signature.h>

gr_ctrlport_probe2_c_sptr
gr_make_ctrlport_probe2_c(const std::string &id,
			  const std::string &desc, int len)
{
  return gnuradio::get_initial_sptr
    (new gr_ctrlport_probe2_c(id, desc, len));
}

gr_ctrlport_probe2_c::gr_ctrlport_probe2_c(const std::string &id,
					   const std::string &desc, int len)
  : gr_sync_block("probe2_c",
		  gr_make_io_signature(1, 1, sizeof(gr_complex)),
		  gr_make_io_signature(0, 0, 0)),
    d_id(id), d_desc(desc), d_len(len)
{
  set_length(len);
}

gr_ctrlport_probe2_c::~gr_ctrlport_probe2_c()
{
}

void
gr_ctrlport_probe2_c::forecast(int noutput_items, gr_vector_int &ninput_items_required)
{
  // make sure all inputs have noutput_items available
  unsigned ninputs = ninput_items_required.size();
  for(unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = d_len;
}
    
//    boost::shared_mutex mutex_buffer;
//    mutable boost::mutex mutex_notify;
//    boost::condition_variable condition_buffer_ready;
std::vector<gr_complex>
gr_ctrlport_probe2_c::get()
{
  mutex_buffer.lock();
  d_buffer.clear();
  mutex_buffer.unlock();

  // wait for condition
  boost::mutex::scoped_lock lock(mutex_notify);
  condition_buffer_ready.wait(lock);

  mutex_buffer.lock();
  std::vector<gr_complex> buf_copy = d_buffer;
  assert(buf_copy.size() == d_len);
  mutex_buffer.unlock();

  return buf_copy;
}

void
gr_ctrlport_probe2_c::set_length(int len)
{
  if(len > 8191) {
    std::cerr << "probe2_c: length " << len
	      << " exceeds maximum buffer size of 8191" << std::endl;
    len = 8191;
  }

  d_len = len;
  d_buffer.reserve(d_len);
}

int
gr_ctrlport_probe2_c::length() const
{
  return (int)d_len;
}

int
gr_ctrlport_probe2_c::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex*)input_items[0];

  // copy samples to get buffer if we need samples
  mutex_buffer.lock();
  if(d_buffer.size() < d_len) {
    // copy smaller of remaining buffer space and num inputs to work()
    int num_copy = std::min( (int)(d_len - d_buffer.size()), noutput_items );

    // TODO: convert this to a copy operator for speed...
    for(int i = 0; i < num_copy; i++) {
      d_buffer.push_back(in[i]);
    }
    
    // notify the waiting get() if we fill up the buffer
    if(d_buffer.size() == d_len) {
      condition_buffer_ready.notify_one();
    }
  }
  mutex_buffer.unlock();
    
  return noutput_items;
}

void
gr_ctrlport_probe2_c::setup_rpc()
{
#ifdef GR_CTRLPORT
  int len = static_cast<int>(d_len);
  d_rpc_vars.push_back(
    rpcbasic_sptr(new rpcbasic_register_get<gr_ctrlport_probe2_c, std::vector<std::complex<float> > >(
      alias(), d_id.c_str(), &gr_ctrlport_probe2_c::get,
      pmt::make_c32vector(0,-2),
      pmt::make_c32vector(0,2),
      pmt::make_c32vector(0,0), 
      "volts", d_desc.c_str(), RPC_PRIVLVL_MIN, DISPXYSCATTER)));

  d_rpc_vars.push_back(
    rpcbasic_sptr(new rpcbasic_register_get<gr_ctrlport_probe2_c, int>(
      alias(), "length", &gr_ctrlport_probe2_c::length,
      pmt::mp(1), pmt::mp(10*len), pmt::mp(len),
      "samples", "get vector length", RPC_PRIVLVL_MIN, DISPNULL)));

  d_rpc_vars.push_back(
    rpcbasic_sptr(new rpcbasic_register_set<gr_ctrlport_probe2_c, int>(
      alias(), "length", &gr_ctrlport_probe2_c::set_length,
      pmt::mp(1), pmt::mp(10*len), pmt::mp(len),
      "samples", "set vector length", RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
}
