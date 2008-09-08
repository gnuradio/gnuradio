

module serdes_fc_rx
  #(parameter LWMARK = 64,
    parameter HWMARK = 320)
    (input clk, input rst,
     input [15:0] fifo_space, 
     output reg send_xon,
     output reg send_xoff,
     input sent);
    
   reg [15:0] 	  countdown;
   reg 		  send_xon_int, send_xoff_int;
   
   always @(posedge clk)
     if(rst)
       begin
	  send_xon_int <= 0;
	  send_xoff_int <= 0;
	  countdown <= 0;
       end
     else 
       begin
	  send_xon_int <= 0;
	  send_xoff_int <= 0;
	  if(countdown == 0)
	    if(fifo_space < LWMARK)
	      begin
		 send_xoff_int <= 1;
		 countdown <= 240;
	      end
	    else
	      ;
	  else
	    if(fifo_space > HWMARK)
	      begin
		 send_xon_int <= 1;
		 countdown <= 0;
	      end
	    else
	      countdown <= countdown - 1;
       end // else: !if(rst)

   // If we are between the high and low water marks, we let the countdown expire

   always @(posedge clk)
     if(rst)
       send_xon <= 0;
     else if(send_xon_int)
       send_xon <= 1;
     else if(sent)
       send_xon <= 0;

   always @(posedge clk)
     if(rst)
       send_xoff <= 0;
     else if(send_xoff_int)
       send_xoff <= 1;
     else if(sent)
       send_xoff <= 0;
   
endmodule // serdes_fc_rx
