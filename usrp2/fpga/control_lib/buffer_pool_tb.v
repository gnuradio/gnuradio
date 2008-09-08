
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

   wire [31:0] wr0_dat_i;
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
      
      .wr0_dat_i(wr0_dat_i), .wr0_write_i(), .wr0_done_i(), .wr0_error_i(), .wr0_ready_o(), .wr0_full_o(),
      .wr1_dat_i(), .wr1_write_i(), .wr1_done_i(), .wr1_error_i(), .wr1_ready_o(), .wr1_full_o(),
      .wr2_dat_i(), .wr2_write_i(), .wr2_done_i(), .wr2_error_i(), .wr2_ready_o(), .wr2_full_o(),
      .wr3_dat_i(), .wr3_write_i(), .wr3_done_i(), .wr3_error_i(), .wr3_ready_o(), .wr3_full_o(),
      
      .rd0_dat_o(), .rd0_read_i(), .rd0_done_i(), .rd0_error_i(), .rd0_ready_o(), .rd0_empty_o(),
      .rd1_dat_o(), .rd1_read_i(), .rd1_done_i(), .rd1_error_i(), .rd1_ready_o(), .rd1_empty_o(),
      .rd2_dat_o(), .rd2_read_i(), .rd2_done_i(), .rd2_error_i(), .rd2_ready_o(), .rd2_empty_o(),
      .rd3_dat_o(), .rd3_read_i(), .rd3_done_i(), .rd3_error_i(), .rd3_ready_o(), .rd3_empty_o()
      );
   
endmodule // buffer_pool_tb
