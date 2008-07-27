/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_MSDD_SOURCE_SIMPLE_H
#define INCLUDED_MSDD_SOURCE_SIMPLE_H

#include <gr_sync_block.h>
#include <msdd6000.h>
#include <boost/scoped_ptr.hpp>


class msdd_source_simple;
typedef boost::shared_ptr<msdd_source_simple> msdd_source_simple_sptr;


// public shared_ptr constructor

msdd_source_simple_sptr msdd_make_source_simple ( const char *src, unsigned short port_src);


class msdd_source_simple : public gr_sync_block {
 private:
  friend msdd_source_simple_sptr
  msdd_make_source_simple ( const char *src, unsigned short port_src);

  boost::scoped_ptr<MSDD6000> rcv;
  int d_lastseq;

 protected:
  msdd_source_simple (const char *src, unsigned short port_src);

 public:
  ~msdd_source_simple ();
  bool stop();
  bool start();

  bool set_decim_rate(unsigned int);
  bool set_rx_freq(int,double);
  bool set_pga(int,double);

  int work(int, gr_vector_const_void_star&, gr_vector_void_star&);
  
  long adc_freq();
  int decim_rate();
  std::vector<int> gain_range();
  std::vector<float> freq_range();
};

#endif /* INCLUDED_MSDD_SOURCE_C_H */
