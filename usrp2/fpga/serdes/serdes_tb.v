
// FIXME need to add flow control

module serdes_tb();
   
   reg clk, rst;
   wire ser_rx_clk, ser_tx_clk;
   wire ser_rklsb, ser_rkmsb, ser_tklsb, ser_tkmsb;
   wire [15:0] ser_r, ser_t;
   
   initial clk = 0;
   initial rst = 1;
   initial #1000 rst = 0;
   always #100 clk = ~clk;
   
   // Wishbone
   reg [31:0]  wb_dat_i;
   wire [31:0] wb_dat_o_rx, wb_dat_o_tx;
   reg 	       wb_we, wb_en_rx, wb_en_tx;
   reg [8:0]   wb_adr;

   // Buffer Control
   reg 	       go, clear, read, write;
   reg [3:0]   buf_num;
   wire [31:0] ctrl_word = {buf_num,3'b0,clear,write,read,step,lastline,firstline};
   reg [8:0]   firstline = 0, lastline = 0;
   reg [3:0]   step = 1;
   reg 	       first_tx = 1, first_rx = 1;  // for verif
   
   // TX Side
   reg 	       wb_we_tx;
   wire        en_tx, we_tx;
   wire [8:0]  addr_tx;
   wire [31:0] f2r_tx, r2f_tx;
   wire [31:0] data_tx;
   wire        read_tx, done_tx, error_tx, sop_tx, eop_tx;
   
   wire        fdone_tx, ferror_tx;
   
   reg 	       even;
   reg 	       channel_error = 0;
   
   serdes_tx serdes_tx
     (.clk(clk),.rst(rst),
      .ser_tx_clk(ser_tx_clk),.ser_t(ser_t),.ser_tklsb(ser_tklsb),.ser_tkmsb(ser_tkmsb),
      .rd_dat_i(data_tx),.rd_read_o(read_tx),.rd_done_o(done_tx),
      .rd_error_o(error_tx),.rd_sop_i(sop_tx),.rd_eop_i(eop_tx) );
   
   ram_2port #(.DWIDTH(32),.AWIDTH(9))
     ram_tx(.clka(clk),.ena(wb_en_tx),.wea(wb_we_tx),.addra(wb_adr),.dia(wb_dat_i),.doa(wb_dat_o_tx),
            .clkb(clk),.enb(en_tx),.web(we_tx),.addrb(addr_tx),.dib(f2r_tx),.dob(r2f_tx));
   
   buffer_int #(.BUFF_NUM(1)) buffer_int_tx
     (.clk(clk),.rst(rst),
      .ctrl_word(ctrl_word),.go(go),
      .done(fdone_tx),.error(ferror_tx),
      
      .en_o(en_tx),.we_o(we_tx),.addr_o(addr_tx),.dat_to_buf(f2r_tx),.dat_from_buf(r2f_tx),
      
      .wr_dat_i(0),.wr_write_i(0),.wr_done_i(0),
      .wr_error_i(0),.wr_ready_o(),.wr_full_o(),
      
      .rd_dat_o(data_tx),.rd_read_i(read_tx),.rd_done_i(done_tx),
      .rd_error_i(error_tx),.rd_sop_o(sop_tx),.rd_eop_o(eop_tx) );


   // RX Side
   reg 	       wb_we_rx;
   wire        en_rx, we_rx;
   wire [8:0]  addr_rx;
   wire [31:0] f2r_rx, r2f_rx;
   wire [31:0] data_rx;
   wire        write_rx, done_rx, error_rx, ready_rx, empty_rx;
   
   wire        fdone_rx, ferror_rx;
   
   serdes_rx serdes_rx
     (.clk(clk),.rst(rst),
      .ser_rx_clk(ser_rx_clk),.ser_r(ser_r),.ser_rklsb(ser_rklsb),.ser_rkmsb(ser_rkmsb),
      .wr_dat_o(data_rx),.wr_write_o(write_rx),.wr_done_o(done_rx),
      .wr_error_o(error_rx),.wr_ready_i(ready_rx),.wr_full_i(full_rx) );

   ram_2port #(.DWIDTH(32),.AWIDTH(9))
     ram_rx(.clka(clk),.ena(wb_en_rx),.wea(wb_we_rx),.addra(wb_adr),.dia(wb_dat_i),.doa(wb_dat_o_rx),
            .clkb(clk),.enb(en_rx),.web(we_rx),.addrb(addr_rx),.dib(f2r_rx),.dob(r2f_rx) );
   
   buffer_int #(.BUFF_NUM(0)) buffer_int_rx
     (.clk(clk),.rst(rst),
      .ctrl_word(ctrl_word),.go(go),
      .done(fdone_rx),.error(ferror_rx),
      
      .en_o(en_rx),.we_o(we_rx),.addr_o(addr_rx),.dat_to_buf(f2r_rx),.dat_from_buf(r2f_rx),
      
      .wr_dat_i(data_rx),.wr_write_i(write_rx),.wr_done_i(done_rx),
      .wr_error_i(error_rx),.wr_ready_o(ready_rx),.wr_full_o(full_rx),
      
      .rd_dat_o(),.rd_read_i(0),.rd_done_i(0),
      .rd_error_i(0),.rd_sop_o(),.rd_eop_o() );

   // Simulate the connection
   serdes_model serdes_model
     (.ser_tx_clk(ser_tx_clk), .ser_tkmsb(ser_tkmsb), .ser_tklsb(ser_tklsb), .ser_t(ser_t),
      .ser_rx_clk(ser_rx_clk), .ser_rkmsb(ser_rkmsb), .ser_rklsb(ser_rklsb), .ser_r(ser_r),
      .even(even), .error(channel_error) );
   
   initial begin
      wb_en_rx <= 0;
      wb_en_tx <=0;
      wb_we_tx <= 0;
      wb_we_rx <= 0;
      wb_adr <= 0;
      wb_dat_i <= 0;
      go <= 0;
      even <= 0;
      @(negedge rst);
      @(posedge clk);
      FillTXRAM;
      ClearRXRAM;
      ResetBuffer(0);
      ResetBuffer(1);

      // receive a full buffer
      ReceiveSERDES(0,10);
      SendSERDES(0,10);

      // Receive a partial buffer
      SendSERDES(11,20);
      ReceiveSERDES(11,50);

      // Receive too many for buffer
      SendSERDES(21,100);
      ReceiveSERDES(21,30);

      // Send 3 packets, then wait to receive them, so they stack up in the rx fifo
      SendSERDES(31,40);
      SendSERDES(41,50);
      SendSERDES(51,60);
      repeat (10)
	@(posedge clk);
      ReceiveSERDES(31,40);
      ReceiveSERDES(41,50);
      repeat (1000)
	@(posedge clk);
      ReceiveSERDES(51,60);

      // Overfill the FIFO, should get an error on 3rd packet
      SendSERDES(1,400);
      SendSERDES(1,400);
      
      
      WaitForTX;
      //WaitForRX;
      

      repeat(1000)
	@(posedge clk);
      ReceiveSERDES(101,500);
      ReceiveSERDES(101,500);
      ReadRAM(80);
      $finish;
   end // initial begin

   always @(posedge clk)
     if(write_rx)
       $display("SERDES RX, FIFO WRITE %x, FIFO RDY %d, FIFO FULL %d",data_rx, ready_rx, full_rx);
   
   always @(posedge clk)
     if(read_tx)
       $display("SERDES TX, FIFO READ %x, SOP %d, EOP %d",data_tx, sop_tx, eop_tx);
   
   initial begin
      $dumpfile("serdes_tb.vcd");
      $dumpvars(0,serdes_tb);
   end

   initial #10000000 $finish;

   initial #259300 channel_error <= 1;
   initial #259500 channel_error <= 0;
   
   task FillTXRAM;
      begin
	 wb_adr <= 0;
	 wb_dat_i <= 32'h10802000;
	 wb_we_tx <= 1;
	 wb_en_tx <= 1;
	 @(posedge clk);
	 repeat(511) begin
	    wb_dat_i <= wb_dat_i + 32'h00010001;
	    wb_adr <= wb_adr + 1;
	    @(posedge clk);
	 end // repeat (511)
	 wb_we_tx <= 0;
	 wb_en_tx <= 0;
	 @(posedge clk);
	 $display("Done entering Data into TX RAM\n");
      end
   endtask // FillTXRAM
   
   task ClearRXRAM;
      begin
	 wb_adr <= 0;
	 wb_dat_i <= 0;
	 wb_we_rx <= 1;
	 wb_en_rx <= 1;
	 wb_dat_i <= 0;
	 @(posedge clk);
	 repeat(511) begin
	    wb_adr <= wb_adr + 1;
	    @(posedge clk);
	 end // repeat (511)
	 wb_we_rx <= 0;
	 wb_en_rx <= 0;
	 @(posedge clk);
	 $display("Done clearing RX RAM\n");
      end
   endtask // FillRAM
   
   task ReadRAM;
      input [8:0] lastline;
      begin
	 wb_en_rx <= 1;
	 wb_adr <= 0;
	 @(posedge clk);
	 @(posedge clk);
	 repeat(lastline) begin
	    $display("ADDR: %h  DATA %h", wb_adr, wb_dat_o_rx);
	    wb_adr <= wb_adr + 1;
	    @(posedge clk);
	    @(posedge clk);
	 end // repeat (511)
	 $display("ADDR: %h  DATA %h", wb_adr, wb_dat_o_rx);
	 wb_en_rx <= 0;
	 @(posedge clk);
	 $display("Done reading out RX RAM\n");
      end
   endtask // FillRAM
   
   task ResetBuffer;
      input [3:0] buffer_num;
      begin
	 buf_num <= buffer_num;
	 clear <= 1; read <= 0; write <= 0;
	 go <= 1;
	 @(posedge clk);
	 go <= 0;
	 @(posedge clk);
	 $display("Buffer Reset");
      end
   endtask // ClearBuffer
   
   task SetBufferWrite;
      input [3:0] buffer_num;
      input [8:0] start;
      input [8:0] stop;
      begin
	 buf_num <= buffer_num;
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
      input [3:0] buffer_num;
      input [8:0] start;
      input [8:0] stop;
      begin
	 buf_num <= buffer_num;
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

   task WaitForTX;
      begin
	 while (!(fdone_tx | ferror_tx))
	   @(posedge clk);
      end
   endtask // WaitForTX
   
   task WaitForRX;
      begin
	 while (!(fdone_rx | ferror_rx))
	   @(posedge clk);
      end
   endtask // WaitForRX
   
   task SendSERDES;
      input [8:0] start;
      input [8:0] stop;
      begin
	 if(~first_tx)
	   WaitForTX;
	 else
	   first_tx <= 0;
	 ResetBuffer(1);
	 SetBufferRead(1,start,stop);
	 $display("Here");
      end
   endtask // SendSERDES
   
   task ReceiveSERDES;
      input [8:0] start;
      input [8:0] stop;
      begin
	 if(~first_rx)
	   WaitForRX;
	 else
	   first_rx <= 0;
	 ResetBuffer(0);
	 SetBufferWrite(0,start,stop);
	 $display("Here2");
      end
   endtask // ReceiveSERDES
   
endmodule // serdes_tb
