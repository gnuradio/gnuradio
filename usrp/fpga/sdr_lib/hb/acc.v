

module acc (input clock, input reset, input clear, input enable_in, output reg enable_out,
	    input signed [30:0] addend, output reg signed [33:0] sum );

   always @(posedge clock)
     if(reset)
       sum <= #1 34'd0;
     //else if(clear & enable_in)
     //  sum <= #1 addend;
     //else if(clear)
     //  sum <= #1 34'd0;
     else if(clear)
       sum <= #1 addend;
     else if(enable_in)
       sum <= #1 sum + addend;

   always @(posedge clock)
     enable_out <= #1 enable_in;
   
endmodule // acc

