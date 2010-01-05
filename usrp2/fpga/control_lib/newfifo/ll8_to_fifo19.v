
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
   
   localparam XFER_EMPTY       = 0;
   localparam XFER_HALF        = 1;
   localparam XFER_HALF_WRITE  = 3;
   
   // Why anybody would use active low in an FPGA is beyond me...
   wire  ll_sof      = ~ll_sof_n;
   wire  ll_eof      = ~ll_eof_n;
   wire  ll_src_rdy  = ~ll_src_rdy_n;
   wire  ll_dst_rdy;
   assign    ll_dst_rdy_n  = ~ll_dst_rdy;
   
   wire  xfer_out 	   = f19_src_rdy_o & f19_dst_rdy_i;
   wire  xfer_in 	   = ll_src_rdy & ll_dst_rdy; 
   
   reg 	 hold_sof;
   wire  f19_sof, f19_eof, f19_occ;
   
   reg [1:0] state;
   reg [7:0] hold_reg;
   
   always @(posedge clk)
     if(ll_src_rdy & (state==XFER_EMPTY))
       hold_reg 	      <= ll_data;
   
   always @(posedge clk)
     if(ll_sof & (state==XFER_EMPTY))
       hold_sof 	      <= 1;
     else if(xfer_out)
       hold_sof 	      <= 0;
   
   always @(posedge clk)
     if(reset | clear)
       state 		      <= XFER_EMPTY;
     else
       case(state)
	 XFER_EMPTY :
	   if(ll_src_rdy)
	     if(ll_eof)
	       state 	      <= XFER_HALF_WRITE;
	     else
	       state 	      <= XFER_HALF;
	 XFER_HALF :
	   if(ll_src_rdy & f19_dst_rdy_i)
	       state 	      <= XFER_EMPTY;
         XFER_HALF_WRITE :
	   if(f19_dst_rdy_i)
	     state 	<= XFER_EMPTY;
       endcase // case (state)
      
   assign ll_dst_rdy 	 = (state==XFER_EMPTY) | ((state==XFER_HALF)&f19_dst_rdy_i);
   assign f19_src_rdy_o  = (state==XFER_HALF_WRITE) | ((state==XFER_HALF)&ll_src_rdy);
   
   assign f19_sof 	 = hold_sof | (ll_sof & (state==XFER_HALF));
   assign f19_eof 	 = (state == XFER_HALF_WRITE) | ll_eof;
   assign f19_occ 	 = (state == XFER_HALF_WRITE);
   
   assign f19_data 	 = {f19_occ,f19_eof,f19_sof,hold_reg,ll_data};
      
endmodule // ll8_to_fifo19

