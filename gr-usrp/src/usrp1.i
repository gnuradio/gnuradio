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

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"				// the common stuff

%{

#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include "usrp1_sink_c.h"
#include "usrp1_sink_s.h"
#include "usrp1_source_c.h"
#include "usrp1_source_s.h"
#include <stdexcept>
#include <usrp_standard.h>
#include <usrp_spi_defs.h>
%}

%include <usrp_spi_defs.h>

%constant int FPGA_MODE_NORMAL   = usrp_standard_rx::FPGA_MODE_NORMAL;
%constant int FPGA_MODE_LOOPBACK = usrp_standard_rx::FPGA_MODE_LOOPBACK;
%constant int FPGA_MODE_COUNTING = usrp_standard_rx::FPGA_MODE_COUNTING;

// ================================================================
//			   abstract classes
// ================================================================

class usrp1_sink_base : public gr_sync_block {
protected:
  usrp1_sink_base (const std::string &name,
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

  virtual void copy_to_usrp_buffer (gr_vector_const_void_star &input_items,
				    int  input_index,
				    int	 input_items_available,
				    int  &input_items_consumed,
				    void *usrp_buffer,
				    int  usrp_buffer_length,
				    int	 &bytes_written) = 0;
 public:
  ~usrp1_sink_base ();

  /*!
   * \brief Set interpolator rate.  \p rate must be in [4, 1024] and a multiple of 4.
   *
   * The final complex sample rate across the USB is
   *   dac_freq () * nchannels () / interp_rate ()
   */
  bool set_interp_rate (unsigned int rate);
  bool set_nchannels (int nchan);
  bool set_mux (int mux);

  /*!
   * \brief set the frequency of the digital up converter.
   *
   * \p channel must be 0 or 1.  \p freq is the center frequency in Hz.
   * It must be in the range [-44M, 44M].  The frequency specified is
   * quantized.  Use tx_freq to retrieve the actual value used.
   */
  bool set_tx_freq (int channel, double freq);

  void set_verbose (bool verbose);

  // ACCESSORS

  long fpga_master_clock_freq() const;
  long converter_rate() const;      // D/A sample rate
  long dac_rate() const;            // alias
  long dac_freq () const;           // deprecated name.  Use converter_rate() or dac_rate().

  unsigned int interp_rate () const;
  double tx_freq (int channel) const;
  int nunderruns () const { return d_nunderruns; }

  /*!
   * \brief Set Programmable Gain Amplifier (PGA)
   *
   * \param which	which D/A [0,3]
   * \param gain_in_db	gain value (linear in dB)
   *
   * gain is rounded to closest setting supported by hardware.
   * Note that DAC 0 and DAC 1 share a gain setting as do DAC 2 and DAC 3.
   * Setting DAC 0 affects DAC 1 and vice versa.  Same with DAC 2 and DAC 3.
   *
   * \returns true iff sucessful.
   *
   * \sa pga_min(), pga_max(), pga_db_per_step()
   */
  bool set_pga (int which, double gain_in_db);

  /*!
   * \brief Return programmable gain amplifier gain in dB.
   *
   * \param which	which D/A [0,3]
   */
  double pga (int which) const;

  /*!
   * \brief Return minimum legal PGA gain in dB.
   */
  double pga_min () const;

  /*!
   * \brief Return maximum legal PGA gain in dB.
   */
  double pga_max () const;

  /*!
   * \brief Return hardware step size of PGA (linear in dB).
   */
  double pga_db_per_step () const;

  /*!
   * \brief Return daughterboard ID for given Tx daughterboard slot [0,1].
   *
   * \return daughterboard id >= 0 if successful
   * \return -1 if no daugherboard
   * \return -2 if invalid EEPROM on daughterboard
   */
  int daughterboard_id (int which_dboard) const;

  /*!
   * \brief Set ADC offset correction
   * \param which	which ADC[0,3]: 0 = RX_A I, 1 = RX_A Q...
   * \param offset	16-bit value to subtract from raw ADC input.
   */
  bool set_adc_offset (int which, int offset);

  /*!
   * \brief Set DAC offset correction
   * \param which	which DAC[0,3]: 0 = TX_A I, 1 = TX_A Q...
   * \param offset	10-bit offset value (ambiguous format:  See AD9862 datasheet).
   * \param offset_pin	1-bit value.  If 0 offset applied to -ve differential pin;
   *                                  If 1 offset applied to +ve differential pin.
   */
  bool set_dac_offset (int which, int offset, int offset_pin);

  /*!
   * \brief Control ADC input buffer
   * \param which 	which ADC[0,3]
   * \param bypass	if non-zero, bypass input buffer and connect input
   *	                directly to switched cap SHA input of RxPGA.
   */
  bool set_adc_buffer_bypass (int which, bool bypass);

  /*!
   * \brief return the usrp's serial number.
   *
   * \returns non-zero length string iff successful.
   */
  std::string serial_number();

  /*!
   * \brief Write direction register (output enables) for pins that go to daughterboard.
   *
   * \param which_dboard	[0,1] which d'board
   * \param value		value to write into register
   * \param mask		which bits of value to write into reg
   *
   * Each d'board has 16-bits of general purpose i/o.
   * Setting the bit makes it an output from the FPGA to the d'board.
   *
   * This register is initialized based on a value stored in the
   * d'board EEPROM.  In general, you shouldn't be using this routine
   * without a very good reason.  Using this method incorrectly will
   * kill your USRP motherboard and/or daughterboard.
   */
  bool _write_oe (int which_dboard, int value, int mask);

  /*!
   * \brief Write daughterboard i/o pin value
   *
   * \param which_dboard	[0,1] which d'board
   * \param value		value to write into register
   * \param mask		which bits of value to write into reg
   */
  bool write_io (int which_dboard, int value, int mask);

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param which_dboard	[0,1] which d'board
   * \returns register value if successful, else READ_FAILED
   */
  int read_io (int which_dboard);

  bool write_aux_dac (int which_dboard, int which_dac, int value);
  int read_aux_adc (int which_dboard, int which_adc);
  bool write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf);
  std::string read_eeprom (int i2c_addr, int eeprom_offset, int len);
  bool write_i2c (int i2c_addr, const std::string buf);
  std::string read_i2c (int i2c_addr, int len);

  bool _write_fpga_reg (int regno, int value);	//< 7-bit regno, 32-bit value
  int  _read_fpga_reg (int regno);
  bool _write_9862 (int which_codec, int regno, unsigned char value);
  int  _read_9862 (int which_codec, int regno) const;

  /*!
   * \brief Write data to SPI bus peripheral.
   *
   * \param optional_header	0,1 or 2 bytes to write before buf.
   * \param enables		bitmask of peripherals to write. See usrp_spi_defs.h
   * \param format		transaction format.  See usrp_spi_defs.h SPI_FMT_*
   * \param buf			the data to write
   * \returns true iff successful
   * Writes are limited to a maximum of 64 bytes.
   *
   * If \p format specifies that optional_header bytes are present, they are
   * written to the peripheral immediately prior to writing \p buf.
   */
  bool _write_spi (int optional_header, int enables, int format, std::string buf);

  /*
   * \brief Read data from SPI bus peripheral.
   *
   * \param optional_header	0,1 or 2 bytes to write before buf.
   * \param enables		bitmask of peripheral to read. See usrp_spi_defs.h
   * \param format		transaction format.  See usrp_spi_defs.h SPI_FMT_*
   * \param len			number of bytes to read.  Must be in [0,64].
   * \returns the data read if sucessful, else a zero length string.
   *
   * Reads are limited to a maximum of 64 bytes.
   *
   * If \p format specifies that optional_header bytes are present, they
   * are written to the peripheral first.  Then \p len bytes are read from
   * the peripheral and returned.
   */
  std::string _read_spi (int optional_header, int enables, int format, int len);
};

// ----------------------------------------------------------------

class usrp1_source_base : public gr_sync_block {
 protected:

  usrp1_source_base (const std::string &name,
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

  virtual int ninput_bytes_reqd_for_noutput_items (int noutput_items) = 0;

  virtual void copy_from_usrp_buffer (gr_vector_void_star &output_items,
				      int output_index,
				      int output_items_available,
				      int &output_items_produced,
				      const void *usrp_buffer,
				      int usrp_buffer_length,
				      int &bytes_read) = 0;
 public:
  ~usrp1_source_base ();


  /*!
   * \brief Set decimator rate.  \p rate must be EVEN and in [8, 256].
   *
   * The final complex sample rate across the USB is
   *   adc_freq () / decim_rate ()
   */
  bool set_decim_rate (unsigned int rate);
  bool set_nchannels (int nchan);
  bool set_mux (int mux);

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


  void set_verbose (bool verbose);

  // ACCESSORS

  long fpga_master_clock_freq() const;
  long converter_rate() const;  // A/D sample rate
  long adc_rate() const;        // alias
  long adc_freq() const;        // Deprecated name.  Use converter_rate() or adc_rate().

  unsigned int decim_rate () const;
  double rx_freq (int channel) const;
  int noverruns () const { return d_noverruns; }


  // PGA stuff
  /*!
   * \brief Set Programmable Gain Amplifier (PGA)
   *
   * \param which	which A/D [0,3]
   * \param gain_in_db	gain value (linear in dB)
   *
   * gain is rounded to closest setting supported by hardware.
   *
   * \returns true iff sucessful.
   *
   * \sa pga_min(), pga_max(), pga_db_per_step()
   */
  bool set_pga (int which, double gain_in_db);

  /*!
   * \brief Return programmable gain amplifier gain setting in dB.
   *
   * \param which	which A/D [0,3]
   */
  double pga (int which) const;

  /*!
   * \brief Return minimum legal PGA setting in dB.
   */
  double pga_min () const;

  /*!
   * \brief Return maximum legal PGA setting in dB.
   */
  double pga_max () const;

  /*!
   * \brief Return hardware step size of PGA (linear in dB).
   */
  double pga_db_per_step () const;

  /*!
   * \brief Return daughterboard ID for given Rx daughterboard slot [0,1].
   *
   * \return daughterboard id >= 0 if successful
   * \return -1 if no daugherboard
   * \return -2 if invalid EEPROM on daughterboard
   */
  int daughterboard_id (int which_dboard) const;

  /*!
   * \brief Set ADC offset correction
   * \param which	which ADC[0,3]: 0 = RX_A I, 1 = RX_A Q...
   * \param offset	16-bit value to subtract from raw ADC input.
   */
  bool set_adc_offset (int which, int offset);

  /*!
   * \brief Set DAC offset correction
   * \param which	which DAC[0,3]: 0 = TX_A I, 1 = TX_A Q...
   * \param offset	10-bit offset value (ambiguous format:  See AD9862 datasheet).
   * \param offset_pin	1-bit value.  If 0 offset applied to -ve differential pin;
   *                                  If 1 offset applied to +ve differential pin.
   */
  bool set_dac_offset (int which, int offset, int offset_pin);

  /*!
   * \brief Control ADC input buffer
   * \param which 	which ADC[0,3]
   * \param bypass	if non-zero, bypass input buffer and connect input
   *	                directly to switched cap SHA input of RxPGA.
   */
  bool set_adc_buffer_bypass (int which, bool bypass);

  /*!
   * \brief return the usrp's serial number.
   *
   * \returns non-zero length string iff successful.
   */
  std::string serial_number();

  /*!
   * \brief Write direction register (output enables) for pins that go to daughterboard.
   *
   * \param which_dboard	[0,1] which d'board
   * \param value		value to write into register
   * \param mask		which bits of value to write into reg
   *
   * Each d'board has 16-bits of general purpose i/o.
   * Setting the bit makes it an output from the FPGA to the d'board.
   *
   * This register is initialized based on a value stored in the
   * d'board EEPROM.  In general, you shouldn't be using this routine
   * without a very good reason.  Using this method incorrectly will
   * kill your USRP motherboard and/or daughterboard.
   */
  bool _write_oe (int which_dboard, int value, int mask);

  /*!
   * \brief Write daughterboard i/o pin value
   *
   * \param which_dboard	[0,1] which d'board
   * \param value		value to write into register
   * \param mask		which bits of value to write into reg
   */
  bool write_io (int which_dboard, int value, int mask);

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param which_dboard	[0,1] which d'board
   * \returns register value if successful, else READ_FAILED
   */
  int read_io (int which_dboard);

  /*!
   * \brief Enable/disable automatic DC offset removal control loop in FPGA
   *
   * \param bits  which control loops to enable
   * \param mask  which \p bits to pay attention to
   *
   * If the corresponding bit is set, enable the automatic DC
   * offset correction control loop.
   *
   * <pre>
   * The 4 low bits are significant:
   *
   *   ADC0 = (1 << 0)
   *   ADC1 = (1 << 1)
   *   ADC2 = (1 << 2)
   *   ADC3 = (1 << 3)
   * </pre>
   *
   * By default the control loop is enabled on all ADC's.
   */
  bool set_dc_offset_cl_enable(int bits, int mask);

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




  bool write_aux_dac (int which_dboard, int which_dac, int value);
  int read_aux_adc (int which_dboard, int which_adc);
  bool write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf);
  std::string read_eeprom (int i2c_addr, int eeprom_offset, int len);
  bool write_i2c (int i2c_addr, const std::string buf);
  std::string read_i2c (int i2c_addr, int len);
  bool _write_fpga_reg (int regno, int value);	//< 7-bit regno, 32-bit value
  bool _write_fpga_reg_masked (int regno, int value, int mask); //< 7-bit regno, 16-bit value, 16-bit mask
  int  _read_fpga_reg (int regno);
  bool _write_9862 (int which_codec, int regno, unsigned char value);
  int  _read_9862 (int which_codec, int regno) const;

  bool _write_spi (int optional_header, int enables, int format, std::string buf);

  /*
   * \brief Read data from SPI bus peripheral.
   *
   * \param optional_header	0,1 or 2 bytes to write before buf.
   * \param enables		bitmask of peripheral to read. See usrp_spi_defs.h
   * \param format		transaction format.  See usrp_spi_defs.h SPI_FMT_*
   * \param len			number of bytes to read.  Must be in [0,64].
   * \returns the data read if sucessful, else a zero length string.
   *
   * Reads are limited to a maximum of 64 bytes.
   *
   * If \p format specifies that optional_header bytes are present, they
   * are written to the peripheral first.  Then \p len bytes are read from
   * the peripheral and returned.
   */
  std::string _read_spi (int optional_header, int enables, int format, int len);
};


// ================================================================
//			concrete sinks
// ================================================================


GR_SWIG_BLOCK_MAGIC(usrp1,sink_c)

usrp1_sink_c_sptr
usrp1_make_sink_c (int which_board,
		   unsigned int interp_rate,
		   int nchan,
		   int mux,
		   int fusb_block_size,
		   int fusb_nblocks,
		   const std::string fpga_filename,
		   const std::string firmware_filename
		   ) throw (std::runtime_error);


class usrp1_sink_c : public usrp1_sink_base {
 protected:
  usrp1_sink_c (int which_board, unsigned int interp_rate,
		int nchan, int mux);

 public:
  ~usrp1_sink_c ();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(usrp1,sink_s)

usrp1_sink_s_sptr
usrp1_make_sink_s (int which_board,
		   unsigned int interp_rate,
		   int nchan,
		   int mux,
		   int fusb_block_size,
		   int fusb_nblocks,
		   const std::string fpga_filename,
		   const std::string firmware_filename
		   ) throw (std::runtime_error);


class usrp1_sink_s : public usrp1_sink_base {
 protected:
  usrp1_sink_s (int which_board, unsigned int interp_rate,
		int nchan, int mux);

 public:
  ~usrp1_sink_s ();
};

// ================================================================
//			concrete sources
// ================================================================

GR_SWIG_BLOCK_MAGIC(usrp1,source_c)


usrp1_source_c_sptr
usrp1_make_source_c (int which_board,
		     unsigned int decim_rate,
		     int nchan,
		     int mux,
		     int mode,
		     int fusb_block_size,
		     int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename
		     ) throw (std::runtime_error);

class usrp1_source_c : public usrp1_source_base {
 protected:
  usrp1_source_c (int which_board, unsigned int decim_rate,
		  int nchan, int mux, int mode);

 public:
  ~usrp1_source_c ();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(usrp1,source_s)

usrp1_source_s_sptr
usrp1_make_source_s (int which_board, 
		     unsigned int decim_rate,
		     int nchan,
		     int mux,
		     int mode,
		     int fusb_block_size,
		     int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename
		     ) throw (std::runtime_error);


class usrp1_source_s : public usrp1_source_base {
 protected:
  usrp1_source_s (int which_board, unsigned int decim_rate,
		  int nchan, int mux, int mode);

 public:
  ~usrp1_source_s ();
};

