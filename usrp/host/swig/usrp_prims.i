/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2009 Free Software Foundation, Inc.
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
 * Low level primitives for directly messing with USRP hardware.
 *
 * If you're trying to use the USRP, you'll probably want to take a
 * look at the usrp_standard_rx and usrp_standard_tx classes.  They
 * hide a bunch of low level details and provide high performance
 * streaming i/o.
 *
 * This interface is built on top of libusb, which allegedly works under
 * Linux, *BSD and Mac OS/X.  http://libusb.sourceforge.net
 */

%include <stl.i>	// pick up string stuff


%{
#include <usrp/usrp_prims.h>
%}


enum usrp_load_status_t { ULS_ERROR = 0, ULS_OK, ULS_ALREADY_LOADED };


/*!
 * \brief initialize libusb; probe busses and devices.
 * Safe to call more than once.
 */
void usrp_one_time_init (libusb_context **ctx = NULL);

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
bool usrp_load_standard_bits (int nth, bool force);


%include <fpga_regs_common.h>
%include <fpga_regs_standard.h>


bool usrp_write_fpga_reg (libusb_device_handle *udh, int reg, int value);

%inline %{

int 
usrp_read_fpga_reg (libusb_device_handle *udh, int reg)
{
  int value;
  bool ok = usrp_read_fpga_reg (udh, reg, &value);
  if (ok)
    return value;
  else
    return -999;
}

%}

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
		     void *buf, int len);

bool usrp_i2c_read (libusb_device_handle *udh, int i2c_addr,
		    void *buf, int len);

// spi_read and spi_write are limited to a maximum of 64 bytes
// See usrp_spi_defs.h for more info

bool usrp_spi_write (libusb_device_handle *udh,
		     int optional_header, int enables, int format,
		     unsigned char *buf, int len);

bool usrp_spi_read (libusb_device_handle *udh,
		     int optional_header, int enables, int format,
		     unsigned char *buf, int len);


bool usrp_9862_write (libusb_device_handle *udh,
		      int which_codec,			// [0,  1]
		      int regno,			// [0, 63]
		      int value);			// [0, 255]	

%inline %{

int 
usrp_9862_read (libusb_device_handle *udh, int which_codec, int reg)
{
  unsigned char value;
  bool ok = usrp_9862_read (udh, which_codec, reg, &value);
  if (ok)
    return value;
  else
    return -999;
}

%}

%inline %{

bool 
usrp_eeprom_write (libusb_device_handle *udh, int i2c_addr,
		   int eeprom_offset, const std::string buf)
{
  return usrp_eeprom_write (udh, i2c_addr, eeprom_offset,
			    buf.data (), buf.size ());
}
  
std::string
usrp_eeprom_read (libusb_device_handle *udh, int i2c_addr,
		  int eeprom_offset, int len)
{
  if (len <= 0)
    return "";
  
  char buf[len];

  if (!usrp_eeprom_read (udh, i2c_addr, eeprom_offset, buf, len))
    return "";

  return std::string (buf, len);
}

%}

bool usrp_write_aux_dac (libusb_device_handle *uhd, int slot,
			 int which_dac, int value);

%inline %{

int usrp_read_aux_adc (libusb_device_handle *udh, int slot, int which_adc)
{
  int value;
  bool ok = usrp_read_aux_adc (udh, slot, which_adc, &value);
  if (ok)
    return value;
  else
    return -999;
}

%}

/*!
 * \brief return a usrp's serial number.
 *
 * Note that this only works on a configured usrp.
 * \returns non-zero length string iff successful.
 */
std::string usrp_serial_number(libusb_device_handle *udh);

/*!
 * \brief usrp daughterboard id to name mapping
 */
const std::string usrp_dbid_to_string (int dbid);

%inline %{
#include "../../firmware/include/fpga_regs_common.h"
#include "../../firmware/include/fpga_regs_standard.h"
%}
