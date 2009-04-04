

module simple_gemac_tb;
`include "eth_tasks.v"
     
   reg clk = 0;
   reg reset = 1;

   initial #1000 reset = 0;
   always #50 clk = ~clk;

   wire GMII_RX_DV, GMII_RX_ER, GMII_TX_EN, GMII_TX_ER, GMII_GTX_CLK;
   wire [7:0] GMII_RXD, GMII_TXD;

   wire rx_valid, rx_error, rx_ack;
   wire tx_ack, tx_valid, tx_error;
   
   wire [7:0] rx_data, tx_data;
   
   reg [15:0] pause_time;
   reg pause_req      = 0;

   wire GMII_RX_CLK   = GMII_GTX_CLK;

   reg [7:0] FORCE_DAT_ERR = 0;
   reg FORCE_ERR = 0;
   
   // Loopback
   assign GMII_RX_DV  = GMII_TX_EN;
   assign GMII_RX_ER  = GMII_TX_ER | FORCE_ERR;
   assign GMII_RXD    = GMII_TXD ^ FORCE_DAT_ERR;

   wire [47:0] ucast_addr = 48'hF1F2_F3F4_F5F6;
   wire [47:0] mcast_addr = 0;
   wire        pass_ucast  =1, pass_mcast=0, pass_bcast=1, pass_pause=0, pass_all=0;
   
   simple_gemac simple_gemac
     (.clk125(clk),  .reset(reset),
      .GMII_GTX_CLK(GMII_GTX_CLK), .GMII_TX_EN(GMII_TX_EN),  
      .GMII_TX_ER(GMII_TX_ER), .GMII_TXD(GMII_TXD),
      .GMII_RX_CLK(GMII_RX_CLK), .GMII_RX_DV(GMII_RX_DV),  
      .GMII_RX_ER(GMII_RX_ER), .GMII_RXD(GMII_RXD),
      .pause_req(pause_req), .pause_time(pause_time), .pause_en(1),
      .ucast_addr(ucast_addr), .mcast_addr(mcast_addr),
      .pass_ucast(pass_ucast), .pass_mcast(pass_mcast), .pass_bcast(pass_bcast), 
      .pass_pause(pass_pause), .pass_all(pass_all),
      .rx_clk(rx_clk), .rx_data(rx_data),
      .rx_valid(rx_valid), .rx_error(rx_error), .rx_ack(rx_ack),
      .tx_clk(tx_clk), .tx_data(tx_data), 
      .tx_valid(tx_valid), .tx_error(tx_error), .tx_ack(tx_ack)
      );

   wire rx_ll_sof, rx_ll_eof, rx_ll_src_rdy, rx_ll_dst_rdy;
   wire rx_ll_sof2, rx_ll_eof2, rx_ll_src_rdy2;
   reg rx_ll_dst_rdy2 = 1;
   wire [7:0] rx_ll_data, rx_ll_data2;
   wire rx_ll_error, rx_ll_error2;
   
   rxmac_to_ll8 rx_adapt
     (.clk(clk), .reset(reset), .clear(0),
      .rx_data(rx_data), .rx_valid(rx_valid), .rx_error(rx_error), .rx_ack(rx_ack),
      .ll_data(rx_ll_data), .ll_sof(rx_ll_sof), .ll_eof(rx_ll_eof), .ll_error(rx_ll_error),
      .ll_src_rdy(rx_ll_src_rdy), .ll_dst_rdy(rx_ll_dst_rdy));

   ll8_shortfifo rx_sfifo
     (.clk(clk), .reset(reset), .clear(0),
      .datain(rx_ll_data), .sof_i(rx_ll_sof), .eof_i(rx_ll_eof),
      .error_i(rx_ll_error), .src_rdy_i(rx_ll_src_rdy), .dst_rdy_o(rx_ll_dst_rdy),
      .dataout(rx_ll_data2), .sof_o(rx_ll_sof2), .eof_o(rx_ll_eof2),
      .error_o(rx_ll_error2), .src_rdy_o(rx_ll_src_rdy2), .dst_rdy_i(rx_ll_dst_rdy2));

   wire tx_ll_sof, tx_ll_eof, tx_ll_src_rdy, tx_ll_dst_rdy;
   reg tx_ll_sof2=0, tx_ll_eof2=0;
   reg tx_ll_src_rdy2 = 0;
   wire tx_ll_dst_rdy2;
   wire [7:0] tx_ll_data;
   reg [7:0] tx_ll_data2 = 0;
   wire tx_ll_error;
   wire tx_ll_error2 = 0;

   ll8_shortfifo tx_sfifo
     (.clk(clk), .reset(reset), .clear(clear),
      .datain(tx_ll_data2), .sof_i(tx_ll_sof2), .eof_i(tx_ll_eof2),
      .error_i(tx_ll_error2), .src_rdy_i(tx_ll_src_rdy2), .dst_rdy_o(tx_ll_dst_rdy2),
      .dataout(tx_ll_data), .sof_o(tx_ll_sof), .eof_o(tx_ll_eof),
      .error_o(tx_ll_error), .src_rdy_o(tx_ll_src_rdy), .dst_rdy_i(tx_ll_dst_rdy));
   
   ll8_to_txmac ll8_to_txmac
     (.clk(clk), .reset(reset), .clear(clear),
      .ll_data(tx_ll_data), .ll_sof(tx_ll_sof), .ll_eof(tx_ll_eof),
      .ll_src_rdy(tx_ll_src_rdy), .ll_dst_rdy(tx_ll_dst_rdy),
      .tx_data(tx_data), .tx_valid(tx_valid), .tx_error(tx_error), .tx_ack(tx_ack));

   initial $dumpfile("simple_gemac_tb.vcd");
   initial $dumpvars(0,simple_gemac_tb);

   integer i; 
   reg [7:0] pkt_rom[0:65535];
   reg [1023:0] ROMFile;
   
   initial
     for (i=0;i<65536;i=i+1)
       pkt_rom[i] <= 8'h0;

   initial
     begin
	@(negedge reset);
	repeat (10)
	  @(posedge clk);
	SendFlowCtrl(16'h0007);  // Send flow control
	@(posedge clk);
	#30000;
	@(posedge clk);
	SendFlowCtrl(16'h0009);  // Increas flow control before it expires
	#10000;
	@(posedge clk);
	SendFlowCtrl(16'h0000);  // Cancel flow control before it expires
	@(posedge clk); 

	SendPacket_to_ll8(8'hAA,10);    // This packet gets dropped by the filters
	repeat (10)
	  @(posedge clk);

 	SendPacketFromFile_ll8(60,0,0);  // The rest are valid packets
	repeat (10)
	  @(posedge clk);

 	SendPacketFromFile_ll8(61,0,0);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_ll8(62,0,0);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_ll8(63,0,0);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_ll8(64,0,0);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_ll8(59,0,0);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_ll8(58,0,0);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_ll8(100,0,0);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_ll8(200,150,30);  // waiting 14 empties the fifo, 15 underruns
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_ll8(100,0,30);
	#10000 $finish;
     end

   // Force a CRC error
    initial
     begin
	#90000;
	@(posedge clk);
	FORCE_DAT_ERR <= 8'h10;
	@(posedge clk);
	FORCE_DAT_ERR <= 8'h00;
     end

   // Force an RX_ER error (i.e. link loss)
   initial
     begin
	#116000;
	@(posedge clk);
	FORCE_ERR <= 1;
	@(posedge clk);
	FORCE_ERR <= 0;
     end

   // Cause receive fifo to fill, causing an RX overrun
   initial
     begin
	#126000;
	@(posedge clk);
	rx_ll_dst_rdy2 <= 0;
	repeat (30)          // Repeat of 14 fills the shortfifo, but works.  15 overflows
	  @(posedge clk);
	rx_ll_dst_rdy2 <= 1;
     end
   
   // Tests: Send and recv flow control, send and receive good packets, RX CRC err, RX_ER, RX overrun, TX underrun
   // Still need to test: CRC errors on Pause Frames
   
   always @(posedge clk)
     if(rx_ll_src_rdy2 & rx_ll_dst_rdy2)
       begin
	  if(rx_ll_sof2 & ~rx_ll_eof2)
	    $display("RX-PKT-START %d",$time);
	  $display("RX-PKT SOF %d EOF %d ERR%d DAT %x",rx_ll_sof2,rx_ll_eof2,rx_ll_error2,rx_ll_data2);
	  if(rx_ll_eof2 & ~rx_ll_sof2)
	    $display("RX-PKT-END %d",$time);
       end
   
endmodule // simple_gemac_tb
