/* -*- c++ -*- */
/*
 * Copyright 2003,2009 Free Software Foundation, Inc.
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
 * Internal usrp_prims header file
 */

#ifndef _USRP_PRIMSI_H_
#define _USRP_PRIMSI_H_

#include "usrp/usrp_prims.h"

/*
 * Internal functions
 */

libusb_device_handle *
usrp_open_interface(libusb_device *dev, int interface, int altinterface);

int write_cmd (libusb_device_handle *udh, int request, int value, int index,
               unsigned char *bytes, int len);

/*
 * Compatibility functions
 */

libusb_device *_get_usb_device (libusb_device_handle *udh);

libusb_device_descriptor _get_usb_device_descriptor (libusb_device *q);

int _get_usb_string_descriptor (libusb_device_handle *udh, int index,
                                unsigned char* data, int length);

int _usb_control_transfer (libusb_device_handle *udh, int request_type,
                           int request, int value, int index,
                           unsigned char *data, int length,
                           unsigned int timeout);

#endif /* _USRP_PRIMSI_H_ */

