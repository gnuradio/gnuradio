// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2007 Corgan Enterprises LLC
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
//

`include "../../../../usrp/firmware/include/fpga_regs_common.v"
`include "../../../../usrp/firmware/include/fpga_regs_standard.v"

`define FR_RADAR_MODE           `FR_USER_0
`define bmFR_RADAR_MODE_RESET	32'b1 << 0
`define bmFR_RADAR_TXSIDE       32'b1 << 1
`define bmFR_RADAR_LOOP         32'b1 << 2
`define bmFR_RADAR_META         32'b1 << 3
`define bmFR_RADAR_DERAMP       32'b1 << 4
`define bmFR_RADAR_CHIRPS       32'b11 << 5
`define bmFR_RADAR_DEBUG        32'b1 << 7

`define FR_RADAR_TON            `FR_USER_1
`define FR_RADAR_TSW            `FR_USER_2
`define FR_RADAR_TLOOK          `FR_USER_3
`define FR_RADAR_TIDLE          `FR_USER_4
`define FR_RADAR_AMPL           `FR_USER_5
`define FR_RADAR_FSTART         `FR_USER_6
`define FR_RADAR_FINCR          `FR_USER_7
`define FR_RADAR_ATRDEL         `FR_USER_8
