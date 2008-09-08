
module simple_uart
  #(parameter TXDEPTH = 1,
    parameter RXDEPTH = 1)
    (input clk_i, input rst_i,
     input we_i, input stb_i, input cyc_i, output reg ack_o,
     input [2:0] adr_i, input [31:0] dat_i, output reg [31:0] dat_o,
     output rx_int_o, output tx_int_o, output tx_o, input rx_i, output baud_o);
   
   // Register Map
   localparam SUART_CLKDIV = 0;
   localparam SUART_TXLEVEL = 1;
   localparam SUART_RXLEVEL = 2;
   localparam SUART_TXCHAR = 3;
   localparam SUART_RXCHAR = 4;
   
   wire       wb_acc = cyc_i & stb_i;            // WISHBONE access
   wire       wb_wr  = wb_acc & we_i;            // WISHBONE write access
   
   reg [15:0] clkdiv;
   wire [7:0] rx_char;
   wire       tx_fifo_full, rx_fifo_empty;   
   wire [7:0] tx_fifo_level, rx_fifo_level;
   
   always @(posedge clk_i)
     if (rst_i)
       ack_o <= 1'b0;
     else
       ack_o <= wb_acc & ~ack_o;
   
   always @(posedge clk_i)
     if (rst_i)
       clkdiv <= 0;
     else if (wb_wr)
       case(adr_i)
	 SUART_CLKDIV : clkdiv <= dat_i[15:0];
       endcase // case(adr_i)
   
   always @(posedge clk_i)
     case (adr_i)
       SUART_TXLEVEL : dat_o <= tx_fifo_level;
       SUART_RXLEVEL : dat_o <= rx_fifo_level;
       SUART_RXCHAR : dat_o <= rx_char;
     endcase // case(adr_i)
   
   simple_uart_tx #(.DEPTH(TXDEPTH)) simple_uart_tx
     (.clk(clk_i),.rst(rst_i),
      .fifo_in(dat_i[7:0]),.fifo_write(ack_o && wb_wr && (adr_i == SUART_TXCHAR)),
      .fifo_level(tx_fifo_level),.fifo_full(tx_fifo_full),
      .clkdiv(clkdiv),.baudclk(baud_o),.tx(tx_o));
   
   simple_uart_rx #(.DEPTH(RXDEPTH)) simple_uart_rx
     (.clk(clk_i),.rst(rst_i),
      .fifo_out(rx_char),.fifo_read(ack_o && ~wb_wr && (adr_i == SUART_RXCHAR)),
      .fifo_level(rx_fifo_level),.fifo_empty(rx_fifo_empty),
      .clkdiv(clkdiv),.rx(rx_i));
   
   assign     tx_int_o = ~tx_fifo_full;
   assign     rx_int_o = ~rx_fifo_empty;
   
endmodule // simple_uart
