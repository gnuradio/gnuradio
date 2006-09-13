// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003 Matt Ettus
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


// testbench for fullchip

module interp_tb();

`include "usrp_tasks.v"
   
	reg clk_120mhz;
	reg usbclk;
	reg reset;

	reg [11:0] adc1_data, adc2_data;
	wire [13:0] dac1_data, dac2_data;

	wire [5:0] usbctl;
	wire [5:0] usbrdy;

	wire [15:0] usbdata;

	reg WE, RD, OE;

        assign usbctl[0] = WE;
        assign usbctl[1] = RD;
        assign usbctl[2] = OE;
	assign usbctl[5:3] = 0;

	reg tb_oe;
	assign usbdata = tb_oe ? usbdatareg : 16'hxxxx;
	reg serload, serenable, serclk, serdata;
	reg enable_tx, enable_rx;
	reg [15:0] usbdatareg;

///////////////////////////////////////////////
//  Simulation Control
initial
begin
	$dumpfile("interp_tb.vcd");
	$dumpvars(0, fc_tb);
end

initial #100000 $finish;

///////////////////////////////////////////////
//  Monitors

reg [7:0] counter_interp;
wire [7:0] interp_rate;
assign interp_rate = 32;
initial $monitor(dac1_data);

       always @(posedge clk_120mhz)
        begin
                if(reset | ~enable_tx)
                        counter_interp <= #1 0;
                else if(counter_interp == 0)
                        counter_interp <= #1 interp_rate - 8'b1;
                else
                        counter_interp <= #1 counter_interp - 8'b1;
        end

///////////////////////////////////////////////
// Clock and reset

initial clk_120mhz = 0;
initial usbclk = 0;
always #48 clk_120mhz = ~clk_120mhz;
always #120 usbclk = ~usbclk;

initial reset = 1'b1;
initial #500 reset = 1'b0;


initial enable_tx = 1'b1;

	wire [31:0] interp_out, q_interp_out;
	wire [31:0] decim_out;
	wire [31:0] phase;
			
	cic_interp #(.bitwidth(32),.stages(4)) 
		interp_i(.clock(clk_120mhz),.reset(reset),.enable(enable_tx),
			.strobe(counter_interp == 8'b0),.signal_in(32'h1),.signal_out(interp_out));

	cic_decim #(.bitwidth(32),.stages(4)) 
		decim(.clock(clk_120mhz),.reset(reset),.enable(enable_tx),
			.strobe(counter_interp == 8'b0),.signal_in(32'h1),.signal_out(decim_out));
			
endmodule
