/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2007,2014 Free Software Foundation, Inc.
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

namespace gr {

  class GR_RUNTIME_API io_signature
  {
    io_signature(int min_streams, int max_streams,
                 const std::vector<int> &sizeof_stream_items,
		 const std::vector<uint32_t> &stream_flags);

  public:
    typedef boost::shared_ptr<io_signature> sptr;

    typedef enum io_flags {
      DEFAULT_FLAGS  = 0,
      MEM_BLOCK_ALLOC = 1 << 0,
      MEM_BLOCK_SINGLE = 1 << 1,
      MEM_BLOCK_ODD = 1 << 2
    } io_flags_t;

    // Avoids a swig warning, otherwise we could just
    // #include <gnuradio/io_signature.h> instead of redoing this
    // #entire class
    //static const int IO_INFINITE = -1;

    ~io_signature();


    static sptr make(int min_streams, int max_streams,
                     int sizeof_stream_item,
		     uint32_t flags = DEFAULT_FLAGS);
    static sptr make2(int min_streams, int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
		      uint32_t flags1 = DEFAULT_FLAGS,
		      uint32_t flags2 = DEFAULT_FLAGS);
    static sptr make3(int min_streams, int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
                      int sizeof_stream_item3,
		      uint32_t flags1 = DEFAULT_FLAGS,
		      uint32_t flags2 = DEFAULT_FLAGS,
		      uint32_t flags3 = DEFAULT_FLAGS);
    static sptr makev(int min_streams, int max_streams,
                      const std::vector<int> &sizeof_stream_items,
		      const std::vector<uint32_t> &stream_flags = std::vector<uint32_t>(0));

    int min_streams() const { return d_min_streams; }
    int max_streams() const { return d_max_streams; }
    int sizeof_stream_item(int index) const;
    uint32_t stream_flags(int index) const;
    std::vector<int> sizeof_stream_items() const;
    std::vector<uint32_t> stream_flags() const;
  };

} /* namespace gr */


%template(io_signature_sptr) boost::shared_ptr<gr::io_signature>;
%pythoncode %{
io_signature_sptr.__repr__ = lambda self: "<io_signature: %d, %d>" % (self.min_streams(), self.max_streams())
io_signaturev = io_signature.makev;
io_signature3 = io_signature.make3;
io_signature2 = io_signature.make2;
io_signature = io_signature.make;
%}
