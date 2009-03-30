
module buffer_int_tb ();

   reg clk = 0;
   reg rst = 1;

   initial #100 rst = 0;
   always #5 clk = ~clk;

   wire en, we;
   wire [8:0] addr;
   wire [31:0] fifo2buf, buf2fifo;
   
   wire [31:0] rd_data_o;
   wire [3:0]  rd_flags_o;
   wire        rd_sop_o, rd_eop_o;
   reg 	       rd_error_i = 0, rd_read_i = 0;
   
   reg [31:0]  wr_data_i = 0;
   wire [3:0]  wr_flags_i;
   reg 	       wr_eop_i = 0, wr_sop_i = 0;
   reg 	       wr_write_i = 0;
   wire        wr_ready_o, wr_full_o;
   
   reg 	       clear = 0, write = 0, read = 0;
   reg [8:0]   firstline = 0, lastline = 0;
   wire [3:0]  step = 1;
   wire [31:0] ctrl_word = {4'b0,3'b0,clear,write,read,step,lastline,firstline};
   reg 	       go = 0;
   wire        done, error;

   assign      wr_flags_i = {2'b00, wr_eop_i, wr_sop_i};
   assign      rd_sop_o = rd_flags_o[0];
   assign      rd_eop_o = rd_flags_o[1];
   
   buffer_int buffer_int
     (.clk(clk),.rst(rst),
      .ctrl_word(ctrl_word),.go(go),
      .done(done),.error(error),
      
      // Buffer Interface
      .en_o(en),.we_o(we),.addr_o(addr),
      .dat_to_buf(fifo2buf),.dat_from_buf(buf2fifo),

      // Write FIFO Interface
      .wr_data_i(wr_data_i), .wr_flags_i(wr_flags_i), .wr_write_i(wr_write_i), .wr_ready_o(wr_ready_o),

      // Read FIFO Interface
      .rd_data_o(rd_data_o), .rd_flags_o(rd_flags_o), .rd_ready_o(rd_ready_o), .rd_read_i(rd_read_i)
      );
   
   reg 	       ram_en = 0, ram_we = 0;
   reg [8:0]   ram_addr = 0;
   reg [31:0]  ram_data = 0;
   
   ram_2port #(.DWIDTH(32),.AWIDTH(9)) ram_2port
     (.clka(clk), .ena(ram_en), .wea(ram_we), .addra(ram_addr), .dia(ram_data), .doa(),
      .clkb(clk), .enb(en), .web(we), .addrb(addr), .dib(fifo2buf), .dob(buf2fifo) );
   
   initial
     begin
	@(negedge rst);
	@(posedge clk);
	FillRAM;

	ResetBuffer;
	SetBufferRead(5,10);
	$display("Testing full read, no wait states.");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(6,0);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferRead(5,10);
	$display("Testing full read, 2 wait states.");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(6,2);
	repeat (10)
	  @(posedge clk);

	ResetBuffer;
	SetBufferRead(5,10);
	$display("Testing partial read, 0 wait states, then nothing after last.");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(3,0);
	repeat (10)
	  @(posedge clk);

	ResetBuffer;
	SetBufferRead(5,10);
	$display("Testing partial read, 0 wait states, then done at same time as last.");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(2,0);
	ReadALine;
	repeat (10)
	  @(posedge clk);

	ResetBuffer;
	SetBufferRead(5,10);
	$display("Testing partial read, 3 wait states, then error at same time as last.");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(2,3);
	rd_error_i <= 1;
	ReadALine;
	rd_error_i <= 0;
	repeat (10)
	  @(posedge clk);

	ResetBuffer;
	SetBufferRead(500,511);
	$display("Testing full read, to the end of the buffer.");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(12,0);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferRead(0,511);
	$display("Testing full read, start to end of the buffer.");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(512,0);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferRead(505,3);
	$display("Testing full read, wraparound");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(11,0);
	repeat (10)
	  @(posedge clk);

	ResetBuffer;
	SetBufferWrite(10,15);
	$display("Testing Full Write, no wait states");
	while(!wr_ready_o)
	  @(posedge clk);
	WriteLines(6,0,72);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferWrite(18,23);
	$display("Testing Full Write, 1 wait states");
	while(!wr_ready_o)
	  @(posedge clk);
	WriteLines(6,1,101);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferWrite(27,40);
	$display("Testing Partial Write, 0 wait states");
	while(!wr_ready_o)
	  @(posedge clk);
	WriteLines(6,0,201);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferWrite(45,200);
	$display("Testing Partial Write, 0 wait states, then done and write simultaneously");
	while(!wr_ready_o)
	  @(posedge clk);
	wr_sop_i <= 1; wr_eop_i <= 0;
	WriteLines(6,0,301);
	wr_sop_i <= 0; wr_eop_i <= 1;
	WriteALine(400);
	wr_sop_i <= 0; wr_eop_i <= 0;
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferWrite(55,200);
	$display("Testing Partial Write, 0 wait states, then error");
	while(!wr_ready_o)
	  @(posedge clk);
	WriteLines(6,0,501);
	wr_sop_i <= 1; wr_eop_i <= 1;
	WriteALine(400);
	@(posedge clk);
	repeat (10)
	  @(posedge clk);
	wr_sop_i <= 0; wr_eop_i <= 0;
	
	ResetBuffer;
	SetBufferRead(0,82);
	$display("Testing read after all the writes");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(83,0);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferWrite(508,4);
	$display("Testing wraparound write");
	while(!wr_ready_o)
	  @(posedge clk);
	WriteLines(9,0,601);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferRead(506,10);
	$display("Reading wraparound write");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(17,0);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferWrite(0,511);
	$display("Testing Whole Buffer write");
	while(!wr_ready_o)
	  @(posedge clk);
	WriteLines(512,0,1000);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferRead(0,511);
	$display("Reading Whole Buffer write");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(512,0);
	repeat (10)
	  @(posedge clk);

	/*
	ResetBuffer;
	SetBufferWrite(5,10);
	$display("Testing Write Too Many");
	while(!wr_ready_o)
	  @(posedge clk);
	WriteLines(12,0,2000);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferRead(0,15);
	$display("Reading back Write Too Many");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(16,0);
	repeat (10)
	  @(posedge clk);
	*/
	ResetBuffer;
	SetBufferWrite(15,20);
	$display("Testing Write One Less Than Full");
	while(!wr_ready_o)
	  @(posedge clk);
	wr_sop_i <= 1; wr_eop_i <= 0;
	WriteALine(400);
	wr_sop_i <= 0; wr_eop_i <= 0;
	WriteLines(3,0,2000);
	wr_sop_i <= 0; wr_eop_i <= 1;
	WriteALine(400);
	wr_sop_i <= 0; wr_eop_i <= 0;
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	SetBufferRead(13,22);
	$display("Reading back Write One Less Than Full");
	while(!rd_sop_o)
	  @(posedge clk);
	ReadLines(10,0);
	repeat (10)
	  @(posedge clk);
	
	ResetBuffer;
	repeat(100)
	  @(posedge clk);
	$finish;
     end
   
   always @(posedge clk)
     if(rd_read_i == 1'd1)
       $display("READ Buffer %d, rd_sop_o %d, rd_eop_o %d", rd_data_o, rd_sop_o, rd_eop_o);

   always @(posedge clk)
     if(wr_write_i == 1'd1)
       $display("WRITE Buffer %d,  wr_ready_o %d, wr_full_o %d", wr_data_i, wr_ready_o, wr_full_o);
	   
   initial begin
      $dumpfile("buffer_int_tb.lxt");
      $dumpvars(0,buffer_int_tb);
   end

   task FillRAM;
      begin
	 ram_addr <= 0;
	 ram_data <= 0;
	 @(posedge clk);
	 ram_en <= 1;
	 ram_we <= 1;
	 @(posedge clk);
	 repeat (511)
	   begin
	      ram_addr <= ram_addr + 1;
	      ram_data <= ram_data + 1;
	      ram_en <= 1;
	      ram_we <= 1;
	      @(posedge clk);
	   end
	 ram_en <= 0;
	 ram_we <= 0;
	 @(posedge clk);
	 $display("Filled the RAM");
      end
   endtask // FillRAM

   task ResetBuffer;
      begin
	 clear <= 1; read <= 0; write <= 0;
	 go <= 1;
	 @(posedge clk);
	 go <= 0;
	 @(posedge clk);
	 $display("Buffer Reset");
      end
   endtask // ClearBuffer
   
   task SetBufferWrite;
      input [8:0] start;
      input [8:0] stop;
      begin
	 clear <= 0; read <= 0; write <= 1;
	 firstline <= start;
	 lastline <= stop;
	 go <= 1;
	 @(posedge clk);
	 go <= 0;
	 @(posedge clk);
	 $display("Buffer Set for Write");
      end
   endtask // SetBufferWrite
   
   task SetBufferRead;
      input [8:0] start;
      input [8:0] stop;
      begin
	 clear <= 0; read <= 1; write <= 0;
	 firstline <= start;
	 lastline <= stop;
	 go <= 1;
	 @(posedge clk);
	 go <= 0;
	 @(posedge clk);
	 $display("Buffer Set for Read");
      end
   endtask // SetBufferRead

   task ReadALine;
      begin
	 while(~rd_ready_o)
	   @(posedge clk);
	 #1 rd_read_i <= 1;
	 @(posedge clk);
	 rd_read_i <= 0;
      end
   endtask // ReadALine

   task ReadLines;
      input [9:0] lines;
      input [7:0] wait_states;
      begin
	 $display("Read Lines: Number %d, Wait States %d",lines,wait_states);
	 repeat (lines)
	   begin
	      ReadALine;
	      repeat (wait_states)
		@(posedge clk);
	   end
      end
   endtask // ReadLines
   
   task WriteALine;
      input [31:0] value;
      begin
	 while(~wr_ready_o)
	   @(posedge clk);
	 #1 wr_write_i <= 1;
	 wr_data_i <= value;
	 @(posedge clk);
	 wr_write_i <= 0;
      end
   endtask // WriteALine
   
   task WriteLines;
      input [9:0] lines;
      input [7:0] wait_states;
      input [31:0] value;
      begin
	 $display("Write Lines: Number %d, Wait States %d",lines,wait_states);
	 repeat(lines)
	   begin
	      value <= value + 1;
	      WriteALine(value);
	      repeat(wait_states)
		@(posedge clk);
	   end
      end
   endtask // WriteLines
   
endmodule // buffer_int_tb
