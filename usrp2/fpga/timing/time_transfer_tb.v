
`timescale 1ns / 1ps

module time_transfer_tb();

   reg clk = 0, rst = 1;
   always #5 clk = ~clk;
   
   initial 
     begin
	@(negedge clk);
	@(negedge clk);
	rst <= 0;
     end

   initial $dumpfile("time_transfer_tb.vcd");
   initial $dumpvars(0,time_transfer_tb);
   
   initial #100000000 $finish;

   wire exp_pps, pps, pps_rcv;
   wire [31:0] master_clock_rcv;
   reg [31:0]  master_clock = 0;
   reg [31:0]  counter = 0;

   localparam  PPS_PERIOD = 109;
   always @(posedge clk)
     if(counter == PPS_PERIOD)
       counter <= 0;
     else
       counter <= counter + 1;
   assign      pps = (counter == (PPS_PERIOD-1));
   
   always @(posedge clk)
     master_clock <= master_clock + 1;
   
   time_sender time_sender
     (.clk(clk),.rst(rst),
      .master_clock(master_clock),
      .pps(pps),
      .exp_pps_out(exp_pps) );

   time_receiver time_receiver
     (.clk(clk),.rst(rst),
      .master_clock(master_clock_rcv),
      .pps(pps_rcv),
      .exp_pps_in(exp_pps) );

   wire [31:0] delta = master_clock - master_clock_rcv;
endmodule // time_transfer_tb
