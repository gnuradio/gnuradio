module fifo_tb();
   
   reg clk, rst;
   wire short_full, short_empty, long_full, long_empty;
   wire casc_full, casc_empty, casc2_full, casc2_empty;
   reg 	read, write;
   
   wire [7:0] short_do, long_do;
   wire [7:0] casc_do, casc2_do;
   reg [7:0]  di;

   reg 	      clear = 0;
   
   shortfifo #(.WIDTH(8)) shortfifo
     (.clk(clk),.rst(rst),.datain(di),.dataout(short_do),.clear(clear),
      .read(read),.write(write),.full(short_full),.empty(short_empty));
   
   longfifo #(.WIDTH(8), .SIZE(4)) longfifo
     (.clk(clk),.rst(rst),.datain(di),.dataout(long_do),.clear(clear),
      .read(read),.write(write),.full(long_full),.empty(long_empty));
   
   cascadefifo #(.WIDTH(8), .SIZE(4)) cascadefifo
     (.clk(clk),.rst(rst),.datain(di),.dataout(casc_do),.clear(clear),
      .read(read),.write(write),.full(casc_full),.empty(casc_empty));
   
   cascadefifo2 #(.WIDTH(8), .SIZE(4)) cascadefifo2
     (.clk(clk),.rst(rst),.datain(di),.dataout(casc2_do),.clear(clear),
      .read(read),.write(write),.full(casc2_full),.empty(casc2_empty));
   
   initial rst = 1;
   initial #1000 rst = 0;
   initial clk = 0;
   always #50 clk = ~clk;
   
   initial di = 8'hAE;
   initial read = 0;
   initial write = 0;

   always @(posedge clk)
     if(write)
       di <= di + 1;
   
   always @(posedge clk)
     begin
	if(short_full != long_full)
	  $display("Error: FULL mismatch");
	if(short_empty != long_empty)
	  $display("Note: EMPTY mismatch, usually not a problem (longfifo has 2 cycle latency)");
	if(read & (short_do != long_do))
	  $display("Error: DATA mismatch");
     end
   
   initial $dumpfile("fifo_tb.vcd");
   initial $dumpvars(0,fifo_tb);

   initial
     begin
	@(negedge rst);
	@(posedge clk);
	repeat (10)
	  @(posedge clk);
	write <= 1;
	@(posedge clk);
	write <= 0;
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	read <= 1;
	@(posedge clk);
	read <= 0;
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);

	repeat(10)
	  begin
	     write <= 1;
	     @(posedge clk);
	     write <= 0;
	     @(posedge clk);
	     @(posedge clk);
	     @(posedge clk);
	     read <= 1;
	     @(posedge clk);
	     read <= 0;
	     @(posedge clk);
	     @(posedge clk);
	     @(posedge clk);
	     @(posedge clk);
	     @(posedge clk);
	  end // repeat (10)
	
	write <= 1;
	repeat (4)
	  @(posedge clk);
	write <= 0;
	@(posedge clk);
	read <= 1;
	repeat (4)
	  @(posedge clk);
	read <= 0;
	@(posedge clk);


	write <= 1;
	repeat (4)
	  @(posedge clk);
	write <= 0;
	@(posedge clk);
	repeat (4)
	  begin
	     read <= 1;
	     @(posedge clk);
	     read <= 0;
	     @(posedge clk);
	  end

	write <= 1;
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	read <= 1;
	repeat (5)
	  @(posedge clk);
	write <= 0;
	  @(posedge clk);
	  @(posedge clk);
	read <= 0;
	@(posedge clk);

	write <= 1;
	repeat (16)
	  @(posedge clk);
	write <= 0;
	@(posedge clk);
	
	read <= 1;
	repeat (16)
	  @(posedge clk);
	read <= 0;
	@(posedge clk);
		 
	repeat (10)
	  @(posedge clk);
	$finish;
     end
endmodule // longfifo_tb
