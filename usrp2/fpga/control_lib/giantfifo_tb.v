module fifo_tb();

   localparam WIDTH = 36;
   reg clk, rst;
   wire short_full, short_empty, long_full, long_empty, giant_full, giant_empty;
   wire casc_full, casc_empty, casc2_full, casc2_empty;
   reg 	read, write;
   
   wire [WIDTH-1:0] short_do, long_do, casc_do, casc2_do, giant_do;
   reg [WIDTH-1:0]  di;

   reg 	      clear = 0;
   
   shortfifo #(.WIDTH(WIDTH)) shortfifo
     (.clk(clk),.rst(rst),.datain(di),.dataout(short_do),.clear(clear),
      .read(read),.write(write),.full(short_full),.empty(short_empty));
   
   longfifo #(.WIDTH(WIDTH), .SIZE(4)) longfifo
     (.clk(clk),.rst(rst),.datain(di),.dataout(long_do),.clear(clear),
      .read(read),.write(write),.full(long_full),.empty(long_empty));
   
   cascadefifo #(.WIDTH(WIDTH), .SIZE(4)) cascadefifo
     (.clk(clk),.rst(rst),.datain(di),.dataout(casc_do),.clear(clear),
      .read(read),.write(write),.full(casc_full),.empty(casc_empty));
   
   cascadefifo2 #(.WIDTH(WIDTH), .SIZE(4)) cascadefifo2
     (.clk(clk),.rst(rst),.datain(di),.dataout(casc2_do),.clear(clear),
      .read(read),.write(write),.full(casc2_full),.empty(casc2_empty));

   wire [17:0] RAM_D;
   wire [18:0] RAM_A;
   wire        RAM_CLK, RAM_WEn, RAM_LDn, RAM_CE1n, RAM_OEn, RAM_CENn;
   
   giantfifo #(.WIDTH(WIDTH)) giantfifo
     (.clk(clk),.rst(rst),.datain(di),.dataout(giant_do),.clear(clear),
      .read(read),.write(write),.full(giant_full),.empty(giant_empty),
      .RAM_D(RAM_D),.RAM_A(RAM_A),.RAM_CE1n(RAM_CE1n),.RAM_CENn(RAM_CENn),
      .RAM_CLK(RAM_CLK),.RAM_WEn(RAM_WEn),.RAM_OEn(RAM_OEn),.RAM_LDn(RAM_LDn)
      );

   wire        MODE = 1'b0;
   cy1356 ram_model(.d(RAM_D),.clk(RAM_CLK),.a(RAM_A),
		    .bws(2'b00),.we_b(RAM_WEn),.adv_lb(RAM_LDn),
		    .ce1b(RAM_CE1n),.ce2(1'b1),.ce3b(1'b0),
		    .oeb(RAM_OEn),.cenb(RAM_CENn),.mode(MODE) 
		    );
   
   initial rst = 1;
   initial #1000 rst = 0;
   initial clk = 0;
   always #50 clk = ~clk;
   
   initial di = 36'h300AE;
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
   
   initial $dumpfile("giantfifo_tb.vcd");
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
