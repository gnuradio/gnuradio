
module simple_gemac_tx
  (input clk125, input reset,
   output GMII_GTX_CLK, output reg GMII_TX_EN, output reg GMII_TX_ER, output reg [7:0] GMII_TXD,
   output tx_clk, input [7:0] tx_data, input tx_valid, input tx_error, output tx_ack,
   input [7:0] ifg, input [47:0] mac_addr,
   input pause_req, input [15:0] pause_time,
   input pause_apply, output pause_applied
   );

   reg tx_en_pre, tx_er_pre;
   reg [7:0] txd_pre;
   
   assign GMII_GTX_CLK 	= clk125;
   assign tx_clk 	= clk125;
   
   reg [7:0] tx_state;
   reg [7:0] ifg_ctr;
   reg [15:0] frame_len_ctr;
   reg [7:0] pause_ctr, pause_dat;

   wire in_ifg 		     = (ifg_ctr != 0);

   wire [31:0] crc_out;
   
   localparam MIN_FRAME_LEN  = 64 + 8 - 4; // Min frame length includes preamble but not CRC
   localparam MAX_FRAME_LEN  = 8192;       // How big are the jumbo frames we want to handle?
   always @(posedge tx_clk)
     if(reset |(tx_state == TX_IDLE))
       frame_len_ctr 	    <= 0;
     else
       frame_len_ctr 	    <= frame_len_ctr + 1;
   
   localparam TX_IDLE 	     = 0;
   localparam TX_PREAMBLE    = 1;
   localparam TX_SOF_DEL     = TX_PREAMBLE + 7;
   localparam TX_FIRSTBYTE   = TX_SOF_DEL + 1;
   localparam TX_IN_FRAME    = TX_FIRSTBYTE + 1;
   localparam TX_IN_FRAME_2  = TX_IN_FRAME + 1;
   localparam TX_PAD 	     = TX_IN_FRAME_2 + 1;
   localparam TX_CRC_0 	     = 16;
   localparam TX_CRC_1 	     = TX_CRC_0 + 1;
   localparam TX_CRC_2 	     = TX_CRC_0 + 2;
   localparam TX_CRC_3 	     = TX_CRC_0 + 3;
   localparam TX_ERROR 	     = 32;
   localparam TX_PAUSE 	     = 56;
   localparam TX_PAUSE_SOF   = 63;
   localparam TX_PAUSE_END   = TX_PAUSE_SOF + 18;

   reg send_pause;
   reg [15:0] pause_time_held;

   always @(posedge tx_clk)
     if(reset)
       send_pause      <= 0;
     else if(pause_req)
       send_pause      <= 1;
     else if(tx_state == TX_PAUSE)
       send_pause      <= 0;

   always @(posedge tx_clk)
     if(pause_req)
       pause_time_held <= pause_time;
   
   always @(posedge tx_clk)
     if(reset)
       tx_state        <= TX_IDLE;
     else 
       case(tx_state)
	 TX_IDLE :
	   if(~in_ifg)
	     if(send_pause)
	       tx_state <= TX_PAUSE;
	     else if(tx_valid)
	       tx_state <= TX_PREAMBLE;
	 TX_FIRSTBYTE :
	   if(tx_error)
	     tx_state <= TX_ERROR;   // underrun
	   else if(~tx_valid)
	     tx_state <= TX_PAD;
	   else
	     tx_state <= TX_IN_FRAME;
	 TX_IN_FRAME :
	   if(tx_error)
	     tx_state <= TX_ERROR;   // underrun
	   else if(~tx_valid)
	     tx_state <= TX_PAD;
	   else if(frame_len_ctr == MIN_FRAME_LEN - 1)
	     tx_state <= TX_IN_FRAME_2;
	 TX_IN_FRAME_2 :
	   if(tx_error)
	     tx_state <= TX_ERROR;   // underrun
	   else if(~tx_valid)
	     tx_state <= TX_CRC_0;
	 TX_PAD :
	   if(frame_len_ctr == MIN_FRAME_LEN)
	     tx_state <= TX_CRC_0;
	 TX_CRC_3 :
	   tx_state <= TX_IDLE;
	 TX_ERROR :
	   tx_state <= TX_IDLE;
	 TX_PAUSE_END :
	   tx_state <= TX_PAD;
	 default :
	   tx_state <= tx_state + 1;
       endcase // case (tx_state)

   always @(posedge tx_clk)
     if(reset)
       begin
	  tx_en_pre <= 0;
	  tx_er_pre <= 0;
	  txd_pre   <= 0;
       end
     else
       casex(tx_state)
	 TX_IDLE :
	   begin
	      tx_en_pre <= 0;
	      tx_er_pre <= 0;
	      txd_pre <= 0;
	   end
	 TX_PREAMBLE, TX_PAUSE :
	   begin
	      txd_pre 	 <= 8'h55;
	      tx_en_pre <= 1;
	   end
	 TX_SOF_DEL, TX_PAUSE_SOF :
	   txd_pre <= 8'hD5;
	 TX_FIRSTBYTE, TX_IN_FRAME, TX_IN_FRAME_2 :
	   txd_pre <= tx_valid ? tx_data : 0;
	 TX_ERROR :
	   begin
	      tx_er_pre <= 1;
	      txd_pre 	 <= 0;
	   end
	 TX_CRC_3 :
	   tx_en_pre <= 0;
	 TX_PAD :
	   txd_pre <= 0;
 
	 8'b01xx_xxxx :  // In Pause Frame
	   txd_pre <= pause_dat;
       endcase // case (tx_state)

   localparam SGE_FLOW_CTRL_ADDR  = 48'h01_80_C2_00_00_01;
   always @(posedge tx_clk)
     case(tx_state)
       TX_PAUSE_SOF :
	 pause_dat    <= SGE_FLOW_CTRL_ADDR[47:40];  // Note everything must be 1 cycle early
       TX_PAUSE_SOF + 1:
	 pause_dat    <= SGE_FLOW_CTRL_ADDR[39:32];
       TX_PAUSE_SOF + 2:
	 pause_dat    <= SGE_FLOW_CTRL_ADDR[31:24];
       TX_PAUSE_SOF + 3:
	 pause_dat    <= SGE_FLOW_CTRL_ADDR[23:16];
       TX_PAUSE_SOF + 4:
	 pause_dat    <= SGE_FLOW_CTRL_ADDR[15:8];
       TX_PAUSE_SOF + 5:
	 pause_dat    <= SGE_FLOW_CTRL_ADDR[7:0];
       TX_PAUSE_SOF + 6:
	 pause_dat    <= mac_addr[47:40];
       TX_PAUSE_SOF + 7:
	 pause_dat    <= mac_addr[39:32];
       TX_PAUSE_SOF + 8:
	 pause_dat    <= mac_addr[31:24];
       TX_PAUSE_SOF + 9:
	 pause_dat    <= mac_addr[23:16];
       TX_PAUSE_SOF + 10:
	 pause_dat    <= mac_addr[15:8];
       TX_PAUSE_SOF + 11:
	 pause_dat <= mac_addr[7:0];
       TX_PAUSE_SOF + 12:
	 pause_dat <= 8'h88;   // Type = 8808 = MAC ctrl frame
       TX_PAUSE_SOF + 13:
	 pause_dat <= 8'h08;
       TX_PAUSE_SOF + 14:
	 pause_dat <= 8'h00;   // Opcode = 0001 = PAUSE
       TX_PAUSE_SOF + 15:
	 pause_dat <= 8'h01;
       TX_PAUSE_SOF + 16:
	 pause_dat <= pause_time_held[15:8];
       TX_PAUSE_SOF + 17:
	 pause_dat <= pause_time_held[7:0];
     endcase // case (tx_state)
   
   wire start_ifg   = (tx_state == TX_CRC_3);
   always @(posedge tx_clk)
     if(reset)
       ifg_ctr 	   <= 100;
     else if(start_ifg)
       ifg_ctr 	   <= ifg;
     else if(ifg_ctr != 0)
       ifg_ctr 	   <= ifg_ctr - 1;

   wire clear_crc   = (tx_state == TX_IDLE);

   wire calc_crc_pre = (tx_state==TX_FIRSTBYTE)||(tx_state==TX_IN_FRAME)||
	((tx_state==TX_IN_FRAME_2)&tx_valid )||(tx_state==TX_PAD )||(tx_state[6]);
   reg calc_crc;
   always @(posedge tx_clk)
     calc_crc <= calc_crc_pre;
   
   crc crc(.clk(tx_clk), .reset(reset), .clear(clear_crc),
	    .data(txd_pre), .calc(calc_crc), .crc_out(crc_out));

   assign tx_ack    = (tx_state == TX_FIRSTBYTE);

   reg tx_valid_d1;
   always @(posedge tx_clk)
     tx_valid_d1   <= tx_valid;
   
   always @(posedge tx_clk) 
     begin
	GMII_TX_EN <= tx_en_pre;
	GMII_TX_ER <= tx_er_pre;
	case(tx_state)
	  TX_CRC_0 :
	    GMII_TXD <= crc_out[31:24];
	  TX_CRC_1 :
	    GMII_TXD <= crc_out[23:16];
	  TX_CRC_2 :
	    GMII_TXD <= crc_out[15:8];
	  TX_CRC_3 :
	    GMII_TXD <= crc_out[7:0];
//	  TX_IN_FRAME : 
//	  TX_PAD :
//	    GMII_TXD <= tx_valid_d1 ? txd_pre : 0;
	  default :
	    GMII_TXD <= txd_pre;
	endcase // case (tx_state)
     end
   
endmodule // simple_gemac_tx
