

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
   
   simple_gemac simple_gemac
     (.clk125(clk),  .reset(reset),
      .GMII_GTX_CLK(GMII_GTX_CLK), .GMII_TX_EN(GMII_TX_EN),  
      .GMII_TX_ER(GMII_TX_ER), .GMII_TXD(GMII_TXD),
      .GMII_RX_CLK(GMII_RX_CLK), .GMII_RX_DV(GMII_RX_DV),  
      .GMII_RX_ER(GMII_RX_ER), .GMII_RXD(GMII_RXD),
      .pause_req(pause_req), .pause_time(pause_time),
      .rx_clk(rx_clk), .rx_data(rx_data),
      .rx_valid(rx_valid), .rx_error(rx_error), .rx_ack(rx_ack),
      .tx_clk(tx_clk), .tx_data(tx_data), 
      .tx_valid(tx_valid), .tx_error(tx_error), .tx_ack(tx_ack)
      );

   wire rx_ll_sof, rx_ll_eof, rx_ll_src_rdy, rx_ll_dst_rdy;
   wire rx_ll_sof2, rx_ll_eof2, rx_ll_src_rdy2, rx_ll_dst_rdy2;
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

   assign rx_ll_dst_rdy2 	= 1;

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
	SendFlowCtrl(16'h0000);  // Cancel flow control befor it expires
	@(posedge clk); 

	SendPacket_to_ll8(8'hAA,10);    // This packet gets dropped by the filters
	repeat (10)
	  @(posedge clk);

 	SendPacketFromFile_ll8(60);  // The rest are valid packets
	repeat (10)
	  @(posedge clk);

 	SendPacketFromFile_ll8(61);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_ll8(62);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_ll8(63);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_ll8(64);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_ll8(59);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_ll8(58);
	#100000 $finish;
     end

    initial
     begin
	#90000;
	@(posedge clk);
	FORCE_DAT_ERR <= 8'h10;
	@(posedge clk);
	FORCE_DAT_ERR <= 8'h00;
     end

   initial
     begin
	#116000;
	@(posedge clk);
	FORCE_ERR <= 1;
	@(posedge clk);
	FORCE_ERR <= 0;
     end

   // Tests: Send and recv flow control, send and receive good packets, RX CRC err, RX_ER
   // Still need to test: RX overrun, TX underrun
   
   /*
   always @(posedge clk)
     if(GMII_TX_EN)
       $display("%x",GMII_TXD);
   */

   always @(posedge clk)
     if(rx_ll_src_rdy2 & rx_ll_dst_rdy2)
       $display("RX-PKT SOF %d EOF %d ERR%d DAT %x, TIME=%d",rx_ll_sof2,rx_ll_eof2,rx_ll_error2,rx_ll_data2,$time);
   
endmodule // simple_gemac_tb

/*
    if ( !$value$plusargs( "rom=%s", ROMFile ) )
        begin
           $display( "Using default ROM file, 'flash.rom'" );
           ROMFile = "flash.rom";
        end
      else
	$display( "Using %s as ROM file.", ROMFile);
      
      #1 $readmemh( ROMFile,rom );     
   end
 */
