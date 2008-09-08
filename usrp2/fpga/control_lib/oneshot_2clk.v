
// Retime a single bit from one clock domain to another
// Guarantees that no matter what the relative clock rates, if the in signal is high for at least
//   one clock cycle in the clk_in domain, then the out signal will be high for at least one
//   clock cycle in the clk_out domain.  If the in signal goes high again before the process is done
//   the behavior is undefined.  No other guarantees.  Designed for passing reset into a new
//   clock domain.

module oneshot_2clk
  (input clk_in,
   input in,
   input clk_out,
   output reg out);

   reg 	  del_in = 0;
   reg 	  sendit = 0, gotit = 0;
   reg 	  sendit_d = 0, gotit_d = 0;
   
   always @(posedge clk_in) del_in <= in;

   always @(posedge clk_in)
     if(in & ~del_in)  // we have a positive edge
       sendit <= 1;
     else if(gotit)
       sendit <= 0;

   always @(posedge clk_out) sendit_d <= sendit;
   always @(posedge clk_out) out <= sendit_d;

   always @(posedge clk_in) gotit_d <= out;
   always @(posedge clk_in) gotit <= gotit_d;

endmodule // oneshot_2clk

  
