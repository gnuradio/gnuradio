//////////////////////////////////////////////////////////////////////
////                                                              ////
////  MAC_top.v                                                   ////
////                                                              ////
////  This file is part of the Ethernet IP core project           ////
////  http://www.opencores.org/projects.cgi/web/ethernet_tri_mode/////
////                                                              ////
////  Author(s):                                                  ////
////      - Jon Gao (gaojon@yahoo.com)                            ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2001 Authors                                   ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////

module MAC_top
  #(parameter TX_FF_DEPTH = 9, 
    parameter RX_FF_DEPTH = 9)
    (
  // System signals
  input         Clk_125M,
  input         Clk_user,

     input rst_mac,
     input rst_user,
     
  // Wishbone compliant core host interface
  input         RST_I, // Active high (async) reset of the Wishbone interface
  input         CLK_I, // Wishbone interface clock (nominally 50 MHz)
  input         STB_I, // Active high module-select
  input         CYC_I, // Active high cycle-enable
  input [6:0]   ADR_I, // Module register address
  input         WE_I,  // Active high for writes, low for reads
  input [31:0]  DAT_I, // Write data
  output [31:0] DAT_O, // Read data
  output        ACK_O, // Acknowledge output – single high pulse

  // User (packet) interface
  output        Rx_mac_empty,
  input         Rx_mac_rd,
  output [31:0] Rx_mac_data,
  output [1:0]  Rx_mac_BE,
  output        Rx_mac_sop,
  output        Rx_mac_eop,
  output        Rx_mac_err,

  output        Tx_mac_wa,
  input         Tx_mac_wr,
  input [31:0]  Tx_mac_data,
  input [1:0]   Tx_mac_BE,
  input         Tx_mac_sop,
  input         Tx_mac_eop,

  // PHY interface (GMII/MII)
  output        Gtx_clk, // Used only in GMII mode
  input         Rx_clk,
  input         Tx_clk,  // Used only in MII mode
  output        Tx_er,
  output        Tx_en,
  output [7:0]  Txd,
  input         Rx_er,
  input         Rx_dv,
  input [7:0]   Rxd,
  input         Crs,
  input         Col,

  // MDIO interface (to PHY)
  inout         Mdio,
  output        Mdc,

     // FIFO levels
     output [15:0] rx_fifo_occupied,
     output rx_fifo_full,
     output rx_fifo_empty,
     output [15:0] tx_fifo_occupied,
     output tx_fifo_full,
     output tx_fifo_empty,
     
     // Debug Interface
     output [31:0] debug0,
     output [31:0] debug1
);

   wire 	   rst_mac_rx = rst_mac;
   wire 	   rst_mac_tx = rst_mac;
   wire [2:0] 	   Speed;
   
   wire [31:0] 	   debug_rx;
   wire [31:0] 	   debug_tx0;
   wire [31:0] 	   debug_tx1;
   
  //-------------------------------------------------------------------------
  // Local declarations
  //-------------------------------------------------------------------------

  // RMON interface
  wire [15:0] Rx_pkt_length_rmon;
  wire        Rx_apply_rmon;
  wire [2:0]  Rx_pkt_err_type_rmon;
  wire [2:0]  Rx_pkt_type_rmon;
  wire [2:0]  Tx_pkt_type_rmon;
  wire [15:0] Tx_pkt_length_rmon;
  wire        Tx_apply_rmon;
  wire [2:0]  Tx_pkt_err_type_rmon;

  // PHY interface
  wire        MCrs_dv;
  wire [7:0]  MRxD;
  wire        MRxErr;

   // Flow-control signals
   wire [15:0] pause_quanta;
   wire        pause_quanta_val;
   wire [15:0] rx_fifo_space;
   wire        pause_apply, pause_quanta_sub;
   wire        xon_gen, xoff_gen, xon_gen_complete, xoff_gen_complete;
   wire [15:0] fc_hwmark, fc_lwmark;
   
  //PHY interface
  wire [7:0]  MTxD;
  wire        MTxEn;
  wire        MCRS;

  // Interface clk signals
  wire        MAC_tx_clk;
  wire        MAC_rx_clk;
  wire        MAC_tx_clk_div;
  wire        MAC_rx_clk_div;

  // Reg signals
  wire [4:0]  Tx_Hwmark;
  wire [4:0]  Tx_Lwmark;
  wire        pause_frame_send_en;
  wire [15:0] pause_quanta_set;
  wire        MAC_tx_add_en;
  wire        FullDuplex;
  wire [3:0]  MaxRetry;
  wire [5:0]  IFGset;
  wire [7:0]  MAC_tx_add_prom_data;
  wire [2:0]  MAC_tx_add_prom_add;
  wire        MAC_tx_add_prom_wr;
  wire        tx_pause_en;

  // Rx host interface
  wire        MAC_rx_add_chk_en;
  wire [7:0]  MAC_rx_add_prom_data;
  wire [2:0]  MAC_rx_add_prom_add;
  wire        MAC_rx_add_prom_wr;
  wire        broadcast_filter_en;
  wire        RX_APPEND_CRC;
  wire [4:0]  Rx_Hwmark;
  wire [4:0]  Rx_Lwmark;
  wire        CRC_chk_en;
  wire [5:0]  RX_IFG_SET;
  wire [15:0] RX_MAX_LENGTH;
  wire [6:0]  RX_MIN_LENGTH;

  // RMON host interface
  wire [5:0]  CPU_rd_addr;
  wire        CPU_rd_apply;
  wire        CPU_rd_grant;
  wire [31:0] CPU_rd_dout;

  // PHY int host interface
  wire        Line_loop_en;

  // MII to CPU             
  wire [7:0]  Divider;
  wire [15:0] CtrlData;
  wire [4:0]  Rgad;
  wire [4:0]  Fiad;
  wire        NoPre;
  wire        WCtrlData;
  wire        RStat;
  wire        ScanStat;
  wire        Busy;
  wire        LinkFail;
  wire        Nvalid;
  wire [15:0] Prsd;
  wire        WCtrlDataStart;
  wire        RStatStart;
  wire        UpdateMIIRX_DATAReg;
  wire [15:0] broadcast_bucket_depth;
  wire [15:0] broadcast_bucket_interval;

  //-------------------------------------------------------------------------
  // Instantiation of sub-modules
  //-------------------------------------------------------------------------

   MAC_rx #(.RX_FF_DEPTH(RX_FF_DEPTH))
     U_MAC_rx(
    .Reset                    ( rst_mac_rx                ),
    .Clk_user                 ( Clk_user                  ),
    .Clk                      ( MAC_rx_clk_div            ),

    // RMII interface
    .MCrs_dv                  ( MCrs_dv                   ),
    .MRxD                     ( MRxD                      ),
    .MRxErr                   ( MRxErr                    ),

    // Flow-control signals
    .pause_quanta             ( pause_quanta              ),
    .pause_quanta_val         ( pause_quanta_val          ),
    .rx_fifo_space            ( rx_fifo_space             ),
	      
    // User interface
    .Rx_mac_empty             ( Rx_mac_empty              ),
    .Rx_mac_rd                ( Rx_mac_rd                 ),
    .Rx_mac_data              ( Rx_mac_data               ),
    .Rx_mac_BE                ( Rx_mac_BE                 ),
    .Rx_mac_sop               ( Rx_mac_sop                ),
    .Rx_mac_eop               ( Rx_mac_eop                ),
    .Rx_mac_err               ( Rx_mac_err                ),

    // CPU
    .MAC_rx_add_chk_en        ( MAC_rx_add_chk_en         ),
    .MAC_add_prom_data        ( MAC_rx_add_prom_data      ),
    .MAC_add_prom_add         ( MAC_rx_add_prom_add       ),
    .MAC_add_prom_wr          ( MAC_rx_add_prom_wr        ),
    .broadcast_filter_en      ( broadcast_filter_en       ),
    .broadcast_bucket_depth   ( broadcast_bucket_depth    ),
    .broadcast_bucket_interval( broadcast_bucket_interval ),
    .RX_APPEND_CRC            ( RX_APPEND_CRC             ),
    .Rx_Hwmark                ( Rx_Hwmark                 ),
    .Rx_Lwmark                ( Rx_Lwmark                 ),
    .CRC_chk_en               ( CRC_chk_en                ),
    .RX_IFG_SET               ( RX_IFG_SET                ),
    .RX_MAX_LENGTH            ( RX_MAX_LENGTH             ),
    .RX_MIN_LENGTH            ( RX_MIN_LENGTH             ),

    // RMON interface
    .Rx_pkt_length_rmon       ( Rx_pkt_length_rmon        ),
    .Rx_apply_rmon            ( Rx_apply_rmon             ),
    .Rx_pkt_err_type_rmon     ( Rx_pkt_err_type_rmon      ),
    .Rx_pkt_type_rmon         ( Rx_pkt_type_rmon          ),

	      .rx_fifo_occupied(rx_fifo_occupied),
	      .rx_fifo_full(rx_fifo_full),
	      .rx_fifo_empty(rx_fifo_empty),
	      .debug(debug_rx)
  );

   MAC_tx #(.TX_FF_DEPTH(TX_FF_DEPTH))
     U_MAC_tx(
    .Reset               ( rst_mac_tx           ),
    .Clk                 ( MAC_tx_clk_div       ),
    //.Clk_user            ( Clk_user             ),
    .Clk_user            ( MAC_tx_clk_div             ),

    // PHY interface
    .TxD                 ( MTxD                 ),
    .TxEn                ( MTxEn                ),
    .CRS                 ( MCRS                 ),

    // RMON
    .Tx_pkt_type_rmon    ( Tx_pkt_type_rmon     ),
    .Tx_pkt_length_rmon  ( Tx_pkt_length_rmon   ),
    .Tx_apply_rmon       ( Tx_apply_rmon        ),
    .Tx_pkt_err_type_rmon( Tx_pkt_err_type_rmon ),

    // User interface
    .Tx_mac_wa           ( Tx_mac_wa            ),
    .Tx_mac_wr           ( Tx_mac_wr            ),
    .Tx_mac_data         ( Tx_mac_data          ),
    .Tx_mac_BE           ( Tx_mac_BE            ),
    .Tx_mac_sop          ( Tx_mac_sop           ),
    .Tx_mac_eop          ( Tx_mac_eop           ),

    // Host interface
    .Tx_Hwmark           ( Tx_Hwmark            ),
    .Tx_Lwmark           ( Tx_Lwmark            ),
    .MAC_tx_add_en       ( MAC_tx_add_en        ),
    .FullDuplex          ( FullDuplex           ),
    .MaxRetry            ( MaxRetry             ),
    .IFGset              ( IFGset               ),
    .MAC_add_prom_data   ( MAC_tx_add_prom_data ),
    .MAC_add_prom_add    ( MAC_tx_add_prom_add  ),
    .MAC_add_prom_wr     ( MAC_tx_add_prom_wr   ),

    .pause_apply         ( pause_apply          ),
    .pause_quanta_sub    ( pause_quanta_sub     ),
    .pause_quanta_set    ( pause_quanta_set     ),
    .xoff_gen            ( xoff_gen             ),
    .xon_gen             ( xon_gen              ),
    .xoff_gen_complete   ( xoff_gen_complete    ),
    .xon_gen_complete    ( xon_gen_complete     ),
	      .debug0(debug_tx0),
	      .debug1(debug_tx1)
    );

   // Flow control outbound -- when other side sends PAUSE, we wait
   flow_ctrl_tx flow_ctrl_tx
     (.rst(rst_mac_tx), 
      .tx_clk(MAC_tx_clk_div), 
      // Setting
      .tx_pause_en              ( tx_pause_en               ),
      // From RX side
      .pause_quanta (pause_quanta), 
      .pause_quanta_val(pause_quanta_val), // Other guy sent a PAUSE
      // To TX side
      .pause_apply (pause_apply),          // To TX, stop sending new frames
      .pause_quanta_sub (pause_quanta_sub) // From TX, indicates we have used up 1 quanta
      );
   
   flow_ctrl_rx flow_ctrl_rx  //  When we are running out of RX space, send a PAUSE
     (.rst(rst_mac_rx),  // FIXME
      // Settings
      .pause_frame_send_en      ( pause_frame_send_en       ),
      .pause_quanta_set         ( pause_quanta_set          ),
      .fc_hwmark (fc_hwmark),
      .fc_lwmark (fc_lwmark),
      // From RX side
      .rx_clk(MAC_rx_clk_div),
      .rx_fifo_space (rx_fifo_space),    // Decide if we need to send a PAUSE
      // To TX side
      .tx_clk(MAC_tx_clk_div), 
      .xoff_gen (xoff_gen), 
      .xon_gen(xon_gen),  // Tell our TX to send PAUSE frames
      .xoff_gen_complete (xoff_gen_complete), 
      .xon_gen_complete(xon_gen_complete)
      );
/*   
  RMON U_RMON(
    .Clk                 ( CLK_I                ),
    .Reset               ( RST_I                ),

    // Tx RMON
    .Tx_pkt_type_rmon    ( Tx_pkt_type_rmon     ),
    .Tx_pkt_length_rmon  ( Tx_pkt_length_rmon   ),
    .Tx_apply_rmon       ( Tx_apply_rmon        ),
    .Tx_pkt_err_type_rmon( Tx_pkt_err_type_rmon ),

    // Rx RMON
    .Rx_pkt_type_rmon    ( Rx_pkt_type_rmon     ),
    .Rx_pkt_length_rmon  ( Rx_pkt_length_rmon   ),
    .Rx_apply_rmon       ( Rx_apply_rmon        ),
    .Rx_pkt_err_type_rmon( Rx_pkt_err_type_rmon ),

    // CPU
    .CPU_rd_addr         ( CPU_rd_addr          ),
    .CPU_rd_apply        ( CPU_rd_apply         ),
    .CPU_rd_grant        ( CPU_rd_grant         ),
    .CPU_rd_dout         ( CPU_rd_dout          )
  );
*/
   Phy_int U_Phy_int(
    .rst_mac_rx  ( rst_mac_rx   ),
    .rst_mac_tx  ( rst_mac_tx   ),
    .MAC_rx_clk  ( MAC_rx_clk   ),
    .MAC_tx_clk  ( MAC_tx_clk   ),
    // Rx interface
    .MCrs_dv     ( MCrs_dv      ),
    .MRxD        ( MRxD         ),
    .MRxErr      ( MRxErr       ),
    // Tx interface
    .MTxD        ( MTxD         ),
    .MTxEn       ( MTxEn        ),
    .MCRS        ( MCRS         ),
    // PHY interface
    .Tx_er       ( Tx_er        ),
    .Tx_en       ( Tx_en        ),
    .Txd         ( Txd          ),
    .Rx_er       ( Rx_er        ),
    .Rx_dv       ( Rx_dv        ),
    .Rxd         ( Rxd          ),
    .Crs         ( Crs          ),
    .Col         ( Col          ),
    // Host interface
    .Line_loop_en( Line_loop_en ),
    .Speed       ( Speed        )  );

  Clk_ctrl U_Clk_ctrl(
    .Reset         ( rst_mac        ),
    .Clk_125M      ( Clk_125M       ),

    // Host interface
    .Speed         ( Speed          ),

    // Phy interface
    .Gtx_clk       ( Gtx_clk        ),
    .Rx_clk        ( Rx_clk         ),
    .Tx_clk        ( Tx_clk         ),

    // Interface clocks
    .MAC_tx_clk    ( MAC_tx_clk     ),
    .MAC_rx_clk    ( MAC_rx_clk     ),
    .MAC_tx_clk_div( MAC_tx_clk_div ),
    .MAC_rx_clk_div( MAC_rx_clk_div )
  );

  eth_miim U_eth_miim(
    .Clk                ( CLK_I               ),
    .Reset              ( RST_I               ),
    .Divider            ( Divider             ),
    .NoPre              ( NoPre               ),
    .CtrlData           ( CtrlData            ),
    .Rgad               ( Rgad                ),
    .Fiad               ( Fiad                ),
    .WCtrlData          ( WCtrlData           ),
    .RStat              ( RStat               ),
    .ScanStat           ( ScanStat            ),
    .Mdio               ( Mdio                ),
    .Mdc                ( Mdc                 ),
    .Busy               ( Busy                ),
    .Prsd               ( Prsd                ),
    .LinkFail           ( LinkFail            ),
    .Nvalid             ( Nvalid              ),
    .WCtrlDataStart     ( WCtrlDataStart      ),
    .RStatStart         ( RStatStart          ),
    .UpdateMIIRX_DATAReg( UpdateMIIRX_DATAReg )
  );

  Reg_int U_Reg_int(
    // Wishbone compliant core host interface
    .CLK_I( CLK_I ),
    .RST_I( RST_I ),
    .STB_I( STB_I ),
    .CYC_I( CYC_I ),
    .ADR_I( ADR_I ),
    .WE_I ( WE_I  ),
    .DAT_I( DAT_I ),
    .DAT_O( DAT_O ),
    .ACK_O( ACK_O ),

    // Tx host interface
    .Tx_Hwmark                ( Tx_Hwmark                 ),
    .Tx_Lwmark                ( Tx_Lwmark                 ),
    .MAC_tx_add_en            ( MAC_tx_add_en             ),
    .FullDuplex               ( FullDuplex                ),
    .MaxRetry                 ( MaxRetry                  ),
    .IFGset                   ( IFGset                    ),
    .MAC_tx_add_prom_data     ( MAC_tx_add_prom_data      ),
    .MAC_tx_add_prom_add      ( MAC_tx_add_prom_add       ),
    .MAC_tx_add_prom_wr       ( MAC_tx_add_prom_wr        ),

    // Rx host interface
    .MAC_rx_add_chk_en        ( MAC_rx_add_chk_en         ),
    .MAC_rx_add_prom_data     ( MAC_rx_add_prom_data      ),
    .MAC_rx_add_prom_add      ( MAC_rx_add_prom_add       ),
    .MAC_rx_add_prom_wr       ( MAC_rx_add_prom_wr        ),
    .broadcast_filter_en      ( broadcast_filter_en       ),
    .broadcast_bucket_depth   ( broadcast_bucket_depth    ),
    .broadcast_bucket_interval( broadcast_bucket_interval ),
    .RX_APPEND_CRC            ( RX_APPEND_CRC             ),
    .Rx_Hwmark                ( Rx_Hwmark                 ),
    .Rx_Lwmark                ( Rx_Lwmark                 ),
    .CRC_chk_en               ( CRC_chk_en                ),
    .RX_IFG_SET               ( RX_IFG_SET                ),
    .RX_MAX_LENGTH            ( RX_MAX_LENGTH             ),
    .RX_MIN_LENGTH            ( RX_MIN_LENGTH             ),

    // Flow Control settings
    .pause_frame_send_en      ( pause_frame_send_en       ),
    .pause_quanta_set         ( pause_quanta_set          ),
    .tx_pause_en              ( tx_pause_en               ),
    .fc_hwmark                ( fc_hwmark                 ),
    .fc_lwmark                ( fc_lwmark                 ),
		    
    // RMON host interface
    .CPU_rd_addr              ( CPU_rd_addr               ),
    .CPU_rd_apply             ( CPU_rd_apply              ),
    .CPU_rd_grant             ( CPU_rd_grant              ),
    .CPU_rd_dout              ( CPU_rd_dout               ),

    // PHY int host interface
    .Line_loop_en             ( Line_loop_en              ),
    .Speed                    ( Speed                     ),

    // MII to CPU
    .Divider                  ( Divider                   ),
    .CtrlData                 ( CtrlData                  ),
    .Rgad                     ( Rgad                      ),
    .Fiad                     ( Fiad                      ),
    .NoPre                    ( NoPre                     ),
    .WCtrlData                ( WCtrlData                 ),
    .RStat                    ( RStat                     ),
    .ScanStat                 ( ScanStat                  ),
    .Busy                     ( Busy                      ),
    .LinkFail                 ( LinkFail                  ),
    .Nvalid                   ( Nvalid                    ),
    .Prsd                     ( Prsd                      ),
    .WCtrlDataStart           ( WCtrlDataStart            ),
    .RStatStart               ( RStatStart                ),
    .UpdateMIIRX_DATAReg      ( UpdateMIIRX_DATAReg       )
  );

   assign     debug0 = {xon_gen, xoff_gen, Tx_en, Rx_dv};
   //assign     debug0 = {{debug_rx[3:0], xon_gen, xon_gen_complete, xoff_gen, xoff_gen_complete},
   //			{1'b0,Rx_mac_err,Rx_mac_empty,Rx_mac_rd,Rx_mac_sop,Rx_mac_eop,Rx_mac_BE[1:0]},
   //			{rx_fifo_space}};
   //assign     debug0 = debug_tx0;
   //assign debug1 = debug_tx1;
endmodule
