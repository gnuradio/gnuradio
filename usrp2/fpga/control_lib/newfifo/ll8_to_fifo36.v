
module ll8_to_fifo36
  (input clk, reset,
   input [7:0] ll_data,
   input ll_sof_n,
   input ll_eof_n,
   input ll_src_rdy_n,
   output ll_dst_rdy_n,

   output [35:0] f36_data,
   output f36_src_rdy_o,
   input f36_dst_rdy_i );

   wire f36_full     = ~f36_dst_rdy_i;
   wire f36_write    = f36_src_rdy_o & f36_dst_rdy_i;
      
   // Why anybody would use active low in an FPGA is beyond me...
   wire  ll_sof      = ~ll_sof_n;
   wire  ll_eof      = ~ll_eof_n;
   wire  ll_src_rdy  = ~ll_src_rdy_n;
   wire  ll_dst_rdy;
   assign    ll_dst_rdy_n = ~ll_dst_rdy;

   reg 	 f36_sof, f36_eof;
   reg [1:0] f36_occ;
   
   
   reg [2:0] state;
   reg [7:0] dat0, dat1, dat2, dat3;

   always @(posedge clk)
     if(ll_src_rdy & ((state==0)|f36_write))
       f36_sof <= ll_sof;

   always @(posedge clk)
     if(ll_src_rdy & ((state !=4)|f36_write))
       f36_eof <= ll_eof;

   always @(posedge clk)
     if(ll_eof)
       f36_occ <= state[1:0];
     else
       f36_occ <= 0;
   
   always @(posedge clk)
     if(reset)
       state   <= 0;
     else
       if(ll_src_rdy)
	 case(state)
	   0 : 
	     if(ll_eof)
	       state <= 4;
	     else
	       state <= 1;
	   1 : 
	     if(ll_eof)
	       state <= 4;
	     else
	       state <= 2;
	   2 : 
	     if(ll_eof)
	       state <= 4;
	     else 
	       state <= 3;
	   3 : state <= 4;
	   4 : if(~f36_full)
	     state 	   <= 1;
	 endcase // case(state)
       else
	 if(f36_write)
	   state 	   <= 0;

   always @(posedge clk)
     if(ll_src_rdy & (state==3))
       dat3 		   <= ll_data;

   always @(posedge clk)
     if(ll_src_rdy & (state==2))
       dat2 		   <= ll_data;

   always @(posedge clk)
     if(ll_src_rdy & (state==1))
       dat1 		   <= ll_data;

   always @(posedge clk)
     if(ll_src_rdy & ((state==0) | f36_write))
       dat0 		   <= ll_data;
   
   assign    ll_dst_rdy     = ~f36_full | (state != 4);
   assign    f36_data 	    = {f36_occ,f36_eof,f36_sof,dat0,dat1,dat2,dat3};  // FIXME endianess
   assign    f36_src_rdy_o  = (state == 4);
      
endmodule // ll8_to_fifo36
