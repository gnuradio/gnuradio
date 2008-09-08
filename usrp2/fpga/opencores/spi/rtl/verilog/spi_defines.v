//////////////////////////////////////////////////////////////////////
////                                                              ////
////  spi_define.v                                                ////
////                                                              ////
////  This file is part of the SPI IP core project                ////
////  http://www.opencores.org/projects/spi/                      ////
////                                                              ////
////  Author(s):                                                  ////
////      - Simon Srot (simons@opencores.org)                     ////
////                                                              ////
////  All additional information is avaliable in the Readme.txt   ////
////  file.                                                       ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2002 Authors                                   ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////

//
// Number of bits used for devider register. If used in system with
// low frequency of system clock this can be reduced.
// Use SPI_DIVIDER_LEN for fine tuning theexact number.
//
//`define SPI_DIVIDER_LEN_8
`define SPI_DIVIDER_LEN_16
//`define SPI_DIVIDER_LEN_24
//`define SPI_DIVIDER_LEN_32

`ifdef SPI_DIVIDER_LEN_8
  `define SPI_DIVIDER_LEN       8    // Can be set from 1 to 8
`endif                                                          
`ifdef SPI_DIVIDER_LEN_16                                       
  `define SPI_DIVIDER_LEN       16   // Can be set from 9 to 16
`endif                                                          
`ifdef SPI_DIVIDER_LEN_24                                       
  `define SPI_DIVIDER_LEN       24   // Can be set from 17 to 24
`endif                                                          
`ifdef SPI_DIVIDER_LEN_32                                       
  `define SPI_DIVIDER_LEN       32   // Can be set from 25 to 32 
`endif

//
// Maximum nuber of bits that can be send/received at once. 
// Use SPI_MAX_CHAR for fine tuning the exact number, when using
// SPI_MAX_CHAR_32, SPI_MAX_CHAR_24, SPI_MAX_CHAR_16, SPI_MAX_CHAR_8.
//
`define SPI_MAX_CHAR_128
//`define SPI_MAX_CHAR_64
//`define SPI_MAX_CHAR_32
//`define SPI_MAX_CHAR_24
//`define SPI_MAX_CHAR_16
//`define SPI_MAX_CHAR_8

`ifdef SPI_MAX_CHAR_128
  `define SPI_MAX_CHAR          128  // Can only be set to 128 
  `define SPI_CHAR_LEN_BITS     7
`endif
`ifdef SPI_MAX_CHAR_64
  `define SPI_MAX_CHAR          64   // Can only be set to 64 
  `define SPI_CHAR_LEN_BITS     6
`endif
`ifdef SPI_MAX_CHAR_32
  `define SPI_MAX_CHAR          32   // Can be set from 25 to 32 
  `define SPI_CHAR_LEN_BITS     5
`endif
`ifdef SPI_MAX_CHAR_24
  `define SPI_MAX_CHAR          24   // Can be set from 17 to 24 
  `define SPI_CHAR_LEN_BITS     5
`endif
`ifdef SPI_MAX_CHAR_16
  `define SPI_MAX_CHAR          16   // Can be set from 9 to 16 
  `define SPI_CHAR_LEN_BITS     4
`endif
`ifdef SPI_MAX_CHAR_8
  `define SPI_MAX_CHAR          8    // Can be set from 1 to 8 
  `define SPI_CHAR_LEN_BITS     3
`endif

//
// Number of device select signals. Use SPI_SS_NB for fine tuning the 
// exact number.
//
`define SPI_SS_NB_8
//`define SPI_SS_NB_16
//`define SPI_SS_NB_24
//`define SPI_SS_NB_32

`ifdef SPI_SS_NB_8
  `define SPI_SS_NB             8    // Can be set from 1 to 8
`endif
`ifdef SPI_SS_NB_16
  `define SPI_SS_NB             16   // Can be set from 9 to 16
`endif
`ifdef SPI_SS_NB_24
  `define SPI_SS_NB             24   // Can be set from 17 to 24
`endif
`ifdef SPI_SS_NB_32
  `define SPI_SS_NB             32   // Can be set from 25 to 32
`endif

//
// Bits of WISHBONE address used for partial decoding of SPI registers.
//
`define SPI_OFS_BITS	          4:2

//
// Register offset
//
`define SPI_RX_0                0
`define SPI_RX_1                1
`define SPI_RX_2                2
`define SPI_RX_3                3
`define SPI_TX_0                0
`define SPI_TX_1                1
`define SPI_TX_2                2
`define SPI_TX_3                3
`define SPI_CTRL                4
`define SPI_DEVIDE              5
`define SPI_SS                  6

//
// Number of bits in ctrl register
//
`define SPI_CTRL_BIT_NB         14

//
// Control register bit position
//
`define SPI_CTRL_ASS            13
`define SPI_CTRL_IE             12
`define SPI_CTRL_LSB            11
`define SPI_CTRL_TX_NEGEDGE     10
`define SPI_CTRL_RX_NEGEDGE     9
`define SPI_CTRL_GO             8
`define SPI_CTRL_RES_1          7
`define SPI_CTRL_CHAR_LEN       6:0

