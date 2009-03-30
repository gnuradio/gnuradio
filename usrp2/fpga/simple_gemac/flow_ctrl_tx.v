
// TX side of flow control -- when other side sends PAUSE, we wait

module flow_ctrl_tx
  (input        rst,
   input        tx_clk,
   //host processor
   input        tx_pause_en,
   // From MAC_rx_ctrl
   input [15:0] pause_quanta,
   input        pause_quanta_val,
   // MAC_tx_ctrl
   output       pause_apply,
   input        pause_quanta_sub);
     
   // ******************************************************************************        
   // Inhibit our TX from transmitting because they sent us a PAUSE frame
   // ******************************************************************************

   reg [15:0] 	pause_quanta_counter;
   reg 		pqval_d1, pqval_d2;		

   always @(posedge tx_clk) pqval_d1 <= pause_quanta_val;
   always @(posedge tx_clk) pqval_d2 <= pqval_d1;

   always @ (posedge tx_clk or posedge rst)
     if (rst)
       pause_quanta_counter <= 0;
     else if (pqval_d1 & ~pqval_d2)
       pause_quanta_counter <= pause_quanta; 
     else if((pause_quanta_counter!=0) & pause_quanta_sub)
       pause_quanta_counter <= pause_quanta_counter - 1;

   assign	pause_apply = tx_pause_en & (pause_quanta_counter != 0);
   
endmodule // flow_ctrl
