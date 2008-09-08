

module rx_dcoffset 
  #(parameter WIDTH=14,
    parameter ADDR=8'd0)
    (input clk, input rst, 
     input set_stb, input [7:0] set_addr, input [31:0] set_data,
     input signed [WIDTH-1:0] adc_in, output signed [WIDTH-1:0] adc_out);

   // Because of some extra delays to make timing easier, the transfer function is:
   //     (z-1)/(z^2-z-alpha)  where alpha is 1/2^n
   
   wire 	       set_now = set_stb & (ADDR == set_addr);
   
   reg 		       fixed;  // uses fixed offset
   wire 	       signed [WIDTH-1:0] fixed_dco;
   reg 		       signed [31:0] integrator;

   always @(posedge clk)
     if(rst)
       begin
	  fixed <= 0;
	  integrator <= 32'd0;
       end
     else if(set_now)
       begin
	  integrator <= {set_data[WIDTH-1:0],{(32-WIDTH){1'b0}}};
	  fixed <= set_data[31];
       end
     else if(~fixed)
       integrator <= integrator + adc_out;
   
   wire [WIDTH:0] scaled_integrator;
   
   round #(.bits_in(33),.bits_out(15)) round (.in({integrator[31],integrator}),.out(scaled_integrator));
   
   wire [WIDTH:0]      adc_out_int = {adc_in[WIDTH-1],adc_in} - scaled_integrator;

   clip_reg #(.bits_in(WIDTH+1),.bits_out(WIDTH)) clip_adc
     (.clk(clk),.in(adc_out_int),.out(adc_out));
   

endmodule // rx_dcoffset
