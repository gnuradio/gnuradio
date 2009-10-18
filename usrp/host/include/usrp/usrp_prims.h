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

#ifndef INCLUDED_USRP_PRIMS_H
#define INCLUDED_USRP_PRIMS_H

#include <usrp/usrp_slots.h>
#include <usrp/libusb_types.h>
#include <string>

struct libusb_context;

static const int USRP_HASH_SIZE = 16;

enum usrp_load_status_t { ULS_ERROR = 0, ULS_OK, ULS_ALREADY_LOADED };

/*!
 * \brief initialize libusb; Behavior differs depending on libusb version
 *
 * libusb-0.12
 *
 * Probe busses and devices. The argument is ignored and defaults to NULL.
 * Safe to call more than once.
 *
 * libusb-1.0
 *
 * If an location to a libusb_context is passed in, create and write in the new
 * context. If no argument is provided, initialize libusb with the default
 * (NULL) context.
 *
 * Generally _not_ safe to call more than once with non-NULL argument since a
 * new context will be created each time.
 */

void usrp_one_time_init (libusb_context **ctx = NULL);

/*!
 * \brief deinitialize libusb
 *
 * libusb-0.1: No effect
 *
 * libusb-1.0: Deinitialize context ctx 
 */
void usrp_deinit (libusb_context *ctx);

/*
 * force a rescan of the buses and devices
 */
void usrp_rescan ();

/*!
 * \brief locate Nth (zero based) USRP device in system.
 * Return pointer or 0 if not found.
 *
 * The following kinds of devices are considered USRPs:
 *
 *   unconfigured USRP (no firwmare loaded)
 *   configured USRP (firmware loaded)
 *   unconfigured Cypress FX2 (only if fx2_ok_p is true)
 */
libusb_device *usrp_find_device (int nth, bool fx2_ok_p = false, libusb_context *ctx = NULL);

bool usrp_usrp_p (libusb_device *q);		//< is this a USRP
bool usrp_usrp0_p (libusb_device *q);		//< is this a USRP Rev 0
bool usrp_usrp1_p (libusb_device *q);		//< is this a USRP Rev 1
bool usrp_usrp2_p (libusb_device *q);		//< is this a USRP Rev 2
int  usrp_hw_rev (libusb_device *q);		//< return h/w rev code

bool usrp_fx2_p (libusb_device *q);			//< is this an unconfigured Cypress FX2

bool usrp_unconfigured_usrp_p (libusb_device *q);	//< some kind of unconfigured USRP
bool usrp_configured_usrp_p (libusb_device *q);	//< some kind of configured USRP

/*!
 * \brief given a libusb_device return an instance of the appropriate libusb_device_handle
 *
 * These routines claim the specified interface and select the
 * correct alternate interface.  (USB nomenclature is totally screwed!)
 *
 * If interface can't be opened, or is already claimed by some other
 * process, 0 is returned.
 */
libusb_device_handle *usrp_open_cmd_interface (libusb_device *dev);
libusb_device_handle *usrp_open_rx_interface (libusb_device *dev);
libusb_device_handle *usrp_open_tx_interface (libusb_device *dev);

/*!
 * \brief close interface.
 */
bool usrp_close_interface (libusb_device_handle *udh);

/*!
 * \brief load intel hex format file into USRP/Cypress FX2 (8051).
 *
 * The filename extension is typically *.ihx
 *
 * Note that loading firmware may cause the device to renumerate.  I.e.,
 * change its configuration, invalidating the current device handle.
 */

usrp_load_status_t 
usrp_load_firmware (libusb_device_handle *udh, const char *filename, bool force);

/*!
 * \brief load intel hex format file into USRP FX2 (8051).
 *
 * The filename extension is typically *.ihx
 *
 * Note that loading firmware may cause the device to renumerate.  I.e.,
 * change its configuration, invalidating the current device handle.
 * If the result is ULS_OK, usrp_load_firmware_nth delays 1 second
 * then rescans the busses and devices.
 */
usrp_load_status_t
usrp_load_firmware_nth (int nth, const char *filename, bool force, libusb_context *ctx = NULL);

/*!
 * \brief load fpga configuration bitstream
 */
usrp_load_status_t
usrp_load_fpga (libusb_device_handle *udh, const char *filename, bool force);

/*!
 * \brief load the regular firmware and fpga bitstream in the Nth USRP.
 *
 * This is the normal starting point...
 */
bool usrp_load_standard_bits (int nth, bool force,
			      const std::string fpga_filename = "",
			      const std::string firmware_filename = "",
			      libusb_context *ctx = NULL);

/*!
 * \brief copy the given \p hash into the USRP hash slot \p which.
 * The usrp implements two hash slots, 0 and 1.
 */
bool usrp_set_hash (libusb_device_handle *udh, int which,
		    const unsigned char hash[USRP_HASH_SIZE]);

/*!
 * \brief retrieve the \p hash from the USRP hash slot \p which.
 * The usrp implements two hash slots, 0 and 1.
 */
bool usrp_get_hash (libusb_device_handle *udh, int which,
		    unsigned char hash[USRP_HASH_SIZE]);

bool usrp_write_fpga_reg (libusb_device_handle *udh, int reg, int value);
bool usrp_read_fpga_reg (libusb_device_handle *udh, int reg, int *value);
bool usrp_set_fpga_reset (libusb_device_handle *udh, bool on);
bool usrp_set_fpga_tx_enable (libusb_device_handle *udh, bool on);
bool usrp_set_fpga_rx_enable (libusb_device_handle *udh, bool on);
bool usrp_set_fpga_tx_reset (libusb_device_handle *udh, bool on);
bool usrp_set_fpga_rx_reset (libusb_device_handle *udh, bool on);
bool usrp_set_led (libusb_device_handle *udh, int which, bool on);

bool usrp_check_rx_overrun (libusb_device_handle *udh, bool *overrun_p);
bool usrp_check_tx_underrun (libusb_device_handle *udh, bool *underrun_p);

// i2c_read and i2c_write are limited to a maximum len of 64 bytes.

bool usrp_i2c_write (libusb_device_handle *udh, int i2c_addr,
		     const void *buf, int len);

bool usrp_i2c_read (libusb_device_handle *udh, int i2c_addr,
		    void *buf, int len);

// spi_read and spi_write are limited to a maximum of 64 bytes
// See usrp_spi_defs.h for more info

bool usrp_spi_write (libusb_device_handle *udh,
		     int optional_header, int enables, int format,
		     const void *buf, int len);

bool usrp_spi_read (libusb_device_handle *udh,
		     int optional_header, int enables, int format,
		     void *buf, int len);


bool usrp_9862_write (libusb_device_handle *udh,
		      int which_codec,			// [0,  1]
		      int regno,			// [0, 63]
		      int value);			// [0, 255]	

bool usrp_9862_read (libusb_device_handle *udh,
		     int which_codec,			// [0,  1]
		     int regno,				// [0, 63]
		     unsigned char *value);		// [0, 255]

/*!
 * \brief Write multiple 9862 regs at once.
 *
 * \p buf contains alternating register_number, register_value pairs.
 * \p len must be even and is the length of buf in bytes.
 */
bool usrp_9862_write_many (libusb_device_handle *udh, int which_codec,
			   const unsigned char *buf, int len);
			   

/*!
 * \brief write specified regs to all 9862's in the system
 */
bool usrp_9862_write_many_all (libusb_device_handle *udh,
			       const unsigned char *buf, int len);
			   

// Write 24LC024 / 24LC025 EEPROM on motherboard or daughterboard.
// Which EEPROM is determined by i2c_addr.  See i2c_addr.h

bool usrp_eeprom_write (libusb_device_handle *udh, int i2c_addr,
			int eeprom_offset, const void *buf, int len);


// Read 24LC024 / 24LC025 EEPROM on motherboard or daughterboard.
// Which EEPROM is determined by i2c_addr.  See i2c_addr.h

bool usrp_eeprom_read (libusb_device_handle *udh, int i2c_addr,
		       int eeprom_offset, void *buf, int len);


// Slot specific i/o routines

/*!
 * \brief write to the specified aux dac.
 *
 * \p slot: which Tx or Rx slot to write.
 *    N.B., SLOT_TX_A and SLOT_RX_A share the same AUX DAC's
 *          SLOT_TX_B and SLOT_RX_B share the same AUX DAC's
 *
 * \p which_dac: [0,3]  RX slots must use only 0 and 1.
 *			TX slots must use only 2 and 3.
 *
 * AUX DAC 3 is really the 9862 sigma delta output.
 *
 * \p value to write to aux dac.  All dacs take straight
 * binary values.  Although dacs 0, 1 and 2 are 8-bit and dac 3 is 12-bit,
 * the interface is in terms of 12-bit values [0,4095]
 */
bool usrp_write_aux_dac (libusb_device_handle *uhd, int slot,
			 int which_dac, int value);

/*!
 * \brief Read the specified aux adc
 *
 * \p slot: which Tx or Rx slot to read aux dac
 * \p which_adc: [0,1]  which of the two adcs to read
 * \p *value: return value, 12-bit straight binary.
 */
bool usrp_read_aux_adc (libusb_device_handle *udh, int slot,
			int which_adc, int *value);


/*!
 * \brief usrp daughterboard id to name mapping
 */
const std::string usrp_dbid_to_string (int dbid);


enum usrp_dbeeprom_status_t { UDBE_OK, UDBE_BAD_SLOT, UDBE_NO_EEPROM, UDBE_INVALID_EEPROM };

struct usrp_dboard_eeprom {
  unsigned short	id;		// d'board identifier code
  unsigned short	oe;		// fpga output enables:
					//   If bit set, i/o pin is an output from FPGA.
  short			offset[2];	// ADC/DAC offset correction
};

/*!
 * \brief Read and return parsed daughterboard eeprom
 */
usrp_dbeeprom_status_t
usrp_read_dboard_eeprom (libusb_device_handle *udh,
			 int slot_id, usrp_dboard_eeprom *eeprom);

/*!
 * \brief write ADC/DAC offset calibration constants to d'board eeprom
 */
bool usrp_write_dboard_offsets (libusb_device_handle *udh, int slot_id,
				short offset0, short offset1);

/*!
 * \brief return a usrp's serial number.
 *
 * Note that this only works on a configured usrp.
 * \returns non-zero length string iff successful.
 */
std::string usrp_serial_number(libusb_device_handle *udh);

#endif /* INCLUDED_USRP_PRIMS_H */
