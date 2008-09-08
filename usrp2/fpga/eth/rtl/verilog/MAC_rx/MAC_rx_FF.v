
// ////////////////////////////////////////////////////////////////////
// Completely Rewritten by M. Ettus, no John Gao code left
// ////////////////////////////////////////////////////////////////////

module MAC_rx_FF 
  #(parameter RX_FF_DEPTH = 9)
    (input         Reset,
     input         Clk_MAC,
     input         Clk_SYS,
     
     // MAC_rx_ctrl interface 
     input   [7:0] Fifo_data,
     input         Fifo_data_en,
     output        Fifo_full,
     input         Fifo_data_err,
     input         Fifo_data_end,
     output [15:0] Fifo_space,

     // CPU
     input         RX_APPEND_CRC,
     input [4:0]   Rx_Hwmark,
     input [4:0]   Rx_Lwmark,
     
     // User interface 
     output        Rx_mac_empty,
     input         Rx_mac_rd,
     output [31:0] Rx_mac_data,
     output [1:0]  Rx_mac_BE,
     output        Rx_mac_sop,
     output        Rx_mac_eop,
     output        Rx_mac_err,

     // FIFO Levels
     output [15:0] fifo_occupied,
     output fifo_full_dbg,
     output fifo_empty
     );

   reg [1:0] 	   FF_state;	   
   reg [2:0] 	   PKT_state;
   reg [31:0] 	   staging;
   reg [35:0] 	   staging2;
   reg 		   line_ready, line_ready_d1;
   wire 	   sop_i, eop_i;
   reg [1:0] 	   be;
   
   always @(posedge Clk_MAC or posedge Reset)
     if(Reset)
       FF_state <= 0;
     else
       if(Fifo_data_err | Fifo_data_end)
	 FF_state <= 0;
       else if(Fifo_data_en)
	 FF_state <= FF_state + 1;
   
   always @(posedge Clk_MAC or posedge Reset)
     if(Reset)
       staging[31:0] <= 0;
     else if(Fifo_data_en)
       case(FF_state)
	 0 : staging[31:24] <= Fifo_data;
	 1 : staging[23:16] <= Fifo_data;
	 2 : staging[15:8] <= Fifo_data;
	 3 : staging[7:0] <= Fifo_data;
       endcase // case(FF_state)

   localparam 	   PKT_idle = 0;
   localparam 	   PKT_sop = 1;
   localparam 	   PKT_pkt = 2;
   localparam 	   PKT_end = 3;
   localparam 	   PKT_err = 4;

   always @(posedge Clk_MAC or posedge Reset)
     if(Reset)
       PKT_state <= 0;
     else
       case(PKT_state)
	 PKT_idle :
	   if(Fifo_data_en)
	     PKT_state <= PKT_sop;
	 PKT_sop, PKT_pkt :
	   if(Fifo_data_err | (line_ready & Fifo_full))
	     PKT_state <= PKT_err;
	   else if(Fifo_data_end)
	     PKT_state <= PKT_end;
	   else if(line_ready & ~Fifo_full)
	     PKT_state <= PKT_pkt;
	 PKT_end :
	   PKT_state <= PKT_idle;
	 PKT_err :
	   if(~Fifo_full)
	     PKT_state <= PKT_idle;
       endcase // case(PKT_state)

   assign 	   sop_i = (PKT_state == PKT_sop);
   assign 	   eop_i = (PKT_state == PKT_end);
   
   always @(posedge Clk_MAC)
     if(line_ready)
       staging2 <= {sop_i, eop_i, be[1:0], staging};
   
   always @(posedge Clk_MAC)
     if(Reset)
       line_ready <= 0;
     else if((Fifo_data_en & (FF_state==2'd3)) | Fifo_data_end | Fifo_data_err)
       line_ready <= 1;
     else
       line_ready <= 0;

   always @(posedge Clk_MAC)
     line_ready_d1 <= line_ready;
   
   always @(posedge Clk_MAC)
     if(Fifo_data_end | Fifo_data_err)
       be <= FF_state;
     else
       be <= 0;
   
   wire 	   sop_o, eop_o, empty;
   wire [1:0] 	   be_o;
   wire [RX_FF_DEPTH-1:0] occupied, occupied_sysclk;
   wire [31:0] 	   dataout;

/*
   fifo_2clock #(.DWIDTH(36),.AWIDTH(RX_FF_DEPTH)) mac_rx_fifo
     (.wclk(Clk_MAC),.datain((PKT_state==PKT_err) ? 36'hF_FFFF_FFFF : staging2),.write(~Fifo_full & (line_ready_d1|(PKT_state==PKT_err))),
      .full(Fifo_full),.level_wclk(occupied),
      .rclk(Clk_SYS),.dataout({sop_o,eop_o,be_o[1:0],dataout}),.read(Rx_mac_rd),
      .empty(empty),.level_rclk(),
      .arst(Reset) );
  */

   fifo_xlnx_2Kx36_2clk mac_rx_ff_core
     (
      .din((PKT_state==PKT_err) ? 36'hF_FFFF_FFFF : staging2), // Bus [35 : 0] 
      .rd_clk(Clk_SYS),
      .rd_en(Rx_mac_rd),
      .rst(Reset),
      .wr_clk(Clk_MAC),
      .wr_en(~Fifo_full & (line_ready_d1|(PKT_state==PKT_err))),
      .dout({sop_o,eop_o,be_o[1:0],dataout}), // Bus [35 : 0] 
      .empty(empty),
      .full(Fifo_full),
      .rd_data_count(occupied_sysclk), // Bus [11 : 0] 
      .wr_data_count(occupied)); // Bus [11 : 0] 
   
   assign 	   Fifo_space[15:RX_FF_DEPTH] = 0;
   assign 	   Fifo_space[RX_FF_DEPTH-1:0] = ~occupied;
   assign 	   fifo_occupied = occupied_sysclk;
   assign 	   fifo_full_dbg = Fifo_full;   // FIXME -- in wrong clock domain
   assign 	   fifo_empty = empty;
   
   // mac side fifo interface
   //   Input - Rx_mac_rd
   //   Output - Rx_mac_empty, Rx_mac_sop, Rx_mac_eop, Rx_mac_err, Rx_mac_data, Rx_mac_BE

   assign 	   Rx_mac_BE = be_o;
   assign 	   Rx_mac_sop = sop_o & ~eop_o;
   assign 	   Rx_mac_eop = eop_o;
   assign 	   Rx_mac_err = sop_o & eop_o;
   assign 	   Rx_mac_empty = empty;
   assign 	   Rx_mac_data = dataout;
   
endmodule // MAC_rx_FF

// FIXME  Should we send out an "almost full" signal instead of full?
