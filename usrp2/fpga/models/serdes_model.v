
module serdes_model
  (input ser_tx_clk,
   input ser_tkmsb,
   input ser_tklsb,
   input [15:0] ser_t,
   
   output ser_rx_clk,
   output ser_rkmsb,
   output ser_rklsb,
   output [15:0] ser_r,
   
   input even,
   input error);
   
   wire [15:0] ser_r_odd;
   wire  ser_rklsb_odd, ser_rkmsb_odd;   
   
   reg [7:0] hold_dat;
   reg 	     hold_k;
   
   always @(posedge ser_tx_clk) hold_k <= ser_tklsb;
   always @(posedge ser_tx_clk) hold_dat <= ser_t[15:8];
   assign    ser_rklsb_odd = hold_k;
   assign    ser_rkmsb_odd = ser_tklsb;
   assign    ser_r_odd = {ser_t[7:0], hold_dat};
   
   // Set outputs
   assign    ser_rx_clk = ser_tx_clk;
   assign    ser_rkmsb = even ? ser_tkmsb : ser_rkmsb_odd;
   assign    ser_rklsb = even ? ser_tklsb : ser_rklsb_odd;
   assign    ser_r = error ^ (even ? ser_t : ser_r_odd);
   
endmodule // serdes_model
