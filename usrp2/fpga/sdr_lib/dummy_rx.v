
`define DSP_CORE_RX_BASE 160
module dummy_rx
  (input clk, input rst,
   input set_stb, input [7:0] set_addr, input [31:0] set_data,

   input [13:0] adc_a, input adc_ovf_a,
   input [13:0] adc_b, input adc_ovf_b,
   
   output [31:0] sample,
   input run,
   output strobe
   );

   wire [15:0] scale_i, scale_q;
   wire [31:0] phase_inc;
   reg [31:0]  phase;

   wire [23:0] i_decim, q_decim;
   wire [7:0]  decim_rate;
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+0)) sr_0
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(phase_inc),.changed());
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+1)) sr_1
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({scale_i,scale_q}),.changed());
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+2)) sr_2
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(decim_rate),.changed());
   
   strobe_gen strobe_gen(.clock(clk),.reset(rst),.enable(run),.rate(decim_rate),
			 .strobe_in(1),.strobe(strobe) );

   reg [15:0] i_out, q_out;
   assign      sample = {i_out,q_out};

   always @(posedge clk)
     if(rst)
       i_out <= 0;
     else if(~run)
       i_out <= 0;
     else if(strobe)
       i_out <= i_out + 1;

   reg 	       run_d1;
   always @(posedge clk)
     if(rst)
       run_d1 <= 0;
     else
       run_d1 <= run;

   always @(posedge clk)
     if(rst)
       q_out <= 0;
     else if (run & ~run_d1)
       q_out <= q_out + 1;
       
   
endmodule // dsp_core_rx
