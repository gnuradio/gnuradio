/* -*- c++ -*- */
/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP1_SINK_BASE_H
#define INCLUDED_USRP1_SINK_BASE_H

#include <gr_sync_block.h>
#include <stdexcept>

class usrp_standard_tx;


/*!
 * \brief abstract interface to Universal Software Radio Peripheral Tx path (Rev 1)
 */
class usrp1_sink_base : public gr_sync_block {
 private:
  usrp_standard_tx	*d_usrp;
  int			 d_nunderruns;
  
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

  /*!
   * \brief convert between input item format and usrp native format
   *
   * \param input_items[in]		stream(s) of input items
   * \param input_index[in]		starting index in input_items
   * \param input_items_available[in]	number of items available starting at item[index]
   * \param input_items_consumed[out]  	number of input items consumed by copy
   * \param usrp_buffer[out]		destination buffer
   * \param usrp_buffer_length[in] 	\p usrp_buffer length in bytes
   * \param bytes_written[out]		number of bytes written into \p usrp_buffer
   */
  virtual void copy_to_usrp_buffer (gr_vector_const_void_star &input_items,
				    int  input_index,
				    int	 input_items_available,
				    int  &input_items_consumed,
				    void *usrp_buffer,
				    int  usrp_buffer_length,
				    int	 &bytes_written) = 0;

 public:
  //! magic value used on alternate register read interfaces
  static const int READ_FAILED = -99999;


  ~usrp1_sink_base ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  bool start();
  bool stop();

  /*!
   * \brief Set interpolator rate.  \p rate must be in [4, 1024] and a multiple of 4.
   *
   * The final complex sample rate across the USB is
   *   dac_freq () / interp_rate () * nchannels ()
   */
  bool set_interp_rate (unsigned int rate);
  bool set_nchannels (int nchan);
  bool set_mux (int mux);

  /*!
   * \brief set the frequency of the digital up converter.
   *
   * \p channel must be 0.  \p freq is the center frequency in Hz.
   * It must be in the range [-44M, 44M].  The frequency specified is
   * quantized.  Use tx_freq to retrieve the actual value used.
   */
  bool set_tx_freq (int channel, double freq);

  void set_verbose (bool verbose);

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


  // ACCESSORS

  long fpga_master_clock_freq() const;
  long converter_rate() const;
  long dac_rate() const { return converter_rate(); }	// alias
  long dac_freq() const { return converter_rate(); }	// deprecated alias

  unsigned int interp_rate () const;
  int nchannels () const;
  int mux () const;
  double tx_freq (int channel) const;
  int nunderruns () const { return d_nunderruns; }

  /*!
   * \brief Return daughterboard ID for given Rx daughterboard slot [0,1].
   *
   * \return daughterboard id >= 0 if successful
   * \return -1 if no daugherboard
   * \return -2 if invalid EEPROM on daughterboard
   */
  int daughterboard_id (int which_dboard) const;

  /*!
   * \brief Write auxiliary digital to analog converter.
   *
   * \param which_dboard	[0,1] which d'board
   *    			N.B., SLOT_TX_A and SLOT_RX_A share the same AUX DAC's.
   *          			SLOT_TX_B and SLOT_RX_B share the same AUX DAC's.
   * \param which_dac		[2,3] TX slots must use only 2 and 3.
   * \param value		[0,4095]
   * \returns true iff successful
   */
  bool write_aux_dac (int which_board, int which_dac, int value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param which_dboard	[0,1] which d'board
   * \param which_adc		[0,1]
   * \returns value in the range [0,4095] if successful, else READ_FAILED.
   */
  int read_aux_adc (int which_dboard, int which_adc);

  /*!
   * \brief Write EEPROM on motherboard or any daughterboard.
   * \param i2c_addr		I2C bus address of EEPROM
   * \param eeprom_offset	byte offset in EEPROM to begin writing
   * \param buf			the data to write
   * \returns true iff sucessful
   */
  bool write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf);

  /*!
   * \brief Write EEPROM on motherboard or any daughterboard.
   * \param i2c_addr		I2C bus address of EEPROM
   * \param eeprom_offset	byte offset in EEPROM to begin reading
   * \param len			number of bytes to read
   * \returns the data read if successful, else a zero length string.
   */
  std::string read_eeprom (int i2c_addr, int eeprom_offset, int len);

  /*!
   * \brief Write to I2C peripheral
   * \param i2c_addr		I2C bus address (7-bits)
   * \param buf			the data to write
   * \returns true iff successful
   * Writes are limited to a maximum of of 64 bytes.
   */
  bool write_i2c (int i2c_addr, const std::string buf);

  /*!
   * \brief Read from I2C peripheral
   * \param i2c_addr		I2C bus address (7-bits)
   * \param len			number of bytes to read
   * \returns the data read if successful, else a zero length string.
   * Reads are limited to a maximum of of 64 bytes.
   */
  std::string read_i2c (int i2c_addr, int len);

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

  //
  // internal routines...
  // You probably shouldn't be using these...
  //
  /*!
   * \brief Write FPGA register.
   * \param regno	7-bit register number
   * \param value	32-bit value
   * \returns true iff successful
   */
  bool _write_fpga_reg (int regno, int value);	//< 7-bit regno, 32-bit value

  /*!
   * \brief Read FPGA register.
   * \param regno	7-bit register number
   * \returns register value if successful, else READ_FAILED
   */
  int  _read_fpga_reg (int regno);

  /*!
   * \brief Write AD9862 register.
   * \param which_codec 0 or 1
   * \param regno	6-bit register number
   * \param value	8-bit value
   * \returns true iff successful
   */
  bool _write_9862 (int which_codec, int regno, unsigned char value);

  /*!
   * \brief Read AD9862 register.
   * \param which_codec 0 or 1
   * \param regno	6-bit register number
   * \returns register value if successful, else READ_FAILED
   */
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

#endif /* INCLUDED_USRP1_SINK_BASE_H */
