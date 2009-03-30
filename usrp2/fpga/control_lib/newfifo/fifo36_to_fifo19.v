
module fifo36_to_fifo19
  (input clk, input reset, input clear,
   input [35:0] f36_datain,
   input f36_src_rdy_i,
   output f36_dst_rdy_o,
   
   output [18:0] f19_dataout,
   output f19_src_rdy_o,
   input f19_dst_rdy_i );

   wire   f36_sof  = f36_datain[32];
   wire   f36_eof  = f36_datain[33];
   wire   f36_occ  = f36_datain[35:34];

   reg phase;

   wire half_line 	   = f36_eof & ((f36_occ==1)|(f36_occ==2));
   
   assign f19_dataout[15:0] = phase ? f36_datain[15:0] : f36_datain[31:16];
   assign f19_dataout[16]  = phase ? 0 : f36_sof;
   assign f19_dataout[17]  = phase ? f36_eof : half_line;
   assign f19_dataout[18]  = f19_dataout[17] & ((f36_occ==1)|(f36_occ==3));
   
   assign f19_src_rdy_o    = f36_src_rdy_i;
   assign f36_dst_rdy_o    = (phase | half_line) & f19_dst_rdy_i;
	
   wire f19_xfer 	   = f19_src_rdy_o & f19_dst_rdy_i;
   wire f36_xfer 	   = f36_src_rdy_i & f36_dst_rdy_o;

   always @(posedge clk)
     if(reset)
       phase 		  <= 0;
     else if(f36_xfer)
       phase 		  <= 0;
     else if(f19_xfer)
       phase 		  <= 1;
   
       
endmodule // fifo36_to_fifo19

