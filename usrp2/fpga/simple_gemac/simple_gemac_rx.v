

module simple_gemac_rx
  (input clk125, input reset,
   input GMII_RX_CLK, input GMII_RX_DV, input GMII_RX_ER, input [7:0] GMII_RXD,
   output rx_clk, output [7:0] rx_data, output rx_valid, output rx_error, output rx_ack,
   output reg [15:0] pause_quanta_rcvd, output reg pause_rcvd );

   
   
   initial 
     begin
	pause_rcvd    <= 0;
	pause_quanta_rcvd  = 10;
	#50000 pause_rcvd <= 1;
	@(posedge rx_clk)
	  pause_rcvd <= 0;
	repeat (100)
	  @(posedge rx_clk);
	pause_quanta_rcvd <= 15;
	pause_rcvd 	  <= 1;
	@(posedge rx_clk)
	  pause_rcvd <= 0;
	repeat (1200)
	  @(posedge rx_clk);
	pause_rcvd 	  <= 1;
	@(posedge rx_clk)
	  pause_rcvd <= 0;
	
     end
   
   assign rx_clk 	   = GMII_RX_CLK;
   
endmodule // simple_gemac_rx
