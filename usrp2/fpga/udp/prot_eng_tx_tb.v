module prot_eng_tx_tb();

   localparam BASE = 128;
   reg clk    = 0;
   reg rst    = 1;
   reg clear  = 0;
   initial #1000 rst = 0;
   always #50 clk = ~clk;
   
   reg [31:0] f36_data;
   reg [1:0] f36_occ;
   reg f36_sof, f36_eof;
   
   wire [35:0] f36_in = {f36_occ,f36_eof,f36_sof,f36_data};
   reg src_rdy_f36i  = 0;
   reg [15:0] count;

   wire [35:0] casc_do;
   wire [18:0] final_out, prot_out;

   wire src_rdy_final, dst_rdy_final, src_rdy_prot;
   reg dst_rdy_prot =0;
   
   wire dst_rdy_f36o ;
   fifo_long #(.WIDTH(36), .SIZE(4)) fifo_cascade36
     (.clk(clk),.reset(rst),.clear(clear),
      .datain(f36_in),.src_rdy_i(src_rdy_f36i),.dst_rdy_o(dst_rdy_f36i),
      .dataout(casc_do),.src_rdy_o(src_rdy_f36o),.dst_rdy_i(dst_rdy_f36o));

   fifo36_to_fifo19 fifo_converter
     (.clk(clk),.reset(rst),.clear(clear),
      .f36_datain(casc_do),.f36_src_rdy_i(src_rdy_f36o),.f36_dst_rdy_o(dst_rdy_f36o),
      .f19_dataout(final_out),.f19_src_rdy_o(src_rdy_final),.f19_dst_rdy_i(dst_rdy_final));

   reg set_stb;
   reg [7:0] set_addr;
   reg [31:0] set_data;
	
   prot_eng_tx #(.BASE(BASE)) prot_eng_tx
     (.clk(clk), .reset(rst),
      .set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .datain(final_out[18:0]),.src_rdy_i(src_rdy_final),.dst_rdy_o(dst_rdy_final),
      .dataout(prot_out[18:0]),.src_rdy_o(src_rdy_prot),.dst_rdy_i(dst_rdy_prot));
   
   reg [35:0] printer;

   task WriteSREG;
      input [7:0] addr;
      input [31:0] data;

      begin
	 @(posedge clk);
	 set_addr <= addr;
	 set_data <= data;
	 set_stb  <= 1;
	 @(posedge clk);
	 set_stb <= 0;
      end
   endtask // WriteSREG
   	
   task ReadFromFIFO36;
      begin
	 $display("Read from FIFO36");
	 #1 dst_rdy_prot <= 1;
	 while(~src_rdy_prot)
	   @(posedge clk);
	 while(1)
	   begin
	      while(~src_rdy_prot)
		@(posedge clk);
	      $display("Read: %h",prot_out);
	      @(posedge clk);
	   end
      end
   endtask // ReadFromFIFO36
   
   task PutPacketInFIFO36;
      input [31:0] data_start;
      input [31:0] data_len;
      begin
	 count 	      <= 4;
	 src_rdy_f36i <= 1;
	 f36_data     <= 32'h0001_000c;
	 f36_sof      <= 1;
	 f36_eof      <= 0;
	 f36_occ      <= 0;
	
	 $display("Put Packet in FIFO36");
	 while(~dst_rdy_f36i)
	   @(posedge clk);
	 @(posedge clk);
	 $display("PPI_FIFO36: Entered First Line");
	 f36_sof  <= 0;
	 f36_data <= data_start;
	 while(~dst_rdy_f36i)
	   @(posedge clk);
	 @(posedge clk);
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
   
   initial $dumpfile("prot_eng_tx_tb.vcd");
   initial $dumpvars(0,prot_eng_tx_tb);

   initial
     begin
	#10000;
	@(posedge clk);
	ReadFromFIFO36;
     end
   
   initial
     begin
	@(negedge rst);
	@(posedge clk);
	WriteSREG(BASE, {12'b0, 4'h0, 16'h0000});
	WriteSREG(BASE+1, {12'b0, 4'h0, 16'h0000});
	WriteSREG(BASE+2, {12'b0, 4'h0, 16'hABCD});
	WriteSREG(BASE+3, {12'b0, 4'h0, 16'h1234});
	WriteSREG(BASE+4, {12'b0, 4'h8, 16'h5678});
	WriteSREG(BASE+5, {12'b0, 4'h0, 16'hABCD});
	WriteSREG(BASE+6, {12'b0, 4'h0, 16'hABCD});
	WriteSREG(BASE+7, {12'b0, 4'h0, 16'hABCD});
	WriteSREG(BASE+8, {12'b0, 4'h0, 16'hABCD});
	WriteSREG(BASE+9, {12'b0, 4'h0, 16'hABCD});
	@(posedge clk);
	PutPacketInFIFO36(32'hA0B0C0D0,16);
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
endmodule // prot_eng_tx_tb
