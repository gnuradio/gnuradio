// Final halfband decimator 
// Implements impulse responses of the form [A 0 B 0 C .. 0 H 0.5 H 0 .. C 0 B 0 A]
// Strobe in cannot come faster than every 2nd clock cycle
// These taps designed by halfgen4 from ldoolittle
// myfilt = round(2^18 * halfgen4(.7/4,8))

module hb_dec
  #(parameter IWIDTH=18, OWIDTH=18, CWIDTH=18, ACCWIDTH=24)
    (input clk,
     input rst,
     input bypass,
     input run,
     input [8:0] cpi,  // Clocks per input -- equal to the decimation ratio ahead of this block
     input stb_in,
     input [IWIDTH-1:0] data_in,
     output reg stb_out,
     output reg [OWIDTH-1:0] data_out);

   // Control
   reg [3:0] 		addr_odd_a, addr_odd_b, addr_odd_c, addr_odd_d;
   wire 		write_odd, write_even, do_mult;
   reg 			odd;
   reg [2:0] 		phase, phase_d1;
   reg 			stb_out_int;
   wire 		clear, do_acc;
   assign 		do_mult = 1;
   
   always @(posedge clk)
     if(rst | ~run)
       odd <= 0;
     else if(stb_in)
       odd <= ~odd;

   assign 		write_odd = stb_in & odd;
   assign 		write_even = stb_in & ~odd;

   always @(posedge clk)
     if(rst | ~run)
       phase <= 0;
     else if(stb_in & odd)
       phase <= 1;
     else if(phase == 4)
       phase <= 0;
     else if(phase != 0)
       phase <= phase + 1;

   always @(posedge clk)
     phase_d1 <= phase;
   
   reg [15:0] 		stb_out_pre;
   always @(posedge clk)
     if(rst)
       stb_out_pre <= 0;
     else
       stb_out_pre <= {stb_out_pre[14:0],(stb_in & odd)};

   always @*
     case(phase)
       1 : begin addr_odd_a = 0; addr_odd_b = 15; end
       2 : begin addr_odd_a = 1; addr_odd_b = 14; end
       3 : begin addr_odd_a = 2; addr_odd_b = 13; end
       4 : begin addr_odd_a = 3; addr_odd_b = 12; end
       default : begin addr_odd_a = 0; addr_odd_b = 15; end
     endcase // case(phase)

   always @*
     case(phase)
       1 : begin addr_odd_c = 4; addr_odd_d = 11; end
       2 : begin addr_odd_c = 5; addr_odd_d = 10; end
       3 : begin addr_odd_c = 6; addr_odd_d = 9; end
       4 : begin addr_odd_c = 7; addr_odd_d = 8; end
       default : begin addr_odd_c = 4; addr_odd_d = 11; end
     endcase // case(phase)

   assign do_acc = |stb_out_pre[6:3];
   assign clear = stb_out_pre[3];
   
   // Data
   wire [IWIDTH-1:0] data_odd_a, data_odd_b, data_odd_c, data_odd_d;
   wire [IWIDTH-1:0] sum1, sum2;	
   wire [OWIDTH-1:0] final_sum;
   reg [CWIDTH-1:0]  coeff1, coeff2;
   wire [35:0] 	     prod1, prod2;

   always @*            // Outer coeffs
     case(phase_d1)
       1 : coeff1 = -107;
       2 : coeff1 = 445;
       3 : coeff1 = -1271;
       4 : coeff1 = 2959;
       default : coeff1 = -107;
     endcase // case(phase)
   
   always @*            //  Inner coeffs
     case(phase_d1)
       1 : coeff2 = -6107;
       2 : coeff2 = 11953;
       3 : coeff2 = -24706;
       4 : coeff2 = 82359;
       default : coeff2 = -6107;
     endcase // case(phase)
   
   srl #(.WIDTH(IWIDTH)) srl_odd_a
     (.clk(clk),.write(write_odd),.in(data_in),.addr(addr_odd_a),.out(data_odd_a));
   srl #(.WIDTH(IWIDTH)) srl_odd_b
     (.clk(clk),.write(write_odd),.in(data_in),.addr(addr_odd_b),.out(data_odd_b));
   srl #(.WIDTH(IWIDTH)) srl_odd_c
     (.clk(clk),.write(write_odd),.in(data_in),.addr(addr_odd_c),.out(data_odd_c));
   srl #(.WIDTH(IWIDTH)) srl_odd_d
     (.clk(clk),.write(write_odd),.in(data_in),.addr(addr_odd_d),.out(data_odd_d));

   add2_reg /*_and_round_reg*/ #(.WIDTH(IWIDTH)) add1 (.clk(clk),.in1(data_odd_a),.in2(data_odd_b),.sum(sum1));
   add2_reg /*_and_round_reg*/ #(.WIDTH(IWIDTH)) add2 (.clk(clk),.in1(data_odd_c),.in2(data_odd_d),.sum(sum2));

   wire [IWIDTH-1:0] data_even;
   reg [3:0] 	     addr_even;

   always @(posedge clk)
     case(cpi)
       //  1 is an error
       2 : addr_even <= 9;  // Maximum speed (overall decim by 4)
       3, 4, 5, 6, 7 : addr_even <= 8;
       default : addr_even <= 7;
     endcase // case(cpi)
   
   srl #(.WIDTH(IWIDTH)) srl_even
     (.clk(clk),.write(write_even),.in(data_in),.addr(addr_even),.out(data_even));

   localparam 		MWIDTH = ACCWIDTH-2;   
   wire [MWIDTH-1:0] 	sum_of_prod;

   MULT18X18S mult1(.C(clk), .CE(do_mult), .R(rst), .P(prod1), .A(coeff1), .B(sum1) );
   MULT18X18S mult2(.C(clk), .CE(do_mult), .R(rst), .P(prod2), .A(coeff2), .B(sum2) );
   add2_and_round_reg #(.WIDTH(MWIDTH)) 
     add3 (.clk(clk),.in1(prod1[35:36-MWIDTH]),.in2(prod2[35:36-MWIDTH]),.sum(sum_of_prod));

   wire [ACCWIDTH-1:0] 	acc_out;
   
   acc #(.IWIDTH(MWIDTH),.OWIDTH(ACCWIDTH)) 
     acc (.clk(clk),.clear(clear),.acc(do_acc),.in(sum_of_prod),.out(acc_out));

   localparam 		SHIFT_FACTOR = ACCWIDTH-IWIDTH-5;
   wire [ACCWIDTH-1:0] 	data_even_signext;
   wire [ACCWIDTH:0] 	final_sum_unrounded;

   sign_extend #(.bits_in(IWIDTH),.bits_out(ACCWIDTH-SHIFT_FACTOR))
     signext_data_even (.in(data_even),.out(data_even_signext[ACCWIDTH-1:SHIFT_FACTOR]));
   assign 		data_even_signext[SHIFT_FACTOR-1:0] = 0;
   
   add2_reg /* add2_and_round_reg */ #(.WIDTH(ACCWIDTH+1)) 
     final_adder (.clk(clk), .in1({acc_out,1'b0}), .in2({data_even_signext,1'b0}), .sum(final_sum_unrounded));

   round_reg #(.bits_in(ACCWIDTH-4),.bits_out(OWIDTH))
     final_round (.clk(clk),.in(final_sum_unrounded[ACCWIDTH-5:0]),.out(final_sum));

   // Output
   always @(posedge clk)
     if(bypass)
       data_out <= data_in;
     else if(stb_out_pre[9])
       data_out <= final_sum;

   always @(posedge clk)
     if(rst)
       stb_out <= 0;
     else if(bypass)
       stb_out <= stb_in;
     else
       stb_out <= stb_out_pre[9];
 
endmodule // hb_dec
