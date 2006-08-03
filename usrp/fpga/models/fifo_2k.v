

module fifo_2k
  (  input [15:0] data,
     input 	wrreq,
     input 	rdreq,
     input 	rdclk,
     input 	wrclk,
     input 	aclr,
     output [15:0] q,
     output 	 rdfull,
     output 	 rdempty,
     output [10:0] rdusedw,
     output 	 wrfull,
     output 	 wrempty,
     output [10:0]  wrusedw
     );

fifo #(.width(16),.depth(2048),.addr_bits(11)) fifo_2k 
  ( data, wrreq, rdreq, rdclk, wrclk, aclr, q,
    rdfull, rdempty, rdusedw, wrfull, wrempty, wrusedw);
   
endmodule // fifo_1k
   
