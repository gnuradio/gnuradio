
// RX side of flow control -- when we are running out of RX space, send a PAUSE

module flow_ctrl_rx
  (input pause_request_en, input [15:0] pause_time, input [15:0] pause_thresh,
   input rx_clk, input rx_reset, input [15:0] rx_fifo_space,
   input tx_clk, input tx_reset, output reg pause_req, output reg [15:0] pause_time_req
   );
   
   // ******************************************************************************        
   // Force our TX to send a PAUSE frame because our RX is nearly full
   // ******************************************************************************

   // RX Clock Domain
   reg xon, xoff;
   reg [21:0] countdown;

   wire [15:0] pause_low_thresh = pause_thresh;
   wire [15:0] pause_hi_thresh = 16'hFFFF;
   wire [21:0] pq_reduced = {pause_time,6'd0} - 1700;
   
   always @(posedge rx_clk)
     if(rx_reset)
       xoff <= 0;
     else
       xoff <= (pause_request_en & (countdown==0) & (rx_fifo_space < pause_low_thresh));
   
   always @(posedge rx_clk)
     if(rx_reset)
       xon  <= 0;
     else
       xon  <= ((countdown!=0) & (rx_fifo_space > pause_hi_thresh));
   
   always @(posedge rx_clk)
     if(rx_reset)
       countdown <= 0;
     else if(xoff)
       countdown <= pq_reduced;
     else if(xon)
       countdown <= 0;
     else if(countdown != 0)
       countdown <= countdown - 1;

   // Cross clock domains
   wire        xon_tx, xoff_tx;
   oneshot_2clk send_xon (.clk_in(rx_clk), .in(xon), .clk_out(tx_clk), .out(xon_tx));
   oneshot_2clk send_xoff (.clk_in(rx_clk), .in(xoff), .clk_out(tx_clk), .out(xoff_tx));
   
   always @(posedge tx_clk)
     if(xoff_tx)
       pause_time_req <= pause_time;
     else if(xon_tx)
       pause_time_req <= 0;

   always @(posedge tx_clk)
     if(tx_reset)
       pause_req      <= 0;
     else 
       pause_req      <= xon_tx | xoff_tx;
   
endmodule // flow_ctrl_rx
