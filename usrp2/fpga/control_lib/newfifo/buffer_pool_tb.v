
module buffer_pool_tb();
   
   wire wb_clk_i;
   wire wb_rst_i;
   wire wb_we_i;
   wire wb_stb_i;
   wire [15:0] wb_adr_i;
   wire [31:0] wb_dat_i;   
   wire [31:0] wb_dat_o;
   wire wb_ack_o;
   wire wb_err_o;
   wire wb_rty_o;

   wire stream_clk, stream_rst;

   wire set_stb;
   wire [7:0] set_addr;
   wire [31:0] set_data;

   wire [31:0] wr0_data, wr1_data, wr2_data, wr3_data;
   wire [31:0] rd0_data, rd1_data, rd2_data, rd3_data;
   wire [3:0]  wr0_flags, wr1_flags, wr2_flags, wr3_flags;
   wire [3:0]  rd0_flags, rd1_flags, rd2_flags, rd3_flags;
   wire        wr0_ready, wr1_ready, wr2_ready, wr3_ready;
   wire        rd0_ready, rd1_ready, rd2_ready, rd3_ready;
   wire        wr0_write, wr1_write, wr2_write, wr3_write;
   wire        rd0_read, rd1_read, rd2_read, rd3_read;

   buffer_pool dut
     (.wb_clk_i(wb_clk_i),
      .wb_rst_i(wb_rst_i),
      .wb_we_i(wb_we_i),
      .wb_stb_i(wb_stb_i),
      .wb_adr_i(wb_adr_i),
      .wb_dat_i(wb_dat_i),   
      .wb_dat_o(wb_dat_o),
      .wb_ack_o(wb_ack_o),
      .wb_err_o(wb_err_o),
      .wb_rty_o(wb_rty_o),
      
      .stream_clk(stream_clk),
      .stream_rst(stream_rst),
      
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      
      .wr0_data_i(wr0_data), .wr0_write_i(wr0_write), .wr0_flags_i(wr0_flags), .wr0_ready_o(wr0_ready),
      .wr1_data_i(wr1_data), .wr1_write_i(wr1_write), .wr1_flags_i(wr1_flags), .wr1_ready_o(wr1_ready),
      .wr2_data_i(wr2_data), .wr2_write_i(wr2_write), .wr2_flags_i(wr2_flags), .wr2_ready_o(wr2_ready),
      .wr3_data_i(wr3_data), .wr3_write_i(wr3_write), .wr3_flags_i(wr3_flags), .wr3_ready_o(wr3_ready),
      
      .rd0_data_o(rd0_data), .rd0_read_i(rd0_read), .rd0_flags_o(rd0_flags), .rd0_ready_o(rd0_ready),
      .rd1_data_o(rd1_data), .rd1_read_i(rd1_read), .rd1_flags_o(rd1_flags), .rd1_ready_o(rd1_ready),
      .rd2_data_o(rd2_data), .rd2_read_i(rd2_read), .rd2_flags_o(rd2_flags), .rd2_ready_o(rd2_ready),
      .rd3_data_o(rd3_data), .rd3_read_i(rd3_read), .rd3_flags_o(rd3_flags), .rd3_ready_o(rd3_ready)
      );
   
endmodule // buffer_pool_tb
