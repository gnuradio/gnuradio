

`include "../../firmware/include/fpga_regs_common.v"
`include "../../firmware/include/fpga_regs_standard.v"

module adc_interface
  (input clock, input reset, input enable,
   input wire [6:0] serial_addr, input wire [31:0] serial_data, input serial_strobe,
   input wire [11:0] rx_a_a, input wire [11:0] rx_b_a, input wire [11:0] rx_a_b, input wire [11:0] rx_b_b,
   output wire [31:0] rssi_0, output wire [31:0] rssi_1, output wire [31:0] rssi_2, output wire [31:0] rssi_3,
   output reg [15:0] ddc0_in_i, output reg [15:0] ddc0_in_q, 
   output reg [15:0] ddc1_in_i, output reg [15:0] ddc1_in_q, 
   output reg [15:0] ddc2_in_i, output reg [15:0] ddc2_in_q, 
   output reg [15:0] ddc3_in_i, output reg [15:0] ddc3_in_q,
   output wire [3:0] rx_numchan);
      
    // Buffer at input to chip
   reg [11:0] adc0,adc1,adc2,adc3;
   always @(posedge clock)
     begin
	adc0 <= #1 rx_a_a;
	adc1 <= #1 rx_b_a;
	adc2 <= #1 rx_a_b;
	adc3 <= #1 rx_b_b;
     end
   
   // then scale and subtract dc offset
   wire [3:0] dco_en;
   wire [15:0] 	adc0_corr,adc1_corr,adc2_corr,adc3_corr;
   
   setting_reg #(`FR_DC_OFFSET_CL_EN) sr_dco_en(.clock(clock),.reset(reset),.strobe(serial_strobe),.addr(serial_addr),.in(serial_data),
				 .out(dco_en));

   rx_dcoffset #(`FR_ADC_OFFSET_0) rx_dcoffset0(.clock(clock),.enable(dco_en[0]),.reset(reset),.adc_in({adc0[11],adc0,3'b0}),.adc_out(adc0_corr),
						.serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   rx_dcoffset #(`FR_ADC_OFFSET_1) rx_dcoffset1(.clock(clock),.enable(dco_en[1]),.reset(reset),.adc_in({adc1[11],adc1,3'b0}),.adc_out(adc1_corr),
						.serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   rx_dcoffset #(`FR_ADC_OFFSET_2) rx_dcoffset2(.clock(clock),.enable(dco_en[2]),.reset(reset),.adc_in({adc2[11],adc2,3'b0}),.adc_out(adc2_corr),
						.serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));
   rx_dcoffset #(`FR_ADC_OFFSET_3) rx_dcoffset3(.clock(clock),.enable(dco_en[3]),.reset(reset),.adc_in({adc3[11],adc3,3'b0}),.adc_out(adc3_corr),
						.serial_addr(serial_addr),.serial_data(serial_data),.serial_strobe(serial_strobe));

   // Level sensing for AGC
   rssi rssi_block_0 (.clock(clock),.reset(reset),.enable(enable),.adc(adc0),.rssi(rssi_0[15:0]),.over_count(rssi_0[31:16]));
   rssi rssi_block_1 (.clock(clock),.reset(reset),.enable(enable),.adc(adc1),.rssi(rssi_1[15:0]),.over_count(rssi_1[31:16]));
   rssi rssi_block_2 (.clock(clock),.reset(reset),.enable(enable),.adc(adc2),.rssi(rssi_2[15:0]),.over_count(rssi_2[31:16]));
   rssi rssi_block_3 (.clock(clock),.reset(reset),.enable(enable),.adc(adc3),.rssi(rssi_3[15:0]),.over_count(rssi_3[31:16]));
   
   // And mux to the appropriate outputs
   wire [3:0] 	ddc3mux,ddc2mux,ddc1mux,ddc0mux;
   wire 	rx_realsignals;
   
   setting_reg #(`FR_RX_MUX) sr_rxmux(.clock(clock),.reset(reset),.strobe(serial_strobe),.addr(serial_addr),
				      .in(serial_data),.out({ddc3mux,ddc2mux,ddc1mux,ddc0mux,rx_realsignals,rx_numchan[3:1]}));
   assign 	rx_numchan[0] = 1'b0;
   
   always @(posedge clock)
     begin
	ddc0_in_i <= #1 ddc0mux[1] ? (ddc0mux[0] ? adc3_corr : adc2_corr) : (ddc0mux[0] ? adc1_corr : adc0_corr);
	ddc0_in_q <= #1 rx_realsignals ? 16'd0 : ddc0mux[3] ? (ddc0mux[2] ? adc3_corr : adc2_corr) : (ddc0mux[2] ? adc1_corr : adc0_corr);
	ddc1_in_i <= #1 ddc1mux[1] ? (ddc1mux[0] ? adc3_corr : adc2_corr) : (ddc1mux[0] ? adc1_corr : adc0_corr);
	ddc1_in_q <= #1 rx_realsignals ? 16'd0 : ddc1mux[3] ? (ddc1mux[2] ? adc3_corr : adc2_corr) : (ddc1mux[2] ? adc1_corr : adc0_corr);
	ddc2_in_i <= #1 ddc2mux[1] ? (ddc2mux[0] ? adc3_corr : adc2_corr) : (ddc2mux[0] ? adc1_corr : adc0_corr);
	ddc2_in_q <= #1 rx_realsignals ? 16'd0 : ddc2mux[3] ? (ddc2mux[2] ? adc3_corr : adc2_corr) : (ddc2mux[2] ? adc1_corr : adc0_corr);
	ddc3_in_i <= #1 ddc3mux[1] ? (ddc3mux[0] ? adc3_corr : adc2_corr) : (ddc3mux[0] ? adc1_corr : adc0_corr);
	ddc3_in_q <= #1 rx_realsignals ? 16'd0 : ddc3mux[3] ? (ddc3mux[2] ? adc3_corr : adc2_corr) : (ddc3mux[2] ? adc1_corr : adc0_corr);
     end

endmodule // adc_interface

   
