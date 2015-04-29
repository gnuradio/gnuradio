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

#ifndef INCLUDED_GR_UHD_USRP_SINK_H
#define INCLUDED_GR_UHD_USRP_SINK_H

#include <gnuradio/uhd/usrp_block.h>

// TODO In 3.8, UHD 3.4 will be required and we can remove all these ifdefs
#ifndef INCLUDED_UHD_STREAM_HPP
namespace uhd {
  struct GR_UHD_API stream_args_t
  {
    stream_args_t(const std::string &cpu = "",
                  const std::string &otw = "")
      {
        cpu_format = cpu;
        otw_format = otw;
      }
    std::string cpu_format;
    std::string otw_format;
    device_addr_t args;
    std::vector<size_t> channels;
  };
}
#  define INCLUDED_UHD_STREAM_HPP
#else
#  define GR_UHD_USE_STREAM_API
#endif

namespace gr {
  namespace uhd {

    class uhd_usrp_sink;

    /*! USRP Sink -- Radio Transmitter
     * \ingroup uhd_blk
     *
     *
     * The USRP sink block reads a stream and transmits the samples.
     * The sink block also provides API calls for transmitter settings.
     * See also gr::uhd::usrp_block for more public API calls.
     *
     * \section uhd_tx_tagging TX Stream tagging
     *
     * The following tag keys will be consumed by the work function:
     *  - pmt::string_to_symbol("tx_sob")
     *  - pmt::string_to_symbol("tx_eob")
     *  - pmt::string_to_symbol("tx_time")
     *  - pmt::string_to_symbol("tx_freq")
     *  - pmt::string_to_symbol("tx_command")
     *  - pmt::string_to_symbol(tsb_tag_name)
     *
     * Any other tag will be ignored.
     *
     * \section uhd_tx_burstys Bursty Transmission
     *
     * There are multiple ways to do bursty transmission without triggering
     * underruns:
     * - Using SOB/EOB tags
     * - Using tagged streams (See \ref page_tagged_stream_blocks)
     *
     * The sob and eob (start and end of burst) tag values are pmt booleans.
     * When present, burst tags should be set to true (pmt::PMT_T).
     *
     * If `tsb_tag_name` is not an empty string, all "tx_sob" and "tx_eob"
     * tags will be ignored, and the input is assumed to a tagged stream.
     *
     * If sob/eob tags or length tags are used, this block understands that
     * the data is bursty, and will configure the USRP to make sure there's
     * no underruns after transmitting the final sample of a burst.
     *
     * \section uhd_tx_time Timestamps
     *
     * The timestamp tag value is a PMT tuple of the following:
     * (uint64 seconds, double fractional seconds).
     *
     * The tx_freq tag has to be a double or a pair of form (channel, frequency),
     * with frequency being a double and channel being an integer.
     * This tag will trigger a tune command to the USRP
     * to the given frequency, if possible. Note that oscillators need some time
     * to stabilize after this! Don't expect clean data to be sent immediately after this command.
     * If channel is omitted, and only a double is given, it will set this frequency to all
     * channels.
     *
     * The command tag can carry a PMT command. See the following section.
     *
     * \section uhd_tx_commands Command interface
     *
     * There are two ways of passing commands to this block:
     * 1. tx_command tag. The command is attached to a sample, and will executed
     *    before the sample is transmitted, and after the previous sample.
     * 2. The 'command' message port. The command is executed asynchronously,
     *    as soon as possible.
     *
     * In both cases, the payload of the command is a PMT command, as described
     * in Section \ref uhd_command_syntax.
     *
     * For a more general description of the gr-uhd components, see \ref page_uhd.
     */
    class GR_UHD_API usrp_sink : virtual public usrp_block
    {
    public:
      // gr::uhd::usrp_sink::sptr
      typedef boost::shared_ptr<usrp_sink> sptr;

      /*!
       * \brief DEPRECATED Make a new USRP sink block using the deprecated io_type_t.
       * \ingroup uhd_blk
       *
       * This function will be removed in the future. Please use the other make function,
       * gr::uhd::usrp_sink::make(const ::uhd::device_addr_t, const ::uhd::stream_args_t, const std::string).
       */
      static sptr make(const ::uhd::device_addr_t &device_addr,
                       const ::uhd::io_type_t &io_type,
                       size_t num_channels);

      /*!
       * \param device_addr the address to identify the hardware
       * \param stream_args the IO format and channel specification
       * \param tsb_tag_name the name of the tag identifying tagged stream length
       * \return a new USRP sink block object
       */
      static sptr make(const ::uhd::device_addr_t &device_addr,
                       const ::uhd::stream_args_t &stream_args,
                       const std::string &tsb_tag_name = "");

      /*!
       * Set the start time for outgoing samples.
       * To control when samples are transmitted,
       * set this value before starting the flow graph.
       * The value is cleared after each run.
       * When not specified, the start time will be:
       *  - Immediately for the one channel case
       *  - in the near future for multi-channel
       *
       * \param time the absolute time for transmission to begin
       */
      virtual void set_start_time(const ::uhd::time_spec_t &time) = 0;

      /*!
       * Returns identifying information about this USRP's configuration.
       * Returns motherboard ID, name, and serial.
       * Returns daughterboard TX ID, subdev name and spec, serial, and antenna.
       * \param chan channel index 0 to N-1
       * \return TX info
       */
      virtual ::uhd::dict<std::string, std::string> get_usrp_info(size_t chan = 0) = 0;

      /*!
       * Set a constant DC offset value.
       * The value is complex to control both I and Q.
       * \param offset the dc offset (1.0 is full-scale)
       * \param chan the channel index 0 to N-1
       */
      virtual void set_dc_offset(const std::complex<double> &offset,
                                 size_t chan = 0) = 0;

      /*!
       * Set the RX frontend IQ imbalance correction.
       * Use this to adjust the magnitude and phase of I and Q.
       *
       * \param correction the complex correction (1.0 is full-scale)
       * \param chan the channel index 0 to N-1
       */
      virtual void set_iq_balance(const std::complex<double> &correction,
                                  size_t chan = 0) = 0;

    };

  } /* namespace uhd */
} /* namespace gr */

#endif /* INCLUDED_GR_UHD_USRP_SINK_H */
