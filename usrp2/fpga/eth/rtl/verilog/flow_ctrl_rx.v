
// RX side of flow control -- when we are running out of RX space, send a PAUSE

module flow_ctrl_rx
  (input        rst,
   //host processor
   input        pause_frame_send_en,
   input [15:0] pause_quanta_set,
   input [15:0] fc_hwmark,
   input [15:0] fc_lwmark,
   // From MAC_rx_ctrl
   input        rx_clk,
   input [15:0] rx_fifo_space,
   // MAC_tx_ctrl
   input        tx_clk,
   output reg   xoff_gen,
   output reg   xon_gen,
   input        xoff_gen_complete,
   input        xon_gen_complete
   );
   
   // ******************************************************************************        
   // Force our TX to send a PAUSE frame because our RX is nearly full
   // ******************************************************************************

   reg xon_int, xoff_int;
   reg [21:0] countdown;
 
   always @(posedge rx_clk or posedge rst)
     if(rst)
       begin
	  xon_int <= 0;
	  xoff_int <= 0;
       end
     else 
       begin
	  xon_int <= 0;
	  xoff_int <= 0;
	  if(pause_frame_send_en)
	    if(countdown == 0)
	      if(rx_fifo_space < fc_lwmark)
		xoff_int <= 1;
	      else
		;
	    else
	      if(rx_fifo_space > fc_hwmark)
		xon_int <= 1;
       end // else: !if(rst)
   
   reg xoff_int_d1, xon_int_d1;

   always @(posedge rx_clk)
     xon_int_d1 <= xon_int;
   always @(posedge rx_clk)
     xoff_int_d1 <= xoff_int;
   
   always @ (posedge tx_clk or posedge rst)
     if (rst)
       xoff_gen        <=0;
     else if (xoff_gen_complete)
       xoff_gen        <=0;
     else if (xoff_int | xoff_int_d1)
       xoff_gen        <=1;
   
   always @ (posedge tx_clk or posedge rst)
     if (rst)
       xon_gen     <=0;
     else if (xon_gen_complete)
       xon_gen     <=0;
     else if (xon_int | xon_int_d1)
       xon_gen     <=1;                     

   wire [15:0] pq_reduced = pause_quanta_set - 2;
   
   always @(posedge tx_clk or posedge rst)
     if(rst)
       countdown <= 0;
     else if(xoff_gen)
       countdown <= {pq_reduced,6'd0};
     else if(xon_gen)
       countdown <= 0;
     else if(countdown != 0)
       countdown <= countdown - 1;
   
endmodule // flow_ctrl
