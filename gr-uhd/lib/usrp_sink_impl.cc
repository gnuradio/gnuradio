/* -*- c++ -*- */
/*
 * Copyright 2010-2013 Free Software Foundation, Inc.
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

#include "usrp_sink_impl.h"
#include "gr_uhd_common.h"
#include <gnuradio/io_signature.h>
#include <boost/make_shared.hpp>
#include <stdexcept>

namespace gr {
  namespace uhd {

    usrp_sink::sptr
    usrp_sink::make(const ::uhd::device_addr_t &device_addr,
                    const ::uhd::io_type_t &io_type,
                    size_t num_channels)
    {
      //fill in the streamer args
      ::uhd::stream_args_t stream_args;
      switch(io_type.tid) {
      case ::uhd::io_type_t::COMPLEX_FLOAT32: stream_args.cpu_format = "fc32"; break;
      case ::uhd::io_type_t::COMPLEX_INT16: stream_args.cpu_format = "sc16"; break;
      default: throw std::runtime_error("only complex float and shorts known to work");
      }

      stream_args.otw_format = "sc16"; //only sc16 known to work
      for(size_t chan = 0; chan < num_channels; chan++)
        stream_args.channels.push_back(chan); //linear mapping

      return usrp_sink::make(device_addr, stream_args);
    }

    usrp_sink::sptr
    usrp_sink::make(const ::uhd::device_addr_t &device_addr,
                    const ::uhd::stream_args_t &stream_args)
    {
      check_abi();
      return usrp_sink::sptr
        (new usrp_sink_impl(device_addr, stream_args_ensure(stream_args)));
    }

    usrp_sink_impl::usrp_sink_impl(const ::uhd::device_addr_t &device_addr,
                                   const ::uhd::stream_args_t &stream_args)
      : sync_block("gr uhd usrp sink",
                      args_to_io_sig(stream_args),
                      io_signature::make(0, 0, 0)),
        _stream_args(stream_args),
        _nchan(stream_args.channels.size()),
        _stream_now(_nchan == 1),
        _start_time_set(false)
    {
      if(stream_args.cpu_format == "fc32")
        _type = boost::make_shared< ::uhd::io_type_t >(::uhd::io_type_t::COMPLEX_FLOAT32);
      if(stream_args.cpu_format == "sc16")
        _type = boost::make_shared< ::uhd::io_type_t >(::uhd::io_type_t::COMPLEX_INT16);
      _dev = ::uhd::usrp::multi_usrp::make(device_addr);
    }

    usrp_sink_impl::~usrp_sink_impl()
    {
    }

    ::uhd::dict<std::string, std::string>
    usrp_sink_impl::get_usrp_info(size_t chan)
    {
      chan = _stream_args.channels[chan];
#ifdef UHD_USRP_MULTI_USRP_GET_USRP_INFO_API
      return _dev->get_usrp_tx_info(chan);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    void
    usrp_sink_impl::set_subdev_spec(const std::string &spec,
                                    size_t mboard)
    {
      return _dev->set_tx_subdev_spec(spec, mboard);
    }

    std::string
    usrp_sink_impl::get_subdev_spec(size_t mboard)
    {
      return _dev->get_tx_subdev_spec(mboard).to_string();
    }

    void
    usrp_sink_impl::set_samp_rate(double rate)
    {
        BOOST_FOREACH(const size_t chan, _stream_args.channels)
        {
            _dev->set_tx_rate(rate, chan);
        }
      _sample_rate = this->get_samp_rate();
    }

    double
    usrp_sink_impl::get_samp_rate(void)
    {
      return _dev->get_tx_rate(_stream_args.channels[0]);
    }

    ::uhd::meta_range_t
    usrp_sink_impl::get_samp_rates(void)
    {
#ifdef UHD_USRP_MULTI_USRP_GET_RATES_API
      return _dev->get_tx_rates(_stream_args.channels[0]);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    ::uhd::tune_result_t
    usrp_sink_impl::set_center_freq(const ::uhd::tune_request_t tune_request,
                                    size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->set_tx_freq(tune_request, chan);
    }

    double
    usrp_sink_impl::get_center_freq(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_freq(chan);
    }

    ::uhd::freq_range_t
    usrp_sink_impl::get_freq_range(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_freq_range(chan);
    }

    void
    usrp_sink_impl::set_gain(double gain, size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->set_tx_gain(gain, chan);
    }

    void
    usrp_sink_impl::set_gain(double gain,
                             const std::string &name,
                             size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->set_tx_gain(gain, name, chan);
    }

    double
    usrp_sink_impl::get_gain(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_gain(chan);
    }

    double
    usrp_sink_impl::get_gain(const std::string &name, size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_gain(name, chan);
    }

    std::vector<std::string>
    usrp_sink_impl::get_gain_names(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_gain_names(chan);
    }

    ::uhd::gain_range_t
    usrp_sink_impl::get_gain_range(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_gain_range(chan);
    }

    ::uhd::gain_range_t
    usrp_sink_impl::get_gain_range(const std::string &name,
                                   size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_gain_range(name, chan);
    }

    void
    usrp_sink_impl::set_antenna(const std::string &ant,
                                size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->set_tx_antenna(ant, chan);
    }

    std::string
    usrp_sink_impl::get_antenna(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_antenna(chan);
    }

    std::vector<std::string>
    usrp_sink_impl::get_antennas(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_antennas(chan);
    }

    void
    usrp_sink_impl::set_bandwidth(double bandwidth, size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->set_tx_bandwidth(bandwidth, chan);
    }

    double
    usrp_sink_impl::get_bandwidth(size_t chan)
    {
        chan = _stream_args.channels[chan];
        return _dev->get_tx_bandwidth(chan);
    }

    ::uhd::freq_range_t
    usrp_sink_impl::get_bandwidth_range(size_t chan)
    {
        chan = _stream_args.channels[chan];
        return _dev->get_tx_bandwidth_range(chan);
    }

    void
    usrp_sink_impl::set_dc_offset(const std::complex<double> &offset,
                                  size_t chan)
    {
      chan = _stream_args.channels[chan];
#ifdef UHD_USRP_MULTI_USRP_FRONTEND_CAL_API
      return _dev->set_tx_dc_offset(offset, chan);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    void
    usrp_sink_impl::set_iq_balance(const std::complex<double> &correction,
                                   size_t chan)
    {
      chan = _stream_args.channels[chan];
#ifdef UHD_USRP_MULTI_USRP_FRONTEND_CAL_API
      return _dev->set_tx_iq_balance(correction, chan);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    ::uhd::sensor_value_t
    usrp_sink_impl::get_sensor(const std::string &name, size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_sensor(name, chan);
    }

    std::vector<std::string>
    usrp_sink_impl::get_sensor_names(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_sensor_names(chan);
    }

    ::uhd::sensor_value_t
    usrp_sink_impl::get_mboard_sensor(const std::string &name,
                                       size_t mboard)
    {
      return _dev->get_mboard_sensor(name, mboard);
    }

    std::vector<std::string>
    usrp_sink_impl::get_mboard_sensor_names(size_t mboard)
    {
      return _dev->get_mboard_sensor_names(mboard);
    }

    void
    usrp_sink_impl::set_clock_config(const ::uhd::clock_config_t &clock_config,
                                     size_t mboard)
    {
      return _dev->set_clock_config(clock_config, mboard);
    }

    void
    usrp_sink_impl::set_time_source(const std::string &source,
                                    const size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
      return _dev->set_time_source(source, mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    std::string
    usrp_sink_impl::get_time_source(const size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
      return _dev->get_time_source(mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    std::vector<std::string>
    usrp_sink_impl::get_time_sources(const size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
      return _dev->get_time_sources(mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    void
    usrp_sink_impl::set_clock_source(const std::string &source,
                                     const size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
      return _dev->set_clock_source(source, mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    std::string
    usrp_sink_impl::get_clock_source(const size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
      return _dev->get_clock_source(mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    std::vector<std::string>
    usrp_sink_impl::get_clock_sources(const size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
      return _dev->get_clock_sources(mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    double
    usrp_sink_impl::get_clock_rate(size_t mboard)
    {
      return _dev->get_master_clock_rate(mboard);
    }

    void
    usrp_sink_impl::set_clock_rate(double rate, size_t mboard)
    {
      return _dev->set_master_clock_rate(rate, mboard);
    }

    ::uhd::time_spec_t
    usrp_sink_impl::get_time_now(size_t mboard)
    {
      return _dev->get_time_now(mboard);
    }

    ::uhd::time_spec_t
    usrp_sink_impl::get_time_last_pps(size_t mboard)
    {
      return _dev->get_time_last_pps(mboard);
    }

    void
    usrp_sink_impl::set_time_now(const ::uhd::time_spec_t &time_spec,
                                 size_t mboard)
    {
      return _dev->set_time_now(time_spec, mboard);
    }

    void
    usrp_sink_impl::set_time_next_pps(const ::uhd::time_spec_t &time_spec)
    {
      return _dev->set_time_next_pps(time_spec);
    }

    void
    usrp_sink_impl::set_time_unknown_pps(const ::uhd::time_spec_t &time_spec)
    {
      return _dev->set_time_unknown_pps(time_spec);
    }

    void
    usrp_sink_impl::set_command_time(const ::uhd::time_spec_t &time_spec,
                                     size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_COMMAND_TIME_API
      return _dev->set_command_time(time_spec, mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    void
    usrp_sink_impl::clear_command_time(size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_COMMAND_TIME_API
      return _dev->clear_command_time(mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }

    ::uhd::usrp::dboard_iface::sptr
    usrp_sink_impl::get_dboard_iface(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_dboard_iface(chan);
    }

    ::uhd::usrp::multi_usrp::sptr
    usrp_sink_impl::get_device(void)
    {
      return _dev;
    }

    void
    usrp_sink_impl::set_user_register(const uint8_t addr,
                                      const uint32_t data,
                                      size_t mboard)
    {
#ifdef UHD_USRP_MULTI_USRP_USER_REGS_API
      _dev->set_user_register(addr, data, mboard);
#else
      throw std::runtime_error("not implemented in this version");
#endif
    }


    /***********************************************************************
     * Work
     **********************************************************************/
    int
    usrp_sink_impl::work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
    {
      int ninput_items = noutput_items; //cuz its a sync block

      //send a mid-burst packet with time spec
      _metadata.start_of_burst = false;
      _metadata.end_of_burst = false;

      //collect tags in this work()
      const uint64_t samp0_count = nitems_read(0);
      get_tags_in_range(_tags, 0, samp0_count, samp0_count + ninput_items);
      if(not _tags.empty())
        this->tag_work(ninput_items);

#ifdef GR_UHD_USE_STREAM_API
      //send all ninput_items with metadata
      const size_t num_sent = _tx_stream->send
        (input_items, ninput_items, _metadata, 1.0);
#else
      const size_t num_sent = _dev->get_device()->send
        (input_items, ninput_items, _metadata,
         *_type, ::uhd::device::SEND_MODE_FULL_BUFF, 1.0);
#endif

      //increment the timespec by the number of samples sent
      _metadata.time_spec += ::uhd::time_spec_t(0, num_sent, _sample_rate);
      return num_sent;
    }

    /***********************************************************************
     * Tag Work
     **********************************************************************/
    void
    usrp_sink_impl::tag_work(int &ninput_items)
    {
      //the for loop below assumes tags sorted by count low -> high
      std::sort(_tags.begin(), _tags.end(), tag_t::offset_compare);

      //extract absolute sample counts
      const tag_t &tag0 = _tags.front();
      const uint64_t tag0_count = tag0.offset;
      const uint64_t samp0_count = this->nitems_read(0);

      //only transmit nsamples from 0 to the first tag
      //this ensures that the next work starts on a tag
      if(samp0_count != tag0_count) {
        ninput_items = tag0_count - samp0_count;
        return;
      }

      //time will not be set unless a time tag is found
      _metadata.has_time_spec = false;

      //process all of the tags found with the same count as tag0
      BOOST_FOREACH(const tag_t &my_tag, _tags) {
        const uint64_t my_tag_count = my_tag.offset;
        const pmt::pmt_t &key = my_tag.key;
        const pmt::pmt_t &value = my_tag.value;

        //determine how many samples to send...
        //from zero until the next tag or end of work
        if(my_tag_count != tag0_count) {
          ninput_items = my_tag_count - samp0_count;
          break;
        }

        //handle end of burst with a mini end of burst packet
        else if(pmt::equal(key, EOB_KEY)) {
          _metadata.end_of_burst = pmt::to_bool(value);
          ninput_items = 1;
          return;
        }

        //set the start of burst flag in the metadata
        else if(pmt::equal(key, SOB_KEY)) {
          _metadata.start_of_burst = pmt::to_bool(value);
        }

        //set the time specification in the metadata
        else if(pmt::equal(key, TIME_KEY)) {
          _metadata.has_time_spec = true;
          _metadata.time_spec = ::uhd::time_spec_t
            (pmt::to_uint64(pmt::tuple_ref(value, 0)),
             pmt::to_double(pmt::tuple_ref(value, 1)));
        }
      }
    }

    void
    usrp_sink_impl::set_start_time(const ::uhd::time_spec_t &time)
    {
      _start_time = time;
      _start_time_set = true;
      _stream_now = false;
    }

    //Send an empty start-of-burst packet to begin streaming.
    //Set at a time in the near future to avoid late packets.
    bool
    usrp_sink_impl::start(void)
    {
#ifdef GR_UHD_USE_STREAM_API
      _tx_stream = _dev->get_tx_stream(_stream_args);
#endif

      _metadata.start_of_burst = true;
      _metadata.end_of_burst = false;
      _metadata.has_time_spec = not _stream_now;
      if(_start_time_set) {
        _start_time_set = false; //cleared for next run
        _metadata.time_spec = _start_time;
      }
      else {
        _metadata.time_spec = get_time_now() + ::uhd::time_spec_t(0.01);
      }

#ifdef GR_UHD_USE_STREAM_API
      _tx_stream->send
        (gr_vector_const_void_star(_nchan), 0, _metadata, 1.0);
#else
      _dev->get_device()->send
        (gr_vector_const_void_star(_nchan), 0, _metadata,
         *_type, ::uhd::device::SEND_MODE_ONE_PACKET, 1.0);
#endif
      return true;
    }

    //Send an empty end-of-burst packet to end streaming.
    //Ending the burst avoids an underflow error on stop.
    bool
    usrp_sink_impl::stop(void)
    {
      _metadata.start_of_burst = false;
      _metadata.end_of_burst = true;
      _metadata.has_time_spec = false;

#ifdef GR_UHD_USE_STREAM_API
      _tx_stream->send(gr_vector_const_void_star(_nchan), 0, _metadata, 1.0);
#else
      _dev->get_device()->send
        (gr_vector_const_void_star(_nchan), 0, _metadata,
         *_type, ::uhd::device::SEND_MODE_ONE_PACKET, 1.0);
#endif
      return true;
    }

    void
    usrp_sink_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<usrp_sink, double>(
	  alias(), "samp_rate",
	  &usrp_sink::get_samp_rate,
	  pmt::mp(100000.0f), pmt::mp(25000000.0f), pmt::mp(1000000.0f),
	  "sps", "TX Sample Rate", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTSTRIP)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_set<usrp_sink, double>(
	  alias(), "samp_rate",
	  &usrp_sink::set_samp_rate,
	  pmt::mp(100000.0f), pmt::mp(25000000.0f), pmt::mp(1000000.0f),
	  "sps", "TX Sample Rate",
	  RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace uhd */
} /* namespace gr */
