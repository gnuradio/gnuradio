/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP_SOURCE_BASE_H
#define INCLUDED_USRP_SOURCE_BASE_H

#include <usrp_base.h>
#include <stdexcept>
#include <usrp/usrp_tune_result.h>
#include <usrp/usrp_dbid.h>

class usrp_standard_rx;

/*!
 * \brief abstract interface to Universal Software Radio Peripheral Rx path (Rev 1)
 */
class usrp_source_base : public usrp_base {
 private:
  boost::shared_ptr<usrp_standard_rx>	d_usrp;
  int			 d_noverruns;
  
 protected:
  usrp_source_base (const std::string &name,
		     gr_io_signature_sptr output_signature,
		     int which_board,
		     unsigned int decim_rate,
		     int nchan,
		     int mux,
		     int mode,
		     int fusb_block_size,
		     int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename
		     ) throw (std::runtime_error);

  /*!
   * \brief return number of usrp input bytes required to produce noutput items.
   */
  virtual int ninput_bytes_reqd_for_noutput_items (int noutput_items) = 0;

  /*!
   * \brief number of bytes in a low-level sample
   */
  unsigned int sizeof_basic_sample() const;

  /*!
   * \brief convert between native usrp format and output item format
   *
   * \param[out] output_items		stream(s) of output items
   * \param[in] output_index		starting index in output_items
   * \param[in] output_items_available	number of empty items available at item[index]
   * \param[out] output_items_produced	number of items produced by copy
   * \param[in] usrp_buffer		source buffer
   * \param[in] usrp_buffer_length	number of bytes available in \p usrp_buffer
   * \param[out] bytes_read		number of bytes read from \p usrp_buffer
   *
   * The copy must consume all bytes available.  That is, \p bytes_read must equal
   * \p usrp_buffer_length.
   */
  virtual void copy_from_usrp_buffer (gr_vector_void_star &output_items,
				      int output_index,
				      int output_items_available,
				      int &output_items_produced,
				      const void *usrp_buffer,
				      int usrp_buffer_length,
				      int &bytes_read) = 0;

 public:
  ~usrp_source_base ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  /*!
   * \brief Set decimator rate.  \p rate must be EVEN and in [8, 256].
   *
   * The final complex sample rate across the USB is
   *   adc_freq () / decim_rate ()
   */
  bool set_decim_rate (unsigned int rate);
  bool set_nchannels (int nchan);
  bool set_mux (int mux);
  int determine_rx_mux_value(usrp_subdev_spec ss);
  int determine_rx_mux_value(usrp_subdev_spec ss_a, usrp_subdev_spec ss_b);

  /*!
   * \brief set the center frequency of the digital down converter.
   *
   * \p channel must be 0.  \p freq is the center frequency in Hz.
   * It must be in the range [-FIXME, FIXME].  The frequency specified is
   * quantized.  Use rx_freq to retrieve the actual value used.
   */
  bool set_rx_freq (int channel, double freq);

  /*!
   * \brief set fpga special modes
   */
  bool set_fpga_mode (int mode);

  /*!
   * \brief Set the digital down converter phase register.
   *
   * \param channel	which ddc channel [0, 3]
   * \param phase	32-bit integer phase value.
   */
  bool set_ddc_phase(int channel, int phase);

  long adc_rate() const { return converter_rate(); }   // alias
  long adc_freq() const { return converter_rate(); }   // deprecated alias

  unsigned int decim_rate () const;
  int nchannels () const;
  int mux () const;
  double rx_freq (int channel) const;
  int noverruns () const { return d_noverruns; }

  bool has_rx_halfband();
  bool has_tx_halfband();
  int nddcs();
  int nducs();

  /*!
   * \brief Called to enable drivers, etc for i/o devices.
   *
   * This allows a block to enable an associated driver to begin
   * transfering data just before we start to execute the scheduler.
   * The end result is that this reduces latency in the pipeline when
   * dealing with audio devices, usrps, etc.
   */
  bool start();

  /*!
   * \brief Called to disable drivers, etc for i/o devices.
   */
  bool stop();

  /*!
   * \brief Specify Rx data format.
   *
   * \param format	format specifier
   *
   * Rx data format control register
   *
   *     3                   2                   1                       
   *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   *  +-----------------------------------------+-+-+---------+-------+
   *  |          Reserved (Must be zero)        |B|Q|  WIDTH  | SHIFT |
   *  +-----------------------------------------+-+-+---------+-------+
   *
   *  SHIFT specifies arithmetic right shift [0, 15]
   *  WIDTH specifies bit-width of I & Q samples across the USB [1, 16] (not all valid)
   *  Q     if set deliver both I & Q, else just I
   *  B     if set bypass half-band filter.
   *
   * Right now the acceptable values are:
   *
   *   B  Q  WIDTH  SHIFT
   *   0  1    16     0
   *   0  1     8     8
   *
   * More valid combos to come.
   *
   * Default value is 0x00000300  16-bits, 0 shift, deliver both I & Q.
   */
  bool set_format(unsigned int format);

  /*!
   * \brief return current format
   */
  unsigned int format () const;

  static unsigned int make_format(int width=16, int shift=0,
				  bool want_q=true, bool bypass_halfband=false);
  static int format_width(unsigned int format);
  static int format_shift(unsigned int format);
  static bool format_want_q(unsigned int format);
  static bool format_bypass_halfband(unsigned int format);

  /*!
   * \brief High-level "tune" method.  Works for the single channel case.
   *
   * This method adjusts both the daughterboard LO and the DDC so that
   * target_freq ends up at DC in the complex baseband samples.
   *
   * \param chan  which DDC channel we're controlling (almost always 0).
   * \param db    the daughterboard we're controlling.
   * \param target_freq the RF frequency we want at DC in the complex baseband.
   * \param[out] result details how the hardware was configured.
   *
   * \returns true iff everything was successful.
   */
  bool tune(int chan, db_base_sptr db, double target_freq, usrp_tune_result *result);

  /*!
   * \brief Select suitable Rx daughterboard
   */
  usrp_subdev_spec pick_rx_subdevice();
};

#endif /* INCLUDED_USRP_SOURCE_BASE_H */
