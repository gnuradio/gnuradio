

module fifo_4k_18
  (input  [17:0] data,
   input         wrreq,
   input         wrclk,
   output 	 wrfull,
   output 	 wrempty,
   output [11:0] wrusedw,

   output [17:0] q,
   input         rdreq,
   input         rdclk,
   output 	 rdfull,
   output 	 rdempty,
   output [11:0] rdusedw,

   input 	 aclr );

fifo #(.width(18),.depth(4096),.addr_bits(12)) fifo_4k 
  ( data, wrreq, rdreq, rdclk, wrclk, aclr, q,
    rdfull, rdempty, rdusedw, wrfull, wrempty, wrusedw);
   
endmodule // fifo_4k_18

   
