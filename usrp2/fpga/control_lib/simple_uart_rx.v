

module simple_uart_rx
  #(parameter DEPTH=0)
    (input clk, input rst, 
     output [7:0] fifo_out, input fifo_read, output [7:0] fifo_level, output fifo_empty, 
     input [15:0] clkdiv, input rx);

   reg 		  rx_d1, rx_d2;
   always @(posedge clk)
     if(rst)
       {rx_d2,rx_d1} <= 0;
     else
       {rx_d2,rx_d1} <= {rx_d1,rx};
   
   reg [15:0] 	  baud_ctr;
   reg [3:0] 	  bit_ctr;
   reg [7:0] 	  sr;

   wire 	  neg_trans = rx_d2 & ~rx_d1;
   wire 	  shift_now = baud_ctr == (clkdiv>>1);
   wire 	  stop_now = (bit_ctr == 10) && shift_now;
   wire 	  go_now = (bit_ctr == 0) && neg_trans;
   
   always @(posedge clk)
     if(rst)
       sr <= 0;
     else if(shift_now)
       sr <= {rx_d2,sr[7:1]};
   
   always @(posedge clk)
     if(rst)
       baud_ctr <= 0;
     else
       if(go_now)
	 baud_ctr <= 1;
       else if(stop_now)
	 baud_ctr <= 0;
       else if(baud_ctr >= clkdiv)
	 baud_ctr <= 1;
       else if(baud_ctr != 0)
	 baud_ctr <= baud_ctr + 1;

   always @(posedge clk)
     if(rst)
       bit_ctr <= 0;
     else 
       if(go_now)
	 bit_ctr <= 1;
       else if(stop_now)
	 bit_ctr <= 0;
       else if(baud_ctr == clkdiv)
	 bit_ctr <= bit_ctr + 1;
   
   wire 	  full;
   wire 	  write = ~full & rx_d2 & stop_now;
   
   medfifo #(.WIDTH(8),.DEPTH(DEPTH)) fifo
     (.clk(clk),.rst(rst),
      .datain(sr),.write(write),.full(full),
      .dataout(fifo_out),.read(fifo_read),.empty(fifo_empty),
      .clear(0),.space(),.occupied(fifo_level) );
   
endmodule // simple_uart_rx
