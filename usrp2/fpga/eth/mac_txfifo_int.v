
module mac_txfifo_int
  (input clk, input rst, input mac_clk,

   // To MAC
   input Tx_mac_wa,
   output Tx_mac_wr,
   output [31:0] Tx_mac_data,
   output [1:0] Tx_mac_BE,
   output Tx_mac_sop,
   output Tx_mac_eop,

   // To buffer interface
   input [31:0] rd_dat_i,
   output rd_read_o,
   output rd_done_o,
   output rd_error_o,
   input rd_sop_i,
   input rd_eop_i,

   // FIFO Status
   output [15:0] fifo_occupied,
   output fifo_full,
   output fifo_empty  );

   wire  empty, full, sfifo_write, sfifo_read;
   wire [33:0] sfifo_in, sfifo_out;

   /*
   shortfifo #(.WIDTH(34)) txmac_sfifo
     (.clk(clk),.rst(rst),.clear(0),
      .datain(sfifo_in),.write(sfifo_write),.full(full),
      .dataout(sfifo_out),.read(sfifo_read),.empty(empty));
    */
   fifo_xlnx_512x36_2clk mac_tx_fifo_2clk
     (.rst(rst),
      .wr_clk(clk),.din({2'b0,sfifo_in}),.full(full),.wr_en(sfifo_write),.wr_data_count(fifo_occupied[8:0]),
      .rd_clk(mac_clk),.dout(sfifo_out),.empty(empty),.rd_en(sfifo_read),.rd_data_count() );
   assign      fifo_occupied[15:9] = 0;
   assign      fifo_full = full;
   assign      fifo_empty = empty;   // Note empty is in wrong clock domain
   
   // MAC side signals
   //  We are allowed to do one more write after we are told the FIFO is full
   //  This allows us to register the _wa signal and speed up timing.
   
   reg 	       tx_mac_wa_d1;
   always @(posedge clk)
     tx_mac_wa_d1 <= Tx_mac_wa;
   
   assign      sfifo_read = ~empty & tx_mac_wa_d1;

   assign      Tx_mac_wr = sfifo_read;
   assign      Tx_mac_data = sfifo_out[31:0];
   assign      Tx_mac_BE = 0;  // Since we only deal with packets that are multiples of 32 bits long
   assign      Tx_mac_sop = sfifo_out[33];
   assign      Tx_mac_eop = sfifo_out[32];


   // BUFFER side signals
   reg 	       xfer_active;
   always @(posedge clk)
     if(rst)
       xfer_active <= 0;
     else if(rd_eop_i & ~full)
       xfer_active <= 0;
     else if(rd_sop_i)
       xfer_active <= 1;
   
   assign      sfifo_in = {rd_sop_i, rd_eop_i, rd_dat_i};
   assign      sfifo_write = xfer_active & ~full;

   assign      rd_read_o = sfifo_write;
   assign      rd_done_o = 0;  // Always send everything we're given?
   assign      rd_error_o = 0;  // No possible error situations?
   
endmodule // mac_txfifo_int
