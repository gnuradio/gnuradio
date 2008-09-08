
// Buffer pool.  Contains 8 buffers, each 2K (512 by 32).  Each buffer
// is a dual-ported RAM.  Port A on each of them is indirectly connected 
// to the wishbone bus by a bridge.  Port B may be connected any one of the
// 8 (4 rd, 4 wr) FIFO-like streaming interaces, or disconnected.  The wishbone bus
// provides access to all 8 buffers, and also controls the connections
// between the ports and the buffers, allocating them as needed.

// wb_adr is 16 bits -- 
//  bits 13:11 select which buffer
//  bits 10:2 select line in buffer
//  bits 1:0 are unused (32-bit access only)

module buffer_pool
  (input wb_clk_i,
   input wb_rst_i,
   input wb_we_i,
   input wb_stb_i,
   input [15:0] wb_adr_i,
   input [31:0] wb_dat_i,   
   output [31:0] wb_dat_o,
   output reg wb_ack_o,
   output wb_err_o,
   output wb_rty_o,
   
   input stream_clk,
   input stream_rst,
   
   input set_stb, input [7:0] set_addr, input [31:0] set_data,
   output [31:0] status,
   output sys_int_o,

   output [31:0] s0, output [31:0] s1, output [31:0] s2, output [31:0] s3,
   output [31:0] s4, output [31:0] s5, output [31:0] s6, output [31:0] s7,
   
   // Write Interfaces
   input [31:0] wr0_dat_i, input wr0_write_i, input wr0_done_i, input wr0_error_i, output wr0_ready_o, output wr0_full_o,
   input [31:0] wr1_dat_i, input wr1_write_i, input wr1_done_i, input wr1_error_i, output wr1_ready_o, output wr1_full_o,
   input [31:0] wr2_dat_i, input wr2_write_i, input wr2_done_i, input wr2_error_i, output wr2_ready_o, output wr2_full_o,
   input [31:0] wr3_dat_i, input wr3_write_i, input wr3_done_i, input wr3_error_i, output wr3_ready_o, output wr3_full_o,
   
   // Read Interfaces
   output [31:0] rd0_dat_o, input rd0_read_i, input rd0_done_i, input rd0_error_i, output rd0_sop_o, output rd0_eop_o,
   output [31:0] rd1_dat_o, input rd1_read_i, input rd1_done_i, input rd1_error_i, output rd1_sop_o, output rd1_eop_o,
   output [31:0] rd2_dat_o, input rd2_read_i, input rd2_done_i, input rd2_error_i, output rd2_sop_o, output rd2_eop_o,
   output [31:0] rd3_dat_o, input rd3_read_i, input rd3_done_i, input rd3_error_i, output rd3_sop_o, output rd3_eop_o
   );

   wire [7:0] 	 sel_a;
   
   wire [2:0] 	 which_buf = wb_adr_i[13:11];   // address 15:14 selects the buffer pool
   wire [8:0] 	 buf_addra = wb_adr_i[10:2];     // ignore address 1:0, 32-bit access only
   
   decoder_3_8 dec(.sel(which_buf),.res(sel_a));
   
   genvar 	 i;
   
   wire 	 go;

   reg [2:0] 	 port[0:7];	 
   reg [3:0] 	 read_src[0:3];
   reg [3:0] 	 write_src[0:3];
   
   wire [7:0] 	 done;
   wire [7:0] 	 error;
   wire [7:0] 	 idle;
   
   wire [31:0] 	 buf_doa[0:7];
   
   wire [7:0] 	 buf_enb;
   wire [7:0] 	 buf_web;
   wire [8:0] 	 buf_addrb[0:7];
   wire [31:0] 	 buf_dib[0:7];
   wire [31:0] 	 buf_dob[0:7];
   
   wire [31:0] 	 wr_dat_i[0:7];
   wire [7:0] 	 wr_write_i;
   wire [7:0] 	 wr_done_i;
   wire [7:0] 	 wr_error_i;
   wire [7:0] 	 wr_ready_o;
   wire [7:0] 	 wr_full_o;
   
   wire [31:0] 	 rd_dat_o[0:7];
   wire [7:0] 	 rd_read_i;
   wire [7:0] 	 rd_done_i;
   wire [7:0] 	 rd_error_i;
   wire [7:0] 	 rd_sop_o;
   wire [7:0] 	 rd_eop_o;
   
   assign 	 status = {8'd0,idle[7:0],error[7:0],done[7:0]};

   assign 	 s0 = {23'd0,buf_addrb[0]};
   assign 	 s1 = {23'd0,buf_addrb[1]};
   assign 	 s2 = {23'd0,buf_addrb[2]};
   assign 	 s3 = {23'd0,buf_addrb[3]};
   assign 	 s4 = {23'd0,buf_addrb[4]};
   assign 	 s5 = {23'd0,buf_addrb[5]};
   assign 	 s6 = {23'd0,buf_addrb[6]};
   assign 	 s7 = {23'd0,buf_addrb[7]};
   
   wire [31:0] 	 fifo_ctrl;
   setting_reg #(.my_addr(64)) 
     sreg(.clk(stream_clk),.rst(stream_rst),.strobe(set_stb),.addr(set_addr),.in(set_data),
	  .out(fifo_ctrl),.changed(go));

   integer 	 k;
   always @(posedge stream_clk)
     if(stream_rst)
       for(k=0;k<8;k=k+1)
	 port[k] <= 4;   // disabled
     else
       for(k=0;k<8;k=k+1)
	 if(go & (fifo_ctrl[31:28]==k))
	   port[k] <= fifo_ctrl[27:25];

   always @(posedge stream_clk)
     if(stream_rst)
       for(k=0;k<4;k=k+1)
	 read_src[k] <= 8;  // disabled
     else
       for(k=0;k<4;k=k+1)
	 if(go & fifo_ctrl[22] & (fifo_ctrl[27:25]==k))
	   read_src[k] <= fifo_ctrl[31:28];
   
   always @(posedge stream_clk)
     if(stream_rst)
       for(k=0;k<4;k=k+1)
	 write_src[k] <= 8;  // disabled
     else
       for(k=0;k<4;k=k+1)
	 if(go & fifo_ctrl[23] & (fifo_ctrl[27:25]==k))
	   write_src[k] <= fifo_ctrl[31:28];
   
   generate
      for(i=0;i<8;i=i+1)
	begin : gen_buffer
	   RAMB16_S36_S36 dpram
	     (.DOA(buf_doa[i]),.ADDRA(buf_addra),.CLKA(wb_clk_i),.DIA(wb_dat_i),.DIPA(4'h0),
	      .ENA(wb_stb_i & sel_a[i]),.SSRA(0),.WEA(wb_we_i),
	      .DOB(buf_dob[i]),.ADDRB(buf_addrb[i]),.CLKB(stream_clk),.DIB(buf_dib[i]),.DIPB(4'h0),
	      .ENB(buf_enb[i]),.SSRB(0),.WEB(buf_web[i]) );
	   
	   /* ram_2port #(.DWIDTH(32),.AWIDTH(9)) buffer
	     (.clka(wb_clk_i),.ena(wb_stb_i & sel_a[i]),.wea(wb_we_i),
	      .addra(buf_addra),.dia(wb_dat_i),.doa(buf_doa[i]),
	      .clkb(stream_clk),.enb(buf_enb[i]),.web(buf_web[i]),
	      .addrb(buf_addrb[i]),.dib(buf_dib[i]),.dob(buf_dob[i])); */

	   buffer_int #(.BUFF_NUM(i)) fifo_int
	     (.clk(stream_clk),.rst(stream_rst),
	      .ctrl_word(fifo_ctrl),.go(go & (fifo_ctrl[31:28]==i)),
	      .done(done[i]),.error(error[i]),.idle(idle[i]),
	      .en_o(buf_enb[i]),
	      .we_o(buf_web[i]),
	      .addr_o(buf_addrb[i]),
	      .dat_to_buf(buf_dib[i]),
	      .dat_from_buf(buf_dob[i]),
	      .wr_dat_i(wr_dat_i[i]),
	      .wr_write_i(wr_write_i[i]),
	      .wr_done_i(wr_done_i[i]),
	      .wr_error_i(wr_error_i[i]),
	      .wr_ready_o(wr_ready_o[i]),
	      .wr_full_o(wr_full_o[i]),
	      .rd_dat_o(rd_dat_o[i]),
	      .rd_read_i(rd_read_i[i]),
	      .rd_done_i(rd_done_i[i]),
	      .rd_error_i(rd_error_i[i]),
	      .rd_sop_o(rd_sop_o[i]),
	      .rd_eop_o(rd_eop_o[i]) 
	      );

	   // FIXME -- if it is a problem, maybe we don't need enables on these muxes
	   mux4 #(.WIDTH(32)) 
	     mux4_dat_i (.en(~port[i][2]),.sel(port[i][1:0]),.i0(wr0_dat_i),.i1(wr1_dat_i),
			 .i2(wr2_dat_i),.i3(wr3_dat_i),.o(wr_dat_i[i]));
	   mux4 #(.WIDTH(1)) 
	     mux4_write_i (.en(~port[i][2]),.sel(port[i][1:0]),.i0(wr0_write_i),.i1(wr1_write_i),
			   .i2(wr2_write_i),.i3(wr3_write_i),.o(wr_write_i[i]));
	   mux4 #(.WIDTH(1)) 
	     mux4_wrdone_i (.en(~port[i][2]),.sel(port[i][1:0]),.i0(wr0_done_i),.i1(wr1_done_i),
			    .i2(wr2_done_i),.i3(wr3_done_i),.o(wr_done_i[i]));
	   mux4 #(.WIDTH(1)) 
	     mux4_wrerror_i (.en(~port[i][2]),.sel(port[i][1:0]),.i0(wr0_error_i),.i1(wr1_error_i),
			     .i2(wr2_error_i),.i3(wr3_error_i),.o(wr_error_i[i]));
	   mux4 #(.WIDTH(1)) 
	     mux4_read_i (.en(~port[i][2]),.sel(port[i][1:0]),.i0(rd0_read_i),.i1(rd1_read_i),
			  .i2(rd2_read_i),.i3(rd3_read_i),.o(rd_read_i[i]));
	   mux4 #(.WIDTH(1)) 
	     mux4_rddone_i (.en(~port[i][2]),.sel(port[i][1:0]),.i0(rd0_done_i),.i1(rd1_done_i),
			    .i2(rd2_done_i),.i3(rd3_done_i),.o(rd_done_i[i]));
	   mux4 #(.WIDTH(1)) 
	     mux4_rderror_i (.en(~port[i][2]),.sel(port[i][1:0]),.i0(rd0_error_i),.i1(rd1_error_i),
			     .i2(rd2_error_i),.i3(rd3_error_i),.o(rd_error_i[i]));
	end // block: gen_buffer
   endgenerate

   //----------------------------------------------------------------------
   // Wishbone Outputs

   // Use the following lines if ram output and mux can be made fast enough

   assign wb_err_o = 1'b0;  // Unused for now
   assign wb_rty_o = 1'b0;  // Unused for now
   
   always @(posedge wb_clk_i)
     wb_ack_o <= wb_stb_i & ~wb_ack_o;
   assign wb_dat_o = buf_doa[which_buf];

   // Use this if we can't make the RAM+MUX fast enough
   // reg [31:0] wb_dat_o_reg;
   // reg 	      stb_d1;

   // always @(posedge wb_clk_i)
   //  begin
   //   wb_dat_o_reg <= buf_doa[which_buf];
   //   stb_d1 <= wb_stb_i;
   //   wb_ack_o <= (stb_d1 & ~wb_ack_o) | (wb_we_i & wb_stb_i);
   //  end
   //assign     wb_dat_o = wb_dat_o_reg;
   
   mux8 #(.WIDTH(1)) 
     mux8_wr_ready0(.en(~write_src[0][3]),.sel(write_src[0][2:0]), .i0(wr_ready_o[0]), .i1(wr_ready_o[1]),
		    .i2(wr_ready_o[2]), .i3(wr_ready_o[3]), .i4(wr_ready_o[4]),
		    .i5(wr_ready_o[5]), .i6(wr_ready_o[6]), .i7(wr_ready_o[7]),.o(wr0_ready_o));

   mux8 #(.WIDTH(1)) 
     mux8_wr_full0(.en(~write_src[0][3]),.sel(write_src[0][2:0]), .i0(wr_full_o[0]), .i1(wr_full_o[1]),
		   .i2(wr_full_o[2]), .i3(wr_full_o[3]), .i4(wr_full_o[4]),
		   .i5(wr_full_o[5]), .i6(wr_full_o[6]), .i7(wr_full_o[7]),.o(wr0_full_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_wr_ready1(.en(~write_src[1][3]),.sel(write_src[1][2:0]), .i0(wr_ready_o[0]), .i1(wr_ready_o[1]),
		    .i2(wr_ready_o[2]), .i3(wr_ready_o[3]), .i4(wr_ready_o[4]),
		    .i5(wr_ready_o[5]), .i6(wr_ready_o[6]), .i7(wr_ready_o[7]),.o(wr1_ready_o));

   mux8 #(.WIDTH(1)) 
     mux8_wr_full1(.en(~write_src[1][3]),.sel(write_src[1][2:0]), .i0(wr_full_o[0]), .i1(wr_full_o[1]),
		   .i2(wr_full_o[2]), .i3(wr_full_o[3]), .i4(wr_full_o[4]),
		   .i5(wr_full_o[5]), .i6(wr_full_o[6]), .i7(wr_full_o[7]),.o(wr1_full_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_wr_ready2(.en(~write_src[2][3]),.sel(write_src[2][2:0]), .i0(wr_ready_o[0]), .i1(wr_ready_o[1]),
		    .i2(wr_ready_o[2]), .i3(wr_ready_o[3]), .i4(wr_ready_o[4]),
		    .i5(wr_ready_o[5]), .i6(wr_ready_o[6]), .i7(wr_ready_o[7]),.o(wr2_ready_o));

   mux8 #(.WIDTH(1)) 
     mux8_wr_full2(.en(~write_src[2][3]),.sel(write_src[2][2:0]), .i0(wr_full_o[0]), .i1(wr_full_o[1]),
		   .i2(wr_full_o[2]), .i3(wr_full_o[3]), .i4(wr_full_o[4]),
		   .i5(wr_full_o[5]), .i6(wr_full_o[6]), .i7(wr_full_o[7]),.o(wr2_full_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_wr_ready3(.en(~write_src[3][3]),.sel(write_src[3][2:0]), .i0(wr_ready_o[0]), .i1(wr_ready_o[1]),
		    .i2(wr_ready_o[2]), .i3(wr_ready_o[3]), .i4(wr_ready_o[4]),
		    .i5(wr_ready_o[5]), .i6(wr_ready_o[6]), .i7(wr_ready_o[7]),.o(wr3_ready_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_wr_full3(.en(~write_src[3][3]),.sel(write_src[3][2:0]), .i0(wr_full_o[0]), .i1(wr_full_o[1]),
		   .i2(wr_full_o[2]), .i3(wr_full_o[3]), .i4(wr_full_o[4]),
		   .i5(wr_full_o[5]), .i6(wr_full_o[6]), .i7(wr_full_o[7]),.o(wr3_full_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_rd_sop0(.en(~read_src[0][3]),.sel(read_src[0][2:0]), .i0(rd_sop_o[0]), .i1(rd_sop_o[1]),
		  .i2(rd_sop_o[2]), .i3(rd_sop_o[3]), .i4(rd_sop_o[4]),
		  .i5(rd_sop_o[5]), .i6(rd_sop_o[6]), .i7(rd_sop_o[7]),.o(rd0_sop_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_rd_eop0(.en(~read_src[0][3]),.sel(read_src[0][2:0]), .i0(rd_eop_o[0]), .i1(rd_eop_o[1]),
		  .i2(rd_eop_o[2]), .i3(rd_eop_o[3]), .i4(rd_eop_o[4]),
		  .i5(rd_eop_o[5]), .i6(rd_eop_o[6]), .i7(rd_eop_o[7]),.o(rd0_eop_o));
   
   mux8 #(.WIDTH(32))
     mux8_rd_dat_0 (.en(~read_src[0][3]),.sel(read_src[0][2:0]), .i0(rd_dat_o[0]), .i1(rd_dat_o[1]),
		    .i2(rd_dat_o[2]), .i3(rd_dat_o[3]), .i4(rd_dat_o[4]),
		    .i5(rd_dat_o[5]), .i6(rd_dat_o[6]), .i7(rd_dat_o[7]),.o(rd0_dat_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_rd_sop1(.en(~read_src[1][3]),.sel(read_src[1][2:0]), .i0(rd_sop_o[0]), .i1(rd_sop_o[1]),
		  .i2(rd_sop_o[2]), .i3(rd_sop_o[3]), .i4(rd_sop_o[4]),
		  .i5(rd_sop_o[5]), .i6(rd_sop_o[6]), .i7(rd_sop_o[7]),.o(rd1_sop_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_rd_eop1(.en(~read_src[1][3]),.sel(read_src[1][2:0]), .i0(rd_eop_o[0]), .i1(rd_eop_o[1]),
		  .i2(rd_eop_o[2]), .i3(rd_eop_o[3]), .i4(rd_eop_o[4]),
		  .i5(rd_eop_o[5]), .i6(rd_eop_o[6]), .i7(rd_eop_o[7]),.o(rd1_eop_o));
   
   mux8 #(.WIDTH(32))
     mux8_rd_dat_1 (.en(~read_src[1][3]),.sel(read_src[1][2:0]), .i0(rd_dat_o[0]), .i1(rd_dat_o[1]),
		    .i2(rd_dat_o[2]), .i3(rd_dat_o[3]), .i4(rd_dat_o[4]),
		    .i5(rd_dat_o[5]), .i6(rd_dat_o[6]), .i7(rd_dat_o[7]),.o(rd1_dat_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_rd_sop2(.en(~read_src[2][3]),.sel(read_src[2][2:0]), .i0(rd_sop_o[0]), .i1(rd_sop_o[1]),
		  .i2(rd_sop_o[2]), .i3(rd_sop_o[3]), .i4(rd_sop_o[4]),
		  .i5(rd_sop_o[5]), .i6(rd_sop_o[6]), .i7(rd_sop_o[7]),.o(rd2_sop_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_rd_eop2(.en(~read_src[2][3]),.sel(read_src[2][2:0]), .i0(rd_eop_o[0]), .i1(rd_eop_o[1]),
		  .i2(rd_eop_o[2]), .i3(rd_eop_o[3]), .i4(rd_eop_o[4]),
		  .i5(rd_eop_o[5]), .i6(rd_eop_o[6]), .i7(rd_eop_o[7]),.o(rd2_eop_o));
   
   mux8 #(.WIDTH(32))
     mux8_rd_dat_2 (.en(~read_src[2][3]),.sel(read_src[2][2:0]), .i0(rd_dat_o[0]), .i1(rd_dat_o[1]),
		    .i2(rd_dat_o[2]), .i3(rd_dat_o[3]), .i4(rd_dat_o[4]),
		    .i5(rd_dat_o[5]), .i6(rd_dat_o[6]), .i7(rd_dat_o[7]),.o(rd2_dat_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_rd_sop3(.en(~read_src[3][3]),.sel(read_src[3][2:0]), .i0(rd_sop_o[0]), .i1(rd_sop_o[1]),
		  .i2(rd_sop_o[2]), .i3(rd_sop_o[3]), .i4(rd_sop_o[4]),
		  .i5(rd_sop_o[5]), .i6(rd_sop_o[6]), .i7(rd_sop_o[7]),.o(rd3_sop_o));
   
   mux8 #(.WIDTH(1)) 
     mux8_rd_eop3(.en(~read_src[3][3]),.sel(read_src[3][2:0]), .i0(rd_eop_o[0]), .i1(rd_eop_o[1]),
		  .i2(rd_eop_o[2]), .i3(rd_eop_o[3]), .i4(rd_eop_o[4]),
		  .i5(rd_eop_o[5]), .i6(rd_eop_o[6]), .i7(rd_eop_o[7]),.o(rd3_eop_o));
   
   mux8 #(.WIDTH(32))
     mux8_rd_dat_3 (.en(~read_src[3][3]),.sel(read_src[3][2:0]), .i0(rd_dat_o[0]), .i1(rd_dat_o[1]),
		    .i2(rd_dat_o[2]), .i3(rd_dat_o[3]), .i4(rd_dat_o[4]),
		    .i5(rd_dat_o[5]), .i6(rd_dat_o[6]), .i7(rd_dat_o[7]),.o(rd3_dat_o));
   
   assign sys_int_o = (|error) | (|done);
   
endmodule // buffer_pool
