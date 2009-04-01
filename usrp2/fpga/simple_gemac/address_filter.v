

module address_filter
  (input clk,
   input reset,
   input go,
   input [7:0] data,
   input [47:0] address,
   output match,
   output done);

   reg [2:0] af_state;

   always @(posedge clk)
     if(reset)
       af_state     <= 0;
     else
       if(go)
	 af_state <= (data == address[47:40]) ? 1 : 7;
       else
	 case(af_state)
	   1 : af_state <= (data == address[39:32]) ? 2 : 7;
	   2 : af_state <= (data == address[31:24]) ? 3 : 7;
	   3 : af_state <= (data == address[23:16]) ? 4 : 7;
	   4 : af_state <= (data == address[15:8])  ? 5 : 7;
	   5 : af_state <= (data == address[7:0]) ? 6 : 7;
	 endcase // case (af_state)

   assign match  = (af_state==6);
   assign done 	 = (af_state==6)|(af_state==7);
   
endmodule // address_filter

   
