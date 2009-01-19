
`define DSP_CORE_TX_BASE 128

module dsp_core_tx
  (input clk, input rst,
   input set_stb, input [7:0] set_addr, input [31:0] set_data,

   output reg [15:0] dac_a,
   output reg [15:0] dac_b,

   // To tx_control
   input [31:0] sample,
   input run,
   output strobe,
   output [31:0] debug
   );

   wire [15:0] i, q, scale_i, scale_q;
   wire [31:0] phase_inc;
   reg [31:0]  phase;
   wire [7:0]  interp_rate;
   wire [3:0]  dacmux_a, dacmux_b;
   wire        enable_hb1, enable_hb2;

   setting_reg #(.my_addr(`DSP_CORE_TX_BASE+0)) sr_0
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(phase_inc),.changed());

   setting_reg #(.my_addr(`DSP_CORE_TX_BASE+1)) sr_1
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({scale_i,scale_q}),.changed());
   
   setting_reg #(.my_addr(`DSP_CORE_TX_BASE+2)) sr_2
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({enable_hb1, enable_hb2, interp_rate}),.changed());

   setting_reg #(.my_addr(`DSP_CORE_TX_BASE+4)) sr_4
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({dacmux_b,dacmux_a}),.changed());

   // Strobes are all now delayed by 1 cycle for timing reasons
   wire        strobe_cic_pre, strobe_hb1_pre, strobe_hb2_pre;
   reg 	       strobe_cic = 1;
   reg 	       strobe_hb1 = 1;
   reg 	       strobe_hb2 = 1;
   
   cic_strober #(.WIDTH(8))
     cic_strober(.clock(clk),.reset(rst),.enable(run),.rate(interp_rate),
		 .strobe_fast(1),.strobe_slow(strobe_cic_pre) );
   cic_strober #(.WIDTH(2))
     hb2_strober(.clock(clk),.reset(rst),.enable(run),.rate(enable_hb2 ? 2 : 1),
		 .strobe_fast(strobe_cic_pre),.strobe_slow(strobe_hb2_pre) );
   cic_strober #(.WIDTH(2))
     hb1_strober(.clock(clk),.reset(rst),.enable(run),.rate(enable_hb1 ? 2 : 1),
		 .strobe_fast(strobe_hb2_pre),.strobe_slow(strobe_hb1_pre) );
   
   always @(posedge clk) strobe_hb1 <= strobe_hb1_pre;
   always @(posedge clk) strobe_hb2 <= strobe_hb2_pre;
   always @(posedge clk) strobe_cic <= strobe_cic_pre;

   // NCO
   always @(posedge clk)
     if(rst)
       phase <= 0;
     else if(~run)
       phase <= 0;
     else
       phase <= phase + phase_inc;
   
   wire        signed [17:0] da, db;
   wire        signed [35:0] prod_i, prod_q;

   wire [17:0] bb_i = {sample[31:16],2'b0};
   wire [17:0] bb_q = {sample[15:0],2'b0};
   wire [17:0] i_interp, q_interp;

   wire [17:0] hb1_i, hb1_q, hb2_i, hb2_q;

   wire [7:0]  cpo = enable_hb2 ? ({interp_rate,1'b0}) : interp_rate;
   // Note that max CIC rate is 128, which would give an overflow on cpo if enable_hb2 is true,
   //   but the default case inside hb_interp handles this
   
   hb_interp #(.IWIDTH(18),.OWIDTH(18),.ACCWIDTH(24)) hb_interp_i
     (.clk(clk),.rst(rst),.bypass(~enable_hb1),.cpo(cpo),.stb_in(strobe_hb1),.data_in(bb_i),.stb_out(strobe_hb2),.data_out(hb1_i));
   hb_interp #(.IWIDTH(18),.OWIDTH(18),.ACCWIDTH(24)) hb_interp_q
     (.clk(clk),.rst(rst),.bypass(~enable_hb1),.cpo(cpo),.stb_in(strobe_hb1),.data_in(bb_q),.stb_out(strobe_hb2),.data_out(hb1_q));
   
   small_hb_int #(.WIDTH(18)) small_hb_interp_i
     (.clk(clk),.rst(rst),.bypass(~enable_hb2),.stb_in(strobe_hb2),.data_in(hb1_i),
      .output_rate(interp_rate),.stb_out(strobe_cic),.data_out(hb2_i));
   small_hb_int #(.WIDTH(18)) small_hb_interp_q
     (.clk(clk),.rst(rst),.bypass(~enable_hb2),.stb_in(strobe_hb2),.data_in(hb1_q),
      .output_rate(interp_rate),.stb_out(strobe_cic),.data_out(hb2_q));
   
   cic_interp  #(.bw(18),.N(4),.log2_of_max_rate(7))
     cic_interp_i(.clock(clk),.reset(rst),.enable(run),.rate(interp_rate),
		  .strobe_in(strobe_cic),.strobe_out(1),
		  .signal_in(hb2_i),.signal_out(i_interp));
   
   cic_interp  #(.bw(18),.N(4),.log2_of_max_rate(7))
     cic_interp_q(.clock(clk),.reset(rst),.enable(run),.rate(interp_rate),
		  .strobe_in(strobe_cic),.strobe_out(1),
		  .signal_in(hb2_q),.signal_out(q_interp));

   assign      strobe = strobe_hb1;

   localparam  cwidth = 24;  // was 18
   localparam  zwidth = 24;  // was 16

   wire [cwidth-1:0] da_c, db_c;
   
   cordic_z24 #(.bitwidth(cwidth))
     cordic(.clock(clk), .reset(rst), .enable(run),
	    .xi({i_interp,{(cwidth-18){1'b0}}}),.yi({q_interp,{(cwidth-18){1'b0}}}),
	    .zi(phase[31:32-zwidth]),
	    .xo(da_c),.yo(db_c),.zo() );
   
   MULT18X18S MULT18X18S_inst 
     (.P(prod_i),    // 36-bit multiplier output
      .A(da_c[cwidth-1:cwidth-18]),    // 18-bit multiplier input
      .B({{2{scale_i[15]}},scale_i}),    // 18-bit multiplier input
      .C(clk),    // Clock input
      .CE(1),  // Clock enable input
      .R(rst)     // Synchronous reset input
      );
   
   MULT18X18S MULT18X18S_inst_2 
     (.P(prod_q),    // 36-bit multiplier output
      .A(db_c[cwidth-1:cwidth-18]),    // 18-bit multiplier input
      .B({{2{scale_q[15]}},scale_q}),    // 18-bit multiplier input
      .C(clk),    // Clock input
      .CE(1),  // Clock enable input
      .R(rst)     // Synchronous reset input
      );
   
   always @(posedge clk)
     case(dacmux_a)
       0 : dac_a <= prod_i[28:13];
       1 : dac_a <= prod_q[28:13];
       default : dac_a <= 0;
     endcase // case(dacmux_a)
   
   always @(posedge clk)
     case(dacmux_b)
       0 : dac_b <= prod_i[28:13];
       1 : dac_b <= prod_q[28:13];
       default : dac_b <= 0;
     endcase // case(dacmux_b)
   
   assign      debug = {strobe_cic, strobe_hb1, strobe_hb2,run};

endmodule // dsp_core
