
module vita_rx_framer
  #(parameter BASE=0,
    parameter MAXCHAN=1)
   (input clk, input reset, input clear,
    input set_stb, input [7:0] set_addr, input [31:0] set_data,
    
    // To FIFO interface of Buffer Pool
    output [35:0] data_o,
    input dst_rdy_i,
    output src_rdy_o,
    
    // From vita_rx_control
    input [4+64+(32*MAXCHAN)-1:0] sample_fifo_i,
    input sample_fifo_src_rdy_i,
    output sample_fifo_dst_rdy_o,
    
    // FIFO Levels
    output [15:0] fifo_occupied,
    output fifo_full,
    output fifo_empty,
    
    output [31:0] debug_rx
    );

   localparam SAMP_WIDTH  = 4+64+(32*MAXCHAN);
   reg [3:0] 	  sample_phase;
   wire [3:0] 	  numchan;
   wire [3:0] 	  flags_fifo_o = sample_fifo_i[SAMP_WIDTH-1:SAMP_WIDTH-4];
   wire [63:0] 	  vita_time_fifo_o = sample_fifo_i[SAMP_WIDTH-5:SAMP_WIDTH-68];

   reg [31:0] 	  data_fifo_o;

   // The tools won't synthesize properly without this kludge because of the variable
   // parameter length
   
   wire [127:0]   FIXED_WIDTH_KLUDGE = sample_fifo_i;
   always @*
     case(sample_phase)
       4'd0 : data_fifo_o = FIXED_WIDTH_KLUDGE[31:0];
       4'd1 : data_fifo_o = FIXED_WIDTH_KLUDGE[63:32];
       4'd2 : data_fifo_o = FIXED_WIDTH_KLUDGE[95:64];
       4'd3 : data_fifo_o = FIXED_WIDTH_KLUDGE[127:96];
       default : data_fifo_o = 32'hDEADBEEF;
     endcase // case (sample_phase)
   
   wire 	  clear_pkt_count, pkt_fifo_rdy, sample_fifo_in_rdy;
   
   wire [31:0] 	  vita_header, vita_streamid, vita_trailer;
   wire [15:0] 	  samples_per_packet;
   
   reg [33:0] 	  pkt_fifo_line;
   reg [3:0] 	  vita_state;
   reg [15:0] 	  sample_ctr;
   reg [3:0] 	  pkt_count;
   
   wire [15:0] 	  vita_pkt_len = samples_per_packet + 6;
   //wire [3:0] flags = {signal_overrun,signal_brokenchain,signal_latecmd,signal_cmd_done};

   wire 	  clear_reg;
   wire 	  clear_int  = clear | clear_reg;

   setting_reg #(.my_addr(BASE+3)) sr_clear
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(),.changed(clear_reg));

   setting_reg #(.my_addr(BASE+4)) sr_header
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(vita_header),.changed());

   setting_reg #(.my_addr(BASE+5)) sr_streamid
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(vita_streamid),.changed(clear_pkt_count));

   setting_reg #(.my_addr(BASE+6)) sr_trailer
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(vita_trailer),.changed());

   setting_reg #(.my_addr(BASE+7)) sr_samples_per_pkt
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(samples_per_packet),.changed());

   setting_reg #(.my_addr(BASE+8), .at_reset(1)) sr_numchan
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(numchan),.changed());

   // Output FIFO for packetized data
   localparam VITA_IDLE 	 = 0;
   localparam VITA_HEADER 	 = 1;
   localparam VITA_STREAMID 	 = 2;
   localparam VITA_SECS 	 = 3;
   localparam VITA_TICS 	 = 4;
   localparam VITA_TICS2 	 = 5;
   localparam VITA_PAYLOAD 	 = 6;
   localparam VITA_TRAILER 	 = 7;
   localparam VITA_ERR_HEADER 	 = 9;  // All ERR at 4'b1000 or'ed with base
   localparam VITA_ERR_STREAMID  = 10;
   localparam VITA_ERR_SECS 	 = 11;
   localparam VITA_ERR_TICS 	 = 12;
   localparam VITA_ERR_TICS2 	 = 13;
   localparam VITA_ERR_PAYLOAD 	 = 14;
   localparam VITA_ERR_TRAILER 	 = 15;
      
   always @(posedge clk)
     if(reset | clear_pkt_count)
       pkt_count <= 0;
     else if((vita_state == VITA_TRAILER) & pkt_fifo_rdy)
       pkt_count <= pkt_count + 1;

   always @*
     case(vita_state)
       VITA_HEADER, VITA_ERR_HEADER : pkt_fifo_line <= {2'b01,vita_header[31:20],pkt_count,vita_pkt_len};
       VITA_STREAMID, VITA_ERR_STREAMID : pkt_fifo_line <= {2'b00,vita_streamid};
       VITA_SECS, VITA_ERR_SECS : pkt_fifo_line <= {2'b00,vita_time_fifo_o[63:32]};
       VITA_TICS, VITA_ERR_TICS : pkt_fifo_line <= {2'b00,32'd0};
       VITA_TICS2, VITA_ERR_TICS2 : pkt_fifo_line <= {2'b00,vita_time_fifo_o[31:0]};
       VITA_PAYLOAD : pkt_fifo_line <= {2'b00,data_fifo_o};
       VITA_ERR_PAYLOAD : pkt_fifo_line <= {2'b00,28'd0,flags_fifo_o};
       VITA_TRAILER : pkt_fifo_line <= {2'b10,vita_trailer};
       VITA_ERR_TRAILER : pkt_fifo_line <= {2'b11,vita_trailer};
       default : pkt_fifo_line <= 34'h0_FFFF_FFFF;
       endcase // case (vita_state)

   always @(posedge clk)
     if(reset)
       begin
	  vita_state   <= VITA_IDLE;
	  sample_ctr   <= 0;
	  sample_phase <= 0;
       end
     else
       if(vita_state==VITA_IDLE)
	 begin
	    sample_ctr <= 1;
	    sample_phase <= 0;
	    if(sample_fifo_src_rdy_i)
	      if(|flags_fifo_o[3:1])
		vita_state <= VITA_ERR_HEADER;
	      else
		vita_state <= VITA_HEADER;
	 end
       else if(pkt_fifo_rdy)
	 case(vita_state)
	   VITA_PAYLOAD :
	     if(sample_fifo_src_rdy_i)
	       begin
		  if(sample_phase == (numchan-4'd1))
		    begin
		       sample_phase <= 0;
		       sample_ctr   <= sample_ctr + 1;
		       if(sample_ctr == samples_per_packet)
			 vita_state <= VITA_TRAILER;
		       if(|flags_fifo_o)   // end early if any flag is set
			 vita_state <= VITA_TRAILER;
		    end
		  else
		    sample_phase <= sample_phase + 1;
	       end
	   VITA_TRAILER, VITA_ERR_TRAILER :
	     vita_state <= VITA_IDLE;
	   default :
	     vita_state 	   <= vita_state + 1;
	 endcase // case (vita_state)

   reg req_write_pkt_fifo;
   always @*
     case(vita_state)
       VITA_IDLE :
	 req_write_pkt_fifo <= 0;
       VITA_HEADER, VITA_STREAMID, VITA_SECS, VITA_TICS, VITA_TICS2, VITA_TRAILER :
	 req_write_pkt_fifo <= 1;
       VITA_PAYLOAD :
	 // Write if sample ready and no error flags
     	 req_write_pkt_fifo <= (sample_fifo_src_rdy_i & ~|flags_fifo_o[3:1]);
       VITA_ERR_HEADER, VITA_ERR_STREAMID, VITA_ERR_SECS, VITA_ERR_TICS, VITA_ERR_TICS2, VITA_ERR_PAYLOAD, VITA_ERR_TRAILER :
	 req_write_pkt_fifo <= 1;
       default :
	 req_write_pkt_fifo <= 0;
     endcase // case (vita_state)
   
   //wire req_write_pkt_fifo  = (vita_state != VITA_IDLE) & (sample_fifo_src_rdy_i | (vita_state != VITA_PAYLOAD));
   
   // Short FIFO to buffer between us and the FIFOs outside
   fifo_short #(.WIDTH(34)) rx_pkt_fifo 
     (.clk(clk), .reset(reset), .clear(clear_int),
      .datain(pkt_fifo_line), .src_rdy_i(req_write_pkt_fifo), .dst_rdy_o(pkt_fifo_rdy),
      .dataout(data_o[33:0]), .src_rdy_o(src_rdy_o), .dst_rdy_i(dst_rdy_i),
      .space(),.occupied(fifo_occupied[4:0]) );
   assign fifo_occupied[15:5] = 0;
   assign data_o[35:34] = 2'b00;  // Always write full lines
   assign sample_fifo_dst_rdy_o  = pkt_fifo_rdy & 
				   ( ((vita_state==VITA_PAYLOAD) & 
				      (sample_phase == (numchan-4'd1)) & 
				      ~|flags_fifo_o[3:1]) |
				     (vita_state==VITA_ERR_TRAILER));
   
   assign debug_rx  = 0;
   
endmodule // rx_control
