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

#include "usrp_common.h"
#include "usrp_commands.h"
#include "spi.h"

/*
 * the host side fpga loader code pushes an MD5 hash of the bitstream
 * into hash1.
 */
#define	  USRP_HASH_SIZE      16
xdata at USRP_HASH_SLOT_0_ADDR unsigned char hash0[USRP_HASH_SIZE];


#define enable_codecs() USRP_PA &= ~(bmPA_SEN_CODEC_A | bmPA_SEN_CODEC_B)
#define disable_all()	USRP_PA |=  (bmPA_SEN_CODEC_A | bmPA_SEN_CODEC_B)

static void
write_byte_msb (unsigned char v);

void
write_both_9862s (unsigned char header_lo, unsigned char v)
{
  enable_codecs ();

  write_byte_msb (header_lo);
  write_byte_msb (v);

  disable_all ();
}

// ----------------------------------------------------------------

static void
write_byte_msb (unsigned char v)
{
  unsigned char n = 8;
  do {
    v = (v << 1) | (v >> 7);	// rotate left (MSB into bottom bit)
    bitS_OUT = v & 0x1;
    bitS_CLK = 1;
    bitS_CLK = 0;
  } while (--n != 0);
}

// ----------------------------------------------------------------

#define REG_RX_PWR_DN		 1
#define	REG_TX_PWR_DN		 8
#define	REG_TX_MODULATOR	20

void eeprom_init (void)
{
  unsigned short counter;
  unsigned char	 i;

  // configure IO ports (B and D are used by GPIF)

  IOA = bmPORT_A_INITIAL;	// Port A initial state
  OEA = bmPORT_A_OUTPUTS;	// Port A direction register

  IOC = bmPORT_C_INITIAL;	// Port C initial state
  OEC = bmPORT_C_OUTPUTS;	// Port C direction register

  IOE = bmPORT_E_INITIAL;	// Port E initial state
  OEE = bmPORT_E_OUTPUTS;	// Port E direction register

  EP0BCH = 0;			SYNCDELAY;

  // USBCS &= ~bmRENUM;		// chip firmware handles commands
  USBCS = 0;			// chip firmware handles commands

  USRP_PC &= ~bmPC_nRESET;	// active low reset
  USRP_PC |=  bmPC_nRESET;

  // init_spi ();
  bitS_OUT = 0;			/* idle state has CLK = 0 */

  write_both_9862s (REG_RX_PWR_DN,    0x01);
  write_both_9862s (REG_TX_PWR_DN,    0x0f);	// pwr dn digital and analog_both
  write_both_9862s (REG_TX_MODULATOR, 0x00);	// coarse & fine modulators disabled

  // zero firmware hash slot
  i = 0;
  do {
    hash0[i] = 0;
    i++;
  } while (i != USRP_HASH_SIZE);

  counter = 0;
  while (1){
    counter++;
    if (counter & 0x8000)
      IOC ^= bmPC_LED0;
  }
}
