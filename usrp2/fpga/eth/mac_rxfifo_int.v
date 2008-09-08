
module mac_rxfifo_int
  (input clk, input rst,

   input Rx_mac_empty,
   output Rx_mac_rd,
   input [31:0] Rx_mac_data,
   input [1:0] Rx_mac_BE,
   input Rx_mac_sop,
   input Rx_mac_eop,
   input Rx_mac_err,

   output [31:0] wr_dat_o,
   output wr_write_o,
   output wr_done_o,
   output wr_error_o,
   input wr_ready_i,
   input wr_full_i,

   // FIFO Status
   output [15:0] fifo_occupied,
   output fifo_full,
   output fifo_empty
   );
  
   // Write side of short FIFO
   //   Inputs: full, Rx_mac_empty, Rx_mac_sop, Rx_mac_eop, Rx_mac_err, Rx_mac_data/BE
   //   Controls: write, datain, Rx_mac_rd

   wire  write, full, read, empty, sop_o, eop_o, error_o;

   // Write side of short FIFO
   assign write = ~full & ~Rx_mac_empty;
   assign Rx_mac_rd = write;
 
   shortfifo #(.WIDTH(35)) mac_rx_sfifo
     (.clk(clk),.rst(rst),.clear(0),
      .datain({Rx_mac_sop,Rx_mac_eop,Rx_mac_err,Rx_mac_data}),.write(write),.full(full),
      .dataout({sop_o,eop_o,error_o,wr_dat_o}),.read(read),.empty(empty),
      .space(), .occupied(fifo_occupied[4:0]) );
   assign fifo_occupied[15:5] = 0;
   assign fifo_full = full;
   assign fifo_empty = empty;
   
   // Read side of short FIFO
   //    Inputs:    empty, dataout, wr_ready_i, wr_full_i
   //    Controls:  read, wr_dat_o, wr_write_o, wr_done_o, wr_error_o

   reg [1:0] rd_state;
   localparam RD_IDLE = 0;
   localparam RD_HAVEPKT = 1;
   localparam RD_XFER = 2;
   localparam RD_ERROR = 3;
   
   always @(posedge clk)
     if(rst)
       rd_state <= RD_IDLE;
     else
       case(rd_state)
	 RD_IDLE :
	   if(sop_o & ~empty)
	     rd_state <= RD_HAVEPKT;
	 RD_HAVEPKT :
	   if(wr_ready_i)
	     rd_state <= RD_XFER;
	 RD_XFER :
	   if(eop_o & ~empty)
	     rd_state <= RD_IDLE;
	   else if(wr_full_i)
	     rd_state <= RD_HAVEPKT;
	 RD_ERROR :
	   rd_state <= RD_IDLE;
       endcase // case(rd_state)

   assign     read = ~empty & ((rd_state == RD_XFER) | ((rd_state==RD_IDLE)&~sop_o));
   assign     wr_write_o = ~empty & (rd_state == RD_XFER);
   assign     wr_done_o = ~empty & (rd_state == RD_XFER)  & eop_o;
   assign     wr_error_o = ~empty & (rd_state == RD_XFER) & error_o;

endmodule // mac_rxfifo_int
