
module vita_tx_deframer
  #(parameter BASE=0,
    parameter MAXCHAN=1)
   (input clk, input reset, input clear,
    input set_stb, input [7:0] set_addr, input [31:0] set_data,
    
    // To FIFO interface of Buffer Pool
    input [35:0] data_i,
    input src_rdy_i,
    output dst_rdy_o,
    
    output [4+64+(32*MAXCHAN)-1:0] sample_fifo_o,
    output sample_fifo_src_rdy_o,
    input sample_fifo_dst_rdy_i,
    
    // FIFO Levels
    output [15:0] fifo_occupied,
    output fifo_full,
    output fifo_empty    
    );

   wire [1:0] numchan;
   setting_reg #(.my_addr(BASE+8), .at_reset(0)) sr_numchan
     (.clk(clk),.rst(reset),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(numchan),.changed());

   reg [3:0] vita_state;
   wire      has_streamid, has_classid, has_secs, has_tics, has_trailer;
   assign has_streamid 	= (data_i[31:28]==4'b001);
   assign has_classid 	= data_i[27];
   assign has_secs 	= ~(data_i[23:22]==2'b00);
   assign has_tics 	= ~(data_i[21:20]==2'b00);
   assign has_trailer 	= data_i[26];
   assign is_sob = data_i[25];
   assign is_eob = data_i[24];
   wire      eof = data_i[33];
   
   reg 	     has_streamid_reg, has_classid_reg, has_secs_reg, has_tics_reg;
   reg 	     has_trailer_reg, is_sob_reg, is_eob_reg;
   reg [1:0] vector_phase;
   wire      line_done;
   
   // Output FIFO for packetized data
   localparam VITA_HEADER 	 = 0;
   localparam VITA_STREAMID 	 = 1;
   localparam VITA_CLASSID 	 = 2;
   localparam VITA_CLASSID2 	 = 3;
   localparam VITA_SECS 	 = 4;
   localparam VITA_TICS 	 = 5;
   localparam VITA_TICS2 	 = 6;
   localparam VITA_PAYLOAD 	 = 7;
   localparam VITA_WAIT          = 8;
   localparam VITA_TRAILER 	 = 9;

   always @(posedge clk)
     if(reset | clear)
       begin
	  vita_state 		<= VITA_HEADER;
	  {has_streamid_reg, has_classid_reg, has_secs_reg, has_tics_reg, has_trailer_reg, is_sob_reg, is_eob_reg} 
	    <= 0;
       end
     else 
       if((vita_state == VITA_WAIT) & fifo_space)
	 if(eof)  // FIXME
	   if(has_trailer_reg)
	     vita_state <= VITA_TRAILER;
	   else
	     vita_state <= VITA_HEADER;
	 else
	   vita_state <= VITA_PAYLOAD;
       else if(src_rdy_i)
	 case(vita_state)
	   VITA_HEADER :
	     begin
		{has_streamid_reg, has_classid_reg, has_secs_reg, has_tics_reg, has_trailer_reg, is_sob_reg, is_eob_reg} 
 		  <= {has_streamid, has_classid, has_secs, has_tics, has_trailer, is_sob, is_eob};
		vector_phase <= 0;
		if(has_streamid)
		  vita_state <= VITA_STREAMID;
		else if(has_classid)
		  vita_state <= VITA_CLASSID;
		else if(has_secs)
		  vita_state <= VITA_SECS;
		else if(has_tics)
		  vita_state <= VITA_TICS;
		else
		  vita_state <= VITA_PAYLOAD;
	     end // case: VITA_HEADER
	   VITA_STREAMID :
	     if(has_classid_reg)
	       vita_state <= VITA_CLASSID;
	     else if(has_secs_reg)
	       vita_state <= VITA_SECS;
	     else if(has_tics_reg)
	       vita_state <= VITA_TICS;
	     else
	       vita_state <= VITA_PAYLOAD;
	   VITA_CLASSID :
	     vita_state <= VITA_CLASSID2;
	   VITA_CLASSID2 :
	     if(has_secs_reg)
	       vita_state <= VITA_SECS;
	     else if(has_tics_reg)
	       vita_state <= VITA_TICS;
	     else
	       vita_state <= VITA_PAYLOAD;
	   VITA_SECS :
	     if(has_tics_reg)
	       vita_state <= VITA_TICS;
	     else
	       vita_state <= VITA_PAYLOAD;
	   VITA_TICS :
	     vita_state <= VITA_TICS2;
	   VITA_TICS2 :
	     vita_state <= VITA_PAYLOAD;
	   VITA_PAYLOAD :
	     if(line_done)
	       begin
		  vector_phase <= 0;
		  vita_state <= VITA_WAIT;
	       end
	     else
	       vector_phase <= vector_phase + 1;
	   VITA_TRAILER :
	     vita_state <= VITA_HEADER;
	   VITA_WAIT :
	     ;
	   default :
	     vita_state <= VITA_HEADER;
	 endcase // case (vita_state)

   assign line_done = (vector_phase == numchan);
   
   wire [4+64+32*MAXCHAN-1:0] fifo_i;
   reg [63:0] 		      send_time;
   reg [31:0] 		      sample_a, sample_b, sample_c, sample_d;
   
   always @(posedge clk)
     case(vita_state)
       VITA_SECS :
	 send_time[63:32] <= data_i[31:0];
       VITA_TICS2 :
	 send_time[31:0] <= data_i[31:0];
       VITA_WAIT, VITA_HEADER :
	 send_time[63:0] <= 64'd0;
     endcase // case (vita_state)
   
   always @(posedge clk)
     if(vita_state == VITA_PAYLOAD)
       case(vector_phase)
	 0: sample_a <= data_i[31:0];
	 1: sample_b <= data_i[31:0];
	 2: sample_c <= data_i[31:0];
	 3: sample_d <= data_i[31:0];
       endcase // case (vector_phase)
   

   fifo_short #(.WIDTH(4+64+32*MAXCHAN)) short_tx_q
     (.clk(clk), .reset(reset), .clear(clear),
      .datain(fifo_i), .src_rdy_i(vita_state == VITA_IDLE), .dst_rdy_o(fifo_space),
      .dataout(sample_fifo_o), .src_rdy_o(sample_fifo_src_rdy_o), .dst_rdy_i(sample_fifo_dst_rdy_i) );

   // sob, eob, has_secs (send_at) ignored on all lines except first
   assign fifo_i = {sample_d,sample_c,sample_b,sample_a,has_secs_reg,is_sob_reg,is_eob_reg,eof,send_time};

   assign dst_rdy_o = (vita_state != VITA_PAYLOAD);
   
endmodule // vita_tx_deframer
