
module simple_gemac_wrapper
  (input clk125, input reset,
   // GMII
   output GMII_GTX_CLK, output GMII_TX_EN, output GMII_TX_ER, output [7:0] GMII_TXD,
   input GMII_RX_CLK, input GMII_RX_DV, input GMII_RX_ER, input [7:0] GMII_RXD,
   
   // Flow Control Interface
   input pause_req, input [15:0] pause_time,
   
   // RX Client Interface
   output rx_clk, output [7:0] rx_ll_data, output rx_ll_sof, output rx_ll_eof,
   output rx_ll_error, output rx_ll_src_rdy, input rx_ll_dst_rdy,
   
   // TX Client Interface
   output tx_clk, input [7:0] tx_ll_data, input tx_ll_sof, input tx_ll_eof,
   input tx_ll_src_rdy, output tx_ll_dst_rdy,
   
   // Wishbone Interface
   input wb_clk, input wb_rst, input wb_stb, input wb_cyc, output wb_ack, input wb_we,
   input [7:0] wb_adr, input [31:0] wb_dat_i, output [31:0] wb_dat_o,
   
   // MIIM
   inout mdio, output mdc );
   
   wire [7:0] rx_data, tx_data;
   wire       tx_clk, tx_valid, tx_error, tx_ack;
   wire       rx_clk, rx_valid, rx_error, rx_ack;

   wire [47:0] ucast_addr, mcast_addr;
   wire pass_ucast, pass_mcast, pass_bcast, pass_pause, pass_all, pause_en;

   wire rst_rxclk, rst_txclk;
   reset_sync reset_sync_tx (.clk(tx_clk),.reset_in(reset),.reset_out(tx_reset));
   reset_sync reset_sync_rx (.clk(rx_clk),.reset_in(reset),.reset_out(rx_reset));

   simple_gemac simple_gemac
     (.clk125(clk125),  .reset(reset),
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
   
   simple_gemac_wb simple_gemac_wb
     (.wb_clk(wb_clk), .wb_rst(wb_rst),
      .wb_cyc(wb_cyc), .wb_stb(wb_stb), .wb_ack(wb_ack), .wb_we(wb_we),
      .wb_adr(wb_adr), .wb_dat_i(wb_dat_i), .wb_dat_o(wb_dat_o),
      .mdio(mdio), .mdc(mdc),
      .ucast_addr(ucast_addr), .mcast_addr(mcast_addr),
      .pass_ucast(pass_ucast), .pass_mcast(pass_mcast), .pass_bcast(pass_bcast), 
      .pass_pause(pass_pause), .pass_all(pass_all), .pause_en(pause_en) );

   rxmac_to_ll8 rx_adapt
     (.clk(rx_clk), .reset(rx_reset), .clear(0),
      .rx_data(rx_data), .rx_valid(rx_valid), .rx_error(rx_error), .rx_ack(rx_ack),
      .ll_data(rx_ll_data), .ll_sof(rx_ll_sof), .ll_eof(rx_ll_eof), .ll_error(rx_ll_error),
      .ll_src_rdy(rx_ll_src_rdy), .ll_dst_rdy(rx_ll_dst_rdy));
   
   ll8_to_txmac ll8_to_txmac
     (.clk(tx_clk), .reset(tx_reset), .clear(0),
      .ll_data(tx_ll_data), .ll_sof(tx_ll_sof), .ll_eof(tx_ll_eof),
      .ll_src_rdy(tx_ll_src_rdy), .ll_dst_rdy(tx_ll_dst_rdy),
      .tx_data(tx_data), .tx_valid(tx_valid), .tx_error(tx_error), .tx_ack(tx_ack));

endmodule // simple_gemac_wrapper
