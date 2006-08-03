

module rx_dcoffset (input clock, input enable, input reset, 
		    input signed [15:0] adc_in, output signed [15:0] adc_out,
		    input wire [6:0] serial_addr, input wire [31:0] serial_data, input serial_strobe);
   parameter 		  MYADDR = 0;
   
   reg signed [31:0] 		 integrator;
   wire signed [15:0] 		 scaled_integrator = integrator[31:16] + (integrator[31] & |integrator[15:0]);
   assign 			 adc_out = adc_in - scaled_integrator;

   // FIXME do we need signed?
   //FIXME  What do we do when clipping?
   always @(posedge clock)
     if(reset)
       integrator <= #1 32'd0;
     else if(serial_strobe & (MYADDR == serial_addr))
       integrator <= #1 {serial_data[15:0],16'd0};
     else if(enable)
       integrator <= #1 integrator + adc_out;

endmodule // rx_dcoffset
