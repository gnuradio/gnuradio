/////////////////////////////////////////////////////////////////////
////                                                             ////
////  WISHBONE Master Model                                      ////
////                                                             ////
////                                                             ////
////  Author: Rudolf Usselmann                                   ////
////          rudi@asics.ws                                      ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2001 Rudolf Usselmann                         ////
////                    rudi@asics.ws                            ////
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
//  $Id: wb_mast.v,v 1.1 2001/12/03 21:44:23 gorban Exp $
//
//  $Date: 2001/12/03 21:44:23 $
//  $Revision: 1.1 $
//  $Author: gorban $
//  $Locker:  $
//  $State: Exp $
//
// Change History:
//               $Log: wb_mast.v,v $
//               Revision 1.1  2001/12/03 21:44:23  gorban
//               Updated specification documentation.
//               Added full 32-bit data bus interface, now as default.
//               Address is 5-bit wide in 32-bit data bus mode.
//               Added wb_sel_i input to the core. It's used in the 32-bit mode.
//               Added debug interface with two 32-bit read-only registers in 32-bit mode.
//               Bits 5 and 6 of LSR are now only cleared on TX FIFO write.
//               My small test bench is modified to work with 32-bit mode.
//
//
//
//
//
//                        

/*

task mem_fill;

- Fills local burst read (rd_buf[]) and write(wr_buf[]) buffers with random values.


task wb_wr1( 32 bit address, 4 bit byte select, 32 bit write data);

- Performs a single WISHBONE write


task wb_wr4( 32 bit address, 4 bit byte select, integer delay,
	32 bit data 1, 32 bit data 2, 32 bit data 3, 32 bit data 4);

- Performs 4 consecutive WISHBONE writes
- Strobe is deasserted between writes for 'delay' number of cycles
  (This simulates wait state insertion ...)


task wb_wr_mult( 32 bit address, 4 bit byte select, integer delay,
	integer count);

- Simular to wb_wr4, except it pwrforms "count" number of write cycles.
  The data is taken from the internal wr_bub[] memory.
- Strobe is deasserted between writes for 'delay' number of cycles
  (This simulates wait state insertion ...)


task wb_rmw( 32 bit address, 4 bit byte select, integer delay,
	integer rcount, integer wcount);

- This task performs "rcount" read cycles, followed by wcount write cycles.
- read data is placed in to the internal rd_buf[] memory, write data is
  taken from the internal wr_buf[] memory.
- Strobe is deasserted between writes for 'delay' number of cycles
  (This simulates wait state insertion ...)


task wb_rd1( 32 bit address, 4 bit byte select, 32 bit read data);

- Performs a single WISHBONE write


task wb_rd4( 32 bit address, 4 bit byte select, integer delay,
	32 bit data 1, 32 bit data 2, 32 bit data 3, 32 bit data 4);

- Performs 4 consecutive WISHBONE reads
- Strobe is deasserted between reads for 'delay' number of cycles
  (This simulates wait state insertion ...)


task wb_rd_mult( 32 bit address, 4 bit byte select, integer delay,
	integer count);

- Simular to wb_rd4, except it pwrforms "count" number of read cycles.
  The data is read in to the internal rd_buf[] memory.
- Strobe is deasserted between reads for 'delay' number of cycles
  (This simulates wait state insertion ...)


*/


//`include "wb_model_defines.v"

module wb_mast(clk, rst, adr, din, dout, cyc, stb, sel, we, ack, err, rty);

input		clk, rst;
output	[31:0]	adr;
input	[31:0]	din;
output	[31:0]	dout;
output		cyc, stb;
output	[3:0]	sel;
output		we;
input		ack, err, rty;

////////////////////////////////////////////////////////////////////
//
// Local Wires
//

parameter mem_size = 4096;

reg	[31:0]	adr;
reg	[31:0]	dout;
reg		cyc, stb;
reg	[3:0]	sel;
reg		we;

reg	[31:0]	rd_mem[mem_size:0];
reg	[31:0]	wr_mem[mem_size:0];
integer		rd_cnt;
integer		wr_cnt;

////////////////////////////////////////////////////////////////////
//
// Memory Logic
//

initial
   begin
	adr = 32'hxxxx_xxxx;
	dout = 32'hxxxx_xxxx;
	cyc = 0;
	stb = 0;
	sel = 4'hx;
	we = 1'hx;
	rd_cnt = 0;
	wr_cnt = 0;
	#1;
	$display("\nINFO: WISHBONE MASTER MODEL INSTANTIATED (%m)\n");
   end



task mem_fill;

integer n;
begin
rd_cnt = 0;
wr_cnt = 0;
for(n=0;n<mem_size;n=n+1)
   begin
	rd_mem[n] = $random;
	wr_mem[n] = $random;
   end
end
endtask

////////////////////////////////////////////////////////////////////
//
// Write 1 Word Task
//

task wb_wr1;
input	[31:0]	a;
input	[3:0]	s;
input	[31:0]	d;

begin

@(posedge clk);
#1;
adr = a;
dout = d;
cyc = 1;
stb = 1;
we=1;
sel = s;

@(posedge clk);
while(~ack & ~err)	@(posedge clk);
#1;
cyc=0;
stb=0;
adr = 32'hxxxx_xxxx;
dout = 32'hxxxx_xxxx;
we = 1'hx;
sel = 4'hx;

end
endtask

////////////////////////////////////////////////////////////////////
//
// Write 4 Words Task
//

task wb_wr4;
input	[31:0]	a;
input	[3:0]	s;
input		delay;
input	[31:0]	d1;
input	[31:0]	d2;
input	[31:0]	d3;
input	[31:0]	d4;

integer		delay;

begin

@(posedge clk);
#1;
cyc = 1;
sel = s;

repeat(delay)
   begin
	@(posedge clk);
	#1;
   end
adr = a;
dout = d1;
stb = 1;
we=1;
while(~ack & ~err)	@(posedge clk);
#2;
stb=0;
we=1'bx;
dout = 32'hxxxx_xxxx;
adr = 32'hxxxx_xxxx;

repeat(delay)
   begin
	@(posedge clk);
	#1;
   end
stb=1;
adr = a+4;
dout = d2;
we=1;
@(posedge clk);
while(~ack & ~err)	@(posedge clk);
#2;
stb=0;
we=1'bx;
dout = 32'hxxxx_xxxx;
adr = 32'hxxxx_xxxx;

repeat(delay)
   begin
	@(posedge clk);
	#1;
   end
stb=1;
adr = a+8;
dout = d3;
we=1;
@(posedge clk);
while(~ack & ~err)	@(posedge clk);
#2;
stb=0;
we=1'bx;
dout = 32'hxxxx_xxxx;
adr = 32'hxxxx_xxxx;

repeat(delay)
   begin
	@(posedge clk);
	#1;
   end
stb=1;
adr = a+12;
dout = d4;
we=1;
@(posedge clk);
while(~ack & ~err)	@(posedge clk);
#1;
stb=0;
cyc=0;

adr = 32'hxxxx_xxxx;
dout = 32'hxxxx_xxxx;
we = 1'hx;
sel = 4'hx;

end
endtask


task wb_wr_mult;
input	[31:0]	a;
input	[3:0]	s;
input		delay;
input		count;

integer		delay;
integer		count;
integer		n;

begin

@(posedge clk);
#1;
cyc = 1;

for(n=0;n<count;n=n+1)
   begin
	repeat(delay)
	   begin
		@(posedge clk);
		#1;
	   end
	adr = a + (n*4);
	dout = wr_mem[n + wr_cnt];
	stb = 1;
	we=1;
	sel = s;
	if(n!=0)	@(posedge clk);
	while(~ack & ~err)	@(posedge clk);
	#2;
	stb=0;
	we=1'bx;
	sel = 4'hx;
	dout = 32'hxxxx_xxxx;
	adr = 32'hxxxx_xxxx;
   end

cyc=0;
adr = 32'hxxxx_xxxx;

wr_cnt = wr_cnt + count;
end
endtask


task wb_rmw;
input	[31:0]	a;
input	[3:0]	s;
input		delay;
input		rcount;
input		wcount;

integer		delay;
integer		rcount;
integer		wcount;
integer		n;

begin

@(posedge clk);
#1;
cyc = 1;
we = 0;
sel = s;
repeat(delay)	@(posedge clk);

for(n=0;n<rcount-1;n=n+1)
   begin
	adr = a + (n*4);
	stb = 1;
	while(~ack & ~err)	@(posedge clk);
	rd_mem[n + rd_cnt] = din;
	//$display("Rd Mem[%0d]: %h", (n + rd_cnt), rd_mem[n + rd_cnt] );
	#2;
	stb=0;
	we = 1'hx;
	sel = 4'hx;
	adr = 32'hxxxx_xxxx;
	repeat(delay)
	   begin
		@(posedge clk);
		#1;
	   end
	we = 0;
	sel = s;
   end

adr = a+(n*4);
stb = 1;
@(posedge clk);
while(~ack & ~err)	@(posedge clk);
rd_mem[n + rd_cnt] = din;
//$display("Rd Mem[%0d]: %h", (n + rd_cnt), rd_mem[n + rd_cnt] );
#1;
stb=0;
we = 1'hx;
sel = 4'hx;
adr = 32'hxxxx_xxxx;

rd_cnt = rd_cnt + rcount;

for(n=0;n<wcount;n=n+1)
   begin
	repeat(delay)
	   begin
		@(posedge clk);
		#1;
	   end
	adr = a + (n*4);
	dout = wr_mem[n + wr_cnt];
	stb = 1;
	we=1;
	sel = s;
//	if(n!=0)
		@(posedge clk);
	while(~ack & ~err)	@(posedge clk);
	#2;
	stb=0;
	we=1'bx;
	sel = 4'hx;
	dout = 32'hxxxx_xxxx;
	adr = 32'hxxxx_xxxx;
   end

cyc=0;
adr = 32'hxxxx_xxxx;

wr_cnt = wr_cnt + wcount;
end
endtask



////////////////////////////////////////////////////////////////////
//
// Read 1 Word Task
//

task wb_rd1;
input	[31:0]	a;
input	[3:0]	s;
output	[31:0]	d;

begin

@(posedge clk);
#1;
adr = a;
cyc = 1;
stb = 1;
we  = 0;
sel = s;

while(~ack & ~err)	@(posedge clk);
d = din;
#1;
cyc=0;
stb=0;
adr = 32'hxxxx_xxxx;
dout = 32'hxxxx_xxxx;
we = 1'hx;
sel = 4'hx;

end
endtask


////////////////////////////////////////////////////////////////////
//
// Read 4 Words Task
//


task wb_rd4;
input	[31:0]	a;
input	[3:0]	s;
input		delay;
output	[31:0]	d1;
output	[31:0]	d2;
output	[31:0]	d3;
output	[31:0]	d4;

integer		delay;
begin

@(posedge clk);
#1;
cyc = 1;
we = 0;
sel = s;
repeat(delay)	@(posedge clk);

adr = a;
stb = 1;
while(~ack & ~err)	@(posedge clk);
d1 = din;
#2;
stb=0;
we = 1'hx;
sel = 4'hx;
adr = 32'hxxxx_xxxx;
repeat(delay)
   begin
	@(posedge clk);
	#1;
   end
we = 0;
sel = s;

adr = a+4;
stb = 1;
@(posedge clk);
while(~ack & ~err)	@(posedge clk);
d2 = din;
#2;
stb=0;
we = 1'hx;
sel = 4'hx;
adr = 32'hxxxx_xxxx;
repeat(delay)
   begin
	@(posedge clk);
	#1;
   end
we = 0;
sel = s;


adr = a+8;
stb = 1;
@(posedge clk);
while(~ack & ~err)	@(posedge clk);
d3 = din;
#2;
stb=0;
we = 1'hx;
sel = 4'hx;
adr = 32'hxxxx_xxxx;
repeat(delay)
   begin
	@(posedge clk);
	#1;
   end
we = 0;
sel = s;

adr = a+12;
stb = 1;
@(posedge clk);
while(~ack & ~err)	@(posedge clk);
d4 = din;
#1;
stb=0;
cyc=0;
we = 1'hx;
sel = 4'hx;
adr = 32'hxxxx_xxxx;
end
endtask


task wb_rd_mult;
input	[31:0]	a;
input	[3:0]	s;
input		delay;
input		count;

integer		delay;
integer		count;
integer		n;

begin

@(posedge clk);
#1;
cyc = 1;
we = 0;
sel = s;
repeat(delay)	@(posedge clk);

for(n=0;n<count-1;n=n+1)
   begin
	adr = a + (n*4);
	stb = 1;
	while(~ack & ~err)	@(posedge clk);
	rd_mem[n + rd_cnt] = din;
	#2;
	stb=0;
	we = 1'hx;
	sel = 4'hx;
	adr = 32'hxxxx_xxxx;
	repeat(delay)
	   begin
		@(posedge clk);
		#1;
	   end
	we = 0;
	sel = s;
   end

adr = a+(n*4);
stb = 1;
@(posedge clk);
while(~ack & ~err)	@(posedge clk);
rd_mem[n + rd_cnt] = din;
#1;
stb=0;
cyc=0;
we = 1'hx;
sel = 4'hx;
adr = 32'hxxxx_xxxx;

rd_cnt = rd_cnt + count;
end
endtask

endmodule
