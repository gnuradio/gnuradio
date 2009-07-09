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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_USRP_BASE_H
#define INCLUDED_USRP_BASE_H

#include <gr_sync_block.h>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <usrp/db_base.h>
#include <usrp/usrp_subdev_spec.h>

class usrp_basic;

/*!
 * \brief base class for GNU Radio interface to the USRP
 */
class usrp_base : public gr_sync_block {
private:
  boost::shared_ptr<usrp_basic>	d_usrp_basic;

protected:
  usrp_base(const std::string &name,
	    gr_io_signature_sptr input_signature,
	    gr_io_signature_sptr output_signature)
    : gr_sync_block(name, input_signature, output_signature) {}
    

  void set_usrp_basic(boost::shared_ptr<usrp_basic> u);

public:
  virtual ~usrp_base();

  /* !
   * Return a vector of vectors of daughterboard instances associated with
   * the USRP source or sink.  The first dimension of the returned vector
   * corresponds to the side of the USRP, the second dimension, the subdevice
   * on the particular daughterboard.
   *
   * N.B. To ensure proper lifetime management, the caller should
   * continue to hold these as weak pointers, not shared pointers.  
   * As long as the caller does not attempt to directly use the weak
   * pointers after this usrp object has been destroyed, everything
   * will work out fine.
   */
   std::vector<std::vector<db_base_sptr> > db();

  /*!
   * Return a vector of size 1 or 2 that contains shared pointers
   * to the daughterboard instance(s) associated with the specified side.
   *
   * \param which_side	[0,1] which daughterboard
   *
   * N.B. To ensure proper lifetime management, the caller should
   * continue to hold these as weak pointers, not shared pointers.  
   * As long as the caller does not attempt to directly use the weak
   * pointers after this usrp object has been destroyed, everything
   * will work out fine.
   */
  std::vector<db_base_sptr> db(int which_side);

  /*!
   * Return the daughterboard instance corresponding to the selected
   * side of the USRP and selected daughterboard subdevice.
   * N.B. To ensure proper lifetime management, the caller should
   * continue to hold these as weak pointers, not shared pointers.  
   * As long as the caller does not attempt to directly use the weak
   * pointers after this usrp object has been destroyed, everything
   * will work out fine.
   */
  db_base_sptr db(int which_side, int which_dev);

  /*!
   * \brief given a usrp_subdev_spec, return the corresponding daughterboard object.
   * \throws std::invalid_argument if ss is invalid.
   *
   * \param ss specifies the side and subdevice
   */
  db_base_sptr selected_subdev(usrp_subdev_spec ss);

  /*!
   * \brief return frequency of master oscillator on USRP
   */
  long fpga_master_clock_freq() const;

  /*!
   * Tell API that the master oscillator on the USRP is operating at a non-standard 
   * fixed frequency. This is only needed for custom USRP hardware modified to 
   * operate at a different frequency from the default factory configuration. This
   * function must be called prior to any other API function.
   * \param master_clock USRP2 FPGA master clock frequency in Hz (10..64 MHz)
   */
  void set_fpga_master_clock_freq (long master_clock);

  void set_verbose (bool on);

  //! magic value used on alternate register read interfaces
  static const int READ_FAILED = -99999;

  /*!
   * \brief Write EEPROM on motherboard or any daughterboard.
   * \param i2c_addr		I2C bus address of EEPROM
   * \param eeprom_offset	byte offset in EEPROM to begin writing
   * \param buf			the data to write
   * \returns true iff sucessful
   */
  bool write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf);

  /*!
   * \brief Read EEPROM on motherboard or any daughterboard.
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
   * Reads are limited to a maximum of 64 bytes.
   */
  std::string read_i2c (int i2c_addr, int len);

  /*!
   * \brief Set ADC offset correction
   * \param which_adc	which ADC[0,3]: 0 = RX_A I, 1 = RX_A Q...
   * \param offset	16-bit value to subtract from raw ADC input.
   */
  bool set_adc_offset (int which_adc, int offset);

  /*!
   * \brief Set DAC offset correction
   * \param which_dac	which DAC[0,3]: 0 = TX_A I, 1 = TX_A Q...
   * \param offset	10-bit offset value (ambiguous format:  See AD9862 datasheet).
   * \param offset_pin	1-bit value.  If 0 offset applied to -ve differential pin;
   *                                  If 1 offset applied to +ve differential pin.
   */
  bool set_dac_offset (int which_dac, int offset, int offset_pin);

  /*!
   * \brief Control ADC input buffer
   * \param which_adc 	which ADC[0,3]
   * \param bypass	if non-zero, bypass input buffer and connect input
   *	                directly to switched cap SHA input of RxPGA.
   */
  bool set_adc_buffer_bypass (int which_adc, bool bypass);

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
   * \brief return the usrp's serial number.
   *
   * \returns non-zero length string iff successful.
   */
  std::string serial_number();

  /*!
   * \brief Return daughterboard ID for given side [0,1].
   *
   * \param which_side	[0,1] which daughterboard
   *
   * \return daughterboard id >= 0 if successful
   * \return -1 if no daugherboard
   * \return -2 if invalid EEPROM on daughterboard
   */
  virtual int daughterboard_id (int which_side) const;

  /*!
   * \brief Clock ticks to delay rising of T/R signal
   * \sa write_atr_mask, write_atr_txval, write_atr_rxval
   */
  bool write_atr_tx_delay(int value);

  /*!
   * \brief Clock ticks to delay falling edge of T/R signal
   * \sa write_atr_mask, write_atr_txval, write_atr_rxval
   */
  bool write_atr_rx_delay(int value);

  /*!
   * \brief Set Programmable Gain Amplifier (PGA)
   *
   * \param which_amp	which amp [0,3]
   * \param gain_in_db	gain value (linear in dB)
   *
   * gain is rounded to closest setting supported by hardware.
   *
   * \returns true iff sucessful.
   *
   * \sa pga_min(), pga_max(), pga_db_per_step()
   */
  bool set_pga (int which_amp, double gain_in_db);

  /*!
   * \brief Return programmable gain amplifier gain setting in dB.
   *
   * \param which_amp	which amp [0,3]
   */
  double pga (int which_amp) const;

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
   * \brief Write direction register (output enables) for pins that go to daughterboard.
   *
   * \param which_side	[0,1] which size
   * \param value	value to write into register
   * \param mask	which bits of value to write into reg
   *
   * Each d'board has 16-bits of general purpose i/o.
   * Setting the bit makes it an output from the FPGA to the d'board.
   *
   * This register is initialized based on a value stored in the
   * d'board EEPROM.  In general, you shouldn't be using this routine
   * without a very good reason.  Using this method incorrectly will
   * kill your USRP motherboard and/or daughterboard.
   */
  bool _write_oe (int which_side, int value, int mask);

  /*!
   * \brief Write daughterboard i/o pin value
   *
   * \param which_side	[0,1] which d'board
   * \param value	value to write into register
   * \param mask	which bits of value to write into reg
   */
  bool write_io (int which_side, int value, int mask);

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param which_side	[0,1] which d'board
   * \returns register value if successful, else READ_FAILED
   */
  int read_io (int which_side);

  /*!
   * \brief Write daughterboard refclk config register
   *
   * \param which_side	[0,1] which d'board
   * \param value	value to write into register, see below
   *
   * <pre>
   * Control whether a reference clock is sent to the daughterboards,
   * and what frequency.  The refclk is sent on d'board i/o pin 0.
   * 
   *     3                   2                   1                       
   *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   *  +-----------------------------------------------+-+------------+
   *  |             Reserved (Must be zero)           |E|   DIVISOR  |
   *  +-----------------------------------------------+-+------------+
   * 
   *  Bit 7  -- 1 turns on refclk, 0 allows IO use
   *  Bits 6:0 Divider value
   * </pre>
   */
  bool write_refclk(int which_side, int value);

  bool write_atr_mask(int which_side, int value);
  bool write_atr_txval(int which_side, int value);
  bool write_atr_rxval(int which_side, int value);

  /*!
   * \brief Write auxiliary digital to analog converter.
   *
   * \param which_side	[0,1] which d'board
   *    		N.B., SLOT_TX_A and SLOT_RX_A share the same AUX DAC's.
   *          		SLOT_TX_B and SLOT_RX_B share the same AUX DAC's.
   * \param which_dac	[2,3] TX slots must use only 2 and 3.
   * \param value	[0,4095]
   * \returns true iff successful
   */
  bool write_aux_dac (int which_side, int which_dac, int value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param which_side	[0,1] which d'board
   * \param which_adc	[0,1]
   * \returns value in the range [0,4095] if successful, else READ_FAILED.
   */
  int read_aux_adc (int which_side, int which_adc);

  /*!
   * \brief returns A/D or D/A converter rate in Hz
   */
  long converter_rate() const;


  // ----------------------------------------------------------------
  // Low level implementation routines.
  // You probably shouldn't be using these...
  //

  bool _set_led (int which_led, bool on);

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
   * \param value	32-bit value
   * \returns true iff successful
   */
  bool _read_fpga_reg (int regno, int *value);	//< 7-bit regno, 32-bit value

  /*!
   * \brief Read FPGA register.
   * \param regno	7-bit register number
   * \returns register value if successful, else READ_FAILED
   */
  int  _read_fpga_reg (int regno);

  /*!
   * \brief Write FPGA register with mask.
   * \param regno	7-bit register number
   * \param value	16-bit value
   * \param mask 	16-bit value
   * \returns true if successful
   * Only use this for registers who actually implement a mask in the verilog firmware, like FR_RX_MASTER_SLAVE
   */
  bool _write_fpga_reg_masked (int regno, int value, int mask);

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

  /*!
   * Return an existing daughterboard from list of candidate dbids, or the first found
   * on side A or side B.
   *
   * \param candidates          Vector of candidate dbids
   * 
   * Throws std::runtime_error if not found
   */
  usrp_subdev_spec pick_subdev(std::vector<int> candidates=std::vector<int>(0));
};

#endif /* INCLUDED_USRP_BASE_H */
