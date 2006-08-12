/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

// Opcodes for commands to AVR
#define EZDOP_CMD_ROTATE  0x00    /* No operand */
#define EZDOP_CMD_STOP    0x01    /* No operand */
#define EZDOP_CMD_RATE    0x02    /* One byte operand */
#define EZDOP_CMD_STREAM  0x03    /* No operand */
#define EZDOP_CMD_STROFF  0x04    /* No operand */

#define EZDOP_DEFAULT_RATE 4

// TODO: Update with actual id's once established for product
// These are for FTDI unprogrammed parts
#define EZDOP_VENDORID  0x0403
#define EZDOP_PRODUCTID 0x6001

// These determine the FTDI bitbang settings
#define EZDOP_BAUDRATE  250000     // Fixed in EZ Doppler hardware design
#define EZDOP_BBDIR     0x16       // Bits 4=RESET 2=SCK 1=MOSI 0=MISO, MISO is input
