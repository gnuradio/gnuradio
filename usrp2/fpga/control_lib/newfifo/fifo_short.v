
module fifo_short
  #(parameter WIDTH=32)
   (input clk, input reset, input clear,
    input [WIDTH-1:0] datain,
    input src_rdy_i,
    output dst_rdy_o,
    output [WIDTH-1:0] dataout,
    output src_rdy_o,
    input dst_rdy_i,
    
    output reg [4:0] space,
    output reg [4:0] occupied);

   reg full, empty;
   wire write 	     = src_rdy_i & dst_rdy_o;
   wire read 	     = dst_rdy_i & src_rdy_o;

   assign dst_rdy_o  = ~full;
   assign src_rdy_o  = ~empty;
   
   reg [3:0] 	  a;
   genvar 	  i;
   
   generate
      for (i=0;i<WIDTH;i=i+1)
	begin : gen_srl16
	   SRL16E
	     srl16e(.Q(dataout[i]),
		    .A0(a[0]),.A1(a[1]),.A2(a[2]),.A3(a[3]),
		    .CE(write),.CLK(clk),.D(datain[i]));
	end
   endgenerate
   
   always @(posedge clk)
     if(reset)
       begin
	  a <= 0;
	  empty <= 1;
	  full <= 0;
       end
     else if(clear)
       begin
	  a <= 0;
	  empty <= 1;
	  full<= 0;
       end
     else if(read & ~write)
       begin
	  full <= 0;
	  if(a==0)
	    empty <= 1;
	  else
	    a <= a - 1;
       end
     else if(write & ~read)
       begin
	  empty <= 0;
	  if(~empty)
	    a <= a + 1;
	  if(a == 14)
	    full <= 1;
       end

   // NOTE will fail if you write into a full fifo or read from an empty one

   //////////////////////////////////////////////////////////////
   // space and occupied are used for diagnostics, not 
   // guaranteed correct
   
   //assign space = full ? 0 : empty ? 16 : 15-a;
   //assign occupied = empty ? 0 : full ? 16 : a+1;

   always @(posedge clk)
     if(reset)
       space <= 16;
     else if(clear)
       space <= 16;
     else if(read & ~write)
       space <= space + 1;
     else if(write & ~read)
       space <= space - 1;
   
   always @(posedge clk)
     if(reset)
       occupied <= 0;
     else if(clear)
       occupied <= 0;
     else if(read & ~write)
       occupied <= occupied - 1;
     else if(write & ~read)
       occupied <= occupied + 1;
      
endmodule // fifo_short

