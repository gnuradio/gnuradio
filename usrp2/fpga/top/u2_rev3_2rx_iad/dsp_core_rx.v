`define DSP_CORE_RX_BASE 160
module dsp_core_rx
  (input clk, input rst,
   input set_stb, input [7:0] set_addr, input [31:0] set_data,

   input [13:0] adc_a, input adc_ovf_a,
   input [13:0] adc_b, input adc_ovf_b,
   
   input [15:0] io_rx,

   output reg [31:0] sample,
   input run,
   output strobe,
   output [31:0] debug
   );

   wire [15:0] scale_i, scale_q;
   wire [13:0] adc_a_ofs, adc_b_ofs;
   reg  [13:0] adc_i, adc_q;
   wire [31:0] phase_inc;
   reg  [31:0] phase;

   wire [35:0] prod_i, prod_q;
   wire [23:0] i_cordic_a, q_cordic_a, i_cordic_b, q_cordic_b;
   wire [31:0] i_iad_a, q_iad_a, i_iad_b, q_iad_b;
   wire [15:0] i_out_a, q_out_a, i_out_b, q_out_b;
   
   wire        enable_hb1, enable_hb2; // Correspond to std firmware settings
   wire [7:0]  cic_decim;              // for combined CIC/HB decimator
   wire [9:0]  decim_rate;             // Reconstructed original decimation setting
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+0)) sr_0
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(phase_inc),.changed());
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+1)) sr_1
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({scale_i,scale_q}),.changed());
   
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+2)) sr_2
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out({enable_hb1,enable_hb2,cic_decim}),.changed());

   rx_dcoffset #(.WIDTH(14),.ADDR(`DSP_CORE_RX_BASE+6)) rx_dcoffset_a
     (.clk(clk),.rst(rst),.set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .adc_in(adc_a),.adc_out(adc_a_ofs));
   
   rx_dcoffset #(.WIDTH(14),.ADDR(`DSP_CORE_RX_BASE+7)) rx_dcoffset_b
     (.clk(clk),.rst(rst),.set_stb(set_stb),.set_addr(set_addr),.set_data(set_data),
      .adc_in(adc_b),.adc_out(adc_b_ofs));

`ifdef MUXCTRL
   wire [3:0]  muxctrl;
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+8)) sr_8
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(muxctrl),.changed());
`endif
   
   wire [1:0] gpio_ena;
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+9)) sr_9
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(gpio_ena),.changed());

   // The TVRX connects to what is called adc_b, thus A and B are
   // swapped throughout the design.
   //
   // In the interest of expediency and keeping the s/w sane, we just remap them here.
   // The I & Q fields are mapped the same:
   // 0 -> "the real A" (as determined by the TVRX)
   // 1 -> "the real B"
   // 2 -> const zero

`ifdef MUXCTRL   
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
`else // !`ifdef MUXCTRL
   always @(posedge clk)
     begin
	adc_i <= adc_a_ofs;
	adc_q <= adc_b_ofs;
     end
`endif // !`ifdef MUXCTRL
   
   always @(posedge clk)
     if(rst)
       phase <= 0;
     else if(~run)
       phase <= 0;
     else
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

   
   // Route I,0 to first CORDIC
   cordic_z24 #(.bitwidth(24))
     cordic_a(.clock(clk), .reset(rst), .enable(run),
	      .xi(prod_i[24:1]),. yi(0), .zi(phase[31:8]),
	      .xo(i_cordic_a),.yo(q_cordic_a),.zo() );

   // Route Q,0 to second CORDIC
   cordic_z24 #(.bitwidth(24))
     cordic_b(.clock(clk), .reset(rst), .enable(run),
	      .xi(prod_q[24:1]),. yi(0), .zi(phase[31:8]),
	      .xo(i_cordic_b),.yo(q_cordic_b),.zo() );

   // Reconstruct original decimation rate from standard firmware settings
   assign decim_rate = enable_hb2 ? (enable_hb1 ? {cic_decim,2'b0} : 
                                                  {1'b0,cic_decim,1'b0 }) :
				    cic_decim;

   cic_strober #(.WIDTH(10)) // Convenient reuse of strobe generator
   cic_strober(.clock(clk),.reset(rst),.enable(run),.rate(decim_rate),
	       .strobe_fast(1),.strobe_slow(strobe_iad) );

   wire       strobe_iad_o;
   
   integrate #(.INPUTW(24),.ACCUMW(32),.OUTPUTW(32)) integrator_i_a
     (.clk_i(clk),.rst_i(rst),.ena_i(run),
      .dump_i(strobe_iad),.data_i(i_cordic_a),
      .stb_o(strobe_iad_o),.integ_o(i_iad_a) );

   integrate #(.INPUTW(24),.ACCUMW(32),.OUTPUTW(32)) integrator_q_a
     (.clk_i(clk),.rst_i(rst),.ena_i(run),
      .dump_i(strobe_iad),.data_i(q_cordic_a),
      .stb_o(),.integ_o(q_iad_a) );
   
   integrate #(.INPUTW(24),.ACCUMW(32),.OUTPUTW(32)) integrator_i_b
     (.clk_i(clk),.rst_i(rst),.ena_i(run),
      .dump_i(strobe_iad),.data_i(i_cordic_b),
      .stb_o(),.integ_o(i_iad_b) );

   integrate #(.INPUTW(24),.ACCUMW(32),.OUTPUTW(32)) integrator_q_b
     (.clk_i(clk),.rst_i(rst),.ena_i(run),
      .dump_i(strobe_iad),.data_i(q_cordic_b),
      .stb_o(),.integ_o(q_iad_b) );
   
   round #(.bits_in(32),.bits_out(16)) round_iout_a (.in(i_iad_a),.out(i_out_a));
   round #(.bits_in(32),.bits_out(16)) round_qout_a (.in(q_iad_a),.out(q_out_a));
   round #(.bits_in(32),.bits_out(16)) round_iout_b (.in(i_iad_b),.out(i_out_b));
   round #(.bits_in(32),.bits_out(16)) round_qout_b (.in(q_iad_b),.out(q_out_b));

   reg [31:0]  sample_out_a, sample_out_b, sample_out;
   reg 	       stb_d1, stb_d2, stb_d3, stb_d4, stb_d5;
   reg         strobe_out;
   
   // Register samples on strobe_iad
   // Output A on d1
   // Output B on d5
   always @(posedge clk)
     begin
	stb_d1 <= strobe_iad_o;
	stb_d2 <= stb_d1;
	stb_d3 <= stb_d2;
	stb_d4 <= stb_d3;
	stb_d5 <= stb_d4;
     end
   
   always @(posedge clk)
     if (strobe_iad_o)
       begin	
	  // Streaming GPIO
	  // io_rx[15] => I channel LSB if gpio_ena[0] high
	  // io_rx[14] => Q channel LSB if gpio_ena[1] high
	  sample_out_a <= {i_out_a[15:1], gpio_ena[0] ? io_rx[15] : i_out_a[0],
			   q_out_a[15:1], gpio_ena[1] ? io_rx[14] : q_out_a[0] };
	  sample_out_b <= {i_out_b[15:1], gpio_ena[0] ? io_rx[15] : i_out_b[0],
			   q_out_b[15:1], gpio_ena[1] ? io_rx[14] : q_out_b[0] };
       end

   always @(posedge clk)
     begin
	if (stb_d1)
	  sample <= sample_out_a;
	else if (stb_d5)
	  sample <= sample_out_b;
	strobe_out <= stb_d1|stb_d5;
     end

   assign strobe = strobe_out;
   assign debug = 0;
      
endmodule // dsp_core_rx
