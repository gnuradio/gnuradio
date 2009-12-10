

module vita_tx_tb;

   localparam DECIM  = 8'd4;
   localparam INTERP = 8'd4;
   
   localparam MAXCHAN=4;
   localparam NUMCHAN=1;
   
   reg clk 	     = 0;
   reg reset 	     = 1;

   initial #1000 reset = 0;
   always #50 clk = ~clk;

   initial $dumpfile("vita_tx_tb.vcd");
   initial $dumpvars(0,vita_tx_tb);

   wire [(MAXCHAN*32)-1:0] sample, sample_tx;
   wire        strobe, run;
   wire [35:0] data_o;
   wire        src_rdy;
   wire        dst_rdy;
   
   wire [63:0] vita_time;

   reg 	       set_stb = 0;
   reg [7:0]   set_addr;
   reg [31:0]  set_data;
   wire        set_stb_dsp;
   wire [7:0]  set_addr_dsp;
   wire [31:0] set_data_dsp;

   /*
   settings_bus_crossclock settings_bus_xclk_dsp
     (.clk_i(clk), .rst_i(reset), .set_stb_i(set_stb), .set_addr_i(set_addr), .set_data_i(set_data),
      .clk_o(clk), .rst_o(reset), .set_stb_o(set_stb_dsp), .set_addr_o(set_addr_dsp), .set_data_o(set_data_dsp));
     */
 
   wire        sample_dst_rdy, sample_src_rdy;
   //wire [99:0] sample_data_o;
   wire [64+4+(MAXCHAN*32)-1:0] sample_data_o, sample_data_tx;

   time_64bit #(.TICKS_PER_SEC(100000000), .BASE(0)) time_64bit
     (.clk(clk), .rst(reset),
      .set_stb(set_stb), .set_addr(set_addr), .set_data(set_data),
      .pps(0), .vita_time(vita_time));
   
   rx_dsp_model rx_dsp_model
     (.clk(clk), .reset(reset), .run(run), .decim(DECIM), .strobe(strobe), .sample(sample[31:0]));

   generate
      if(MAXCHAN>1)
	assign sample[(MAXCHAN*32)-1:32] = 0;
   endgenerate
   
   vita_rx_control #(.BASE(0), .WIDTH(32*MAXCHAN)) vita_rx_control
     (.clk(clk), .reset(reset), .clear(0),
      .set_stb(set_stb), .set_addr(set_addr), .set_data(set_data),
      .vita_time(vita_time), .overrun(overrun),
      .sample_fifo_o(sample_data_o), .sample_fifo_dst_rdy_i(sample_dst_rdy), .sample_fifo_src_rdy_o(sample_src_rdy),
      .sample(sample), .run(run), .strobe(strobe));

   vita_rx_framer #(.BASE(0), .MAXCHAN(MAXCHAN)) vita_rx_framer
     (.clk(clk), .reset(reset), .clear(0),
      .set_stb(set_stb), .set_addr(set_addr), .set_data(set_data),
      .data_o(data_o), .dst_rdy_i(dst_rdy), .src_rdy_o(src_rdy),
      .sample_fifo_i(sample_data_o), .sample_fifo_dst_rdy_o(sample_dst_rdy), .sample_fifo_src_rdy_i(sample_src_rdy),
      .fifo_occupied(), .fifo_full(), .fifo_empty() );

   wire [35:0] 			data_tx;
   wire 			src_rdy_tx, dst_rdy_tx;
   wire 			sample_dst_rdy_tx, sample_src_rdy_tx;
   
   fifo_long #(.WIDTH(36)) fifo_short
     (.clk(clk), .reset(reset), .clear(0),
      .datain(data_o), .src_rdy_i(src_rdy), .dst_rdy_o(dst_rdy),
      .dataout(data_tx), .src_rdy_o(src_rdy_tx), .dst_rdy_i(dst_rdy_tx));
   
   vita_tx_deframer #(.BASE(16), .MAXCHAN(MAXCHAN)) vita_tx_deframer
     (.clk(clk), .reset(reset), .clear(0),
      .set_stb(set_stb), .set_addr(set_addr), .set_data(set_data),
      .data_i(data_tx), .dst_rdy_o(dst_rdy_tx), .src_rdy_i(src_rdy_tx),
      .sample_fifo_o(sample_data_tx), 
      .sample_fifo_dst_rdy_i(sample_dst_rdy_tx), .sample_fifo_src_rdy_o(sample_src_rdy_tx),
      .fifo_occupied(), .fifo_full(), .fifo_empty() );

   vita_tx_control #(.BASE(16), .WIDTH(MAXCHAN*32)) vita_tx_control
     (.clk(clk), .reset(reset), .clear(0),
      .set_stb(set_stb), .set_addr(set_addr), .set_data(set_data),
      .vita_time(vita_time-100), .underrun(underrun),
      .sample_fifo_i(sample_data_tx), 
      .sample_fifo_dst_rdy_o(sample_dst_rdy_tx), .sample_fifo_src_rdy_i(sample_src_rdy_tx),
      .sample(sample_tx), .run(run_tx), .strobe(strobe_tx));
   
   tx_dsp_model tx_dsp_model
     (.clk(clk), .reset(reset), .run(run_tx), .interp(INTERP), .strobe(strobe_tx), .sample(sample_tx[31:0] ));

   always @(posedge clk)
     if(src_rdy & dst_rdy)
       begin
	  if(data_o[32] & ~data_o[33])
	    begin
	       $display("RX-PKT-START %d",$time);
	       $display("       RX-PKT-DAT %x",data_o[31:0]);
	    end
	  else if(data_o[32] & data_o[33])
	    begin
	       $display("       RX-PKT-DAT %x -- With ERR",data_o[31:0]);
	       $display("RX-PKT-ERR %d",$time);
	    end
	  else if(~data_o[32] & data_o[33])
	    begin
	       $display("       RX-PKT-DAT %x",data_o[31:0]);
	       $display("RX-PKT-END %d",$time);
	    end
	  else
	    $display("       RX-PKT DAT %x",data_o[31:0]);
       end

   initial 
     begin
	@(negedge reset);
	@(posedge clk);
	write_setting(4,32'h14900008);  // VITA header
	write_setting(5,32'hF00D1234);  // VITA streamid
	write_setting(6,32'h98765432);  // VITA trailer
	write_setting(7,8);  // Samples per VITA packet
	write_setting(8,NUMCHAN);  // Samples per VITA packet
	#10000;
	
	queue_rx_cmd(1,0,8,32'h0,32'h0);  // send imm, single packet
/*
 	queue_rx_cmd(1,0,16,32'h0,32'h0);  // send imm, 2 packets worth
	queue_rx_cmd(1,0,7,32'h0,32'h0);  // send imm, 1 short packet worth
	queue_rx_cmd(1,0,9,32'h0,32'h0);  // send imm, just longer than 1 packet
	
	queue_rx_cmd(1,1,16,32'h0,32'h0);  // chained
	queue_rx_cmd(0,0,8,32'h0,32'h0);  // 2nd in chain
	
	queue_rx_cmd(1,1,17,32'h0,32'h0);  // chained, odd length
	queue_rx_cmd(0,0,9,32'h0,32'h0);  // 2nd in chain, also odd length
	
	queue_rx_cmd(0,0,8,32'h0,32'h340);  // send at, on time
	queue_rx_cmd(0,0,8,32'h0,32'h100);  // send at, but late

	queue_rx_cmd(1,1,8,32'h0,32'h0);  // chained, but break chain
	#100000;
	$display("\nEnd chain with zero samples, shouldn't error\n");
	queue_rx_cmd(1,1,8,32'h0,32'h0);  // chained
	queue_rx_cmd(0,0,0,32'h0,32'h0);  // end chain with zero samples, should keep us out of error
	#100000;

	$display("\nEnd chain with zero samples on odd-length, shouldn't error\n");
	queue_rx_cmd(1,1,14,32'h0,32'h0);  // chained
	queue_rx_cmd(0,0,0,32'h0,32'h0);  // end chain with zero samples, should keep us out of error
	#100000;
	$display("Should have gotten 14 samples and EOF by now\n");
	
	queue_rx_cmd(1,1,9,32'h0,32'h0);  // chained, but break chain, odd length
	#100000;
	//dst_rdy <= 0;  // stop pulling out of fifo so we can get an overrun
	queue_rx_cmd(1,0,100,32'h0,32'h0);  // long enough to fill the fifos
	queue_rx_cmd(1,0,5,32'h0,32'h0);  // this command waits until the previous error packet is sent
	#100000;
	//dst_rdy <= 1;  // restart the reads so we can see what we got
	#100000;
	//dst_rdy <= 0;  // stop pulling out of fifo so we can get an overrun
	queue_rx_cmd(1,1,100,32'h0,32'h0);  // long enough to fill the fifos
	//queue_rx_cmd(1,0,5,32'h0,32'h0);  // this command waits until the previous error packet is sent
	#100000;
	@(posedge clk);
	//dst_rdy <= 1;
	*/
	#100000 $finish;
     end

   task write_setting;
      input [7:0] addr;
      input [31:0] data;
      begin
	 set_stb <= 0;
	 @(posedge clk);
	 set_addr <= addr;
	 set_data <= data;
	 set_stb  <= 1;
	 @(posedge clk);
	 set_stb <= 0;
      end
   endtask // write_setting
   
   task queue_rx_cmd;
      input send_imm;
      input chain;
      input [29:0] lines;
      input [31:0] secs;
      input [31:0] tics;
      begin
	 write_setting(0,{send_imm,chain,lines});
	 write_setting(1,secs);
	 write_setting(2,tics);
      end
   endtask // queue_rx_cmd
   
endmodule // vita_tx_tb


module rx_dsp_model
  (input clk, input reset,
   input run,
   input [7:0] decim,
   output strobe,
   output [31:0] sample);
   
   reg [15:0] 	  pktnum = 0;
   reg [15:0] 	 counter = 0;

   reg 		 run_d1;
   always @(posedge clk) run_d1 <= run;
   
   always @(posedge clk)
     if(run & ~run_d1)
       begin
	  counter 		<= 0;
	  pktnum 		<= pktnum + 1;
       end
     else if(run & strobe)
       counter 			<= counter + 1;
       
   assign sample 		 = {pktnum,counter};

   reg [7:0] stb_ctr = 0;
   
   always @(posedge clk)
     if(reset)
       stb_ctr 	 <= 0;
     else if(run & ~run_d1)
       stb_ctr 	 <= 1;
     else if(run)
       if(stb_ctr == decim-1)
	 stb_ctr <= 0;
       else
	 stb_ctr <= stb_ctr + 1;

   assign strobe  = stb_ctr == decim-1;
   
endmodule // rx_dsp_model

module tx_dsp_model
  (input clk, input reset,
   input run,
   input [7:0] interp,
   output strobe,
   input [31:0] sample);

   cic_strober strober(.clock(clk), .reset(reset), .enable(run), .rate(interp), .strobe_fast(1), .strobe_slow(strobe));

   always @(posedge clk)
     if(strobe)
       $display("Time %d, Sent Sample %x",$time,sample);
   
   
endmodule // tx_dsp_model
