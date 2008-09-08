

module test_hbd();

   reg clock;
   initial clock = 1'b0;
   always #5 clock <= ~clock;

   reg reset;
   initial reset = 1'b1;
   initial #1000 reset = 1'b0;
   
   initial $dumpfile("test_hbd.vcd");
   initial $dumpvars(0,test_hbd);

   reg [15:0] i_in, q_in;
   wire [15:0] i_out, q_out;

   reg 	       strobe_in;
   wire        strobe_out;
   reg 	       coeff_write;
   reg [15:0]  coeff_data;
   reg [4:0]   coeff_addr;
   
   halfband_decim halfband_decim 
     ( .clock(clock),.reset(reset),.enable(),.strobe_in(strobe_in),.strobe_out(strobe_out),
       .data_in(i_in),.data_out(i_out) );
   
   always @(posedge strobe_out)
     if(i_out[15])
       $display("-%d",65536-i_out);
     else
       $display("%d",i_out);

   initial
     begin
	strobe_in = 1'b0;
	@(negedge reset);
	@(posedge clock);
	while(1)
	  begin
	     strobe_in <= #1 1'b1;
	     @(posedge clock);
	     strobe_in <= #1 1'b0;
	     repeat (`RATE)
	       @(posedge clock);
	  end
     end

   initial #10000000 $finish;    // Just in case...

   initial
     begin
	i_in <= #1 16'd0;
	repeat (40) @(posedge strobe_in);
	i_in <= #1 16'd16384;
	@(posedge strobe_in);
	i_in <= #1 16'd0;
	repeat (40) @(posedge strobe_in);
	i_in <= #1 16'd16384;
	@(posedge strobe_in);
	i_in <= #1 16'd0;
	repeat (40) @(posedge strobe_in);
	i_in <= #1 16'd16384;
	repeat (40) @(posedge strobe_in);
	i_in <= #1 16'd0;
	repeat (41) @(posedge strobe_in);
	i_in <= #1 16'd16384;
	repeat (40) @(posedge strobe_in);
	i_in <= #1 16'd0;
	repeat (40) @(posedge strobe_in);
	repeat (7) @(posedge clock);
	$finish;
     end // initial begin
endmodule // test_hb
