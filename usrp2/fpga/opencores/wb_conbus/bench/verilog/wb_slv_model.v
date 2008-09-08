/////////////////////////////////////////////////////////////////////
////                                                             ////
////  WISHBONE Slave Model                                       ////
////                                                             ////
////                                                             ////
////  Author: Rudolf Usselmann                                   ////
////          rudi@asics.ws                                      ////
////                                                             ////
////                                                             ////
////  Downloaded from: http://www.opencores.org/cores/wb_conmax/ ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2000-2002 Rudolf Usselmann                    ////
////                         www.asics.ws                        ////
////                         rudi@asics.ws                       ////
////                                                             ////
//// This source file may be used and distributed without        ////
//// restriction provided that this copyright statement is not   ////
//// removed from the file and that any derivative work contains ////
//// the original copyright notice and the associated disclaimer.////
////                                                             ////
////     THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY     ////
//// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   ////
//// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS   ////
//// FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR      ////
//// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         ////
//// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    ////
//// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE   ////
//// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        ////
//// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  ////
//// LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT  ////
//// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  ////
//// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         ////
//// POSSIBILITY OF SUCH DAMAGE.                                 ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

//  CVS Log
//
//  $Id: wb_slv_model.v,v 1.1.1.1 2003/04/19 08:40:16 johny Exp $
//
//  $Date: 2003/04/19 08:40:16 $
//  $Revision: 1.1.1.1 $
//  $Author: johny $
//  $Locker:  $
//  $State: Exp $
//
// Change History:
//               $Log: wb_slv_model.v,v $
//               Revision 1.1.1.1  2003/04/19 08:40:16  johny
//               no message
//
//               Revision 1.2  2002/10/03 05:40:03  rudi
//               Fixed a minor bug in parameter passing, updated headers and specification.
//
//               Revision 1.1.1.1  2001/10/19 11:04:25  rudi
//               WISHBONE CONMAX IP Core
//
//               Revision 1.1  2001/07/29 08:57:02  rudi
//
//
//               1) Changed Directory Structure
//               2) Added restart signal (REST)
//
//               Revision 1.1.1.1  2001/03/19 13:11:29  rudi
//               Initial Release
//
//
//

`include "wb_model_defines.v"

module wb_slv(clk, rst, adr, din, dout, cyc, stb, sel, we, ack, err, rty);

input		clk, rst;
input	[31:0]	adr, din;
output	[31:0]	dout;
input		cyc, stb;
input	[3:0]	sel;
input		we;
output		ack, err, rty;

////////////////////////////////////////////////////////////////////
//
// Local Wires
//

parameter	mem_size = 13;
parameter	sz = (1<<mem_size)-1;

reg	[31:0]	mem[sz:0];
wire		mem_re, mem_we;
wire	[31:0]	tmp;
reg	[31:0]	dout, tmp2;

reg		err, rty;
reg	[31:0]	del_ack;
reg	[5:0]	delay;

////////////////////////////////////////////////////////////////////
//
// Memory Logic
//

initial
   begin
	delay = 0;
	err = 0;
	rty = 0;
	#2;
	$display("\nINFO: WISHBONE MEMORY MODEL INSTANTIATED (%m)");
	$display("      Memory Size %0d address lines %0d words\n",
	mem_size, sz+1);
   end

assign mem_re = cyc & stb & !we;
assign mem_we = cyc & stb &  we;

assign	tmp = mem[adr[mem_size+1:2]];

always @(sel or tmp or mem_re or ack)
	if(mem_re & ack)
	   begin
		dout[31:24] <= #1 sel[3] ? tmp[31:24] : 8'hxx;
		dout[23:16] <= #1 sel[2] ? tmp[23:16] : 8'hxx;
		dout[15:08] <= #1 sel[1] ? tmp[15:08] : 8'hxx;
		dout[07:00] <= #1 sel[0] ? tmp[07:00] : 8'hxx;
	   end
	else	dout <= #1 32'hzzzz_zzzz;


always @(sel or tmp or din)
   begin
	tmp2[31:24] = !sel[3] ? tmp[31:24] : din[31:24];
	tmp2[23:16] = !sel[2] ? tmp[23:16] : din[23:16];
	tmp2[15:08] = !sel[1] ? tmp[15:08] : din[15:08];
	tmp2[07:00] = !sel[0] ? tmp[07:00] : din[07:00];
   end

always @(posedge clk)
	if(mem_we)	mem[adr[mem_size+1:2]] <= #1 tmp2;

always @(posedge clk)
	del_ack = ack ? 0 : {del_ack[30:0], (mem_re | mem_we)};

assign	#1 ack = cyc & ((delay==0) ? (mem_re | mem_we) : del_ack[delay-1]);

task fill_mem;
input		mode;

integer		n, mode;

begin

for(n=0;n<(sz+1);n=n+1)
   begin
	case(mode)
	   0:	mem[n] = { ~n[15:0], n[15:0] };
	   1:	mem[n] = $random;
	endcase
   end

end
endtask

endmodule
