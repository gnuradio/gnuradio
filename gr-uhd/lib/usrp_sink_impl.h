/* -*- c++ -*- */
/*
 * Copyright 2010-2015 Free Software Foundation, Inc.
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

#include "usrp_block_impl.h"
#include <gnuradio/uhd/usrp_sink.h>
#include <uhd/convert.hpp>

static const pmt::pmt_t SOB_KEY = pmt::string_to_symbol("tx_sob");
static const pmt::pmt_t EOB_KEY = pmt::string_to_symbol("tx_eob");
static const pmt::pmt_t TIME_KEY = pmt::string_to_symbol("tx_time");
static const pmt::pmt_t FREQ_KEY = pmt::string_to_symbol("tx_freq");
static const pmt::pmt_t COMMAND_KEY = pmt::string_to_symbol("tx_command");

namespace gr {
  namespace uhd {

    inline io_signature::sptr
    args_to_io_sig(const ::uhd::stream_args_t &args)
    {
      const size_t nchan = std::max<size_t>(args.channels.size(), 1);
#ifdef GR_UHD_USE_STREAM_API
      const size_t size = ::uhd::convert::get_bytes_per_item(args.cpu_format);
#else
      size_t size = 0;
      if(args.cpu_format == "fc32")
        size = 8;
      if(args.cpu_format == "sc16")
        size = 4;
#endif
      return io_signature::make(nchan, nchan, size);
    }

    /***********************************************************************
     * UHD Multi USRP Sink Impl
     **********************************************************************/
    class usrp_sink_impl : public usrp_sink, public usrp_block_impl
    {
    public:
       usrp_sink_impl(const ::uhd::device_addr_t &device_addr,
                      const ::uhd::stream_args_t &stream_args,
                      const std::string &length_tag_name);
      ~usrp_sink_impl();

      // Symmetric getters/setters (common between source and sink)
      USRP_BLOCK_SYMMETRIC_METHODS_H();

      // Also symmetric, but can't be moved to usrp_block for now:
      ::uhd::dict<std::string, std::string> get_usrp_info(size_t chan);
      ::uhd::meta_range_t get_samp_rates(void);
      void set_dc_offset(const std::complex<double> &offset, size_t chan);
      void set_iq_balance(const std::complex<double> &correction, size_t chan);
      void set_stream_args(const ::uhd::stream_args_t &stream_args);

      // Non-symmetric setters:
      void set_samp_rate(double rate);
      ::uhd::tune_result_t set_center_freq(const ::uhd::tune_request_t tune_request,
                                         size_t chan);
      void set_start_time(const ::uhd::time_spec_t &time);

      // Stream-related commands
      bool start(void);
      bool stop(void);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);

      inline void tag_work(int &ninput_items);

    private:
      //! Like set_center_freq(), but uses _curr_freq and _curr_lo_offset
      ::uhd::tune_result_t _set_center_freq_from_internals(size_t chan);

#ifdef GR_UHD_USE_STREAM_API
      ::uhd::tx_streamer::sptr _tx_stream;
#endif
      ::uhd::tx_metadata_t _metadata;
      double _sample_rate;

      //stream tags related stuff
      std::vector<tag_t> _tags;
      const pmt::pmt_t _length_tag_key;
      long _nitems_to_send;

    };

  } /* namespace uhd */
} /* namespace gr */
