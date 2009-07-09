
/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2008,2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP_SINK_BASE_H
#define INCLUDED_USRP_SINK_BASE_H

#include <usrp_base.h>
#include <stdexcept>
#include <usrp/usrp_tune_result.h>
#include <usrp/usrp_dbid.h>

class usrp_standard_tx;

/*!
 * \brief abstract interface to Universal Software Radio Peripheral Tx path (Rev 1)
 */
class usrp_sink_base : public usrp_base {
 private:
  boost::shared_ptr<usrp_standard_tx>	d_usrp;
  int			 d_nunderruns;
  
 protected:
  usrp_sink_base (const std::string &name,
		   gr_io_signature_sptr input_signature,
		   int which_board,
		   unsigned int interp_rate,
		   int nchan,
		   int mux,
		   int fusb_block_size,
		   int fusb_nblocks,
		   const std::string fpga_filename,
		   const std::string firmware_filename
		   ) throw (std::runtime_error);

  /*!
   * \brief convert between input item format and usrp native format
   *
   * \param[in] input_items		stream(s) of input items
   * \param[in] input_index		starting index in input_items
   * \param[in] input_items_available	number of items available starting at item[index]
   * \param[out] input_items_consumed  	number of input items consumed by copy
   * \param[out] usrp_buffer		destination buffer
   * \param[in] usrp_buffer_length 	\p usrp_buffer length in bytes
   * \param[out] bytes_written		number of bytes written into \p usrp_buffer
   */
  virtual void copy_to_usrp_buffer (gr_vector_const_void_star &input_items,
				    int  input_index,
				    int	 input_items_available,
				    int  &input_items_consumed,
				    void *usrp_buffer,
				    int  usrp_buffer_length,
				    int	 &bytes_written) = 0;

 public:
  ~usrp_sink_base ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  /*!
   * \brief Set interpolator rate.  \p rate must be in [4, 1024] and a multiple of 4.
   *
   * The final complex sample rate across the USB is
   *   dac_freq () / interp_rate () * nchannels ()
   */
  bool set_interp_rate (unsigned int rate);
  bool set_nchannels (int nchan);
  bool set_mux (int mux);
  int determine_tx_mux_value(usrp_subdev_spec ss);
  int determine_tx_mux_value(usrp_subdev_spec ss_a, usrp_subdev_spec ss_b);

  /*!
   * \brief set the frequency of the digital up converter.
   *
   * \p channel must be 0.  \p freq is the center frequency in Hz.
   * It must be in the range [-44M, 44M].  The frequency specified is
   * quantized.  Use tx_freq to retrieve the actual value used.
   */
  bool set_tx_freq (int channel, double freq);

  long dac_rate() const { return converter_rate(); }	// alias
  long dac_freq() const { return converter_rate(); }	// deprecated alias

  unsigned int interp_rate () const;
  int nchannels () const;
  int mux () const;
  double tx_freq (int channel) const;
  int nunderruns () const { return d_nunderruns; }

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
   * \brief High-level "tune" method.  Works for the single channel case.
   *
   * This method adjusts both the daughterboard LO and the DUC so that
   * DC in the complex baseband samples ends up at RF target_freq.
   *
   * \param chan  which DUC channel we're controlling (usually == which_side).
   * \param db    the daughterboard we're controlling.
   * \param target_freq the RF frequency we want our baseband translated to.
   * \param[out] result details how the hardware was configured.
   *
   * \returns true iff everything was successful.
   */
  bool tune(int chan, db_base_sptr db, double target_freq, usrp_tune_result *result);

  /*!
   * \brief Select suitable Tx daughterboard
   */
  usrp_subdev_spec pick_tx_subdevice();
};

#endif /* INCLUDED_USRP_SINK_BASE_H */
