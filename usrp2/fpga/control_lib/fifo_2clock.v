
module fifo_2clock
  #(parameter DWIDTH=32, AWIDTH=9)
    (input wclk, input [DWIDTH-1:0] datain, input write, output full, output reg [AWIDTH-1:0] level_wclk,
     input rclk, output [DWIDTH-1:0] dataout, input read, output empty, output reg [AWIDTH-1:0] level_rclk,
     input arst);

   reg [AWIDTH-1:0] wr_addr, rd_addr;
   wire [AWIDTH-1:0] wr_addr_rclk, rd_addr_wclk;
   wire [AWIDTH-1:0] next_rd_addr;
   wire 	    enb_read;
   
   // Write side management
   wire [AWIDTH-1:0] next_wr_addr = wr_addr + 1;
   always @(posedge wclk or posedge arst)
     if(arst)
       wr_addr <= 0;
     else if(write)
       wr_addr <= next_wr_addr;
   assign 	    full = (next_wr_addr == rd_addr_wclk);

   //  RAM for data storage.  Data out is registered, complicating the
   //     read side logic
   ram_2port #(.DWIDTH(DWIDTH),.AWIDTH(AWIDTH)) mac_rx_ff_ram
     (.clka(wclk),.ena(1'b1),.wea(write),.addra(wr_addr),.dia(datain),.doa(),
      .clkb(rclk),.enb(enb_read),.web(1'b0),.addrb(next_rd_addr),.dib(0),.dob(dataout) );

   // Read side management
   reg 		    data_valid;
   assign 	    empty = ~data_valid;
   assign 	    next_rd_addr = rd_addr + data_valid;
   assign 	    enb_read = read | ~data_valid;

   always @(posedge rclk or posedge arst)
     if(arst)
       rd_addr <= 0;
     else if(read)
       rd_addr <= rd_addr + 1;

   always @(posedge rclk or posedge arst)
     if(arst)
       data_valid <= 0;
     else
       if(read & (next_rd_addr == wr_addr_rclk))
	 data_valid <= 0;
       else if(next_rd_addr != wr_addr_rclk)
	 data_valid <= 1;
	 
   // Send pointers across clock domains via gray code
   gray_send #(.WIDTH(AWIDTH)) send_wr_addr
     (.clk_in(wclk),.addr_in(wr_addr),
      .clk_out(rclk),.addr_out(wr_addr_rclk) );
   
   gray_send #(.WIDTH(AWIDTH)) send_rd_addr
     (.clk_in(rclk),.addr_in(rd_addr),
      .clk_out(wclk),.addr_out(rd_addr_wclk) );

   // Generate fullness info, these are approximate and may be delayed 
   // and are only for higher-level flow control.  
   // Only full and empty are guaranteed exact.
   always @(posedge wclk) 
     level_wclk <= wr_addr - rd_addr_wclk;
   always @(posedge rclk) 
     level_rclk <= wr_addr_rclk - rd_addr;
   
endmodule // fifo_2clock
