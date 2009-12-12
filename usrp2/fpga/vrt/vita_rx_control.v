
module vita_rx_control
  #(parameter BASE=0,
    parameter WIDTH=32)
   (input clk, input reset, input clear,
    input set_stb, input [7:0] set_addr, input [31:0] set_data,
    
    input [63:0] vita_time,
    output overrun,

    // To vita_rx_framer
    output [4+64+WIDTH-1:0] sample_fifo_o,
    output sample_fifo_src_rdy_o,
    input sample_fifo_dst_rdy_i,
    
    // From DSP Core
    input [WIDTH-1:0] sample,
    output run,
    input strobe,
    
    output [31:0] debug_rx
    );

   // FIXME add TX Interruption (halt, pause, continue) functionality
   
   wire [63:0] 	  new_time;
   wire [31:0] 	  new_command;
   wire 	  sc_pre1, clear_int, clear_reg;

   assign clear_int  = clear | clear_reg;
   
   wire [63:0] 	  rcvtime_pre;
   reg [63:0] 	  rcvtime;
   wire [29:0] 	  numlines_pre;
   wire 	  send_imm_pre, chain_pre;
   reg 		  send_imm, chain;
   wire 	  full_ctrl, read_ctrl, empty_ctrl, write_ctrl;
   reg 		  sc_pre2;
   wire [33:0] 	  fifo_line;
   reg [29:0] 	  lines_left;
   reg [2:0] 	  ibs_state;
   wire 	  now, early, late;
   wire 	  sample_fifo_in_rdy;
   
   setting_reg #(.my_addr(BASE)) sr_cmd
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(new_command),.changed());

   setting_reg #(.my_addr(BASE+1)) sr_time_h
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(new_time[63:32]),.changed());
   
   setting_reg #(.my_addr(BASE+2)) sr_time_l
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(new_time[31:0]),.changed(sc_pre1));
   
   setting_reg #(.my_addr(BASE+3)) sr_clear
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(),.changed(clear_reg));

   // FIFO to store commands sent from the settings bus
   always @(posedge clk)
     sc_pre2 		  <= sc_pre1;
   assign      write_ctrl  = sc_pre1 & ~sc_pre2;

   wire [4:0]  command_queue_len;
   shortfifo #(.WIDTH(96)) commandfifo
     (.clk(clk),.rst(reset),.clear(clear_int),
      .datain({new_command,new_time}), .write(write_ctrl&~full_ctrl), .full(full_ctrl),
      .dataout({send_imm_pre,chain_pre,numlines_pre,rcvtime_pre}), 
      .read(read_ctrl), .empty(empty_ctrl),
      .occupied(command_queue_len), .space() );
   
   reg [33:0]  pkt_fifo_line;

   localparam IBS_IDLE = 0;
   localparam IBS_WAITING = 1;
   localparam IBS_RUNNING = 2;
   localparam IBS_OVERRUN = 4;
   localparam IBS_BROKENCHAIN = 5;
   localparam IBS_LATECMD = 6;

   wire signal_cmd_done     = (lines_left == 1) & (~chain | (~empty_ctrl & (numlines_pre==0)));
   wire signal_overrun 	    = (ibs_state == IBS_OVERRUN);
   wire signal_brokenchain  = (ibs_state == IBS_BROKENCHAIN);
   wire signal_latecmd 	    = (ibs_state == IBS_LATECMD);

   // Buffer of samples for while we're writing the packet headers
   wire [3:0] flags = {signal_overrun,signal_brokenchain,signal_latecmd,signal_cmd_done};

   wire       attempt_sample_write    = ((run & strobe) | (ibs_state==IBS_OVERRUN) |
				       (ibs_state==IBS_BROKENCHAIN) | (ibs_state==IBS_LATECMD));
   
   fifo_short #(.WIDTH(4+64+WIDTH)) rx_sample_fifo
     (.clk(clk),.reset(reset),.clear(clear_int),
      .datain({flags,vita_time,sample}), .src_rdy_i(attempt_sample_write), .dst_rdy_o(sample_fifo_in_rdy),
      .dataout(sample_fifo_o), 
      .src_rdy_o(sample_fifo_src_rdy_o), .dst_rdy_i(sample_fifo_dst_rdy_i),
      .space(), .occupied() );
   
   // Inband Signalling State Machine
   time_compare 
     time_compare (.time_now(vita_time), .trigger_time(rcvtime), .now(now), .early(early), .late(late));
   
   wire too_late 	    = late & ~send_imm;
   wire go_now 		    = now | send_imm;
   wire full 		    = ~sample_fifo_in_rdy;
   
   always @(posedge clk)
     if(reset | clear_int)
       begin
	  ibs_state 	   <= IBS_IDLE;
	  lines_left 	   <= 0;
	  rcvtime 	   <= 0;
	  send_imm 	   <= 0;
	  chain 	   <= 0;
       end
     else
       case(ibs_state)
	 IBS_IDLE :
	   if(~empty_ctrl)
	     begin
		lines_left <= numlines_pre;
		rcvtime <= rcvtime_pre;
		ibs_state <= IBS_WAITING;
		send_imm <= send_imm_pre;
		chain <= chain_pre;
	     end
	 IBS_WAITING :
	   if(go_now)
	     ibs_state <= IBS_RUNNING;
	   else if(too_late)
	     ibs_state <= IBS_LATECMD;
	 IBS_RUNNING :
	   if(strobe)
	     if(full)
	       ibs_state 	     <= IBS_OVERRUN;
	     else
	       begin
		  lines_left 	     <= lines_left - 1;
		  if(lines_left == 1)
		    if(~chain)
		      ibs_state      <= IBS_IDLE;
		    else if(empty_ctrl)
		      ibs_state      <= IBS_BROKENCHAIN;
		    else
		      begin
			 lines_left  <= numlines_pre;
			 rcvtime     <= rcvtime_pre;
			 send_imm    <= send_imm_pre;
			 chain 	     <= chain_pre;
			 if(numlines_pre == 0)  // If we are told to stop here
			   ibs_state <= IBS_IDLE;
			 else
			   ibs_state <= IBS_RUNNING;
		      end
	       end // else: !if(full)
	 IBS_OVERRUN, IBS_LATECMD, IBS_BROKENCHAIN :
	   if(sample_fifo_in_rdy)
	     ibs_state <= IBS_IDLE;
       endcase // case(ibs_state)
   
   assign overrun = (ibs_state == IBS_OVERRUN);
   assign run = (ibs_state == IBS_RUNNING);

   assign read_ctrl = ( (ibs_state == IBS_IDLE) | ((ibs_state == IBS_RUNNING) & strobe & ~full & (lines_left==1) & chain) )
     & ~empty_ctrl;
   
   assign debug_rx = { { ibs_state[2:0], command_queue_len },
		       { 8'd0 },
		       { go_now, too_late, run, strobe, read_ctrl, write_ctrl, full_ctrl, empty_ctrl },
		       { 2'b0, overrun, chain_pre, sample_fifo_in_rdy, attempt_sample_write, sample_fifo_src_rdy_o,sample_fifo_dst_rdy_i} };
   
endmodule // rx_control
