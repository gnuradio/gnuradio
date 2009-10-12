/*  -*- c++ -*- */
/*
 * Copyright 2005,2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP_BASIC_H
#define INCLUDED_USRP_BASIC_H

#include <usrp/db_base.h>
#include <usrp/usrp_slots.h>
#include <usrp/usrp_subdev_spec.h>
#include <usrp/libusb_types.h>
#include <string>
#include <vector>
#include <boost/utility.hpp>

class  fusb_devhandle;
class  fusb_ephandle;

enum txrx_t {
  C_RX = 0,
  C_TX = 1
};

/*
 * ----------------------------------------------------------------------
 * Mid level interface to the Universal Software Radio Peripheral (Rev 1)
 *
 * These classes implement the basic functionality for talking to the
 * USRP.  They try to be as independent of the signal processing code
 * in FPGA as possible.  They implement access to the low level
 * peripherals on the board, provide a common way for reading and
 * writing registers in the FPGA, and provide the high speed interface
 * to streaming data across the USB.
 *
 * It is expected that subclasses will be derived that provide
 * access to the functionality to a particular FPGA configuration.
 * ----------------------------------------------------------------------
 */


/*!
 * \brief abstract base class for usrp operations
 * \ingroup usrp
 */
class usrp_basic : boost::noncopyable
{
protected:
  void shutdown_daughterboards();

protected:
  libusb_device_handle		*d_udh;
  struct libusb_context		*d_ctx;
  int				 d_usb_data_rate;	// bytes/sec
  int				 d_bytes_per_poll;	// how often to poll for overruns
  bool				 d_verbose;
  long   			 d_fpga_master_clock_freq;

  static const int		 MAX_REGS = 128;
  unsigned int			 d_fpga_shadows[MAX_REGS];

  int				 d_dbid[2];		// daughterboard ID's (side A, side B)

  /*!
   * Shared pointers to subclasses of db_base.
   *
   * The outer vector is of length 2 (0 = side A, 1 = side B).  The
   * inner vectors are of length 1, 2 or 3 depending on the number of
   * subdevices implemented by the daugherboard.  At this time, only
   * the Basic Rx and LF Rx implement more than 1 subdevice.
   */
  std::vector< std::vector<db_base_sptr> > d_db;

  //! One time call, made only only from usrp_standard_*::make after shared_ptr is created.
  void init_db(usrp_basic_sptr u);


  usrp_basic (int which_board,
	      libusb_device_handle *open_interface (libusb_device *dev),
	      const std::string fpga_filename = "",
	      const std::string firmware_filename = "");

  /*!
   * \brief advise usrp_basic of usb data rate (bytes/sec)
   *
   * N.B., this doesn't tweak any hardware.  Derived classes
   * should call this to inform us of the data rate whenever it's
   * first set or if it changes.
   *
   * \param usb_data_rate	bytes/sec
   */
  void set_usb_data_rate (int usb_data_rate);
  
  /*!
   * \brief Write auxiliary digital to analog converter.
   *
   * \param slot	Which Tx or Rx slot to write.
   *    		N.B., SLOT_TX_A and SLOT_RX_A share the same AUX DAC's.
   *          		SLOT_TX_B and SLOT_RX_B share the same AUX DAC's.
   * \param which_dac	[0,3] RX slots must use only 0 and 1.  TX slots must use only 2 and 3.
   * \param value	[0,4095]
   * \returns true iff successful
   */
  bool _write_aux_dac (int slot, int which_dac, int value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param slot	2-bit slot number. E.g., SLOT_TX_A
   * \param which_adc	[0,1]
   * \param value	return 12-bit value [0,4095]
   * \returns true iff successful
   */
  bool _read_aux_adc (int slot, int which_adc, int *value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param slot	2-bit slot number. E.g., SLOT_TX_A
   * \param which_adc	[0,1]
   * \returns value in the range [0,4095] if successful, else READ_FAILED.
   */
  int _read_aux_adc (int slot, int which_adc);


public:
  virtual ~usrp_basic ();


  /*!
   * Return a vector of vectors that contain shared pointers
   * to the daughterboard instance(s) associated with the specified side.
   *
   * It is an error to use the returned objects after the usrp_basic
   * object has been destroyed.
   */
  std::vector<std::vector<db_base_sptr> > db() const { return d_db; }

  /*!
   * Return a vector of size >= 1 that contains shared pointers
   * to the daughterboard instance(s) associated with the specified side.
   *
   * \param which_side	[0,1] which daughterboard
   *
   * It is an error to use the returned objects after the usrp_basic
   * object has been destroyed.
   */
  std::vector<db_base_sptr> db(int which_side);
 
  /*!
   * \brief is the subdev_spec valid?
   */
  bool is_valid(const usrp_subdev_spec &ss);

  /*!
   * \brief given a subdev_spec, return the corresponding daughterboard object.
   * \throws std::invalid_ argument if ss is invalid.
   *
   * \param ss specifies the side and subdevice
   */
  db_base_sptr selected_subdev(const usrp_subdev_spec &ss);

  /*!
   * \brief return frequency of master oscillator on USRP
   */
  long fpga_master_clock_freq () const { return d_fpga_master_clock_freq; }

  /*!
   * Tell API that the master oscillator on the USRP is operating at a non-standard 
   * fixed frequency. This is only needed for custom USRP hardware modified to 
   * operate at a different frequency from the default factory configuration. This
   * function must be called prior to any other API function.
   * \param master_clock USRP2 FPGA master clock frequency in Hz (10..64 MHz)
   */
  void set_fpga_master_clock_freq (long master_clock) { d_fpga_master_clock_freq = master_clock; }

  /*!
   * \returns usb data rate in bytes/sec
   */
  int usb_data_rate () const { return d_usb_data_rate; }

  void set_verbose (bool on) { d_verbose = on; }

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
  virtual int daughterboard_id (int which_side) const = 0;

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


  // ================================================================
  // Routines to access and control daughterboard specific i/o
  //
  // Those with a common_ prefix access either the Tx or Rx side depending
  // on the txrx parameter.  Those without the common_ prefix are virtual
  // and are overriden in usrp_basic_rx and usrp_basic_tx to access the
  // the Rx or Tx sides automatically.  We provide the common_ versions
  // for those daughterboards such as the WBX and XCVR2450 that share
  // h/w resources (such as the LO) between the Tx and Rx sides.

  // ----------------------------------------------------------------
  // BEGIN common_  daughterboard control functions

  /*!
   * \brief Set Programmable Gain Amplifier(PGA)
   *
   * \param txrx	Tx or Rx?
   * \param which_amp	which amp [0,3]
   * \param gain_in_db	gain value(linear in dB)
   *
   * gain is rounded to closest setting supported by hardware.
   *
   * \returns true iff sucessful.
   *
   * \sa pga_min(), pga_max(), pga_db_per_step()
   */
  bool common_set_pga(txrx_t txrx, int which_amp, double gain_in_db);

  /*!
   * \brief Return programmable gain amplifier gain setting in dB.
   *
   * \param txrx	Tx or Rx?
   * \param which_amp	which amp [0,3]
   */
  double common_pga(txrx_t txrx, int which_amp) const;

  /*!
   * \brief Return minimum legal PGA gain in dB.
   * \param txrx	Tx or Rx?
   */
  double common_pga_min(txrx_t txrx) const;

  /*!
   * \brief Return maximum legal PGA gain in dB.
   * \param txrx	Tx or Rx?
   */
  double common_pga_max(txrx_t txrx) const;

  /*!
   * \brief Return hardware step size of PGA(linear in dB).
   * \param txrx	Tx or Rx?
   */
  double common_pga_db_per_step(txrx_t txrx) const;

  /*!
   * \brief Write direction register(output enables) for pins that go to daughterboard.
   *
   * \param txrx	Tx or Rx?
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
  bool _common_write_oe(txrx_t txrx, int which_side, int value, int mask);

  /*!
   * \brief Write daughterboard i/o pin value
   *
   * \param txrx	Tx or Rx?
   * \param which_side	[0,1] which d'board
   * \param value	value to write into register
   * \param mask	which bits of value to write into reg
   */
  bool common_write_io(txrx_t txrx, int which_side, int value, int mask);

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param txrx	Tx or Rx?
   * \param which_side	[0,1] which d'board
   * \param value	output
   */
  bool common_read_io(txrx_t txrx, int which_side, int *value);

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param txrx	Tx or Rx?
   * \param which_side	[0,1] which d'board
   * \returns register value if successful, else READ_FAILED
   */
  int common_read_io(txrx_t txrx, int which_side);

  /*!
   * \brief Write daughterboard refclk config register
   *
   * \param txrx	Tx or Rx?
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
  bool common_write_refclk(txrx_t txrx, int which_side, int value);

  /*!
   * \brief Automatic Transmit/Receive switching
   * <pre>
   *
   * If automatic transmit/receive (ATR) switching is enabled in the
   * FR_ATR_CTL register, the presence or absence of data in the FPGA
   * transmit fifo selects between two sets of values for each of the 4
   * banks of daughterboard i/o pins.
   *
   * Each daughterboard slot has 3 16-bit registers associated with it:
   *   FR_ATR_MASK_*, FR_ATR_TXVAL_* and FR_ATR_RXVAL_*
   *
   * FR_ATR_MASK_{0,1,2,3}: 
   *
   *   These registers determine which of the daugherboard i/o pins are
   *   affected by ATR switching.  If a bit in the mask is set, the
   *   corresponding i/o bit is controlled by ATR, else it's output
   *   value comes from the normal i/o pin output register:
   *   FR_IO_{0,1,2,3}.
   *
   * FR_ATR_TXVAL_{0,1,2,3}:
   * FR_ATR_RXVAL_{0,1,2,3}:
   *
   *   If the Tx fifo contains data, then the bits from TXVAL that are
   *   selected by MASK are output.  Otherwise, the bits from RXVAL that
   *   are selected by MASK are output.
   * </pre>
   */
  bool common_write_atr_mask(txrx_t txrx, int which_side, int value);
  bool common_write_atr_txval(txrx_t txrx, int which_side, int value);
  bool common_write_atr_rxval(txrx_t txrx, int which_side, int value);

  /*!
   * \brief Write auxiliary digital to analog converter.
   *
   * \param txrx	Tx or Rx?
   * \param which_side	[0,1] which d'board
   *    		N.B., SLOT_TX_A and SLOT_RX_A share the same AUX DAC's.
   *          		SLOT_TX_B and SLOT_RX_B share the same AUX DAC's.
   * \param which_dac	[2,3] TX slots must use only 2 and 3.
   * \param value	[0,4095]
   * \returns true iff successful
   */
  bool common_write_aux_dac(txrx_t txrx, int which_side, int which_dac, int value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param txrx	Tx or Rx?
   * \param which_side	[0,1] which d'board
   * \param which_adc	[0,1]
   * \param value	return 12-bit value [0,4095]
   * \returns true iff successful
   */
  bool common_read_aux_adc(txrx_t txrx, int which_side, int which_adc, int *value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param txrx	Tx or Rx?
   * \param which_side	[0,1] which d'board
   * \param which_adc	[0,1]
   * \returns value in the range [0,4095] if successful, else READ_FAILED.
   */
  int common_read_aux_adc(txrx_t txrx, int which_side, int which_adc);

  // END common_ daughterboard control functions
  // ----------------------------------------------------------------
  // BEGIN virtual daughterboard control functions

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
  virtual bool set_pga (int which_amp, double gain_in_db) = 0;

  /*!
   * \brief Return programmable gain amplifier gain setting in dB.
   *
   * \param which_amp	which amp [0,3]
   */
  virtual double pga (int which_amp) const = 0;

  /*!
   * \brief Return minimum legal PGA gain in dB.
   */
  virtual double pga_min () const = 0;

  /*!
   * \brief Return maximum legal PGA gain in dB.
   */
  virtual double pga_max () const = 0;

  /*!
   * \brief Return hardware step size of PGA (linear in dB).
   */
  virtual double pga_db_per_step () const = 0;

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
  virtual bool _write_oe (int which_side, int value, int mask) = 0;

  /*!
   * \brief Write daughterboard i/o pin value
   *
   * \param which_side	[0,1] which d'board
   * \param value	value to write into register
   * \param mask	which bits of value to write into reg
   */
  virtual bool write_io (int which_side, int value, int mask) = 0;

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param which_side	[0,1] which d'board
   * \param value	output
   */
  virtual bool read_io (int which_side, int *value) = 0;

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param which_side	[0,1] which d'board
   * \returns register value if successful, else READ_FAILED
   */
  virtual int read_io (int which_side) = 0;

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
  virtual bool write_refclk(int which_side, int value) = 0;

  virtual bool write_atr_mask(int which_side, int value) = 0;
  virtual bool write_atr_txval(int which_side, int value) = 0;
  virtual bool write_atr_rxval(int which_side, int value) = 0;

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
  virtual bool write_aux_dac (int which_side, int which_dac, int value) = 0;

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param which_side	[0,1] which d'board
   * \param which_adc	[0,1]
   * \param value	return 12-bit value [0,4095]
   * \returns true iff successful
   */
  virtual bool read_aux_adc (int which_side, int which_adc, int *value) = 0;

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param which_side	[0,1] which d'board
   * \param which_adc	[0,1]
   * \returns value in the range [0,4095] if successful, else READ_FAILED.
   */
  virtual int read_aux_adc (int which_side, int which_adc) = 0;

  /*!
   * \brief returns current fusb block size
   */
  virtual int block_size() const = 0;

  /*!
   * \brief returns A/D or D/A converter rate in Hz
   */
  virtual long converter_rate() const = 0;

  // END virtual daughterboard control functions

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
   * \param value	8-bit value
   * \returns true iff successful
   */
  bool _read_9862 (int which_codec, int regno, unsigned char *value) const;

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
   * \brief Start data transfers.
   * Called in base class to derived class order.
   */
  bool start ();

  /*!
   * \brief Stop data transfers.
   * Called in base class to derived class order.
   */
  bool stop ();
};

/*!
 * \brief class for accessing the receive side of the USRP
 * \ingroup usrp
 */
class usrp_basic_rx : public usrp_basic 
{
private:
  fusb_devhandle	*d_devhandle;
  fusb_ephandle		*d_ephandle;
  int			 d_bytes_seen;		// how many bytes we've seen
  bool			 d_first_read;
  bool			 d_rx_enable;

protected:
  /*!
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512. 
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default. 
   * \param fpga_filename    name of the rbf file to load
   * \param firmware_filename name of ihx file to load
   */
  usrp_basic_rx (int which_board,
		 int fusb_block_size=0,
		 int fusb_nblocks=0,
		 const std::string fpga_filename = "",
		 const std::string firmware_filename = ""
		 );  // throws if trouble

  bool set_rx_enable (bool on);
  bool rx_enable () const { return d_rx_enable; }

  bool disable_rx ();		// conditional disable, return prev state
  void restore_rx (bool on);	// conditional set

  void probe_rx_slots (bool verbose);

public:
  ~usrp_basic_rx ();

  /*!
   * \brief invokes constructor, returns instance or 0 if trouble
   *
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512. 
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default. 
   * \param fpga_filename    name of file that contains image to load into FPGA
   * \param firmware_filename	name of file that contains image to load into FX2
   */
  static usrp_basic_rx *make (int which_board,
			      int fusb_block_size=0,
			      int fusb_nblocks=0,
			      const std::string fpga_filename = "",
			      const std::string firmware_filename = ""
			      );

  /*!
   * \brief tell the fpga the rate rx samples are coming from the A/D's
   *
   * div = fpga_master_clock_freq () / sample_rate
   *
   * sample_rate is determined by a myriad of registers
   * in the 9862.  That's why you have to tell us, so
   * we can tell the fpga.
   */
  bool set_fpga_rx_sample_rate_divisor (unsigned int div);

  /*!
   * \brief read data from the D/A's via the FPGA.
   * \p len must be a multiple of 512 bytes.
   *
   * \returns the number of bytes read, or -1 on error.
   *
   * If overrun is non-NULL it will be set true iff an RX overrun is detected.
   */
  int read (void *buf, int len, bool *overrun);


  //! sampling rate of A/D converter
  virtual long converter_rate() const { return fpga_master_clock_freq(); } // 64M
  long adc_rate() const { return converter_rate(); }
  int daughterboard_id (int which_side) const { return d_dbid[which_side & 0x1]; }

  bool set_pga (int which_amp, double gain_in_db);
  double pga (int which_amp) const;
  double pga_min () const;
  double pga_max () const;
  double pga_db_per_step () const;

  bool _write_oe (int which_side, int value, int mask);
  bool write_io (int which_side, int value, int mask);
  bool read_io (int which_side, int *value);
  int read_io (int which_side);
  bool write_refclk(int which_side, int value);
  bool write_atr_mask(int which_side, int value);
  bool write_atr_txval(int which_side, int value);
  bool write_atr_rxval(int which_side, int value);

  bool write_aux_dac (int which_side, int which_dac, int value);
  bool read_aux_adc (int which_side, int which_adc, int *value);
  int  read_aux_adc (int which_side, int which_adc);

  int block_size() const;

  // called in base class to derived class order
  bool start ();
  bool stop ();
};

/*!
 * \brief class for accessing the transmit side of the USRP
 * \ingroup usrp
 */
class usrp_basic_tx : public usrp_basic 
{
private:
  fusb_devhandle	*d_devhandle;
  fusb_ephandle		*d_ephandle;
  int			 d_bytes_seen;		// how many bytes we've seen
  bool			 d_first_write;
  bool			 d_tx_enable;

 protected:
  /*!
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512.
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default.
   * \param fpga_filename    name of file that contains image to load into FPGA
   * \param firmware_filename	name of file that contains image to load into FX2
   */
  usrp_basic_tx (int which_board,
		 int fusb_block_size=0,
		 int fusb_nblocks=0,
		 const std::string fpga_filename = "",
		 const std::string firmware_filename = ""
		 );		// throws if trouble

  bool set_tx_enable (bool on);
  bool tx_enable () const { return d_tx_enable; }

  bool disable_tx ();		// conditional disable, return prev state
  void restore_tx (bool on);	// conditional set

  void probe_tx_slots (bool verbose);

public:

  ~usrp_basic_tx ();

  /*!
   * \brief invokes constructor, returns instance or 0 if trouble
   *
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512. 
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default. 
   * \param fpga_filename    name of file that contains image to load into FPGA
   * \param firmware_filename	name of file that contains image to load into FX2
   */
  static usrp_basic_tx *make (int which_board, int fusb_block_size=0, int fusb_nblocks=0,
			      const std::string fpga_filename = "",
			      const std::string firmware_filename = ""
			      );

  /*!
   * \brief tell the fpga the rate tx samples are going to the D/A's
   *
   * div = fpga_master_clock_freq () * 2
   *
   * sample_rate is determined by a myriad of registers
   * in the 9862.  That's why you have to tell us, so
   * we can tell the fpga.
   */
  bool set_fpga_tx_sample_rate_divisor (unsigned int div);

  /*!
   * \brief Write data to the A/D's via the FPGA.
   *
   * \p len must be a multiple of 512 bytes.
   * \returns number of bytes written or -1 on error.
   *
   * if \p underrun is non-NULL, it will be set to true iff
   * a transmit underrun condition is detected.
   */
  int write (const void *buf, int len, bool *underrun);

  /*
   * Block until all outstanding writes have completed.
   * This is typically used to assist with benchmarking
   */
  void wait_for_completion ();

  //! sampling rate of D/A converter
  virtual long converter_rate() const { return fpga_master_clock_freq () * 2; } // 128M
  long dac_rate() const { return converter_rate(); }
  int daughterboard_id (int which_side) const { return d_dbid[which_side & 0x1]; }

  bool set_pga (int which_amp, double gain_in_db);
  double pga (int which_amp) const;
  double pga_min () const;
  double pga_max () const;
  double pga_db_per_step () const;

  bool _write_oe (int which_side, int value, int mask);
  bool write_io (int which_side, int value, int mask);
  bool read_io (int which_side, int *value);
  int read_io (int which_side);
  bool write_refclk(int which_side, int value);
  bool write_atr_mask(int which_side, int value);
  bool write_atr_txval(int which_side, int value);
  bool write_atr_rxval(int which_side, int value);

  bool write_aux_dac (int which_side, int which_dac, int value);
  bool read_aux_adc (int which_side, int which_adc, int *value);
  int read_aux_adc (int which_side, int which_adc);

  int block_size() const;

  // called in base class to derived class order
  bool start ();
  bool stop ();
};

#endif /* INCLUDED_USRP_BASIC_H */
