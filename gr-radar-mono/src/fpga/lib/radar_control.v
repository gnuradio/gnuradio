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

`include "../lib/radar_config.vh"

module radar_control(clk_i,rst_i,ena_i,saddr_i,sdata_i,s_strobe_i,
		     reset_o,tx_ena_o,rx_ena_o,ampl_o,freq_o);

   // System interface
   input         clk_i;		// Master clock @ 64 MHz
   input         rst_i;         // Master reset
   input         ena_i;         // Module level enable
   input  [6:0]  saddr_i;	// Configuration bus address
   input  [31:0] sdata_i;	// Configuration bus data
   input 	 s_strobe_i;    // Configuration bus write

   // Configuration outputs
   output 	 reset_o;
   output        tx_ena_o;
   output        rx_ena_o;
   
   output [15:0] ampl_o;
   output [31:0] freq_o;

   // Internal configuration
   wire 	 lp_ena;
   wire 	 dr_ena;
   wire 	 md_ena;
   wire [1:0] 	 chirps;

   // Configuration from host
   setting_reg #(`FR_RADAR_MODE) sr_mode(.clock(clk_i),.reset(rst_i),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
				       .out({chirps,md_ena,dr_ena,lp_ena,rx_ena_o,tx_ena_o,reset_o}));
   				     
   setting_reg #(`FR_RADAR_AMPL) sr_ampl(.clock(clk_i),.reset(rst_i),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
				       .out(ampl_o));

   setting_reg #(`FR_RADAR_FREQ1N) sr_freq(.clock(clk_i),.reset(rst_i),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					 .out(freq_o));
   
endmodule // radar_control
