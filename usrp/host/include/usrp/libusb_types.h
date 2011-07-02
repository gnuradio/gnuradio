/*  -*- Mode: C++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#ifndef _LIBUSB_TYPES_H_
#define _LIBUSB_TYPES_H_

#if 0
#include <libusb-1.0/libusb.h>
struct libusb_device;
struct libusb_device_handle;
struct libusb_device_descriptor;
#else
#include <usb.h>
typedef struct usb_device libusb_device;
typedef struct usb_dev_handle libusb_device_handle;
typedef struct usb_device_descriptor libusb_device_descriptor;
#endif

#endif /* _LIBUSB_TYPES_H_ */
