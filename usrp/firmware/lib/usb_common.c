/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

#include "usb_common.h"
#include "fx2regs.h"
#include "syncdelay.h"
#include "fx2utils.h"
#include "isr.h"
#include "usb_descriptors.h"
#include "usb_requests.h"

extern xdata char str0[];
extern xdata char str1[];
extern xdata char str2[];
extern xdata char str3[];
extern xdata char str4[];
extern xdata char str5[];


#define	bRequestType	SETUPDAT[0]
#define	bRequest	SETUPDAT[1]
#define	wValueL		SETUPDAT[2]
#define	wValueH		SETUPDAT[3]
#define	wIndexL		SETUPDAT[4]
#define	wIndexH		SETUPDAT[5]
#define	wLengthL	SETUPDAT[6]
#define	wLengthH	SETUPDAT[7]

#define MSB(x)	(((unsigned short) x) >> 8)
#define LSB(x)	(((unsigned short) x) & 0xff)

volatile bit _usb_got_SUDAV;

unsigned char	_usb_config = 0;
unsigned char	_usb_alt_setting = 0;	// FIXME really 1/interface

xdata unsigned char *current_device_descr;
xdata unsigned char *current_devqual_descr;
xdata unsigned char *current_config_descr;
xdata unsigned char *other_config_descr;

static void
setup_descriptors (void)
{
  if (USBCS & bmHSM){		// high speed mode
    current_device_descr  = high_speed_device_descr;
    current_devqual_descr = high_speed_devqual_descr;
    current_config_descr  = high_speed_config_descr;
    other_config_descr    = full_speed_config_descr;
  }
  else {
    current_device_descr  = full_speed_device_descr;
    current_devqual_descr = full_speed_devqual_descr;
    current_config_descr  = full_speed_config_descr;
    other_config_descr    = high_speed_config_descr;
  }

  // whack the type fields
  // FIXME, may not be required.
  // current_config_descr[1] = DT_CONFIG;
  // other_config_descr[1]   = DT_OTHER_SPEED;
}

static void
isr_SUDAV (void) interrupt
{
  clear_usb_irq ();
  _usb_got_SUDAV = 1;
}

static void
isr_USBRESET (void) interrupt
{
  clear_usb_irq ();
  setup_descriptors ();
}

static void
isr_HIGHSPEED (void) interrupt
{
  clear_usb_irq ();
  setup_descriptors ();
}

void
usb_install_handlers (void)
{
  setup_descriptors ();	    // ensure that they're set before use

  hook_uv (UV_SUDAV,     (unsigned short) isr_SUDAV);
  hook_uv (UV_USBRESET,  (unsigned short) isr_USBRESET);
  hook_uv (UV_HIGHSPEED, (unsigned short) isr_HIGHSPEED);

  USBIE = bmSUDAV | bmURES | bmHSGRANT;
}

// On the FX2 the only plausible endpoints are 0, 1, 2, 4, 6, 8
// This doesn't check to see that they're enabled

unsigned char
plausible_endpoint (unsigned char ep)
{
  ep &= ~0x80;	// ignore direction bit

  if (ep > 8)
    return 0;

  if (ep == 1)
    return 1;

  return (ep & 0x1) == 0;	// must be even
}

// return pointer to control and status register for endpoint.
// only called with plausible_endpoints

xdata volatile unsigned char *
epcs (unsigned char ep)
{
  if (ep == 0x01)		// ep1 has different in and out CS regs
    return EP1OUTCS;

  if (ep == 0x81)
    return EP1INCS;

  ep &= ~0x80;			// ignore direction bit

  if (ep == 0x00)		// ep0
    return EP0CS;

  return EP2CS + (ep >> 1);	// 2, 4, 6, 8 are consecutive
}

void
usb_handle_setup_packet (void)
{
  _usb_got_SUDAV = 0;

  // handle the standard requests...

  switch (bRequestType & bmRT_TYPE_MASK){

  case bmRT_TYPE_CLASS:
  case bmRT_TYPE_RESERVED:
    fx2_stall_ep0 ();		// we don't handle these.  indicate error
    break;
    
  case bmRT_TYPE_VENDOR:
    // call the application code.
    // If it handles the command it returns non-zero

    if (!app_vendor_cmd ())	
      fx2_stall_ep0 ();
    break;

  case bmRT_TYPE_STD:
    // these are the standard requests...

    if ((bRequestType & bmRT_DIR_MASK) == bmRT_DIR_IN){

      ////////////////////////////////////
      //    handle the IN requests
      ////////////////////////////////////

      switch (bRequest){

      case RQ_GET_CONFIG:
	EP0BUF[0] = _usb_config;	// FIXME app should handle
	EP0BCH = 0;
	EP0BCL = 1;
	break;
	
      // --------------------------------

      case RQ_GET_INTERFACE:
	EP0BUF[0] = _usb_alt_setting;	// FIXME app should handle
	EP0BCH = 0;
	EP0BCL = 1;
	break;

      // --------------------------------

      case RQ_GET_DESCR:
	switch (wValueH){

	case DT_DEVICE:
	  SUDPTRH = MSB (current_device_descr);
	  SUDPTRL = LSB (current_device_descr);
	  break;
	  
	case DT_DEVQUAL:
	  SUDPTRH = MSB (current_devqual_descr);
	  SUDPTRL = LSB (current_devqual_descr);
	  break;

	case DT_CONFIG:
	  if (0 && wValueL != 1)	// FIXME only a single configuration
	    fx2_stall_ep0 ();
	  else {
	    SUDPTRH = MSB (current_config_descr);
	    SUDPTRL = LSB (current_config_descr);
	  }
	  break;

	case DT_OTHER_SPEED:
	  if (0 && wValueL != 1)	// FIXME only a single configuration
	    fx2_stall_ep0 ();
	  else {
	    SUDPTRH = MSB (other_config_descr);
	    SUDPTRL = LSB (other_config_descr);
	  }
	  break;

	case DT_STRING:
	  if (wValueL >= nstring_descriptors)
	    fx2_stall_ep0 ();
	  else {
	    xdata char *p = string_descriptors[wValueL];
	    SUDPTRH = MSB (p);
	    SUDPTRL = LSB (p);
	  }
	  break;

	default:
	  fx2_stall_ep0 ();	// invalid request
	  break;
	}
	break;
	
      // --------------------------------

      case RQ_GET_STATUS:
	switch (bRequestType & bmRT_RECIP_MASK){
	case bmRT_RECIP_DEVICE:
	  EP0BUF[0] = bmGSDA_SELF_POWERED;	// FIXME app should handle
	  EP0BUF[1] = 0;
	  EP0BCH = 0;
	  EP0BCL = 2;
	  break;

	case bmRT_RECIP_INTERFACE:
	  EP0BUF[0] = 0;
	  EP0BUF[1] = 0;
	  EP0BCH = 0;
	  EP0BCL = 2;
	  break;

	case bmRT_RECIP_ENDPOINT:
	  if (plausible_endpoint (wIndexL)){
	    EP0BUF[0] = *epcs (wIndexL) & bmEPSTALL;
	    EP0BUF[1] = 0;
	    EP0BCH = 0;
	    EP0BCL = 2;
	  }
	  else
	    fx2_stall_ep0 ();
	  break;

	default:
	  fx2_stall_ep0 ();
	  break;
	}
	break;

      // --------------------------------

      case RQ_SYNCH_FRAME:	// not implemented
      default:
	fx2_stall_ep0 ();
	break;
      }
    }

    else {

      ////////////////////////////////////
      //    handle the OUT requests
      ////////////////////////////////////

      switch (bRequest){

      case RQ_SET_CONFIG:
	_usb_config = wValueL;		// FIXME app should handle
	break;

      case RQ_SET_INTERFACE:
	_usb_alt_setting = wValueL;	// FIXME app should handle
	break;

      // --------------------------------

      case RQ_CLEAR_FEATURE:
	switch (bRequestType & bmRT_RECIP_MASK){

	case bmRT_RECIP_DEVICE:
	  switch (wValueL){
	  case FS_DEV_REMOTE_WAKEUP:
	  default:
	    fx2_stall_ep0 ();
	  }
	  break;

	case bmRT_RECIP_ENDPOINT:
	  if (wValueL == FS_ENDPOINT_HALT && plausible_endpoint (wIndexL)){
	    *epcs (wIndexL) &= ~bmEPSTALL;
	    fx2_reset_data_toggle (wIndexL);
	  }
	  else
	    fx2_stall_ep0 ();
	  break;

	default:
	  fx2_stall_ep0 ();
	  break;
	}
	break;

      // --------------------------------

      case RQ_SET_FEATURE:
	switch (bRequestType & bmRT_RECIP_MASK){

	case bmRT_RECIP_DEVICE:
	  switch (wValueL){
	  case FS_TEST_MODE:
	    // hardware handles this after we complete SETUP phase handshake
	    break;

	  case FS_DEV_REMOTE_WAKEUP:
	  default:
	    fx2_stall_ep0 ();
	    break;
	  }
	}
	break;

      case bmRT_RECIP_ENDPOINT:
	switch (wValueL){
	case FS_ENDPOINT_HALT:
	  if (plausible_endpoint (wIndexL))
	    *epcs (wIndexL) |= bmEPSTALL;
	  else
	    fx2_stall_ep0 ();
	  break;

	default:
	  fx2_stall_ep0 ();
	  break;
	}
	break;

      // --------------------------------

      case RQ_SET_ADDRESS:	// handled by fx2 hardware
      case RQ_SET_DESCR:	// not implemented
      default:
	fx2_stall_ep0 ();
      }

    }
    break;

  }	// bmRT_TYPE_MASK

  // ack handshake phase of device request
  EP0CS |= bmHSNAK;
}
