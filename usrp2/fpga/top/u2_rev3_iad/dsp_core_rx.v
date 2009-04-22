`define DSP_CORE_RX_BASE 160
module dsp_core_rx
  (input clk, input rst,
   input set_stb, input [7:0] set_addr, input [31:0] set_data,

   input [13:0] adc_a, input adc_ovf_a,
   input [13:0] adc_b, input adc_ovf_b,
   
   input [15:0] io_rx,

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
   wire [31:0] i_iad, q_iad;
   wire [15:0] i_out, q_out;
   
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

   wire [3:0]  muxctrl;
   setting_reg #(.my_addr(`DSP_CORE_RX_BASE+8)) sr_8
     (.clk(clk),.rst(rst),.strobe(set_stb),.addr(set_addr),
      .in(set_data),.out(muxctrl),.changed());

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

   
   cordic_z24 #(.bitwidth(24))
     cordic(.clock(clk), .reset(rst), .enable(run),
	    .xi(prod_i[24:1]),. yi(prod_q[24:1]), .zi(phase[31:8]),
	    .xo(i_cordic),.yo(q_cordic),.zo() );

   // Reconstruct original decimation rate from standard firmware settings
   assign decim_rate = enable_hb1 ? (enable_hb2 ? {cic_decim,2'b0} : {1'b0,cic_decim,1'b0 }) :
				    cic_decim;

   cic_strober #(.WIDTH(10)) // Convenient reuse of strobe generator
   cic_strober(.clock(clk),.reset(rst),.enable(run),.rate(decim_rate),
	       .strobe_fast(1),.strobe_slow(strobe_iad) );

   integrate #(.INPUTW(24),.ACCUMW(32),.OUTPUTW(32)) integrator_i
     (.clk_i(clk),.rst_i(rst),.ena_i(run),
      .dump_i(strobe_iad),.data_i(i_cordic),
      .stb_o(strobe),.integ_o(i_iad) );

   integrate #(.INPUTW(24),.ACCUMW(32),.OUTPUTW(32)) integrator_q
     (.clk_i(clk),.rst_i(rst),.ena_i(run),
      .dump_i(strobe_iad),.data_i(q_cordic),
      .stb_o(),.integ_o(q_iad) );
   
   round #(.bits_in(32),.bits_out(16)) round_iout (.in(i_iad),.out(i_out));
   round #(.bits_in(32),.bits_out(16)) round_qout (.in(q_iad),.out(q_out));
  
   // Streaming GPIO
   //
   // io_rx[15] => I channel LSB if gpio_ena[0] high
   // io_rx[14] => Q channel LSB if gpio_ena[1] high

   reg [31:0] sample_reg;
   always @(posedge clk)
     begin
	sample_reg[31:17] <= i_out[15:1];
	sample_reg[15:1]  <= q_out[15:1];
	sample_reg[16]    <= gpio_ena[0] ? io_rx[15] : i_out[0]; 
	sample_reg[0]     <= gpio_ena[1] ? io_rx[14] : q_out[0];
     end
   
   assign      sample = sample_reg;
   assign      debug = {clk, rst, run, strobe};
   
endmodule // dsp_core_rx
