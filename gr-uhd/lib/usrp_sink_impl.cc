/* -*- c++ -*- */
/*
 * Copyright 2010-2016,2018 Free Software Foundation, Inc.
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

#include <climits>
#include <stdexcept>
#include "usrp_sink_impl.h"
#include "gr_uhd_common.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace uhd {

    usrp_sink::sptr
    usrp_sink::make(const ::uhd::device_addr_t &device_addr,
                    const ::uhd::stream_args_t &stream_args,
                    const std::string &length_tag_name)
    {
      check_abi();
      return usrp_sink::sptr
        (new usrp_sink_impl(device_addr, stream_args_ensure(stream_args), length_tag_name));
    }

    usrp_sink_impl::usrp_sink_impl(const ::uhd::device_addr_t &device_addr,
                                   const ::uhd::stream_args_t &stream_args,
                                   const std::string &length_tag_name)
      : usrp_block("usrp_sink",
                   args_to_io_sig(stream_args),
                   io_signature::make(0, 0, 0)),
        usrp_block_impl(device_addr, stream_args, length_tag_name),
        _length_tag_key(length_tag_name.empty() ? pmt::PMT_NIL : pmt::string_to_symbol(length_tag_name)),
        _nitems_to_send(0),
        _async_event_loop_running(true)
    {
      message_port_register_out(ASYNC_MSGS_PORT_KEY);
      _async_event_thread = gr::thread::thread([this](){
          this->async_event_loop();
        });
      _sample_rate = get_samp_rate();
    }

    usrp_sink_impl::~usrp_sink_impl()
    {
      _async_event_loop_running = false;
      _async_event_thread.join();
    }

    ::uhd::dict<std::string, std::string>
    usrp_sink_impl::get_usrp_info(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_usrp_tx_info(chan);
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
      return _dev->get_tx_rates(_stream_args.channels[0]);
    }

    ::uhd::tune_result_t
    usrp_sink_impl::set_center_freq(const ::uhd::tune_request_t tune_request,
                                    size_t chan)
    {
      _curr_tune_req[chan] = tune_request;
      chan = _stream_args.channels[chan];
      return _dev->set_tx_freq(tune_request, chan);
    }

    ::uhd::tune_result_t
    usrp_sink_impl::_set_center_freq_from_internals(size_t chan, pmt::pmt_t direction)
    {
      _chans_to_tune.reset(chan);
      if (pmt::eqv(direction, ant_direction_rx())) {
        // TODO: what happens if the RX device is not instantiated? Catch error?
        return _dev->set_rx_freq(_curr_tune_req[chan], _stream_args.channels[chan]);
      } else {
        return _dev->set_tx_freq(_curr_tune_req[chan], _stream_args.channels[chan]);
      }
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

    void usrp_sink_impl::set_normalized_gain(double norm_gain, size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_NORMALIZED_GAIN
        _dev->set_normalized_tx_gain(norm_gain, chan);
#else
      if (norm_gain > 1.0 || norm_gain < 0.0) {
        throw std::runtime_error("Normalized gain out of range, must be in [0, 1].");
      }
      ::uhd::gain_range_t gain_range = get_gain_range(chan);
      double abs_gain = (norm_gain * (gain_range.stop() - gain_range.start())) + gain_range.start();
      set_gain(abs_gain, chan);
#endif
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

    double
    usrp_sink_impl::get_normalized_gain(size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_NORMALIZED_GAIN
        return _dev->get_normalized_tx_gain(chan);
#else
      ::uhd::gain_range_t gain_range = get_gain_range(chan);
      double norm_gain =
        (get_gain(chan) - gain_range.start()) /
        (gain_range.stop() - gain_range.start());
      // Avoid rounding errors:
      if (norm_gain > 1.0) return 1.0;
      if (norm_gain < 0.0) return 0.0;
      return norm_gain;
#endif
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
    std::vector<std::string>
    usrp_sink_impl::get_lo_names(size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->get_tx_lo_names(chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    const std::string
    usrp_sink_impl::get_lo_source(const std::string &name, size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->get_tx_lo_source(name, chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    std::vector<std::string>
    usrp_sink_impl::get_lo_sources(const std::string &name, size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->get_tx_lo_sources(name, chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    void
    usrp_sink_impl::set_lo_source(const std::string &src, const std::string &name, size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->set_tx_lo_source(src, name, chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    bool
    usrp_sink_impl::get_lo_export_enabled(const std::string &name, size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->get_tx_lo_export_enabled(name, chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    void
    usrp_sink_impl::set_lo_export_enabled(bool enabled, const std::string &name, size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->set_tx_lo_export_enabled(enabled, name, chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    ::uhd::freq_range_t
    usrp_sink_impl::get_lo_freq_range(const std::string &name, size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->get_tx_lo_freq_range(name, chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    double
    usrp_sink_impl::get_lo_freq(const std::string &name, size_t chan)
    {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->get_tx_lo_freq(name, chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    double
    usrp_sink_impl::set_lo_freq(double freq, const std::string &name, size_t chan) {
#ifdef UHD_USRP_MULTI_USRP_TX_LO_CONFIG_API
        chan = _stream_args.channels[chan];
        return _dev->set_tx_lo_freq(freq, name, chan);
#else
        throw std::runtime_error("not implemented in this version");
#endif
    }

    void
    usrp_sink_impl::set_dc_offset(const std::complex<double> &offset,
                                  size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->set_tx_dc_offset(offset, chan);
    }

    void
    usrp_sink_impl::set_iq_balance(const std::complex<double> &correction,
                                   size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->set_tx_iq_balance(correction, chan);
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

    ::uhd::usrp::dboard_iface::sptr
    usrp_sink_impl::get_dboard_iface(size_t chan)
    {
      chan = _stream_args.channels[chan];
      return _dev->get_tx_dboard_iface(chan);
    }

    void
    usrp_sink_impl::set_stream_args(const ::uhd::stream_args_t &stream_args)
    {
      _update_stream_args(stream_args);
      if (_tx_stream) {
        _tx_stream.reset();
      }
    }

    /***********************************************************************
     * Work
     **********************************************************************/
    int
    usrp_sink_impl::work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
    {
      int ninput_items = noutput_items; //cuz it's a sync block

      // default to send a mid-burst packet
      _metadata.start_of_burst = false;
      _metadata.end_of_burst = false;

      //collect tags in this work()
      const uint64_t samp0_count = nitems_read(0);
      get_tags_in_range(_tags, 0, samp0_count, samp0_count + ninput_items);
      if(not _tags.empty())
        this->tag_work(ninput_items);

      if(not pmt::is_null(_length_tag_key)) {
        //check if there is data left to send from a burst tagged with length_tag
        //If a burst is started during this call to work(), tag_work() should have
        //been called and we should have _nitems_to_send > 0.
        if (_nitems_to_send > 0) {
          ninput_items = std::min<long>(_nitems_to_send, ninput_items);
          //if we run out of items to send, it's the end of the burst
          if(_nitems_to_send - long(ninput_items) == 0)
            _metadata.end_of_burst = true;
        }
        else {
          //There is a tag gap since no length_tag was found immediately following
          //the last sample of the previous burst. Drop samples until the next
          //length_tag is found. Notify the user of the tag gap.
          std::cerr << "tG" << std::flush;
          //increment the timespec by the number of samples dropped
          _metadata.time_spec += ::uhd::time_spec_t(0, ninput_items, _sample_rate);
          return ninput_items;
        }
      }

      //send all ninput_items with metadata
      boost::this_thread::disable_interruption disable_interrupt;
      const size_t num_sent = _tx_stream->send(
              input_items, ninput_items, _metadata, 1.0
      );
      boost::this_thread::restore_interruption restore_interrupt(disable_interrupt);

      //if using length_tags, decrement items left to send by the number of samples sent
      if(not pmt::is_null(_length_tag_key) && _nitems_to_send > 0) {
        _nitems_to_send -= long(num_sent);
      }

      //increment the timespec by the number of samples sent
      _metadata.time_spec += ::uhd::time_spec_t(0, num_sent, _sample_rate);

      // Some post-processing tasks if we actually transmitted the entire burst
      if (not _pending_cmds.empty() && num_sent == size_t(ninput_items)) {
        GR_LOG_DEBUG(d_debug_logger, boost::format("Executing %d pending commands.") % _pending_cmds.size());
        BOOST_FOREACH(const pmt::pmt_t &cmd_pmt, _pending_cmds) {
          msg_handler_command(cmd_pmt);
        }
        _pending_cmds.clear();
      }

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
      const uint64_t samp0_count = this->nitems_read(0);
      uint64_t max_count = samp0_count + ninput_items;

      // Go through tag list until something indicates the end of a burst.
      bool found_time_tag = false;
      bool found_eob = false;
      // For commands that are in the middle of the burst:
      std::vector<pmt::pmt_t> commands_in_burst; // Store the command
      uint64_t in_burst_cmd_offset = 0; // Store its position
      BOOST_FOREACH(const tag_t &my_tag, _tags) {
        const uint64_t my_tag_count = my_tag.offset;
        const pmt::pmt_t &key = my_tag.key;
        const pmt::pmt_t &value = my_tag.value;

        if (my_tag_count >= max_count) {
          break;
        }

        /* I. Tags that can only be on the first sample of a burst
         *
         * This includes:
         * - tx_time
         * - tx_command TODO should also work end-of-burst
         * - tx_sob
         * - length tags
         *
         * With these tags, we check if they're on the first item, otherwise,
         * we stop before that tag so they are on the first item the next time round.
         */
        else if (pmt::equal(key, COMMAND_KEY)) {
          if (my_tag_count != samp0_count) {
            max_count = my_tag_count;
            break;
          }
          // TODO set the command time from the sample time
          msg_handler_command(value);
        }

        //set the time specification in the metadata
        else if(pmt::equal(key, TIME_KEY)) {
          if (my_tag_count != samp0_count) {
            max_count = my_tag_count;
            break;
          }
          found_time_tag = true;
          _metadata.has_time_spec = true;
          _metadata.time_spec = ::uhd::time_spec_t
            (pmt::to_uint64(pmt::tuple_ref(value, 0)),
             pmt::to_double(pmt::tuple_ref(value, 1)));
        }

        //set the start of burst flag in the metadata; ignore if length_tag_key is not null
        else if(pmt::is_null(_length_tag_key) && pmt::equal(key, SOB_KEY)) {
          if (my_tag.offset != samp0_count) {
            max_count = my_tag_count;
            break;
          }
          // Bursty tx will not use time specs, unless a tx_time tag is also given.
          _metadata.has_time_spec = false;
          _metadata.start_of_burst = pmt::to_bool(value);
        }

        //length_tag found; set the start of burst flag in the metadata
        else if(not pmt::is_null(_length_tag_key) && pmt::equal(key, _length_tag_key)) {
          if (my_tag_count != samp0_count) {
            max_count = my_tag_count;
            break;
          }
          //If there are still items left to send, the current burst has been preempted.
          //Set the items remaining counter to the new burst length. Notify the user of
          //the tag preemption.
          else if(_nitems_to_send > 0) {
              std::cerr << "tP" << std::flush;
          }
          _nitems_to_send = pmt::to_long(value);
          _metadata.start_of_burst = true;
        }

        /* II. Tags that can be on the first OR last sample of a burst
         *
         * This includes:
         * - tx_freq
         *
         * With these tags, we check if they're at the start of a burst, and do
         * the appropriate action. Otherwise, make sure the corresponding sample
         * is the last one.
         */
        else if (pmt::equal(key, FREQ_KEY) && my_tag_count == samp0_count) {
          // If it's on the first sample, immediately do the tune:
          GR_LOG_DEBUG(d_debug_logger, boost::format("Received tx_freq on start of burst."));
          pmt::pmt_t freq_cmd = pmt::make_dict();
          freq_cmd = pmt::dict_add(freq_cmd, cmd_freq_key(), value);
          msg_handler_command(freq_cmd);
        }
        else if(pmt::equal(key, FREQ_KEY)) {
          // If it's not on the first sample, queue this command and only tx until here:
          GR_LOG_DEBUG(d_debug_logger, boost::format("Received tx_freq mid-burst."));
          pmt::pmt_t freq_cmd = pmt::make_dict();
          freq_cmd = pmt::dict_add(freq_cmd, cmd_freq_key(), value);
          commands_in_burst.push_back(freq_cmd);
          max_count = my_tag_count + 1;
          in_burst_cmd_offset = my_tag_count;
        }

        /* III. Tags that can only be on the last sample of a burst
         *
         * This includes:
         * - tx_eob
         *
         * Make sure that no more samples are allowed through.
         */
        else if(pmt::is_null(_length_tag_key) && pmt::equal(key, EOB_KEY)) {
          found_eob = true;
          max_count = my_tag_count + 1;
          _metadata.end_of_burst = pmt::to_bool(value);
        }
      } // end foreach

      if(not pmt::is_null(_length_tag_key) && long(max_count - samp0_count) == _nitems_to_send) {
        found_eob = true;
      }

      // If a command was found in-burst that may appear at the end of burst,
      // there's two options:
      // 1) The command was actually on the last sample (eob). Then, stash the
      //    commands for running after work().
      // 2) The command was not on the last sample. In this case, only send()
      //    until before the tag, so it will be on the first sample of the next run.
      if (not commands_in_burst.empty()) {
        if (not found_eob) {
          // ...then it's in the middle of a burst, only send() until before the tag
          max_count = in_burst_cmd_offset;
        } else if (in_burst_cmd_offset < max_count) {
          BOOST_FOREACH(const pmt::pmt_t &cmd_pmt, commands_in_burst) {
            _pending_cmds.push_back(cmd_pmt);
          }
        }
      }

      if (found_time_tag) {
        _metadata.has_time_spec = true;
      }

      // Only transmit up to and including end of burst,
      // or everything if no burst boundaries are found.
      ninput_items = int(max_count - samp0_count);

    } // end tag_work()

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
      if (not _tx_stream)
        _tx_stream = _dev->get_tx_stream(_stream_args);

      _metadata.start_of_burst = true;
      _metadata.end_of_burst = false;
      // Bursty tx will need to send a tx_time to activate time spec
      _metadata.has_time_spec = !_stream_now && pmt::is_null(_length_tag_key);
      _nitems_to_send = 0;

      if(pmt::is_null(_length_tag_key)){ //don't execute this part in burst mode
        _metadata.start_of_burst = true;
        _metadata.end_of_burst = false;
        _metadata.has_time_spec = false;

        if(!_stream_now){
          _metadata.has_time_spec = true;
          if(_start_time_set) {
            _start_time_set = false; //cleared for next run
            _metadata.time_spec = _start_time;
          }
          else {
            _metadata.time_spec = get_time_now() + ::uhd::time_spec_t(0.15);
          }
        }

      _tx_stream->send
        (gr_vector_const_void_star(_nchan), 0, _metadata, 1.0);
      }
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
      _nitems_to_send = 0;

      if (_tx_stream) {
        _tx_stream->send(gr_vector_const_void_star(_nchan), 0, _metadata, 1.0);
      }
      return true;
    }


    void
    usrp_sink_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_handler<usrp_block>(
          alias(), "command",
          "", "UHD Commands",
          RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
    }

    void
    usrp_sink_impl::async_event_loop()
    {
      typedef ::uhd::async_metadata_t md_t;
      md_t metadata;

      while(_async_event_loop_running) {
        while(!_dev->get_device()->recv_async_msg(metadata, 0.1)) {
          if(!_async_event_loop_running){
            return;
          }
        }

        pmt::pmt_t event_list = pmt::PMT_NIL;

        if(metadata.event_code & md_t::EVENT_CODE_BURST_ACK){
          event_list = pmt::list_add(event_list, BURST_ACK_KEY);
        }
        if(metadata.event_code & md_t::EVENT_CODE_UNDERFLOW){
          event_list = pmt::list_add(event_list, UNDERFLOW_KEY);
        }
        if(metadata.event_code & md_t::EVENT_CODE_UNDERFLOW_IN_PACKET){
          event_list = pmt::list_add(event_list, UNDERFLOW_IN_PACKET_KEY);
        }
        if(metadata.event_code & md_t::EVENT_CODE_SEQ_ERROR){
          event_list = pmt::list_add(event_list, SEQ_ERROR_KEY);
        }
        if(metadata.event_code & md_t::EVENT_CODE_SEQ_ERROR_IN_BURST){
          event_list = pmt::list_add(event_list, SEQ_ERROR_IN_BURST_KEY);
        }
        if(metadata.event_code & md_t::EVENT_CODE_TIME_ERROR){
          event_list = pmt::list_add(event_list, TIME_ERROR_KEY);
        }

        if(!pmt::eq(event_list, pmt::PMT_NIL)){
          pmt::pmt_t value = pmt::dict_add(pmt::make_dict(), EVENT_CODE_KEY, event_list);
          if(metadata.has_time_spec){
            pmt::pmt_t time_spec = pmt::cons(
              pmt::from_long(metadata.time_spec.get_full_secs()),
              pmt::from_double(metadata.time_spec.get_frac_secs())
            );
            value = pmt::dict_add(value, TIME_SPEC_KEY, time_spec);
          }
          value = pmt::dict_add(value, CHANNEL_KEY, pmt::from_uint64(metadata.channel));
          pmt::pmt_t msg = pmt::cons(ASYNC_MSG_KEY, value);
          message_port_pub(ASYNC_MSGS_PORT_KEY, msg);
        }
      }
    }

  } /* namespace uhd */
} /* namespace gr */
