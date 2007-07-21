/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP_STANDARD_H
#define INCLUDED_USRP_STANDARD_H

#include <usrp_basic.h>

class usrp_standard_common
{
  int			d_fpga_caps;		// capability register val

protected:
  usrp_standard_common(usrp_basic *parent);

public:
  /*!
   *\brief does the FPGA implement the final Rx half-band filter?
   * If it doesn't, the maximum decimation factor with proper gain 
   * is 1/2 of what it would otherwise be.
   */
  bool has_rx_halfband() const;

  /*!
   * \brief number of digital downconverters implemented in the FPGA
   * This will be 0, 1, 2 or 4.
   */
  int nddcs() const;

  /*!
   *\brief does the FPGA implement the initial Tx half-band filter?
   */
  bool has_tx_halfband() const;

  /*!
   * \brief number of digital upconverters implemented in the FPGA
   * This will be 0, 1, or 2.
   */
  int nducs() const;
};

/*!
 * \brief standard usrp RX class.  
 *
 * Assumes digital down converter in FPGA
 */
class usrp_standard_rx : public usrp_basic_rx, usrp_standard_common
{
 private:
  static const int	MAX_CHAN = 4;
  unsigned int		d_decim_rate;
  int			d_nchan;
  int			d_sw_mux;
  int			d_hw_mux;
  double		d_rx_freq[MAX_CHAN];

 protected:
  usrp_standard_rx (int which_board,
		    unsigned int decim_rate,
		    int nchan = 1,
		    int mux = -1,
		    int mode = 0,
		    int fusb_block_size = 0,
		    int fusb_nblocks = 0,
		    const std::string fpga_filename = "",
		    const std::string firmware_filename = ""
		    );  // throws if trouble

  bool write_hw_mux_reg ();

 public:

  enum {
    FPGA_MODE_NORMAL     = 0x00,
    FPGA_MODE_LOOPBACK   = 0x01,
    FPGA_MODE_COUNTING   = 0x02,
    FPGA_MODE_COUNTING_32BIT   = 0x04
  };

  ~usrp_standard_rx ();

  /*!
   * \brief invokes constructor, returns instance or 0 if trouble
   *
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512. 
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default. 
   */
  static usrp_standard_rx *make (int which_board,
				 unsigned int decim_rate,
				 int nchan = 1,
				 int mux = -1,
				 int mode = 0,
				 int fusb_block_size = 0,
				 int fusb_nblocks = 0,
				 const std::string fpga_filename = "",
				 const std::string firmware_filename = ""
				 );
  /*!
   * \brief Set decimator rate.  \p rate MUST BE EVEN and in [8, 256].
   *
   * The final complex sample rate across the USB is
   *   adc_freq () / decim_rate () * nchannels ()
   */
  bool set_decim_rate  (unsigned int rate);

  /*!
   * \brief Set number of active channels.  \p nchannels must be 1, 2 or 4.
   *
   * The final complex sample rate across the USB is
   *   adc_freq () / decim_rate () * nchannels ()
   */
  bool set_nchannels (int nchannels);

  /*!
   * \brief Set input mux configuration.
   *
   * This determines which ADC (or constant zero) is connected to 
   * each DDC input.  There are 4 DDCs.  Each has two inputs.
   *
   * <pre>
   * Mux value:
   *
   *    3                   2                   1                       
   *  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   * +-------+-------+-------+-------+-------+-------+-------+-------+
   * |   Q3  |   I3  |   Q2  |   I2  |   Q1  |   I1  |   Q0  |   I0  |
   * +-------+-------+-------+-------+-------+-------+-------+-------+
   *
   * Each 4-bit I field is either 0,1,2,3
   * Each 4-bit Q field is either 0,1,2,3 or 0xf (input is const zero)
   * All Q's must be 0xf or none of them may be 0xf
   * </pre>
   */
  bool set_mux  (int mux);

  /*!
   * \brief set the frequency of the digital down converter.
   *
   * \p channel must be in the range [0,3].  \p freq is the center
   * frequency in Hz.  \p freq may be either negative or postive.
   * The frequency specified is quantized.  Use rx_freq to retrieve
   * the actual value used.
   */
  bool set_rx_freq (int channel, double freq);  

  /*!
   * \brief set fpga mode
   */
  bool set_fpga_mode (int mode);

  /*!
   * \brief Set the digital down converter phase register.
   *
   * \param channel	which ddc channel [0, 3]
   * \param phase	32-bit integer phase value.
   */
  bool set_ddc_phase(int channel, int phase);

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

  static unsigned int make_format(int width=16, int shift=0,
				  bool want_q=true, bool bypass_halfband=false);
  static int format_width(unsigned int format);
  static int format_shift(unsigned int format);
  static bool format_want_q(unsigned int format);
  static bool format_bypass_halfband(unsigned int format);

  // ACCESSORS
  unsigned int decim_rate () const;
  double rx_freq (int channel) const;
  int nchannels () const;
  int mux () const;
  unsigned int format () const;

  // called in base class to derived class order
  bool start ();
  bool stop ();
};

// ----------------------------------------------------------------

/*!
 * \brief standard usrp TX class.
 *
 * Uses digital upconverter (coarse & fine modulators) in AD9862...
 */
class usrp_standard_tx : public usrp_basic_tx, usrp_standard_common
{
 public:
  enum coarse_mod_t {
    CM_NEG_FDAC_OVER_4,		// -32 MHz
    CM_NEG_FDAC_OVER_8,		// -16 MHz
    CM_OFF,
    CM_POS_FDAC_OVER_8,		// +16 MHz
    CM_POS_FDAC_OVER_4		// +32 MHz
  };

 protected:
  static const int	MAX_CHAN = 2;
  unsigned int		d_interp_rate;
  int			d_nchan;
  int			d_sw_mux;
  int			d_hw_mux;
  double		d_tx_freq[MAX_CHAN];
  coarse_mod_t		d_coarse_mod[MAX_CHAN];
  unsigned char		d_tx_modulator_shadow[MAX_CHAN];

  virtual bool set_coarse_modulator (int channel, coarse_mod_t cm);
  usrp_standard_tx::coarse_mod_t coarse_modulator (int channel) const;

 protected:
  usrp_standard_tx (int which_board,
		    unsigned int interp_rate,
		    int nchan = 1,
		    int	mux = -1,
		    int fusb_block_size = 0,
		    int fusb_nblocks = 0,
		    const std::string fpga_filename = "",
		    const std::string firmware_filename = ""
		    );	// throws if trouble

  bool write_hw_mux_reg ();

 public:
  ~usrp_standard_tx ();

  /*!
   * \brief invokes constructor, returns instance or 0 if trouble
   *
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512. 
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default. 
   */
  static usrp_standard_tx *make (int which_board,
				 unsigned int interp_rate,
				 int nchan = 1,
				 int mux = -1,
				 int fusb_block_size = 0,
				 int fusb_nblocks = 0,
				 const std::string fpga_filename = "",
				 const std::string firmware_filename = ""
				 );

  /*!
   * \brief Set interpolator rate.  \p rate must be in [4, 512] and a multiple of 4.
   *
   * The final complex sample rate across the USB is
   *   dac_freq () / interp_rate () * nchannels ()
   */
  virtual bool set_interp_rate (unsigned int rate);

  /*!
   * \brief Set number of active channels.  \p nchannels must be 1 or 2.
   *
   * The final complex sample rate across the USB is
   *   dac_freq () / decim_rate () * nchannels ()
   */
  bool set_nchannels  (int nchannels);

  /*!
   * \brief Set output mux configuration.
   *
   * <pre>
   *     3                   2                   1                       
   *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   *  +-------------------------------+-------+-------+-------+-------+
   *  |                               | DAC3  | DAC2  | DAC1  |  DAC0 |
   *  +-------------------------------+-------+-------+-------+-------+
   * 
   *  There are two interpolators with complex inputs and outputs.
   *  There are four DACs.
   * 
   *  Each 4-bit DACx field specifies the source for the DAC and
   *  whether or not that DAC is enabled.  Each subfield is coded
   *  like this: 
   * 
   *     3 2 1 0
   *    +-+-----+
   *    |E|  N  |
   *    +-+-----+
   * 
   *  Where E is set if the DAC is enabled, and N specifies which
   *  interpolator output is connected to this DAC.
   * 
   *   N   which interp output
   *  ---  -------------------
   *   0   chan 0 I
   *   1   chan 0 Q
   *   2   chan 1 I
   *   3   chan 1 Q
   * </pre>
   */
  bool set_mux  (int mux);

  /*!
   * \brief set the frequency of the digital up converter.
   *
   * \p channel must be in the range [0,1].  \p freq is the center
   * frequency in Hz.  It must be in the range [-44M, 44M].
   * The frequency specified is quantized.  Use tx_freq to retrieve
   * the actual value used.
   */
  virtual bool set_tx_freq (int channel, double freq);  // chan: [0,1]

  // ACCESSORS
  unsigned int interp_rate () const;
  double tx_freq (int channel) const;
  int nchannels () const;
  int mux () const;

  // called in base class to derived class order
  bool start ();
  bool stop ();
};

#endif /* INCLUDED_USRP_STANDARD_H */
