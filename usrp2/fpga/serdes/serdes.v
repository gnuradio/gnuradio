
// SERDES TX and RX along with all flow control logic

module serdes
  #(parameter TXFIFOSIZE = 9,
    parameter RXFIFOSIZE = 9)
    (input clk, input rst,
     // TX side
     output ser_tx_clk, output [15:0] ser_t, output ser_tklsb, output ser_tkmsb,
     input [31:0] rd_dat_i, output rd_read_o, output rd_done_o, output rd_error_o,
     input rd_sop_i, input rd_eop_i,
     // RX side
     input ser_rx_clk, input [15:0] ser_r, input ser_rklsb, input ser_rkmsb,
     output [31:0] wr_dat_o, output wr_write_o, output wr_done_o, output wr_error_o,
     input wr_ready_i, input wr_full_i,

     output [15:0] tx_occupied, output tx_full, output tx_empty,
     output [15:0] rx_occupied, output rx_full, output rx_empty,

     output serdes_link_up,
     
     output [31:0] debug0, 
     output [31:0] debug1);

   wire [15:0] fifo_space;
   wire        xon_rcvd, xoff_rcvd, inhibit_tx, send_xon, send_xoff, sent;
   wire [31:0] debug_rx, debug_tx;

   serdes_tx #(.FIFOSIZE(TXFIFOSIZE)) serdes_tx
     (.clk(clk),.rst(rst),
      .ser_tx_clk(ser_tx_clk),.ser_t(ser_t),.ser_tklsb(ser_tklsb),.ser_tkmsb(ser_tkmsb),
      .rd_dat_i(rd_dat_i),.rd_read_o(rd_read_o),.rd_done_o(rd_done_o),.rd_error_o(rd_error_o),
      .rd_sop_i(rd_sop_i),.rd_eop_i(rd_eop_i),
      .inhibit_tx(inhibit_tx), .send_xon(send_xon), .send_xoff(send_xoff), .sent(sent),
      .fifo_occupied(tx_occupied),.fifo_full(tx_full),.fifo_empty(tx_empty),
      .debug(debug_tx) );
   
   serdes_rx #(.FIFOSIZE(RXFIFOSIZE)) serdes_rx
     (.clk(clk),.rst(rst),
      .ser_rx_clk(ser_rx_clk),.ser_r(ser_r),.ser_rklsb(ser_rklsb),.ser_rkmsb(ser_rkmsb),
      .wr_dat_o(wr_dat_o),.wr_write_o(wr_write_o),.wr_done_o(wr_done_o),.wr_error_o(wr_error_o),
      .wr_ready_i(wr_ready_i),.wr_full_i(wr_full_i),
      .fifo_space(fifo_space), .xon_rcvd(xon_rcvd), .xoff_rcvd(xoff_rcvd),
      .fifo_occupied(rx_occupied),.fifo_full(rx_full),.fifo_empty(rx_empty),
      .serdes_link_up(serdes_link_up), .debug(debug_rx) );

   serdes_fc_tx serdes_fc_tx
     (.clk(clk),.rst(rst),
      .xon_rcvd(xon_rcvd),.xoff_rcvd(xoff_rcvd),.inhibit_tx(inhibit_tx) );

   serdes_fc_rx #(.LWMARK(32),.HWMARK(128)) serdes_fc_rx
     (.clk(clk),.rst(rst),
      .fifo_space(fifo_space),.send_xon(send_xon),.send_xoff(send_xoff),.sent(sent) );

   //assign      debug = { fifo_space, send_xon, send_xoff, debug_rx[13:0] };
   //assign      debug = debug_rx;

   assign      debug0 = { { debug_tx[3:0] /* xfer_active,state[2:0] */, rd_read_o, rd_done_o, rd_sop_i, rd_eop_i },
			  { debug_tx[5:4] /* full,empty */ , inhibit_tx, send_xon, send_xoff, sent, ser_tkmsb, ser_tklsb},
			  { ser_t[15:8] },
			  { ser_t[7:0] } };
   
   assign      debug1 = { { debug_rx[7:0] }, /*  odd,xfer_active,sop_i,eop_i,error_i,state[2:0] */
			  { wr_write_o, wr_error_o, wr_ready_i, wr_done_o,  xon_rcvd, xoff_rcvd, ser_rkmsb, ser_rklsb },
			  { ser_r[15:8] },
			  { ser_r[7:0] } };
endmodule // serdes
