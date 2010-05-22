/* -*- c++ -*- */
/*
 * Copyright 2007,2010 Free Software Foundation, Inc.
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


GR_SWIG_BLOCK_MAGIC(gr,udp_sink)

gr_udp_sink_sptr 
gr_make_udp_sink (size_t itemsize, 
		  const char *host, unsigned short port,
		  int payload_size=1472, bool eof=true) throw (std::runtime_error);

class gr_udp_sink : public gr_sync_block
{
 protected:
  gr_udp_sink (size_t itemsize, 
	       const char *host, unsigned short port,
	       int payload_size, bool eof) 
    throw (std::runtime_error);

 public:
  ~gr_udp_sink ();

  int payload_size() { return d_payload_size; }
  void connect( const char *host, unsigned short port );
  void disconnect();

};
