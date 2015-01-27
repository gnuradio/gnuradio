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

#include <gnuradio/uhd/usrp_sink.h>
#include <uhd/convert.hpp>
#include "usrp_common.h"

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
    class usrp_sink_impl : public usrp_sink, public usrp_common_impl
    {
    public:
       usrp_sink_impl(const ::uhd::device_addr_t &device_addr,
                      const ::uhd::stream_args_t &stream_args,
                      const std::string &length_tag_name);
      ~usrp_sink_impl();

      void setup_rpc();

      ::uhd::dict<std::string, std::string> get_usrp_info(size_t chan);
      double get_samp_rate(void);
      ::uhd::meta_range_t get_samp_rates(void);
      double get_center_freq(size_t chan);
      ::uhd::freq_range_t get_freq_range(size_t chan);
      double get_gain(size_t chan);
      double get_gain(const std::string &name, size_t chan);
      double get_normalized_gain(size_t chan);
      std::vector<std::string> get_gain_names(size_t chan);
      ::uhd::gain_range_t get_gain_range(size_t chan);
      ::uhd::gain_range_t get_gain_range(const std::string &name, size_t chan);
      std::string get_antenna(size_t chan);
      std::vector<std::string> get_antennas(size_t chan);
      ::uhd::sensor_value_t get_sensor(const std::string &name, size_t chan);
      std::vector<std::string> get_sensor_names(size_t chan);
      ::uhd::sensor_value_t get_mboard_sensor(const std::string &name, size_t mboard);
      std::vector<std::string> get_mboard_sensor_names(size_t mboard);
      std::string get_time_source(const size_t mboard);
      std::vector<std::string> get_time_sources(const size_t mboard);
      std::string get_clock_source(const size_t mboard);
      std::vector<std::string> get_clock_sources(const size_t mboard);
      double get_clock_rate(size_t mboard);
      ::uhd::time_spec_t get_time_now(size_t mboard = 0);
      ::uhd::time_spec_t get_time_last_pps(size_t mboard);
      ::uhd::usrp::dboard_iface::sptr get_dboard_iface(size_t chan);
      ::uhd::usrp::multi_usrp::sptr get_device(void);

      void set_subdev_spec(const std::string &spec, size_t mboard);
      std::string get_subdev_spec(size_t mboard);
      void set_samp_rate(double rate);
      ::uhd::tune_result_t set_center_freq(const ::uhd::tune_request_t tune_request,
                                         size_t chan);
      void set_gain(double gain, size_t chan);
      void set_gain(double gain, const std::string &name, size_t chan);
      void set_normalized_gain(double gain, size_t chan);
      void set_antenna(const std::string &ant, size_t chan);
      void set_bandwidth(double bandwidth, size_t chan);
      double get_bandwidth(size_t chan);
      ::uhd::freq_range_t get_bandwidth_range(size_t chan);
      void set_dc_offset(const std::complex<double> &offset, size_t chan);
      void set_iq_balance(const std::complex<double> &correction, size_t chan);
      void set_clock_config(const ::uhd::clock_config_t &clock_config, size_t mboard);
      void set_time_source(const std::string &source, const size_t mboard);
      void set_clock_source(const std::string &source, const size_t mboard);
      void set_clock_rate(double rate, size_t mboard);
      void set_time_now(const ::uhd::time_spec_t &time_spec, size_t mboard);
      void set_time_next_pps(const ::uhd::time_spec_t &time_spec);
      void set_time_unknown_pps(const ::uhd::time_spec_t &time_spec);
      void set_command_time(const ::uhd::time_spec_t &time_spec, size_t mboard);
      void clear_command_time(size_t mboard);
      void set_user_register(const uint8_t addr, const uint32_t data, size_t mboard);
      void set_stream_args(const ::uhd::stream_args_t &stream_args);
      void set_start_time(const ::uhd::time_spec_t &time);

      bool start(void);
      bool stop(void);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);

      inline void tag_work(int &ninput_items);

    private:
      /*! \brief Run through all 'lock' sensors and make sure they are actually locked.
       */
      bool _check_sensors_locked();

      //! Like set_center_freq(), but uses _curr_freq and _curr_lo_offset
      ::uhd::tune_result_t _set_center_freq_from_internals(size_t chan);
      //! Calls _set_center_freq_from_internals() on all channels
      void _set_center_freq_from_internals_allchans();

#ifdef GR_UHD_USE_STREAM_API
      ::uhd::tx_streamer::sptr _tx_stream;
#endif
      ::uhd::tx_metadata_t _metadata;
      double _sample_rate;


      //stream tags related stuff
      std::vector<tag_t> _tags;
      const pmt::pmt_t _length_tag_key;
      long _nitems_to_send;

      /****** Command interface related **********/
      //! Stores a list of commands for later execution
      std::vector<pmt::pmt_t> _pending_cmds;
      //! Receives commands and handles them
      void msg_handler_command(pmt::pmt_t msg);
      //! Stores the last value we told the USRP to tune to for every channel
      // (this is not necessarily the true value the USRP is currently tuned to!).
      // We could theoretically ask the device, but during streaming, we want to minimize
      // communication with the USRP.
      std::vector<double> _curr_freq;
      //! Stores the last value we told the USRP to have the LO offset for every channel.
      std::vector<double> _curr_lo_offset;
      //! Stores the last gain value we told the USRP to have for every channel.
      std::vector<double> _curr_gain;
      boost::dynamic_bitset<> _chans_to_tune;
    };

  } /* namespace uhd */
} /* namespace gr */
