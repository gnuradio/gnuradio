

module simple_timer
  #(parameter BASE=0)
   (input clk, input reset,
    input set_stb, input [7:0] set_addr, input [31:0] set_data,
    output reg onetime_int, output reg periodic_int);

   reg [31:0]  onetime_ctr;
   always @(posedge clk)
     if(reset)
       begin
	  onetime_int 	  <= 0;
	  onetime_ctr 	  <= 0;
       end
     else
       if(set_stb & (set_addr == BASE))
	 begin
	    onetime_int   <= 0;
	    onetime_ctr   <= set_data;
	 end
       else 
	 begin
	    if(onetime_ctr == 1)
	      onetime_int <= 1;
	    if(onetime_ctr != 0)
	      onetime_ctr <= onetime_ctr - 1;
	    else
	      onetime_int <= 0;
	 end // else: !if(set_stb & (set_addr == BASE))
   
   reg [31:0]  periodic_ctr, period;
   always @(posedge clk)
     if(reset)
       begin
	  periodic_int 	     <= 0;
	  periodic_ctr 	     <= 0;
	  period 	     <= 0;
       end
     else
       if(set_stb & (set_addr == (BASE+1)))
	 begin
	    periodic_int     <= 0;
	    periodic_ctr     <= set_data;
	    period 	     <= set_data;
	 end
       else 
	 if(periodic_ctr == 1)
	   begin
	      periodic_int   <= 1;
	      periodic_ctr   <= period;
	   end
	 else
	   if(periodic_ctr != 0)
	     begin
		periodic_int <= 0;
		periodic_ctr <= periodic_ctr - 1;
	     end
   
endmodule // simple_timer
