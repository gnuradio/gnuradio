

module serdes_fc_tx
  (input clk, input rst,
   input xon_rcvd, input xoff_rcvd, output reg inhibit_tx);

   // XOFF means stop sending, XON means start sending
   // clock domain stuff happens elsewhere, everything here is on main clk

   reg [15:0] state;
   always @(posedge clk)
     if(rst)
       state <= 0;
     else if(xoff_rcvd)
       state <= 255;
     else if(xon_rcvd)
       state <= 0;
     else if(state !=0)
       state <= state - 1;

   always @(posedge clk)
     inhibit_tx <= (state != 0);
   
endmodule // serdes_fc_tx
