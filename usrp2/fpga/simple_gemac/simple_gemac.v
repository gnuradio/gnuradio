
module simple_gemac
  (input clk125, input reset,
   // GMII
   output GMII_GTX_CLK, output GMII_TX_EN, output GMII_TX_ER, output [7:0] GMII_TXD,
   input GMII_RX_CLK, input GMII_RX_DV, input GMII_RX_ER, input [7:0] GMII_RXD,

   // Flow Control Interface
   input pause_req, input [15:0] pause_time_req, input pause_respect_en,

   // Settings
   input [47:0] ucast_addr, input [47:0] mcast_addr,
   input pass_ucast, input pass_mcast, input pass_bcast, input pass_pause, input pass_all,
   
   // RX Client Interface
   output rx_clk, output [7:0] rx_data, output rx_valid, output rx_error, output rx_ack,

   // TX Client Interface
   output tx_clk, input [7:0] tx_data, input tx_valid, input tx_error, output tx_ack,

   output [31:0] debug
   );

   localparam SGE_IFG 		     = 8'd12;  // 12 should be the absolute minimum

   wire rst_rxclk, rst_txclk;   
   reset_sync reset_sync_tx (.clk(tx_clk),.reset_in(reset),.reset_out(rst_txclk));
   reset_sync reset_sync_rx (.clk(rx_clk),.reset_in(reset),.reset_out(rst_rxclk));

   wire [15:0] pause_quanta_rcvd;
   wire        pause_rcvd, pause_apply, paused;
   
   simple_gemac_tx simple_gemac_tx
     (.clk125(clk125),.reset(rst_txclk),
      .GMII_GTX_CLK(GMII_GTX_CLK), .GMII_TX_EN(GMII_TX_EN),
      .GMII_TX_ER(GMII_TX_ER), .GMII_TXD(GMII_TXD),
      .tx_clk(tx_clk), .tx_data(tx_data), .tx_valid(tx_valid), .tx_error(tx_error), .tx_ack(tx_ack),
      .ifg(SGE_IFG), .mac_addr(ucast_addr),
      .pause_req(pause_req), .pause_time(pause_time_req),  // We request flow control
      .pause_apply(pause_apply), .paused(paused)  // We respect flow control
      );

   simple_gemac_rx simple_gemac_rx
     (.reset(rst_rxclk),
      .GMII_RX_CLK(GMII_RX_CLK), .GMII_RX_DV(GMII_RX_DV), 
      .GMII_RX_ER(GMII_RX_ER), .GMII_RXD(GMII_RXD),
      .rx_clk(rx_clk), .rx_data(rx_data), .rx_valid(rx_valid), .rx_error(rx_error), .rx_ack(rx_ack),
      .ucast_addr(ucast_addr), .mcast_addr(mcast_addr),
      .pass_ucast(pass_ucast), .pass_mcast(pass_mcast), .pass_bcast(pass_bcast), 
      .pass_pause(pass_pause), .pass_all(pass_all),
      .pause_quanta_rcvd(pause_quanta_rcvd), .pause_rcvd(pause_rcvd),
      .debug(debug)
      );

   flow_ctrl_tx flow_ctrl_tx
     (.rst(rst_txclk), .tx_clk(tx_clk),
      .tx_pause_en(pause_respect_en),
      .pause_quanta(pause_quanta_rcvd), // 16 bit value
      .pause_quanta_val(pause_rcvd),
      .pause_apply(pause_apply),
      .paused(paused)
      );

endmodule // simple_gemac
