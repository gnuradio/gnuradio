

module simple_gemac_tb;

     
   reg clk = 0;
   reg reset = 1;

   initial #1000 reset = 0;
   always #50 clk = ~clk;

   wire GMII_RX_DV, GMII_RX_ER, GMII_TX_EN, GMII_TX_ER;
   wire [7:0] GMII_RXD, GMII_TXD;

   wire rx_valid, rx_error, rx_ack;
   wire tx_ack;
   reg tx_valid = 0, tx_error = 0;
   
   wire [7:0] rx_data;
   reg [7:0] tx_data;
   
   wire [15:0] pause_time = 16'hBEEF;
   reg pause_req = 0;
   
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

   task SendFlowCtrl;
     begin
	$display("Sending Flow Control");
	@(posedge clk);
	pause_req <= 1;
	@(posedge clk);
	pause_req <= 0;
     end
   endtask // SendFlowCtrl

   reg [31:0] count;
   task SendPacket;
      input [7:0] data_start;
      input [31:0] data_len;
      begin
	 $display("Sending Packet");
	 count <= 0;
	 tx_data  <= data_start;
	 tx_error <= 0;
	 tx_valid <= 1;
	 while(~tx_ack)
	   @(posedge tx_clk);
	 while(count < data_len)
	   begin
	      tx_data <= tx_data + 1;
	      count   <= count + 1;
	      @(posedge clk);
	   end
	 tx_valid <= 0;
	 @(posedge tx_clk);
      end
   endtask // SendPacket
   	
   initial $dumpfile("simple_gemac_tb.vcd");
   initial $dumpvars(0,simple_gemac_tb);
	
   initial
     begin
	@(negedge reset);
	repeat (20)
	  @(posedge clk);
	SendFlowCtrl;
	repeat (100)
	  @(posedge clk);
	SendPacket(8'hAA,10);
	repeat (1000)
	  @(posedge clk);
	$finish;
     end
   
endmodule // simple_gemac_tb
