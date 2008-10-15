
`define DSP_CORE_RX_BASE 160
module dsp_core_rx
  (input clk, input rst,
   input set_stb, input [7:0] set_addr, input [31:0] set_data,

   input [13:0] adc_a, input adc_ovf_a,
   input [13:0] adc_b, input adc_ovf_b,
   
   output [31:0] sample,
   input run,
   output strobe,
   output [31:0] debug
   );

   wire [15:0] scale_i, scale_q;
   wire [13:0] adc_a_ofs, adc_b_ofs;
   reg [13:0] adc_i, adc_q;
   wire [31:0] phase_inc;
   reg [31:0]  phase;

   wire [35:0] prod_i, prod_q;
   wire [23:0] i_cordic, q_cordic;
   wire [23:0] i_cic, q_cic;
   wire [17:0] i_cic_scaled, q_cic_scaled;
   wire [17:0] i_hb1, q_hb1;
   wire [17:0] i_hb2, q_hb2;
   wire [15:0] i_out, q_out;

   wire        strobe_cic, strobe_hb1, strobe_hb2;
   wire        enable_hb1, enable_hb2;
   wire [7:0]  cic_decim_rate;
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+0)) sr_0
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(phase_inc),.changed());
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+1)) sr_1
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({scale_i,scale_q}),.changed());
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+2)) sr_2
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({enable_hb1, enable_hb2, cic_decim_rate}),.changed());

   rx_dcoffset #(.WIDTH(14),.ADDR(`DSP_CORE_RX_BASE+6)) rx_dcoffset_a
     (.clk(clk),.rst(rst),.set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .adc_in(adc_a),.adc_out(adc_a_ofs));
   
   rx_dcoffset #(.WIDTH(14),.ADDR(`DSP_CORE_RX_BASE+7)) rx_dcoffset_b
     (.clk(clk),.rst(rst),.set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .adc_in(adc_b),.adc_out(adc_b_ofs));

   wire [3:0]  muxctrl;
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+8)) sr_8
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(muxctrl),.changed());

   // The TVRX connects to what is called adc_b, thus A and B are
   // swapped throughout the design.
   //
   // In the interest of expediency and keeping the s/w sane, we just remap them here.
   // The I & Q fields are mapped the same:
   // 0 -> "the real A" (as determined by the TVRX)
   // 1 -> "the real B"
   // 2 -> const zero
   
   always @(posedge clk)
     case(muxctrl[1:0])		// The I mapping
       0: adc_i <= adc_b_ofs;	// "the real A"
       1: adc_i <= adc_a_ofs;
       2: adc_i <= 0;
       default: adc_i <= 0;
     endcase // case(muxctrl[1:0])
          
   always @(posedge clk)
     case(muxctrl[3:2])		// The Q mapping
       0: adc_q <= adc_b_ofs;	// "the real A"
       1: adc_q <= adc_a_ofs;
       2: adc_q <= 0;
       default: adc_q <= 0;
     endcase // case(muxctrl[3:2])
       
   always @(posedge clk)
     if(rst)
       phase <= 0;
     else if(run)
       phase <= phase + phase_inc;

   MULT18X18S mult_i
     (.P(prod_i),    // 36-bit multiplier output
      .A({{4{adc_i[13]}},adc_i} ),    // 18-bit multiplier input
      .B({{2{scale_i[15]}},scale_i}),    // 18-bit multiplier input
      .C(clk),    // Clock input
      .CE(1),  // Clock enable input
      .R(rst)     // Synchronous reset input
      );

   MULT18X18S mult_q
     (.P(prod_q),    // 36-bit multiplier output
      .A({{4{adc_q[13]}},adc_q} ),    // 18-bit multiplier input
      .B({{2{scale_q[15]}},scale_q}),    // 18-bit multiplier input
      .C(clk),    // Clock input
      .CE(1),  // Clock enable input
      .R(rst)     // Synchronous reset input
      ); 

   cordic #(.bitwidth(24))
     cordic(.clock(clk), .reset(rst), .enable(run),
	    .xi(prod_i[23:0]),. yi(prod_q[23:0]), .zi(phase[31:16]),
	    .xo(i_cordic),.yo(q_cordic),.zo() );

   cic_strober cic_strober(.clock(clk),.reset(rst),.enable(run),.rate(cic_decim_rate),
			   .strobe_fast(1),.strobe_slow(strobe_cic) );

   cic_decim #(.bw(24))
     decim_i (.clock(clk),.reset(rst),.enable(run),
	      .rate(cic_decim_rate),.strobe_in(1'b1),.strobe_out(strobe_cic),
	      .signal_in(i_cordic),.signal_out(i_cic));
   
   cic_decim #(.bw(24))
     decim_q (.clock(clk),.reset(rst),.enable(run),
	      .rate(cic_decim_rate),.strobe_in(1'b1),.strobe_out(strobe_cic),
	      .signal_in(q_cordic),.signal_out(q_cic));

   round_reg #(.bits_in(24),.bits_out(18)) round_icic (.clk(clk),.in(i_cic),.out(i_cic_scaled));
   round_reg #(.bits_in(24),.bits_out(18)) round_qcic (.clk(clk),.in(q_cic),.out(q_cic_scaled));
   reg 	       strobe_cic_d1;
   always @(posedge clk) strobe_cic_d1 <= strobe_cic;
   
   small_hb_dec #(.WIDTH(18)) small_hb_i
     (.clk(clk),.rst(rst),.bypass(~enable_hb1),
      .stb_in(strobe_cic_d1),.data_in(i_cic_scaled),.stb_out(strobe_hb1),.data_out(i_hb1));
   
   small_hb_dec #(.WIDTH(18)) small_hb_q
     (.clk(clk),.rst(rst),.bypass(~enable_hb1),
      .stb_in(strobe_cic_d1),.data_in(q_cic_scaled),.stb_out(),.data_out(q_hb1));

   wire [8:0]  cpi_hb = enable_hb1 ? {cic_decim_rate,1'b0} : {1'b0,cic_decim_rate};
   hb_dec #(.IWIDTH(18), .OWIDTH(18), .CWIDTH(18), .ACCWIDTH(24)) hb_i
     (.clk(clk),.rst(rst),.bypass(~enable_hb2),.cpi(cpi_hb),
      .stb_in(strobe_hb1),.data_in(i_hb1),.stb_out(strobe_hb2),.data_out(i_hb2));

   hb_dec #(.IWIDTH(18), .OWIDTH(18), .CWIDTH(18), .ACCWIDTH(24)) hb_q
     (.clk(clk),.rst(rst),.bypass(~enable_hb2),.cpi(cpi_hb),
      .stb_in(strobe_hb1),.data_in(q_hb1),.stb_out(),.data_out(q_hb2));

   round #(.bits_in(18),.bits_out(16)) round_iout (.in(i_hb2),.out(i_out));
   round #(.bits_in(18),.bits_out(16)) round_qout (.in(q_hb2),.out(q_out));

   assign      sample = {i_out,q_out};
   assign      strobe = strobe_hb2;
   assign      debug = {enable_hb1, enable_hb2, run, strobe, strobe_cic, strobe_cic_d1, strobe_hb1, strobe_hb2};
   
endmodule // dsp_core_rx
