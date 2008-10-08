
// Temporary buffer pool storage, mostly useful for pre-generated data streams or
//   for making more space to juggle packets in case of eth frames coming out of order

module extram_interface
  (input clk, input rst,
   input set_stb, input [7:0] set_addr, input [31:0] set_data,
   
   // Buffer pool interfaces
   input [31:0] rd_dat_i, output rd_read_o, output rd_done_o, output rd_error_o,
   input rd_sop_i, input rd_eop_i,
   output [31:0] wr_dat_o, output wr_write_o, output wr_done_o, output wr_error_o,
   input wr_ready_i, input wr_full_i,
   
   // RAM Interface
   inout [17:0] RAM_D,
   output [18:0] RAM_A,
   output RAM_CE1n,
   output RAM_CENn,
   input RAM_CLK,
   output RAM_WEn,
   output RAM_OEn,
   output RAM_LDn );

   // Command format --
   //    Read/_Write , start address[17:0]
   wire [18:0] cmd_in;
   wire        cmd_stb, store_wr_cmd, store_rd_cmd, read_wr_cmd, read_rd_cmd;
   wire        empty_wr_cmd, empty_rd_cmd, full_wr_cmd, full_rd_cmd;
   
   // Dummy logic
   assign RAM_OEn = 1;
   
   setting_reg #(.my_addr(0)) 
     sr_ram_cmd (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
		 .in(set_data),.out(cmd_in),.changed(cmd_stb));
   
   reg 	  cmd_stb_d1;
   always @(posedge clk) cmd_stb_d1 <= cmd_stb;
   assign store_wr_cmd = ~cmd_in[18] & cmd_stb & ~cmd_stb_d1;
   assign store_rd_cmd = cmd_in[18] & cmd_stb & ~cmd_stb_d1;   

   shortfifo #(.WIDTH(19)) wr_cmd_fifo
     (.clk(clk),.rst(rst),.clear(1'b0),
      .datain(cmd_in), .write(store_wr_cmd), .full(full_wr_cmd),
      .dataout(), .read(read_wr_cmd), .empty(empty_wr_cmd) );

   shortfifo #(.WIDTH(19)) rd_cmd_fifo
     (.clk(clk),.rst(rst),.clear(1'b0),
      .datain(cmd_in), .write(store_rd_cmd), .full(full_rd_cmd),
      .dataout(), .read(read_rd_cmd), .empty(empty_rd_cmd) );

endmodule // extram_interface
