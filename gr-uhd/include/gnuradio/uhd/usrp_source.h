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

#ifndef INCLUDED_GR_UHD_USRP_SOURCE_H
#define INCLUDED_GR_UHD_USRP_SOURCE_H

#include <gnuradio/uhd/usrp_block.h>

// TODO In 3.8, UHD 3.6 will be required and we can remove all these ifdefs
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

    class uhd_usrp_source;

    /*! USRP Source -- Radio Receiver
     * \ingroup uhd_blk
     *
     * The USRP source block receives samples and writes to a stream.
     * The source block also provides API calls for receiver settings.
     * See also gr::uhd::usrp_block for more public API calls.
     *
     * RX Stream tagging:
     *
     * The following tag keys will be produced by the work function:
     *  - pmt::string_to_symbol("rx_time")
     *
     * The timestamp tag value is a pmt tuple of the following:
     * (uint64 seconds, and double fractional seconds).
     * A timestamp tag is produced at start() and after overflows.
     *
     * \section uhd_rx_command_iface Command interface
     *
     * This block has a message port, which consumes UHD PMT commands.
     * For a description of the command syntax, see Section \ref uhd_command_syntax.
     *
     * For a more general description of the gr-uhd components, see \ref page_uhd.
     *
     */
    class GR_UHD_API usrp_source : virtual public usrp_block
    {
    public:
      // gr::uhd::usrp_source::sptr
      typedef boost::shared_ptr<usrp_source> sptr;

      /*!
       * \brief DEPRECATED Make a new USRP source block using the deprecated io_type_t.
       * \ingroup uhd_blk
       *
       * This function will be removed in the future. Please use the other make function,
       * gr::uhd::make(const ::uhd::device_addr_t, const ::uhd::stream_args_t, const std::string).
       */
      static sptr make(const ::uhd::device_addr_t &device_addr,
                       const ::uhd::io_type_t &io_type,
                       size_t num_channels);

      /*!
       * \param device_addr the address to identify the hardware
       * \param stream_args the IO format and channel specification
       * \param issue_stream_cmd_on_start enable or disable continuous streaming when flowgraph
       *   starts (default true)
       * \return a new USRP source block object
       */
      static sptr make(const ::uhd::device_addr_t &device_addr,
                       const ::uhd::stream_args_t &stream_args,
                       const bool issue_stream_cmd_on_start = true);

      /*!
       * Set the start time for incoming samples.
       * To control when samples are received,
       * set this value before starting the flow graph.
       * The value is cleared after each run.
       * When not specified, the start time will be:
       *  - Immediately for the one channel case
       *  - in the near future for multi-channel
       *
       * \param time the absolute time for reception to begin
       */
      virtual void set_start_time(const ::uhd::time_spec_t &time) = 0;

      /*!
       * *Advanced use only:*
       * Issue a stream command to all channels in this source block.
       *
       * This method is intended to override the default "always on"
       * behavior. After starting the flow graph, the user should
       * call stop() on this block, then issue any desired arbitrary
       * stream_cmd_t structs to the device. The USRP will be able to
       * enqueue several stream commands in the FPGA.
       *
       * \param cmd the stream command to issue to all source channels
       */
      virtual void issue_stream_cmd(const ::uhd::stream_cmd_t &cmd) = 0;

      /*! Configure the timeout value on the UHD recv() call
       *
       * This is an advanced use parameter. Changing the timeout value affects
       * the latency of this block; a high timeout value can be more optimal
       * for high-throughput applications (e.g., 1 second) and setting it to
       * zero will have the best latency. Changing the timeout value may also
       * be useful for custom FPGA modifications, where traffic might not be
       * continuously streaming.
       * For specialized high-performance use cases, twiddling these knobs may
       * improve performance, but changes are not generally applicable.
       *
       * Note that UHD's recv() call may block until the timeout is over, which
       * means this block might also block for up to the timeout value.
       *
       * \param timeout Timeout parameter in seconds. Cf. the UHD manual for
       *                uhd::rx_streamer::recv() for more details. A lower
       *                value will mean lower latency, but higher CPU load.
       * \param one_packet If true, only receive one packet at a time. Cf. the
       *                   UHD manual for uhd::rx_streamer::recv() for more
       *                   details. A value of true will mean lower latency,
       *                   but higher CPU load.
       */
      virtual void set_recv_timeout(
          const double timeout,
          const bool one_packet=true
      ) = 0;

      /*!
       * Returns identifying information about this USRP's configuration.
       * Returns motherboard ID, name, and serial.
       * Returns daughterboard RX ID, subdev name and spec, serial, and antenna.
       * \param chan channel index 0 to N-1
       * \return RX info
       */
      virtual ::uhd::dict<std::string, std::string> get_usrp_info(size_t chan = 0) = 0;


      /*!
       * Get a list of possible LO stage names
       * \param chan the channel index 0 to N-1
       * \return a vector of strings for possible LO names
       */
      virtual std::vector<std::string> get_lo_names(size_t chan = 0) = 0;

      /*!
       * Set the LO source for the usrp device.
       * For usrps that support selectable LOs, this function
       * allows switching between them.
       * Typical options for source: internal, external.
       * \param src a string representing the LO source
       * \param name the name of the LO stage to update
       * \param chan the channel index 0 to N-1
       */
      virtual void set_lo_source(const std::string &src, const std::string &name, size_t chan = 0) = 0;

      /*!
       * Get the currently set LO source.
       * \param name the name of the LO stage to query
       * \param chan the channel index 0 to N-1
       * \return the configured LO source
       */
      virtual const std::string get_lo_source(const std::string &name, size_t chan = 0) = 0;

      /*!
       * Get a list of possible LO sources.
       * \param name the name of the LO stage to query
       * \param chan the channel index 0 to N-1
       * \return a vector of strings for possible settings
       */
      virtual std::vector<std::string> get_lo_sources(const std::string &name, size_t chan = 0) = 0;

      /*!
       * Set whether the LO used by the usrp device is exported
       * For usrps that support exportable LOs, this function
       * configures if the LO used by chan is exported or not.
       * \param enabled if true then export the LO
       * \param name the name of the LO stage to update
       * \param chan the channel index 0 to N-1 for the source channel
       */
      virtual void set_lo_export_enabled(bool enabled, const std::string &name, size_t chan = 0) = 0;

      /*!
       * Returns true if the currently selected LO is being exported.
       * \param name the name of the LO stage to query
       * \param chan the channel index 0 to N-1
       */
      virtual bool get_lo_export_enabled(const std::string &name, size_t chan = 0) = 0;

      /*!
       * Set the RX LO frequency (Advanced).
       * \param freq the frequency to set the LO to
       * \param name the name of the LO stage to update
       * \param chan the channel index 0 to N-1
       * \return a coerced LO frequency
       */
      virtual double set_lo_freq(double freq, const std::string &name, size_t chan = 0) = 0;

      /*!
       * Get the current RX LO frequency (Advanced).
       * \param name the name of the LO stage to query
       * \param chan the channel index 0 to N-1
       * \return the configured LO frequency
       */
      virtual double get_lo_freq(const std::string &name, size_t chan = 0) = 0;

      /*!
       * Get the LO frequency range of the RX LO.
       * \param name the name of the LO stage to query
       * \param chan the channel index 0 to N-1
       * \return a frequency range object
       */
      virtual ::uhd::freq_range_t get_lo_freq_range(const std::string &name, size_t chan = 0) = 0;

      /*!
       * Enable/disable the automatic DC offset correction.
       * The automatic correction subtracts out the long-run average.
       *
       * When disabled, the averaging option operation is halted.
       * Once halted, the average value will be held constant until
       * the user re-enables the automatic correction or overrides the
       * value by manually setting the offset.
       *
       * \param enb true to enable automatic DC offset correction
       * \param chan the channel index 0 to N-1
       */
      virtual void set_auto_dc_offset(const bool enb, size_t chan = 0) = 0;

      /*!
       * Set a constant DC offset value.
       * The value is complex to control both I and Q.
       * Only set this when automatic correction is disabled.
       * \param offset the dc offset (1.0 is full-scale)
       * \param chan the channel index 0 to N-1
       */
      virtual void set_dc_offset(const std::complex<double> &offset, size_t chan = 0) = 0;

      /*!
       * Enable/Disable the RX frontend IQ imbalance correction.
       *
       * \param enb true to enable automatic IQ imbalance correction
       * \param chan the channel index 0 to N-1
       */
      virtual void set_auto_iq_balance(const bool enb, size_t chan = 0) = 0;

      /*!
       * Set the RX frontend IQ imbalance correction.
       * Use this to adjust the magnitude and phase of I and Q.
       *
       * \param correction the complex correction value
       * \param chan the channel index 0 to N-1
       */
      virtual void set_iq_balance(const std::complex<double> &correction,
                                  size_t chan = 0) = 0;

      /*!
       * Convenience function for finite data acquisition.
       * This is not to be used with the scheduler; rather,
       * one can request samples from the USRP in python.
       * //TODO assumes fc32
       * \param nsamps the number of samples
       * \return a vector of complex float samples
       */
      virtual std::vector<std::complex<float> >
        finite_acquisition(const size_t nsamps) = 0;

      /*!
       * Convenience function for finite data acquisition. This is the
       * multi-channel version of finite_acquisition; This is not to
       * be used with the scheduler; rather, one can request samples
       * from the USRP in python.
       * //TODO assumes fc32
       * \param nsamps the number of samples per channel
       * \return a vector of buffers, where each buffer represents a channel
       */
      virtual std::vector<std::vector<std::complex<float> > >
        finite_acquisition_v(const size_t nsamps) = 0;
    };

  } /* namespace uhd */
} /* namespace gr */

#endif /* INCLUDED_GR_UHD_USRP_SOURCE_H */
