
module vita_tx_control
  #(parameter BASE=0,
    parameter WIDTH=32)
   (input clk, input reset, input clear,
    input set_stb, input [7:0] set_addr, input [31:0] set_data,
    
    input [63:0] vita_time,
    output underrun,

    // From vita_tx_deframer
    input [4+64+WIDTH-1:0] sample_fifo_i,
    input sample_fifo_src_rdy_i,
    output sample_fifo_dst_rdy_o,
    
    // To DSP Core
    output [WIDTH-1:0] sample,
    output run,
    input strobe,

    output [31:0] debug
    );
   
   assign sample = sample_fifo_i[4+64+WIDTH-1:4+64];

   wire [63:0] send_time = sample_fifo_i[63:0];
   wire        eop = sample_fifo_i[64];
   wire        eob = sample_fifo_i[65];
   wire        sob = sample_fifo_i[66];
   wire        send_at = sample_fifo_i[67];
   wire        now, early, late, too_early;

   // FIXME ignore too_early for now for timing reasons
   assign too_early = 0;
   time_compare 
     time_compare (.time_now(vita_time), .trigger_time(send_time), .now(now), .early(early), 
		   .late(late), .too_early());
//		   .late(late), .too_early(too_early));
   
   localparam IBS_IDLE = 0;
   localparam IBS_RUN = 1;  // FIXME do we need this?
   localparam IBS_CONT_BURST = 2;
   localparam IBS_UNDERRUN = 3;
   localparam IBS_UNDERRUN_DONE = 4;
   
   reg [2:0] ibs_state;

   wire      clear_state;
   setting_reg #(.my_addr(BASE+1)) sr
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(),.changed(clear_state));
   
   always @(posedge clk)
     if(reset | clear_state)
       ibs_state <= 0;
     else
       case(ibs_state)
	 IBS_IDLE :
	   if(sample_fifo_src_rdy_i)
	     if(~send_at | now)
	       ibs_state <= IBS_RUN;
	     else if(late | too_early)
	       ibs_state <= IBS_UNDERRUN;
	 
	 IBS_RUN :
	   if(strobe)
	     if(~sample_fifo_src_rdy_i)
	       ibs_state <= IBS_UNDERRUN;
	     else if(eop)
	       if(eob)
		 ibs_state <= IBS_IDLE;
	       else
		 ibs_state <= IBS_CONT_BURST;

	 IBS_CONT_BURST :
	   if(strobe)
	     ibs_state <= IBS_UNDERRUN_DONE;
	   else if(sample_fifo_src_rdy_i)
	     ibs_state <= IBS_RUN;
	 
	 IBS_UNDERRUN :
	   if(sample_fifo_src_rdy_i & eop)
	     ibs_state <= IBS_UNDERRUN_DONE;

	 IBS_UNDERRUN_DONE :
	   ;
       endcase // case (ibs_state)

   assign sample_fifo_dst_rdy_o = (ibs_state == IBS_UNDERRUN) | (strobe & (ibs_state == IBS_RUN));  // FIXME also cleanout
   assign run = (ibs_state == IBS_RUN) | (ibs_state == IBS_CONT_BURST);
   assign underrun = (ibs_state == IBS_UNDERRUN_DONE);

   assign debug = { { now,early,late,too_early,eop,eob,sob,send_at },
		    { sample_fifo_src_rdy_i, sample_fifo_dst_rdy_o, strobe, run, underrun, ibs_state[2:0] },
		    { 8'b0 },
		    { 8'b0 } };
   
endmodule // vita_tx_control
