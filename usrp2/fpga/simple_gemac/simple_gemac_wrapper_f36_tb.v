

module simple_gemac_wrapper_f36_tb;
`include "eth_tasks_f36.v"
     
   reg clk     = 0;
   reg reset   = 1;

   initial #1000 reset = 0;
   always #50 clk = ~clk;

   reg wb_clk 	= 0;
   wire wb_rst 	= reset;
   always #173 wb_clk = ~wb_clk;
       
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


   wire rx_ll_sof, rx_ll_eof, rx_ll_src_rdy, rx_ll_dst_rdy;
   wire rx_ll_sof2, rx_ll_eof2, rx_ll_src_rdy2;
   wire 	rx_ll_dst_rdy2;
   wire [7:0] rx_ll_data, rx_ll_data2;
   wire rx_ll_error, rx_ll_error2;

   wire [31:0] wb_dat_o;
   reg [31:0]  wb_dat_i;
   reg [7:0]   wb_adr;
   reg 	       wb_stb=0, wb_cyc=0, wb_we=0;
   wire        wb_ack;

   reg [35:0]  tx_f36_dat;
   reg 	       tx_f36_src_rdy;
   wire        tx_f36_dst_rdy;

   wire [35:0] rx_f36_dat;
   wire        rx_f36_src_rdy;
   reg 	       rx_f36_dst_rdy  = 1;
   
   simple_gemac_wrapper simple_gemac_wrapper
     (.clk125(clk),  .reset(reset),
      .GMII_GTX_CLK(GMII_GTX_CLK), .GMII_TX_EN(GMII_TX_EN),  
      .GMII_TX_ER(GMII_TX_ER), .GMII_TXD(GMII_TXD),
      .GMII_RX_CLK(GMII_RX_CLK), .GMII_RX_DV(GMII_RX_DV),  
      .GMII_RX_ER(GMII_RX_ER), .GMII_RXD(GMII_RXD),
      .pause_req(pause_req), .pause_time(pause_time),
      .rx_clk(rx_clk), .rx_ll_data(rx_ll_data), .rx_ll_sof(rx_ll_sof),
      .rx_ll_eof(rx_ll_eof), .rx_ll_src_rdy(rx_ll_src_rdy), .rx_ll_dst_rdy(rx_ll_dst_rdy),
      .tx_clk(tx_clk), .tx_ll_data(tx_ll_data), .tx_ll_sof(tx_ll_sof),
      .tx_ll_eof(tx_ll_eof), .tx_ll_src_rdy(tx_ll_src_rdy), .tx_ll_dst_rdy(tx_ll_dst_rdy),
      .wb_clk(wb_clk), .wb_rst(wb_rst), .wb_stb(wb_stb), .wb_cyc(wb_cyc), .wb_ack(wb_ack),
      .wb_we(wb_we), .wb_adr(wb_adr), .wb_dat_i(wb_dat_i), .wb_dat_o(wb_dat_o),
      .mdio(mdio), .mdc(mdc) );

   wire        rx_ll_dst_rdy2_n;
   assign        rx_ll_dst_rdy2  = ~rx_ll_dst_rdy2_n;
      
   ll8_shortfifo rx_sfifo
     (.clk(clk), .reset(reset), .clear(0),
      .datain(rx_ll_data), .sof_i(rx_ll_sof), .eof_i(rx_ll_eof),
      .error_i(rx_ll_error), .src_rdy_i(rx_ll_src_rdy), .dst_rdy_o(rx_ll_dst_rdy),
      .dataout(rx_ll_data2), .sof_o(rx_ll_sof2), .eof_o(rx_ll_eof2),
      .error_o(rx_ll_error2), .src_rdy_o(rx_ll_src_rdy2), .dst_rdy_i(rx_ll_dst_rdy2));

   ll8_to_fifo36 ll8_to_fifo36
     (.clk(clk), .reset(reset), .clear(0),
      .ll_data(rx_ll_data2), .ll_sof_n(~rx_ll_sof2), .ll_eof_n(~rx_ll_eof2),
      .ll_src_rdy_n(~rx_ll_src_rdy2), .ll_dst_rdy_n(rx_ll_dst_rdy2_n),
      .f36_data(rx_f36_dat), .f36_src_rdy_o(rx_f36_src_rdy), .f36_dst_rdy_i(rx_f36_dst_rdy));
   
   wire tx_ll_sof, tx_ll_eof, tx_ll_src_rdy, tx_ll_dst_rdy;
   wire tx_ll_sof2_n, tx_ll_eof2_n;
   wire tx_ll_src_rdy2_n, tx_ll_dst_rdy2;
   wire [7:0] tx_ll_data, tx_ll_data2;
   wire tx_ll_error;
   wire tx_ll_error2 = 0;

   fifo36_to_ll8 fifo36_to_ll8
     (.clk(clk), .reset(reset), .clear(clear),
      .f36_data(tx_f36_dat), .f36_src_rdy_i(tx_f36_src_rdy), .f36_dst_rdy_o(tx_f36_dst_rdy),
      .ll_data(tx_ll_data2), .ll_sof_n(tx_ll_sof2_n), .ll_eof_n(tx_ll_eof2_n),
      .ll_src_rdy_n(tx_ll_src_rdy2_n), .ll_dst_rdy_n(~tx_ll_dst_rdy2));
   
   ll8_shortfifo tx_sfifo
     (.clk(clk), .reset(reset), .clear(clear),
      .datain(tx_ll_data2), .sof_i(~tx_ll_sof2_n), .eof_i(~tx_ll_eof2_n),
      .error_i(tx_ll_error2), .src_rdy_i(~tx_ll_src_rdy2_n), .dst_rdy_o(tx_ll_dst_rdy2),
      .dataout(tx_ll_data), .sof_o(tx_ll_sof), .eof_o(tx_ll_eof),
      .error_o(tx_ll_error), .src_rdy_o(tx_ll_src_rdy), .dst_rdy_i(tx_ll_dst_rdy));
   
   initial $dumpfile("simple_gemac_wrapper_f36_tb.vcd");
   initial $dumpvars(0,simple_gemac_wrapper_f36_tb);

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
	WishboneWR(0,6'b111001);
	WishboneWR(4,16'hF1F2);
	WishboneWR(8,32'hF3F4_F5F6);
	WishboneWR(12,16'h0000);
	WishboneWR(16,32'h0000_0000);
	
	@(posedge clk);
	SendFlowCtrl(16'h0007);  // Send flow control
	@(posedge clk);
	#30000;
	@(posedge clk);
	SendFlowCtrl(16'h0009);  // Increase flow control before it expires
	#10000;
	@(posedge clk);
	SendFlowCtrl(16'h0000);  // Cancel flow control before it expires
	@(posedge clk); 

	SendPacket_to_fifo36(8'hAA,10);    // This packet gets dropped by the filters
	repeat (10)
	  @(posedge clk);

 	SendPacketFromFile_fifo36(60,0,0);  // The rest are valid packets
	repeat (10)
	  @(posedge clk);

 	SendPacketFromFile_fifo36(61,0,0);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_fifo36(62,0,0);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_fifo36(63,0,0);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_fifo36(64,0,0);
	repeat (10)
	  @(posedge clk);
	SendPacketFromFile_fifo36(59,0,0);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_fifo36(58,0,0);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_fifo36(100,0,0);
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_fifo36(200,150,30);  // waiting 14 empties the fifo, 15 underruns
	repeat (1)
	  @(posedge clk);
	SendPacketFromFile_fifo36(100,0,30);
	#10000 $finish;
     end
/*
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
	rx_f36_dst_rdy <= 0;
	repeat (30)          // Repeat of 14 fills the shortfifo, but works.  15 overflows
	  @(posedge clk);
	rx_f36_dst_rdy <= 1;
     end
  */ 
   // Tests: Send and recv flow control, send and receive good packets, RX CRC err, RX_ER, RX overrun, TX underrun
   // Still need to test: CRC errors on Pause Frames, MDIO, wishbone

   task WishboneWR;
      input [7:0] adr;
      input [31:0] value;
      begin
	 wb_adr   <= adr;
	 wb_dat_i <= value;
	 wb_stb   <= 1;
	 wb_cyc   <= 1;
	 wb_we 	  <= 1;
	 while (~wb_ack)
	   @(posedge wb_clk);
	 @(posedge wb_clk);
	 wb_stb <= 0;
	 wb_cyc <= 0;
	 wb_we 	<= 0;
      end
   endtask // WishboneWR
   
   always @(posedge clk)
     if(rx_f36_src_rdy & rx_f36_dst_rdy)
       begin
	  if(rx_f36_dat[32] & ~rx_f36_dat[33])
	    $display("RX-PKT-START %d",$time);
	  $display("RX-PKT SOF %d EOF %d ERR %d OCC %d DAT %x",rx_f36_dat[32],rx_f36_dat[33],
		   &rx_f36_dat[33:32],rx_f36_dat[35:34],rx_f36_dat[31:0]);
	  if(rx_f36_dat[33] & ~rx_f36_dat[32])
	    $display("RX-PKT-END %d",$time);
	  if(rx_f36_dat[33] & rx_f36_dat[32])
	    $display("RX-PKT-ERROR %d",$time);
       end
   
endmodule // simple_gemac_wrapper_tb
