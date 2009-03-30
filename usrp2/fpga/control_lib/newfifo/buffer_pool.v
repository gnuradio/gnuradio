
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

// BUF_SIZE is in address lines (i.e. log2 of number of lines).  
// For S3 it should be 9 (512 words, 2KB)
// For V5 it should be at least 10 (1024 words, 4KB) or 11 (2048 words, 8KB)

module buffer_pool
  #(parameter BUF_SIZE = 9,
    parameter SET_ADDR = 64)
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
     input [31:0] wr0_data_i, input [3:0] wr0_flags_i, input wr0_ready_i, output wr0_ready_o,
     input [31:0] wr1_data_i, input [3:0] wr1_flags_i, input wr1_ready_i, output wr1_ready_o,
     input [31:0] wr2_data_i, input [3:0] wr2_flags_i, input wr2_ready_i, output wr2_ready_o,
     input [31:0] wr3_data_i, input [3:0] wr3_flags_i, input wr3_ready_i, output wr3_ready_o,
     
     // Read Interfaces
     output [31:0] rd0_data_o, output [3:0] rd0_flags_o, output rd0_ready_o, input rd0_ready_i, 
     output [31:0] rd1_data_o, output [3:0] rd1_flags_o, output rd1_ready_o, input rd1_ready_i, 
     output [31:0] rd2_data_o, output [3:0] rd2_flags_o, output rd2_ready_o, input rd2_ready_i, 
     output [31:0] rd3_data_o, output [3:0] rd3_flags_o, output rd3_ready_o, input rd3_ready_i
     );
   
   wire [7:0] 	   sel_a;
   
   wire [BUF_SIZE-1:0] 	   buf_addra = wb_adr_i[BUF_SIZE+1:2];     // ignore address 1:0, 32-bit access only
   wire [2:0] 		   which_buf = wb_adr_i[BUF_SIZE+4:BUF_SIZE+2];   // address 15:14 selects the buffer pool
   
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
   wire [BUF_SIZE-1:0] 	 buf_addrb[0:7];
   wire [31:0] 	 buf_dib[0:7];
   wire [31:0] 	 buf_dob[0:7];
   
   wire [31:0] 	 wr_data_i[0:7];
   wire [3:0] 	 wr_flags_i[0:7];
   wire [7:0] 	 wr_ready_i;
   wire [7:0] 	 wr_ready_o;
   
   wire [31:0] 	 rd_data_o[0:7];
   wire [3:0] 	 rd_flags_o[0:7];
   wire [7:0] 	 rd_ready_o;
   wire [7:0] 	 rd_ready_i;
   
   assign 	 status = {8'd0,idle[7:0],error[7:0],done[7:0]};

   assign 	 s0 = buf_addrb[0];
   assign 	 s1 = buf_addrb[1];
   assign 	 s2 = buf_addrb[2];
   assign 	 s3 = buf_addrb[3];
   assign 	 s4 = buf_addrb[4];
   assign 	 s5 = buf_addrb[5];
   assign 	 s6 = buf_addrb[6];
   assign 	 s7 = buf_addrb[7];
   
   wire [31:0] 	 fifo_ctrl;
   setting_reg #(.my_addr(SET_ADDR)) 
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
	   
/*	   
	   ram_2port #(.DWIDTH(32),.AWIDTH(BUF_SIZE)) buffer
	     (.clka(wb_clk_i),.ena(wb_stb_i & sel_a[i]),.wea(wb_we_i),
	      .addra(buf_addra),.dia(wb_dat_i),.doa(buf_doa[i]),
	      .clkb(stream_clk),.enb(buf_enb[i]),.web(buf_web[i]),
	      .addrb(buf_addrb[i]),.dib(buf_dib[i]),.dob(buf_dob[i]));
 
 */
	   
	   buffer_int #(.BUF_NUM(i),.BUF_SIZE(BUF_SIZE)) buffer_int
	     (.clk(stream_clk),.rst(stream_rst),
	      .ctrl_word(fifo_ctrl),.go(go & (fifo_ctrl[31:28]==i)),
	      .done(done[i]),.error(error[i]),.idle(idle[i]),
	      .en_o(buf_enb[i]),
	      .we_o(buf_web[i]),
	      .addr_o(buf_addrb[i]),
	      .dat_to_buf(buf_dib[i]),
	      .dat_from_buf(buf_dob[i]),
	      .wr_data_i(wr_data_i[i]),
	      .wr_flags_i(wr_flags_i[i]),
	      .wr_ready_i(wr_ready_i[i]),
	      .wr_ready_o(wr_ready_o[i]),
	      .rd_data_o(rd_data_o[i]),
	      .rd_flags_o(rd_flags_o[i]),
	      .rd_ready_o(rd_ready_o[i]),
	      .rd_ready_i(rd_ready_i[i]) );
	   mux4 #(.WIDTH(37))
	     mux4_wr (.en(~port[i][2]),.sel(port[i][1:0]),
		      .i0({wr0_data_i,wr0_flags_i,wr0_ready_i}),
		      .i1({wr1_data_i,wr1_flags_i,wr1_ready_i}),
		      .i2({wr2_data_i,wr2_flags_i,wr2_ready_i}),
		      .i3({wr3_data_i,wr3_flags_i,wr3_ready_i}),
		      .o({wr_data_i[i],wr_flags_i[i],wr_ready_i[i]}) );
	   mux4 #(.WIDTH(1))
	     mux4_rd (.en(~port[i][2]),.sel(port[i][1:0]),
		      .i0(rd0_ready_i),.i1(rd1_ready_i),.i2(rd2_ready_i),.i3(rd3_ready_i),
		      .o(rd_ready_i[i]));
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
     mux8_wr0(.en(~write_src[0][3]),.sel(write_src[0][2:0]), 
	      .i0(wr_ready_o[0]), .i1(wr_ready_o[1]), .i2(wr_ready_o[2]), .i3(wr_ready_o[3]),
	      .i4(wr_ready_o[4]), .i5(wr_ready_o[5]), .i6(wr_ready_o[6]), .i7(wr_ready_o[7]),
	      .o(wr0_ready_o));

   mux8 #(.WIDTH(1)) 
     mux8_wr1(.en(~write_src[1][3]),.sel(write_src[1][2:0]), 
	      .i0(wr_ready_o[0]), .i1(wr_ready_o[1]), .i2(wr_ready_o[2]), .i3(wr_ready_o[3]),
	      .i4(wr_ready_o[4]), .i5(wr_ready_o[5]), .i6(wr_ready_o[6]), .i7(wr_ready_o[7]),
	      .o(wr1_ready_o));

   mux8 #(.WIDTH(1)) 
     mux8_wr2(.en(~write_src[2][3]),.sel(write_src[2][2:0]), 
	      .i0(wr_ready_o[0]), .i1(wr_ready_o[1]), .i2(wr_ready_o[2]), .i3(wr_ready_o[3]),
	      .i4(wr_ready_o[4]), .i5(wr_ready_o[5]), .i6(wr_ready_o[6]), .i7(wr_ready_o[7]),
	      .o(wr2_ready_o));

   mux8 #(.WIDTH(1)) 
     mux8_wr3(.en(~write_src[3][3]),.sel(write_src[3][2:0]), 
	      .i0(wr_ready_o[0]), .i1(wr_ready_o[1]), .i2(wr_ready_o[2]), .i3(wr_ready_o[3]),
	      .i4(wr_ready_o[4]), .i5(wr_ready_o[5]), .i6(wr_ready_o[6]), .i7(wr_ready_o[7]),
	      .o(wr3_ready_o));

   mux8 #(.WIDTH(37)) 
     mux8_rd0(.en(~read_src[0][3]),.sel(read_src[0][2:0]), 
	      .i0({rd_data_o[0],rd_flags_o[0],rd_ready_o[0]}),
	      .i1({rd_data_o[1],rd_flags_o[1],rd_ready_o[1]}),
	      .i2({rd_data_o[2],rd_flags_o[2],rd_ready_o[2]}),
	      .i3({rd_data_o[3],rd_flags_o[3],rd_ready_o[3]}),
	      .i4({rd_data_o[4],rd_flags_o[4],rd_ready_o[4]}),
	      .i5({rd_data_o[5],rd_flags_o[5],rd_ready_o[5]}),
	      .i6({rd_data_o[6],rd_flags_o[6],rd_ready_o[6]}),
	      .i7({rd_data_o[7],rd_flags_o[7],rd_ready_o[7]}),
	      .o({rd0_data_o,rd0_flags_o,rd0_ready_o}));
   
   mux8 #(.WIDTH(37)) 
     mux8_rd1(.en(~read_src[1][3]),.sel(read_src[1][2:0]), 
	      .i0({rd_data_o[0],rd_flags_o[0],rd_ready_o[0]}),
	      .i1({rd_data_o[1],rd_flags_o[1],rd_ready_o[1]}),
	      .i2({rd_data_o[2],rd_flags_o[2],rd_ready_o[2]}),
	      .i3({rd_data_o[3],rd_flags_o[3],rd_ready_o[3]}),
	      .i4({rd_data_o[4],rd_flags_o[4],rd_ready_o[4]}),
	      .i5({rd_data_o[5],rd_flags_o[5],rd_ready_o[5]}),
	      .i6({rd_data_o[6],rd_flags_o[6],rd_ready_o[6]}),
	      .i7({rd_data_o[7],rd_flags_o[7],rd_ready_o[7]}),
	      .o({rd1_data_o,rd1_flags_o,rd1_ready_o}));
   
   mux8 #(.WIDTH(37)) 
     mux8_rd2(.en(~read_src[2][3]),.sel(read_src[2][2:0]), 
	      .i0({rd_data_o[0],rd_flags_o[0],rd_ready_o[0]}),
	      .i1({rd_data_o[1],rd_flags_o[1],rd_ready_o[1]}),
	      .i2({rd_data_o[2],rd_flags_o[2],rd_ready_o[2]}),
	      .i3({rd_data_o[3],rd_flags_o[3],rd_ready_o[3]}),
	      .i4({rd_data_o[4],rd_flags_o[4],rd_ready_o[4]}),
	      .i5({rd_data_o[5],rd_flags_o[5],rd_ready_o[5]}),
	      .i6({rd_data_o[6],rd_flags_o[6],rd_ready_o[6]}),
	      .i7({rd_data_o[7],rd_flags_o[7],rd_ready_o[7]}),
	      .o({rd2_data_o,rd2_flags_o,rd2_ready_o}));
   
   mux8 #(.WIDTH(37)) 
     mux8_rd3(.en(~read_src[3][3]),.sel(read_src[3][2:0]), 
	      .i0({rd_data_o[0],rd_flags_o[0],rd_ready_o[0]}),
	      .i1({rd_data_o[1],rd_flags_o[1],rd_ready_o[1]}),
	      .i2({rd_data_o[2],rd_flags_o[2],rd_ready_o[2]}),
	      .i3({rd_data_o[3],rd_flags_o[3],rd_ready_o[3]}),
	      .i4({rd_data_o[4],rd_flags_o[4],rd_ready_o[4]}),
	      .i5({rd_data_o[5],rd_flags_o[5],rd_ready_o[5]}),
	      .i6({rd_data_o[6],rd_flags_o[6],rd_ready_o[6]}),
	      .i7({rd_data_o[7],rd_flags_o[7],rd_ready_o[7]}),
	      .o({rd3_data_o,rd3_flags_o,rd3_ready_o}));
   
   assign sys_int_o = (|error) | (|done);
   
endmodule // buffer_pool
