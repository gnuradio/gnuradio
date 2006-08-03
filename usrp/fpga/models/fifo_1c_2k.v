// Model of FIFO in Altera

module fifo_1c_2k ( data, wrreq, rdreq, rdclk, wrclk, aclr, q,
		    rdfull, rdempty, rdusedw, wrfull, wrempty, wrusedw);

   parameter width = 32;
   parameter depth = 2048;
   //`define rd_req 0;  // Set this to 0 for rd_ack, 1 for rd_req
      
   input [31:0] data;
   input 	wrreq;
   input 	rdreq;
   input 	rdclk;
   input 	wrclk;
   input 	aclr;
   output [31:0] q;
   output 	 rdfull;
   output 	 rdempty;
   output [10:0]  rdusedw;
   output 	 wrfull;
   output 	 wrempty;
   output [10:0]  wrusedw;
   
   reg [width-1:0] mem [0:depth-1];
   reg [7:0] 	   rdptr;
   reg [7:0] 	   wrptr;
   
`ifdef rd_req
   reg [width-1:0] q;
`else
   wire [width-1:0] q;
`endif
   
   reg [10:0] 	    rdusedw;
   reg [10:0] 	    wrusedw;
   
   integer 	    i;

   always @( aclr)
     begin
	wrptr <= #1 0;
	rdptr <= #1 0;
	for(i=0;i<depth;i=i+1)
	  mem[i] <= #1 0;
     end
   
   always @(posedge wrclk)
     if(wrreq)
       begin
	  wrptr <= #1 wrptr+1;
	  mem[wrptr] <= #1 data;
       end
   
   always @(posedge rdclk)
     if(rdreq)
       begin
   	  rdptr <= #1 rdptr+1;
`ifdef rd_req
	  q <= #1 mem[rdptr];
`endif
       end
   
`ifdef rd_req
`else
   assign q = mem[rdptr];
`endif
   
   // Fix these
   always @(posedge wrclk)
     wrusedw <= #1 wrptr - rdptr;
   
   always @(posedge rdclk)
     rdusedw <= #1 wrptr - rdptr;

   assign wrempty = (wrusedw == 0);
   assign wrfull = (wrusedw == depth-1);
   
   assign rdempty = (rdusedw == 0);
   assign rdfull = (rdusedw == depth-1);
   
endmodule // fifo_1c_2k
