
module fifo19_to_fifo36
  (input clk, input reset, input clear,
   input [18:0] f19_datain,
   input f19_src_rdy_i,
   output f19_dst_rdy_o,

   output [35:0] f36_dataout,
   output f36_src_rdy_o,
   input f36_dst_rdy_i
   );

   reg 	 f36_sof, f36_eof, f36_occ;
   
   reg [1:0] state;
   reg [15:0] dat0, dat1;

   wire f19_sof  = f19_datain[16];
   wire f19_eof  = f19_datain[17];
   wire f19_occ  = f19_datain[18];

   wire xfer_out = f36_src_rdy_o & f36_dst_rdy_i;

   always @(posedge clk)
     if(f19_src_rdy_i & ((state==0)|xfer_out))
       f36_sof 	<= f19_sof;

   always @(posedge clk)
     if(f19_src_rdy_i & ((state != 2)|xfer_out))
       f36_eof 	<= f19_eof;

   always @(posedge clk)    // FIXME check this
     if(f19_eof)
       f36_occ 	<= {state[0],f19_occ};
     else
       f36_occ 	<= 0;
   
   always @(posedge clk)
     if(reset)
       state 	<= 0;
     else
       if(f19_src_rdy_i)
	 case(state)
	   0 : 
	     if(f19_eof)
	       state <= 2;
	     else
	       state <= 1;
	   1 : 
	     state <= 2;
	   2 : 
	     if(xfer_out)
	       state 	   <= 1;
	 endcase // case(state)
       else
	 if(xfer_out)
	   state 	   <= 0;

   always @(posedge clk)
     if(f19_src_rdy_i & (state==1))
       dat1 		   <= f19_datain;

   always @(posedge clk)
     if(f19_src_rdy_i & ((state==0) | xfer_out))
       dat0 		   <= f19_datain;
   
   assign    f19_dst_rdy_o  = xfer_out | (state != 2);
   assign    f36_dataout    = {f36_occ,f36_eof,f36_sof,dat0,dat1};
   assign    f36_src_rdy_o  = (state == 2);
      
endmodule // fifo19_to_fifo36
