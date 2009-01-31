// Medium halfband decimator (intended to be followed by another stage)
// Implements impulse responses of the form [A 0 B 0 C 0 D 0.5 D 0 C 0 B 0 A]
//
// These taps designed by halfgen_test:
//   2 * 131072 * halfgen_test(.8/8,4,1)
// -597, 0, 4283, 0, -17516, 0, 79365, 131072, 79365, 0, -17516, 0, 4283, 0, -597


module med_hb_int
  #(parameter WIDTH=18)
    (input clk,
     input rst,
     input bypass,
     input stb_in,
     input [WIDTH-1:0] data_in,
     input [7:0] output_rate,
     input stb_out,
     output reg [WIDTH-1:0] data_out);

   localparam coeff_a = -597;
   localparam coeff_b = 4283;
   localparam coeff_c = -17516;
   localparam coeff_d = 79365;
   
   reg 	      phase;
   reg [WIDTH-1:0] d1, d2, d3, d4, d5, d6, d7, d8;
   
   localparam 	   MWIDTH = 36;
   wire [MWIDTH-1:0] prod;
   
   reg [6:0] 	     stbin_d;
   
   always @(posedge clk)
     stbin_d <= {stbin_d[5:0],stb_in};
   
   always @(posedge clk)
     if(stb_in)
       begin
	  d1 <= data_in;
	  d2 <= d1;
	  d3 <= d2;
	  d4 <= d3;
	  d5 <= d4;
	  d6 <= d5;
	  d7 <= d6;
	  d8 <= d7;
       end

   wire [WIDTH-1:0] sum_a, sum_b, sum_c, sum_d;
   add2_and_round_reg #(.WIDTH(WIDTH)) add_a (.clk(clk),.in1(d1),.in2(d8),.sum(sum_a));
   add2_and_round_reg #(.WIDTH(WIDTH)) add_b (.clk(clk),.in1(d2),.in2(d7),.sum(sum_b));
   add2_and_round_reg #(.WIDTH(WIDTH)) add_c (.clk(clk),.in1(d3),.in2(d6),.sum(sum_c));
   add2_and_round_reg #(.WIDTH(WIDTH)) add_d (.clk(clk),.in1(d4),.in2(d5),.sum(sum_d));

   MULT18X18S mult1(.C(clk), .CE(1), .R(rst), .P(prod1), .A(stbin_d[1] ? coeff_a : coeff_b), 
		    .B(stbin_d[1] ? sum_a : sum_b) );
   MULT18X18S mult2(.C(clk), .CE(1), .R(rst), .P(prod2), .A(stbin_d[1] ? coeff_c : coeff_d), 
		    .B(stbin_d[1] ? sum_c : sum_d) );

   wire [MWIDTH:0] accum;
   acc #(.IWIDTH(MWIDTH),.OWIDTH(MWIDTH+1)) 
     acc (.clk(clk),.clear(stbin_d[2]),.acc(|stbin_d[3:2]),.in(prod),.out(accum));
   
   wire [WIDTH+2:0] 	 accum_rnd;
   round_reg #(.bits_in(MWIDTH+1),.bits_out(WIDTH+3))
     final_round (.clk(clk),.in(accum),.out(accum_rnd));

   wire [WIDTH-1:0] 	 clipped;
   clip_reg #(.bits_in(WIDTH+3),.bits_out(WIDTH))
     final_clip (.clk(clk),.in(accum_rnd),.out(clipped));

   reg [WIDTH-1:0] 	 saved, saved_d3;
   always @(posedge clk)
     if(stbin_d[6])
       saved <= clipped;

   always @(posedge clk)
     if(stbin_d[3])
       saved_d3 <= d3;
	   
   always @(posedge clk)
     if(bypass)
       data_out <= data_in;
     else if(stb_in & stb_out)
       case(output_rate)
	 1 : data_out <= d6; 
	 2 : data_out <= d4;
	 3, 4, 5, 6, 7 : data_out <= d3;
	 default : data_out <= d2;
       endcase // case(output_rate)
     else if(stb_out)
       data_out <= saved;

endmodule // small_hb_int

