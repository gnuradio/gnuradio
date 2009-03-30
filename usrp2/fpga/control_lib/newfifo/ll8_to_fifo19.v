
module ll8_to_fifo19
  (input clk, input reset, input clear,
   input [7:0] ll_data,
   input ll_sof_n,
   input ll_eof_n,
   input ll_src_rdy_n,
   output ll_dst_rdy_n,

   output [18:0] f19_data,
   output f19_src_rdy_o,
   input f19_dst_rdy_i );

   // Why anybody would use active low in an FPGA is beyond me...
   wire  ll_sof      = ~ll_sof_n;
   wire  ll_eof      = ~ll_eof_n;
   wire  ll_src_rdy  = ~ll_src_rdy_n;
   wire  ll_dst_rdy;
   assign    ll_dst_rdy_n  = ~ll_dst_rdy;

   wire xfer_out 	   = f19_src_rdy_o & f19_dst_rdy_i;
   //  wire xfer_in 	   = ll_src_rdy & ll_dst_rdy;   Not needed
   
   reg 	 f19_sof, f19_eof, f19_occ;
   
   reg [1:0] state;
   reg [7:0] dat0, dat1;

   always @(posedge clk)
     if(ll_src_rdy & ((state==0)|xfer_out))
       f19_sof <= ll_sof;

   always @(posedge clk)
     if(ll_src_rdy & ((state != 2)|xfer_out))
       f19_eof <= ll_eof;

   always @(posedge clk)
     if(ll_eof)
       f19_occ <= ~state[0];
     else
       f19_occ <= 0;
   
   always @(posedge clk)
     if(reset)
       state   <= 0;
     else
       if(ll_src_rdy)
	 case(state)
	   0 : 
	     if(ll_eof)
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
     if(ll_src_rdy & (state==1))
       dat1 		   <= ll_data;

   always @(posedge clk)
     if(ll_src_rdy & ((state==0) | xfer_out))
       dat0 		   <= ll_data;
   
   assign    ll_dst_rdy     = xfer_out | (state != 2);
   assign    f19_data 	    = {f19_occ,f19_eof,f19_sof,dat0,dat1};
   assign    f19_src_rdy_o  = (state == 2);
      
endmodule // ll8_to_fifo19

