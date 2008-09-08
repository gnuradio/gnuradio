// Short halfband decimator (intended to be followed by another stage)
// Implements impulse responses of the form [A 0 B 0.5 B 0 A]
//
// These taps designed by halfgen4 from ldoolittle:
//   2 * 131072 * halfgen4(.75/8,2)

module small_hb_int
  #(parameter WIDTH=18)
    (input clk,
     input rst,
     input bypass,
     input stb_in,
     input [WIDTH-1:0] data_in,
     input [7:0] output_rate,
     input stb_out,
     output reg [WIDTH-1:0] data_out);

   reg 		   phase;
   reg [WIDTH-1:0] d1, d2, d3, d4, d5, d6;

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
       end

   wire [WIDTH-1:0] sum_outer, sum_inner;
   add2_and_round_reg #(.WIDTH(WIDTH)) add_outer (.clk(clk),.in1(d1),.in2(d4),.sum(sum_outer));
   add2_and_round_reg #(.WIDTH(WIDTH)) add_inner (.clk(clk),.in1(d2),.in2(d3),.sum(sum_inner));

   wire [17:0] 	   coeff_outer = -10690;
   wire [17:0] 	   coeff_inner = 75809;

   MULT18X18S mult(.C(clk), .CE(1), .R(rst), .P(prod), .A(stbin_d[1] ? coeff_outer : coeff_inner), 
		   .B(stbin_d[1] ? sum_outer : sum_inner) );

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

