

// Simple printout of characters from the UART
//   Only does 8N1, requires the baud clock

module uart_rx (input baudclk, input rxd);
   reg [8:0] sr = 9'b0;
   reg [3:0]  baud_ctr = 4'b0;

   /*
   wire       byteclk = baud_ctr[3];
   reg 	      rxd_d1 = 0;
   always @(posedge baudclk)
     rxd_d1 <= rxd;
   
   always @(posedge baudclk)
     if(rxd_d1 != rxd)
       baud_ctr <= 0;
     else
       baud_ctr <= baud_ctr + 1;
*/

   wire       byteclk = baudclk;
   
   always @(posedge byteclk)
     sr <= { rxd, sr[8:1] };

   reg [3:0]  state = 0;
   always @(posedge byteclk)
     case(state)
       0 : 
	 if(~sr[8] & sr[7])  // found start bit
	   state  <= 1;
       1, 2, 3, 4, 5, 6, 7, 8 :
	 state <= state + 1;
       9 : 
	 begin
	    state <= 0;
	    $write("%c",sr[7:0]);
	    if(~sr[8])
	      $display("Error, no stop bit\n");
	 end
       default :
	 state <= 0;
     endcase // case(state)

endmodule // uart_rx

