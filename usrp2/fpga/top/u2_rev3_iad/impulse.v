module impulse
  (input clk,
   input rst,
   input ena,
   
   input [13:0] dc_offset_a,
   input [13:0] dc_offset_b,
   input [13:0] amplitude,
   input [15:0] impulse_len,
   input [15:0] zero_len,

   output [13:0] adc_a,
   output [13:0] adc_b,
   output        adc_ovf_a,
   output        adc_ovf_b
   );

   reg [13:0] adc_a_int = 0;
   reg [15:0] count;

   localparam ST_ZERO = 0;
   localparam ST_HIGH = 1;
   reg 	      state;
   
   always @(posedge clk)
     if (rst | ~ena)
       begin
	  adc_a_int <= 0;
	  count <= 0;
	  state <= ST_ZERO;
       end
     else
       case(state)
	 ST_ZERO:
	   if (count == zero_len)
	     begin
		adc_a_int <= amplitude;
		state <= ST_HIGH;
		count <= 0;
	     end
	   else
	     count <= count + 1;

	 ST_HIGH:
	   if (count == impulse_len)
	     begin
		adc_a_int <= 0;
		state <= ST_ZERO;
		count <= 0;
	     end
	   else
	     count <= count + 1;

       endcase // case (state)

   assign adc_a = adc_a_int + dc_offset_a;

   // Ignore for now
   assign adc_b = dc_offset_b;
   assign adc_ovf_a = 0;
   assign adc_ovf_b = 0;

endmodule // adc_model
