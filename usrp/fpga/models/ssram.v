
// Model of Pipelined [ZBT] Synchronous SRAM

module ssram(clock,addr,data,wen,ce);
   parameter addrbits = 19;
   parameter depth = 524288;

   input clock;
   input [addrbits-1:0] addr;
   inout [35:0] data;
   input wen;
   input ce;

   reg [35:0] ram [0:depth-1];
   
   reg read_d1,read_d2;
   reg write_d1,write_d2;
   reg [addrbits-1:0] addr_d1,addr_d2;
   
   always @(posedge clock)
     begin
	read_d1 <= #1 ce & ~wen;
	write_d1 <= #1 ce & wen;
	addr_d1 <= #1 addr;
	read_d2 <= #1 read_d1;
	write_d2 <= #1 write_d1;
	addr_d2 <= #1 addr_d1;
	if(write_d2)
	  ram[addr_d2] = data;
     end // always @ (posedge clock)

   data = (ce & read_d2) ? ram[addr_d2] : 36'bz;

   always @(posedge clock)
     if(~ce & (write_d2 | write_d1 | wen))
       $display("$time ERROR:  RAM CE not asserted during write cycle");
   
endmodule // ssram
