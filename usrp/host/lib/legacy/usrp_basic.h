/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_USRP_BASIC_H
#define INCLUDED_USRP_BASIC_H

#include <usrp_slots.h>
#include <string>

struct usb_dev_handle;
class  fusb_devhandle;
class  fusb_ephandle;

/*!
 * \brief base class for usrp operations
 */
class usrp_basic
{
private:
  // NOT IMPLEMENTED
  usrp_basic (const usrp_basic &rhs);			// no copy constructor
  usrp_basic &operator= (const usrp_basic &rhs);	// no assignment operator

  
protected:
  struct usb_dev_handle	*d_udh;
  int			 d_usb_data_rate;	// bytes/sec
  int			 d_bytes_per_poll;	// how often to poll for overruns
  bool			 d_verbose;

  static const int	 MAX_REGS = 128;
  unsigned int		 d_fpga_shadows[MAX_REGS];

  usrp_basic (int which_board,
	      struct usb_dev_handle *open_interface (struct usb_device *dev),
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
  bool write_aux_dac (int slot, int which_dac, int value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param slot	2-bit slot number. E.g., SLOT_TX_A
   * \param which_adc	[0,1]
   * \param value	return 12-bit value [0,4095]
   * \returns true iff successful
   */
  bool read_aux_adc (int slot, int which_adc, int *value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param slot	2-bit slot number. E.g., SLOT_TX_A
   * \param which_adc	[0,1]
   * \returns value in the range [0,4095] if successful, else READ_FAILED.
   */
  int read_aux_adc (int slot, int which_adc);

public:
  virtual ~usrp_basic ();

  /*!
   * \brief return frequency of master oscillator on USRP
   */
  long  fpga_master_clock_freq () const { return 64000000; }

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

  // ----------------------------------------------------------------
  // Low level implementation routines.
  // You probably shouldn't be using these...
  //

  bool _set_led (int which, bool on);

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
  int			 d_dbid[2];		// Rx daughterboard ID's

  /*!
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512. 
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default. 
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
  int  dboard_to_slot (int dboard) { return (dboard << 1) | 1; }

public:
  ~usrp_basic_rx ();

  /*!
   * \brief invokes constructor, returns instance or 0 if trouble
   *
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512. 
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default. 
   */
  static usrp_basic_rx *make (int which_board,
			      int fusb_block_size=0,
			      int fusb_nblocks=0,
			      const std::string fpga_filename = "",
			      const std::string firmware_filename = ""
			      );

  // MANIPULATORS

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

  // ACCESSORS

  //! sampling rate of A/D converter
  virtual long converter_rate() const { return fpga_master_clock_freq(); } // 64M
  long adc_rate() const { return converter_rate(); }
  long adc_freq() const { return converter_rate(); }   //!< deprecated method name

  /*!
   * \brief Return daughterboard ID for given Rx daughterboard slot [0,1].
   *
   * \param which_dboard	[0,1] which Rx daughterboard
   *
   * \return daughterboard id >= 0 if successful
   * \return -1 if no daugherboard
   * \return -2 if invalid EEPROM on daughterboard
   */
  int daughterboard_id (int which_dboard) const { return d_dbid[which_dboard & 0x1]; }

  // ----------------------------------------------------------------
  // routines for controlling the Programmable Gain Amplifier
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
   * \brief Return minimum legal PGA gain in dB.
   */
  double pga_min () const { return 0.0; }

  /*!
   * \brief Return maximum legal PGA gain in dB.
   */
  double pga_max () const { return 20.0; }

  /*!
   * \brief Return hardware step size of PGA (linear in dB).
   */
  double pga_db_per_step () const { return 20.0 / 20; }

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
   * \param value		output
   */
  bool read_io (int which_dboard, int *value);

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param which_dboard	[0,1] which d'board
   * \returns register value if successful, else READ_FAILED
   */
  int read_io (int which_dboard);

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
   * \param value		return 12-bit value [0,4095]
   * \returns true iff successful
   */
  bool read_aux_adc (int which_dboard, int which_adc, int *value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param which_dboard	[0,1] which d'board
   * \param which_adc		[0,1]
   * \returns value in the range [0,4095] if successful, else READ_FAILED.
   */
  int read_aux_adc (int which_dboard, int which_adc);

  /*!
   * \brief returns current fusb block size
   */
  int block_size() const;

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

  // called in base class to derived class order
  bool start ();
  bool stop ();
};

/*!
 * \brief class for accessing the transmit side of the USRP
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
  int			 d_dbid[2];		// Tx daughterboard ID's

  /*!
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512.
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default.
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
  int  dboard_to_slot (int dboard) { return (dboard << 1) | 0; }

public:

  ~usrp_basic_tx ();

  /*!
   * \brief invokes constructor, returns instance or 0 if trouble
   *
   * \param which_board	     Which USRP board on usb (not particularly useful; use 0)
   * \param fusb_block_size  fast usb xfer block size.  Must be a multiple of 512. 
   *                         Use zero for a reasonable default.
   * \param fusb_nblocks     number of fast usb URBs to allocate.  Use zero for a reasonable default. 
   */
  static usrp_basic_tx *make (int which_board, int fusb_block_size=0, int fusb_nblocks=0,
			      const std::string fpga_filename = "",
			      const std::string firmware_filename = ""
			      );

  // MANIPULATORS

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

  // ACCESSORS

  //! sampling rate of D/A converter
  virtual long converter_rate() const { return fpga_master_clock_freq () * 2; } // 128M
  long dac_rate() const { return converter_rate(); }
  long dac_freq() const { return converter_rate(); }	//!< deprecated method name

  /*!
   * \brief Return daughterboard ID for given Tx daughterboard slot [0,1].
   *
   * \return daughterboard id >= 0 if successful
   * \return -1 if no daugherboard
   * \return -2 if invalid EEPROM on daughterboard
   */
  int daughterboard_id (int which_dboard) const { return d_dbid[which_dboard & 0x1]; }

  // ----------------------------------------------------------------
  // routines for controlling the Programmable Gain Amplifier
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
  double pga_min () const { return -20.0; }

  /*!
   * \brief Return maximum legal PGA gain in dB.
   */
  double pga_max () const { return 0.0; }

  /*!
   * \brief Return hardware step size of PGA (linear in dB).
   */
  double pga_db_per_step () const { return 20.0/255; }

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
   * \param value		return value
   */
  bool read_io (int which_dboard, int *value);

  /*!
   * \brief Read daughterboard i/o pin value
   *
   * \param which_dboard	[0,1] which d'board
   * \returns register value if successful, else READ_FAILED
   */
  int read_io (int which_dboard);

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
   * \param value		return 12-bit value [0,4095]
   * \returns true iff successful
   */
  bool read_aux_adc (int which_dboard, int which_adc, int *value);

  /*!
   * \brief Read auxiliary analog to digital converter.
   *
   * \param which_dboard	[0,1] which d'board
   * \param which_adc		[0,1]
   * \returns value in the range [0,4095] if successful, else READ_FAILED.
   */
  int read_aux_adc (int which_dboard, int which_adc);

  /*!
   * \brief returns current fusb block size
   */
  int block_size() const;

  // called in base class to derived class order
  bool start ();
  bool stop ();
};

#endif
