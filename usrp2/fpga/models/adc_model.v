
module adc_model
  (input clk, input rst,
   output [13:0] adc_a,
   output adc_ovf_a,
   input adc_on_a,
   input adc_oe_a,
   output [13:0] adc_b,
   output adc_ovf_b,
   input adc_on_b,
   input adc_oe_b
   );
   
   math_real math ( ) ;

   reg [13:0] adc_a_int = 0;
   reg [13:0] adc_b_int = 0;
   
   assign     adc_a = adc_oe_a ? adc_a_int : 14'bz;
   assign     adc_ovf_a = adc_oe_a ? 1'b0 : 1'bz;
   assign     adc_b = adc_oe_b ? adc_b_int : 14'bz;
   assign     adc_ovf_b = adc_oe_b ? 1'b0 : 1'bz;
   
   real       phase = 0;
   real       freq = 330000/100000000;

   real       scale = 8190; // math.pow(2,13)-2;
   always @(posedge clk)
     if(rst)
       begin
	  adc_a_int <= 0;
	  adc_b_int <= 0;
       end
     else 
       begin
	  if(adc_on_a)
	    //adc_a_int <= $rtoi(math.round(math.sin(phase*math.MATH_2_PI)*scale)) ;
	    adc_a_int <= adc_a_int + 3;
	  if(adc_on_b)
	    adc_b_int <= adc_b_int - 7;
	  //adc_b_int <= $rtoi(math.round(math.cos(phase*math.MATH_2_PI)*scale)) ;
	  if(phase > 1)
	    phase <= phase + freq - 1;
	  else
	    phase <= phase + freq;
       end
   
endmodule // adc_model
