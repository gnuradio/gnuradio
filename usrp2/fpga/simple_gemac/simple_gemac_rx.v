

module simple_gemac_rx
  (input clk125, input reset,
   input GMII_RX_CLK, input GMII_RX_DV, input GMII_RX_ER, input [7:0] GMII_RXD,
   output rx_clk, output [7:0] rx_data, output reg rx_valid, output rx_error, output reg rx_ack,
   input [47:0] ucast_addr, input [47:0] mcast_addr, 
   input pass_ucast, input pass_mcast, input pass_bcast, input pass_pause, input pass_all,
   output reg [15:0] pause_quanta_rcvd, output pause_rcvd );

   reg [7:0] rxd_d1;
   reg rx_dv_d1, rx_er_d1;
   assign rx_clk     = GMII_RX_CLK;
   
   always @(posedge rx_clk)
     begin
	rx_dv_d1    <= GMII_RX_DV;
	rx_er_d1    <= GMII_RX_ER;
	rxd_d1 	    <= GMII_RXD;
     end

   wire [7:0] rxd_del;
   wire rx_dv_del, rx_er_del;
   reg go_filt;
   
   localparam DELAY  = 6;
   delay_line #(.WIDTH(10)) rx_delay
     (.clk(rx_clk), .delay(DELAY), .din({rx_dv_d1,rx_er_d1,rxd_d1}),.dout({rx_dv_del,rx_er_dl,rxd_del}));

   assign rx_data   = rxd_del;
   assign rx_error  = 0;

   always @(posedge rx_clk)
     if(reset)
       rx_ack 	   <= 0;
     else
       rx_ack <= (rx_state == RX_GOODFRAME);

   wire is_ucast, is_bcast, is_mcast, is_pause;
   wire keep_packet  = (pass_ucast & is_ucast) | (pass_mcast & is_mcast) | 
	(pass_bcast & is_bcast) | (pass_pause & is_pause) | pass_all;
   
   reg [7:0] rx_state;
   always @(posedge rx_clk)
     if(reset)
       rx_valid <= 0;
     else if(keep_packet)
       rx_valid <= 1;
     else if((rx_state == RX_IDLE))//|(rx_state == RX_GOODFRAME))
       rx_valid <= 0;
   
   address_filter af_ucast (.clk(rx_clk), .reset(reset), .go(go_filt), .data(rxd_d1),
			    .address(ucast_addr), .match(is_ucast), .done());
   address_filter af_mcast (.clk(rx_clk), .reset(reset), .go(go_filt), .data(rxd_d1),
			    .address(mcast_addr), .match(is_mcast), .done());
   address_filter af_bcast (.clk(rx_clk), .reset(reset), .go(go_filt), .data(rxd_d1),
			    .address(48'hFFFF_FFFF_FFFF), .match(is_bcast), .done());
   address_filter af_pause (.clk(rx_clk), .reset(reset), .go(go_filt), .data(rxd_d1),
			    .address(48'h0180_c200_0001), .match(is_pause), .done());

   localparam RX_IDLE 		  = 0;
   localparam RX_PREAMBLE 	  = 1;
   localparam RX_FRAME 		  = 2;
   localparam RX_GOODFRAME 	  = 3;
   localparam RX_DO_PAUSE 	  = 4;
   localparam RX_ERROR 		  = 5;
   localparam RX_DROP 		  = 6;

   localparam RX_PAUSE 		  = 16;
   localparam RX_PAUSE_CHK88 	  = RX_PAUSE + 5;
   localparam RX_PAUSE_CHK08 	  = RX_PAUSE_CHK88 + 1;
   localparam RX_PAUSE_CHK00 	  = RX_PAUSE_CHK08 + 1;
   localparam RX_PAUSE_CHK01 	  = RX_PAUSE_CHK00 + 1;
   localparam RX_PAUSE_STORE_MSB  = RX_PAUSE_CHK01 + 1;
   localparam RX_PAUSE_STORE_LSB  = RX_PAUSE_STORE_MSB + 1;
   localparam RX_PAUSE_WAIT_CRC   = RX_PAUSE_STORE_LSB + 1;
   
   
   always @(posedge rx_clk)
     go_filt 			 <= (rx_state==RX_PREAMBLE) & (rxd_d1 == 8'hD5);

   reg [15:0] pkt_len_ctr;
   always @(posedge rx_clk)
     if(reset |(rx_state == RX_IDLE))
       pkt_len_ctr 	<= 0;
     else
       pkt_len_ctr 	<= pkt_len_ctr + 1;

   localparam MIN_PAUSE_LEN = 71;  // 6
   wire pkt_long_enough  = (pkt_len_ctr >= MIN_PAUSE_LEN);
   always @(posedge rx_clk)
     if(reset)
       rx_state 	<= RX_IDLE;
     else
       if(rx_er_d1) // | (~pkt_long_enough & ~rx_dv_d1) & (rx_state != RX_IDLE))
	 rx_state 	<= RX_ERROR;
       else
	 case(rx_state)
	   RX_IDLE :
	     if(rx_dv_d1)
	       if(rxd_d1 == 8'h55)
		 rx_state   <= RX_PREAMBLE;
	       else
		 rx_state   <= RX_ERROR;
	   RX_PREAMBLE :
	     if(~rx_dv_d1)
	       rx_state   <= RX_ERROR;
	     else if(rxd_d1 == 8'hD5)
	       rx_state   <= RX_FRAME;
	     else if(rxd_d1 != 8'h55)
	       rx_state   <= RX_ERROR;
	   RX_FRAME :
	     if(is_pause)
	       rx_state <= RX_PAUSE;
	     else if(~rx_dv_d1)
	       if(match_crc)
		 rx_state <= RX_GOODFRAME;
	       else
		 rx_state <= RX_ERROR;
	   RX_PAUSE_CHK88 :
	     if(rxd_d1 != 8'h88)
	       rx_state <= RX_DROP;
	     else
	       rx_state <= RX_PAUSE_CHK08;
	   RX_PAUSE_CHK08 :
	     if(rxd_d1 != 8'h08)
	       rx_state <= RX_DROP;
	     else
	       rx_state <= RX_PAUSE_CHK00;
	   RX_PAUSE_CHK00 :
	     if(rxd_d1 != 8'h00)
	       rx_state <= RX_DROP;
	     else
	       rx_state <= RX_PAUSE_CHK01;
	   RX_PAUSE_CHK01 :
	     if(rxd_d1 != 8'h01)
	       rx_state <= RX_DROP;
	     else
	       rx_state <= RX_PAUSE_STORE_MSB;
	   RX_PAUSE_WAIT_CRC :
	     if(pkt_long_enough)
	       if(match_crc)
		 rx_state <= RX_DO_PAUSE;
	       else
		 rx_state <= RX_DROP;
	   RX_DO_PAUSE :
	     rx_state <= RX_IDLE;
	   RX_GOODFRAME :
	     rx_state <= RX_IDLE;
	   RX_DROP, RX_ERROR :
	     if(~rx_dv_d1)
	       rx_state <= RX_IDLE;
	   default
	     rx_state <= rx_state + 1;
	 endcase // case (rx_state)

   assign pause_rcvd = (rx_state == RX_DO_PAUSE);
   wire match_crc;
   wire clear_crc 	 = rx_state == RX_IDLE;
   wire calc_crc 	 = (rx_state == RX_FRAME) | rx_state[7:4]==4'h1;
   crc crc_check(.clk(rx_clk),.reset(reset),.clear(clear_crc),
		 .data(rxd_d1),.calc(calc_crc),.crc_out(),.match(match_crc));

   always @(posedge rx_clk)
     if(reset)
       pause_quanta_rcvd <= 0;
     else if(rx_state == RX_PAUSE_STORE_MSB)
       pause_quanta_rcvd[15:8] <= rxd_d1;
     else if(rx_state == RX_PAUSE_STORE_LSB)
       pause_quanta_rcvd[7:0] <= rxd_d1;
   
   assign rx_clk 	  = GMII_RX_CLK;
   
endmodule // simple_gemac_rx
