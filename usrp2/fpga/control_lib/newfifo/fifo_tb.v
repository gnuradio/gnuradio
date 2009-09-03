module fifo_new_tb();
   
   reg clk = 0;
   reg rst = 1;
   reg clear = 0;
   initial #1000 rst = 0;
   always #50 clk = ~clk;
   
   reg [31:0] f36_data = 0;
   reg [1:0] f36_occ = 0;
   reg f36_sof = 0, f36_eof = 0;
   
   wire [35:0] f36_in = {f36_occ,f36_eof,f36_sof,f36_data};
   reg src_rdy_f36i  = 0;
   wire dst_rdy_f36i;

   wire [35:0] f36_out, f36_out2;
   wire src_rdy_f36o;
   reg dst_rdy_f36o  = 0;
   
   //fifo_cascade #(.WIDTH(36), .SIZE(4)) fifo_cascade36
   //fifo_long #(.WIDTH(36), .SIZE(4)) fifo_cascade36

   wire i1_sr, i1_dr;
   wire i2_sr, i2_dr;
   wire i3_sr, i3_dr;
   reg i4_dr = 0;
   wire i4_sr;
      
   wire [35:0] i1, i4;
   wire [18:0] i2, i3;
   
   wire [7:0] ll_data;
   wire ll_src_rdy_n, ll_dst_rdy_n, ll_sof_n, ll_eof_n;
   
   fifo_short #(.WIDTH(36)) fifo_short1
     (.clk(clk),.reset(rst),.clear(clear),
      .datain(f36_in),.src_rdy_i(src_rdy_f36i),.dst_rdy_o(dst_rdy_f36i),
      .dataout(i1),.src_rdy_o(i1_sr),.dst_rdy_i(i1_dr) );

   fifo36_to_fifo19 fifo36_to_fifo19
     (.clk(clk),.reset(rst),.clear(clear),
      .f36_datain(i1),.f36_src_rdy_i(i1_sr),.f36_dst_rdy_o(i1_dr),
      .f19_dataout(i2),.f19_src_rdy_o(i2_sr),.f19_dst_rdy_i(i2_dr) );

   fifo19_to_ll8 fifo19_to_ll8
     (.clk(clk),.reset(rst),.clear(clear),
      .f19_data(i2),.f19_src_rdy_i(i2_sr),.f19_dst_rdy_o(i2_dr),
      .ll_data(ll_data),.ll_sof_n(ll_sof_n),.ll_eof_n(ll_eof_n),
      .ll_src_rdy_n(ll_src_rdy_n),.ll_dst_rdy_n(ll_dst_rdy_n));

   ll8_to_fifo19 ll8_to_fifo19
     (.clk(clk),.reset(rst),.clear(clear),
      .ll_data(ll_data),.ll_sof_n(ll_sof_n),.ll_eof_n(ll_eof_n),
      .ll_src_rdy_n(ll_src_rdy_n),.ll_dst_rdy_n(ll_dst_rdy_n),
      .f19_data(i3),.f19_src_rdy_o(i3_sr),.f19_dst_rdy_i(i3_dr) );

   fifo19_to_fifo36 fifo19_to_fifo36
     (.clk(clk),.reset(rst),.clear(clear),
      .f19_datain(i3),.f19_src_rdy_i(i3_sr),.f19_dst_rdy_o(i3_dr),
      .f36_dataout(i4),.f36_src_rdy_o(i4_sr),.f36_dst_rdy_i(i4_dr) );
     
   task ReadFromFIFO36;
      begin
	 $display("Read from FIFO36");
	 #1 i4_dr <= 1;
	 while(1)
	   begin
	      while(~i4_sr)
		@(posedge clk);
	      $display("Read: %h",i4);
	      @(posedge clk);
	   end
      end
   endtask // ReadFromFIFO36

   reg [15:0] count;
   task PutPacketInFIFO36;
      input [31:0] data_start;
      input [31:0] data_len;
      begin
	 count 	      <= 4;
	 src_rdy_f36i <= 1;
	 f36_data     <= data_start;
	 f36_sof      <= 1;
	 f36_eof      <= 0;
	 f36_occ      <= 0;
	
	 $display("Put Packet in FIFO36");
	 while(~dst_rdy_f36i)
	   @(posedge clk);
	 @(posedge clk);
	 $display("PPI_FIFO36: Entered First Line");
	 f36_sof <= 0;
	 while(count+4 < data_len)
	   begin
	      f36_data <= f36_data + 32'h01010101;
	      count    <= count + 4;
	      while(~dst_rdy_f36i)
		@(posedge clk);
	      @(posedge clk);
	      $display("PPI_FIFO36: Entered New Line");
	   end
	 f36_data  <= f36_data + 32'h01010101;
	 f36_eof   <= 1;
	 if(count + 4 == data_len)
	   f36_occ <= 0;
	 else if(count + 3 == data_len)
	   f36_occ <= 3;
	 else if(count + 2 == data_len)
	   f36_occ <= 2;
	 else
	   f36_occ <= 1;
	 while(~dst_rdy_f36i)
	   @(posedge clk);
	 @(posedge clk);
	 f36_occ      <= 0;
	 f36_eof      <= 0;
	 f36_data     <= 0;
	 src_rdy_f36i <= 0;
	 $display("PPI_FIFO36: Entered Last Line");
      end
   endtask // PutPacketInFIFO36
   
   initial $dumpfile("fifo_new_tb.vcd");
   initial $dumpvars(0,fifo_new_tb);

   initial
     begin
	@(negedge rst);
	//#10000;
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	ReadFromFIFO36;
     end
   
   initial
     begin
	@(negedge rst);
	@(posedge clk);
	@(posedge clk);
	PutPacketInFIFO36(32'hA0B0C0D0,12);
	@(posedge clk);
	@(posedge clk);
	#10000;
	@(posedge clk);
	PutPacketInFIFO36(32'hE0F0A0B0,36);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
     end

   initial #20000 $finish;
endmodule // longfifo_tb
