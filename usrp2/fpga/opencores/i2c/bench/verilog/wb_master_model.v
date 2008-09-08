///////////////////////////////////////////////////////////////////////
////                                                               ////
////  WISHBONE rev.B2 Wishbone Master model                        ////
////                                                               ////
////                                                               ////
////  Author: Richard Herveille                                    ////
////          richard@asics.ws                                     ////
////          www.asics.ws                                         ////
////                                                               ////
////  Downloaded from: http://www.opencores.org/projects/mem_ctrl  ////
////                                                               ////
///////////////////////////////////////////////////////////////////////
////                                                               ////
//// Copyright (C) 2001 Richard Herveille                          ////
////                    richard@asics.ws                           ////
////                                                               ////
//// This source file may be used and distributed without          ////
//// restriction provided that this copyright statement is not     ////
//// removed from the file and that any derivative work contains   ////
//// the original copyright notice and the associated disclaimer.  ////
////                                                               ////
////     THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY       ////
//// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED     ////
//// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     ////
//// FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR        ////
//// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,           ////
//// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES      ////
//// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE     ////
//// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR          ////
//// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    ////
//// LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT    ////
//// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT    ////
//// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           ////
//// POSSIBILITY OF SUCH DAMAGE.                                   ////
////                                                               ////
///////////////////////////////////////////////////////////////////////

//  CVS Log
//
//  $Id: wb_master_model.v,v 1.4 2004/02/28 15:40:42 rherveille Exp $
//
//  $Date: 2004/02/28 15:40:42 $
//  $Revision: 1.4 $
//  $Author: rherveille $
//  $Locker:  $
//  $State: Exp $
//
// Change History:
//
`include "timescale.v"

module wb_master_model(clk, rst, adr, din, dout, cyc, stb, we, sel, ack, err, rty);

parameter dwidth = 32;
parameter awidth = 32;

input                  clk, rst;
output [awidth   -1:0]	adr;
input  [dwidth   -1:0]	din;
output [dwidth   -1:0]	dout;
output                 cyc, stb;
output       	        	we;
output [dwidth/8 -1:0] sel;
input		                ack, err, rty;

////////////////////////////////////////////////////////////////////
//
// Local Wires
//

reg	[awidth   -1:0]	adr;
reg	[dwidth   -1:0]	dout;
reg		               cyc, stb;
reg		               we;
reg [dwidth/8 -1:0] sel;

reg [dwidth   -1:0] q;

////////////////////////////////////////////////////////////////////
//
// Memory Logic
//

initial
	begin
		//adr = 32'hxxxx_xxxx;
		//adr = 0;
		adr  = {awidth{1'bx}};
		dout = {dwidth{1'bx}};
		cyc  = 1'b0;
		stb  = 1'bx;
		we   = 1'hx;
		sel  = {dwidth/8{1'bx}};
		#1;
		$display("\nINFO: WISHBONE MASTER MODEL INSTANTIATED (%m)\n");
	end

////////////////////////////////////////////////////////////////////
//
// Wishbone write cycle
//

task wb_write;
	input   delay;
	integer delay;

	input	[awidth -1:0]	a;
	input	[dwidth -1:0]	d;

	begin

		// wait initial delay
		repeat(delay) @(posedge clk);

		// assert wishbone signal
		#1;
		adr  = a;
		dout = d;
		cyc  = 1'b1;
		stb  = 1'b1;
		we   = 1'b1;
		sel  = {dwidth/8{1'b1}};
		@(posedge clk);

		// wait for acknowledge from slave
		while(~ack)	@(posedge clk);

		// negate wishbone signals
		#1;
		cyc  = 1'b0;
		stb  = 1'bx;
		adr  = {awidth{1'bx}};
		dout = {dwidth{1'bx}};
		we   = 1'hx;
		sel  = {dwidth/8{1'bx}};

	end
endtask

////////////////////////////////////////////////////////////////////
//
// Wishbone read cycle
//

task wb_read;
	input   delay;
	integer delay;

	input	 [awidth -1:0]	a;
	output	[dwidth -1:0]	d;

	begin

		// wait initial delay
		repeat(delay) @(posedge clk);

		// assert wishbone signals
		#1;
		adr  = a;
		dout = {dwidth{1'bx}};
		cyc  = 1'b1;
		stb  = 1'b1;
		we   = 1'b0;
		sel  = {dwidth/8{1'b1}};
		@(posedge clk);

		// wait for acknowledge from slave
		while(~ack)	@(posedge clk);

		// negate wishbone signals
		#1;
		cyc  = 1'b0;
		stb  = 1'bx;
		adr  = {awidth{1'bx}};
		dout = {dwidth{1'bx}};
		we   = 1'hx;
		sel  = {dwidth/8{1'bx}};
		d    = din;

	end
endtask

////////////////////////////////////////////////////////////////////
//
// Wishbone compare cycle (read data from location and compare with expected data)
//

task wb_cmp;
	input   delay;
	integer delay;

	input [awidth -1:0]	a;
	input	[dwidth -1:0]	d_exp;

	begin
		wb_read (delay, a, q);

		if (d_exp !== q)
			$display("Data compare error. Received %h, expected %h at time %t", q, d_exp, $time);
	end
endtask

endmodule


