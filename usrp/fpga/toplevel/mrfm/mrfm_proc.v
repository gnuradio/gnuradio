
`include "mrfm.vh"
`include "../../../firmware/include/fpga_regs_common.v"
`include "../../../firmware/include/fpga_regs_standard.v"

module mrfm_proc (input clock, input reset, input enable,
		  input [6:0] serial_addr, input [31:0] serial_data, input serial_strobe,
		  input [15:0] signal_in, output wire [15:0] signal_out, output wire sync_out,
		  output wire [15:0] i, output wire [15:0] q, 
		  output wire [15:0] ip, output wire [15:0] qp,
		  output wire strobe_out, output wire [63:0] debugbus);

   // Strobes
   wire        sample_strobe, strobe_0, strobe_1, strobe_2;
   assign      sample_strobe = 1'b1;
   wire [7:0]  rate_0, rate_1, rate_2;

   setting_reg #(`FR_MRFM_DECIM) sr_decim(.clock(clock),.reset(reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),.out({rate_2,rate_1,rate_0}));
   
   strobe_gen strobe_gen_0
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(rate_0),.strobe_in(sample_strobe),.strobe(strobe_0) );
   strobe_gen strobe_gen_1
     ( .clock(clock),.reset(reset),.enable(enable),
       .rate(rate_1),.strobe_in(strobe_0),.strobe(strobe_1) );
   
   wire [31:0] phase;

   assign      sync_out = phase[31];
   wire [15:0] i_decim_0, i_decim_1, i_decim_2;
   wire [15:0] q_decim_0, q_decim_1, q_decim_2;

   wire [15:0] i_interp_0, i_interp_1, i_interp_2;
   wire [15:0] q_interp_0, q_interp_1, q_interp_2;

   wire [15:0] i_filt, q_filt, i_comp, q_comp;

   assign      ip=i_comp;
   assign      qp=q_comp;
   
   phase_acc #(`FR_MRFM_FREQ,`FR_MRFM_PHASE,32) rx_phase_acc
     (.clk(clock),.reset(reset),.enable(enable),
      .serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe),
      .strobe(sample_strobe),.phase(phase) );
   
   cordic rx_cordic (.clock(clock),.reset(reset),.enable(enable),
		     .xi(signal_in),.yi(16'd0),.zi(phase[31:16]),
		     .xo(i_decim_0),.yo(q_decim_0),.zo() );
   
   cic_decim cic_decim_i_0 (.clock(clock),.reset(reset),.enable(enable),
			    .rate(rate_0),.strobe_in(sample_strobe),.strobe_out(strobe_0),
			    .signal_in(i_decim_0),.signal_out(i_decim_1));
   cic_decim cic_decim_i_1 (.clock(clock),.reset(reset),.enable(enable),
			    .rate(rate_1),.strobe_in(strobe_0),.strobe_out(strobe_1),
			    .signal_in(i_decim_1),.signal_out(i));

   cic_decim cic_decim_q_0 (.clock(clock),.reset(reset),.enable(enable),
			    .rate(rate_0),.strobe_in(sample_strobe),.strobe_out(strobe_0),
			    .signal_in(q_decim_0),.signal_out(q_decim_1));
   cic_decim cic_decim_q_1 (.clock(clock),.reset(reset),.enable(enable),
			    .rate(rate_1),.strobe_in(strobe_0),.strobe_out(strobe_1),
			    .signal_in(q_decim_1),.signal_out(q));

   assign      strobe_out = strobe_1;

   biquad_2stage iir_i (.clock(clock),.reset(reset),.strobe_in(strobe_1),
			.serial_strobe(serial_strobe),.serial_addr(serial_addr),.serial_data(serial_data),
			.sample_in(i),.sample_out(i_filt),.debugbus(debugbus));

   biquad_2stage iir_q (.clock(clock),.reset(reset),.strobe_in(strobe_1),
			.serial_strobe(serial_strobe),.serial_addr(serial_addr),.serial_data(serial_data),
			.sample_in(q),.sample_out(q_filt),.debugbus());

   mrfm_compensator compensator (.clock(clock),.reset(reset),.strobe_in(strobe_1),
				 .serial_strobe(serial_strobe),.serial_addr(serial_addr),.serial_data(serial_data),
				 .i_in(i_filt),.q_in(q_filt),.i_out(i_comp),.q_out(q_comp));
   
   cic_interp cic_interp_i_0 (.clock(clock),.reset(reset),.enable(enable),
			    .rate(rate_1),.strobe_in(strobe_1),.strobe_out(strobe_0),
			    .signal_in(i_comp),.signal_out(i_interp_0));
   cic_interp cic_interp_i_1 (.clock(clock),.reset(reset),.enable(enable),
			    .rate(rate_0),.strobe_in(strobe_0),.strobe_out(sample_strobe),
			    .signal_in(i_interp_0),.signal_out(i_interp_1));

   cic_interp cic_interp_q_0 (.clock(clock),.reset(reset),.enable(enable),
			    .rate(rate_1),.strobe_in(strobe_1),.strobe_out(strobe_0),
			    .signal_in(q_comp),.signal_out(q_interp_0));
   cic_interp cic_interp_q_1 (.clock(clock),.reset(reset),.enable(enable),
			    .rate(rate_0),.strobe_in(strobe_0),.strobe_out(sample_strobe),
			    .signal_in(q_interp_0),.signal_out(q_interp_1));

   cordic tx_cordic (.clock(clock),.reset(reset),.enable(enable),
		     .xi(i_interp_1),.yi(q_interp_1),.zi(-phase[31:16]),
		     .xo(signal_out),.yo(),.zo() );

endmodule // mrfm_proc
