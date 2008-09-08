// First halfband iterpolator 
// Implements impulse responses of the form [A 0 B 0 C .. 0 H 0.5 H 0 .. C 0 B 0 A]
// Strobe in cannot come faster than every 4th clock cycle, 
// Strobe out cannot come faster than every 2nd clock cycle

// These taps designed by halfgen4 from ldoolittle
// myfilt = round(2^18 * halfgen4(.7/4,8))

module hb_interp
  #(parameter IWIDTH=18, OWIDTH=18, ACCWIDTH=24)
    (input clk,
     input rst,
     input bypass,
     input [7:0] cpo, //  Clocks per output, must be at least 2
     input stb_in,
     input [IWIDTH-1:0] data_in,
     input stb_out,
     output reg [OWIDTH-1:0] data_out);

   localparam MWIDTH = ACCWIDTH-2;
   localparam CWIDTH = 18;

   reg [CWIDTH-1:0] coeff1, coeff2;
   reg [3:0] 	    addr_a, addr_b, addr_c, addr_d, addr_e;
   wire [IWIDTH-1:0] data_a, data_b, data_c, data_d, data_e, sum1, sum2;
   wire [35:0] 	     prod1, prod2;

   reg [2:0] 	     phase, phase_d1, phase_d2, phase_d3, phase_d4, phase_d5;

   always @(posedge clk)
     if(rst)
       phase <= 0;
     else
       if(stb_in)
	 phase <= 1;
       else if(phase==4)
	 phase <= 0;
       else if(phase!=0)
	 phase <= phase + 1;
   always @(posedge clk) phase_d1 <= phase;
   always @(posedge clk) phase_d2 <= phase_d1;
   always @(posedge clk) phase_d3 <= phase_d2;
   always @(posedge clk) phase_d4 <= phase_d3;
   always @(posedge clk) phase_d5 <= phase_d4;     
   
   srl #(.WIDTH(IWIDTH)) srl_a
     (.clk(clk),.write(stb_in),.in(data_in),.addr(addr_a),.out(data_a));
   srl #(.WIDTH(IWIDTH)) srl_b
     (.clk(clk),.write(stb_in),.in(data_in),.addr(addr_b),.out(data_b));
   srl #(.WIDTH(IWIDTH)) srl_c
     (.clk(clk),.write(stb_in),.in(data_in),.addr(addr_c),.out(data_c));
   srl #(.WIDTH(IWIDTH)) srl_d
     (.clk(clk),.write(stb_in),.in(data_in),.addr(addr_d),.out(data_d));
   srl #(.WIDTH(IWIDTH)) srl_e
     (.clk(clk),.write(stb_in),.in(data_in),.addr(addr_e),.out(data_e));

   always @*
     case(phase)
       1 : begin addr_a = 0; addr_b = 15; end
       2 : begin addr_a = 1; addr_b = 14; end
       3 : begin addr_a = 2; addr_b = 13; end
       4 : begin addr_a = 3; addr_b = 12; end
       default : begin addr_a = 0; addr_b = 15; end
     endcase // case(phase)

   always @*
     case(phase)
       1 : begin addr_c = 4; addr_d = 11; end
       2 : begin addr_c = 5; addr_d = 10; end
       3 : begin addr_c = 6; addr_d = 9; end
       4 : begin addr_c = 7; addr_d = 8; end
       default : begin addr_c = 4; addr_d = 11; end
     endcase // case(phase)

   always @*
     case(cpo)
       2 : addr_e <= 9;
       3,4,5,6,7,8 : addr_e <= 8;
       default : addr_e <= 7;  // This case works for 256, which = 0 due to overflow outside this block
     endcase // case(cpo)
   
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

   add2_reg /*_and_round_reg*/ #(.WIDTH(IWIDTH)) add1 (.clk(clk),.in1(data_a),.in2(data_b),.sum(sum1));
   add2_reg /*_and_round_reg*/ #(.WIDTH(IWIDTH)) add2 (.clk(clk),.in1(data_c),.in2(data_d),.sum(sum2));
   // sum1, sum2 available on phase_d1

   wire do_mult = 1;
   MULT18X18S mult1(.C(clk), .CE(do_mult), .R(rst), .P(prod1), .A(coeff1), .B(sum1) );
   MULT18X18S mult2(.C(clk), .CE(do_mult), .R(rst), .P(prod2), .A(coeff2), .B(sum2) );
   // prod1, prod2 available on phase_d2
   
   wire [MWIDTH-1:0] sum_of_prod;
   
   add2_and_round_reg #(.WIDTH(MWIDTH)) 
     add3 (.clk(clk),.in1(prod1[35:36-MWIDTH]),.in2(prod2[35:36-MWIDTH]),.sum(sum_of_prod));
   // sum_of_prod available on phase_d3
   
   wire [ACCWIDTH-1:0] acc_out;
   wire [OWIDTH-1:0]   acc_round;

   wire 	       clear = (phase_d3 == 1);
   wire 	       do_acc = (phase_d3 != 0);
   
   acc #(.IWIDTH(MWIDTH),.OWIDTH(ACCWIDTH)) 
     acc (.clk(clk),.clear(clear),.acc(do_acc),.in(sum_of_prod),.out(acc_out));   
   // acc_out available on phase_d4
   
   wire [ACCWIDTH-6:0] clipped_acc;
   clip #(.bits_in(ACCWIDTH),.bits_out(ACCWIDTH-5)) final_clip(.in(acc_out),.out(clipped_acc));
   
   reg [ACCWIDTH-6:0]   clipped_reg;
   always @(posedge clk)
     if(phase_d4 == 4)
       clipped_reg <= clipped_acc;
   // clipped_reg available on phase_d5
   
   wire [OWIDTH-1:0]   data_out_round;
   round #(.bits_in(ACCWIDTH-5),.bits_out(OWIDTH)) final_round (.in(clipped_reg),.out(data_out_round));

   reg 		      odd;
   always @(posedge clk)
     if(rst)
       odd <= 0;
     else if(stb_in)
       odd <= 0;
     else if(stb_out)
       odd <= 1;

   always @(posedge clk)
     if(bypass)
       data_out <= data_in;
     else if(stb_out)
       if(odd)
	 data_out <= data_e;
       else
	 data_out <= data_out_round;

   // data_out available on phase_d6
   
endmodule // hb_interp
