

module setting_reg_masked
  ( input clock, input reset, input strobe, input wire [6:0] addr,
    input wire [31:0] in, output reg [31:0] out, output reg changed);
/* upper 16 bits are mask, lower 16 bits are value 
 * Note that you get a 16 bit register, not a 32 bit one */

   parameter my_addr = 0;
   
   always @(posedge clock)
     if(reset)
       begin
	  out <= #1 32'd0;
	  changed <= #1 1'b0;
       end
     else
       if(strobe & (my_addr==addr))
	 begin
	    out <= #1 (out & ~in[31:16]) | (in[15:0] & in[31:16] );
	    changed <= #1 1'b1;
	 end
       else
	 changed <= #1 1'b0;
   
endmodule // setting_reg_masked
