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

module radar_control(clk_i,saddr_i,sdata_i,s_strobe_i,reset_o,
		     tx_side_o,dbg_o,tx_strobe_o,tx_ctrl_o,rx_ctrl_o,
		     ampl_o,fstart_o,fincr_o,pulse_num_o,io_tx_ena_o);
   
   // System interface
   input         clk_i;		// Master clock @ 64 MHz
   input  [6:0]  saddr_i;	// Configuration bus address
   input  [31:0] sdata_i;	// Configuration bus data
   input 	 s_strobe_i;    // Configuration bus write

   // Control and configuration outputs
   output 	 reset_o;
   output        tx_side_o;
   output        dbg_o;
   output        tx_strobe_o;
   output        tx_ctrl_o;
   output        rx_ctrl_o;
   output [15:0] ampl_o;
   output [31:0] fstart_o;
   output [31:0] fincr_o;
   output [15:0] pulse_num_o;
   output        io_tx_ena_o;
   
   // Internal configuration
   wire 	 lp_ena;
   wire 	 md_ena;
   wire 	 dr_ena;
   wire   [1:0]  chirps;
   wire   [15:0] t_on;
   wire   [15:0] t_sw;
   wire   [15:0] t_look;
   wire   [31:0] t_idle;
   wire   [31:0] atrdel;
   
   // Configuration from host
   wire [31:0] 	 mode;
   setting_reg #(`FR_RADAR_MODE)   sr_mode(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					   .out(mode));
   assign reset_o   = mode[0];
   assign tx_side_o = mode[1];
   assign lp_ena    = mode[2];
   assign md_ena    = mode[3];
   assign dr_ena    = mode[4];
   assign chirps    = mode[6:5];
   assign dbg_o     = mode[7];
   
   setting_reg #(`FR_RADAR_TON)    sr_ton(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					  .out(t_on));
   
   setting_reg #(`FR_RADAR_TSW)    sr_tsw(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					  .out(t_sw));
   				     
   setting_reg #(`FR_RADAR_TLOOK)  sr_tlook(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					    .out(t_look));
   				     
   setting_reg #(`FR_RADAR_TIDLE)  sr_tidle(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					    .out(t_idle));
   				     
   setting_reg #(`FR_RADAR_AMPL)   sr_ampl(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					   .out(ampl_o));

   setting_reg #(`FR_RADAR_FSTART) sr_fstart(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					     .out(fstart_o));

   setting_reg #(`FR_RADAR_FINCR)  sr_fincr(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					    .out(fincr_o));

   setting_reg #(`FR_RADAR_ATRDEL)  sr_atrdel(.clock(clk_i),.reset(1'b0),.strobe(s_strobe_i),.addr(saddr_i),.in(sdata_i),
					    .out(atrdel));
   
   // Pulse state machine
   `define ST_ON   4'b0001
   `define ST_SW   4'b0010
   `define ST_LOOK 4'b0100
   `define ST_IDLE 4'b1000

   reg [3:0]  state;
   reg [31:0] count;
   reg [15:0] pulse_num_o;
   
   always @(posedge clk_i)
     if (reset_o)
       begin
	  state <= `ST_ON;
	  count <= 32'b0;
	  pulse_num_o <= 16'b0;
       end
     else
       case (state)
	 `ST_ON:
	   if (count == {16'b0,t_on})
	     begin
	        state <= `ST_SW;
	        count <= 32'b0;
		pulse_num_o <= pulse_num_o + 16'b1;
	     end
	   else
	     count <= count + 32'b1;
	 
	 `ST_SW:
	   if (count == {16'b0,t_sw})
	     begin
		state <= `ST_LOOK;
		count <= 32'b0;
	     end
	   else
	     count <= count + 32'b1;

	 `ST_LOOK:
	   if (count == {16'b0,t_look})
	     begin
		state <= `ST_IDLE;
		count <= 32'b0;
	     end
	   else
	     count <= count + 32'b1;

	 `ST_IDLE:
	   if (count == t_idle)
	     begin
		state <= `ST_ON;
		count <= 32'b0;
	     end
	   else
	     count <= count + 32'b1;

	 default:		  // Invalid state, reset state machine
	   begin
	      state <= `ST_ON;
	      count <= 32'b0;
	   end
       endcase
   
   assign tx_strobe_o = count[0]; // Drive DAC inputs at 32 MHz
   assign tx_ctrl_o = (state == `ST_ON);
   assign rx_ctrl_o = (state == `ST_LOOK);

   // Create delayed version of tx_ctrl_o to drive mixers and TX/RX switch
   atr_delay atr_delay(.clk_i(clk_i),.rst_i(reset_o),.ena_i(1'b1),.tx_empty_i(!tx_ctrl_o),
		       .tx_delay_i(atrdel[27:16]),.rx_delay_i(atrdel[11:0]),
		       .atr_tx_o(io_tx_ena_o));
   
endmodule // radar_control
