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
//                                                                    
// CVS Revision History                                               
//                                                                    
// $Log: MAC_top.v,v $
// Revision 1.3  2006/01/19 14:07:52  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:13  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:44  Administrator
// no message
// 

module MAC_top(
                //system signals
input           Reset                   ,
input           Clk_125M                ,
input           Clk_user                ,
input           Clk_reg                 ,
output  [2:0]   Speed                   ,
                //user interface 
output          Rx_mac_ra               ,
input           Rx_mac_rd               ,
output  [31:0]  Rx_mac_data             ,
output  [1:0]   Rx_mac_BE               ,
output          Rx_mac_pa               ,
output          Rx_mac_sop              ,
output          Rx_mac_eop              ,
                //user interface 
output          Tx_mac_wa               ,
input           Tx_mac_wr               ,
input   [31:0]  Tx_mac_data             ,
input   [1:0]   Tx_mac_BE               ,//big endian
input           Tx_mac_sop              ,
input           Tx_mac_eop              ,
                //Phy interface          
                //Phy interface         
output          Gtx_clk                 ,//used only in GMII mode
input           Rx_clk                  ,
input           Tx_clk                  ,//used only in MII mode
output          Tx_er                   ,
output          Tx_en                   ,
output  [7:0]   Txd                     ,
input           Rx_er                   ,
input           Rx_dv                   ,
input   [7:0]   Rxd                     ,
input           Crs                     ,
input           Col                     ,
                //host interface
input           CSB                     ,
input           WRB                     ,
input   [15:0]  CD_in                   ,
output  [15:0]  CD_out                  ,
input   [7:0]   CA                      ,                
                //mdx
inout           Mdio                    ,// MII Management Data In
output          Mdc                      // MII Management Data Clock       

);                       
//******************************************************************************
//internal signals                                                              
//******************************************************************************
                //RMON interface
wire    [15:0]  Rx_pkt_length_rmon      ;
wire            Rx_apply_rmon           ;
wire    [2:0]   Rx_pkt_err_type_rmon    ;
wire    [2:0]   Rx_pkt_type_rmon        ;
wire    [2:0]   Tx_pkt_type_rmon        ;
wire    [15:0]  Tx_pkt_length_rmon      ;
wire            Tx_apply_rmon           ;
wire    [2:0]   Tx_pkt_err_type_rmon    ;
                //PHY interface
wire            MCrs_dv                 ;       
wire    [7:0]   MRxD                    ;       
wire            MRxErr                  ;       
                //flow_control signals  
wire    [15:0]  pause_quanta            ;   
wire            pause_quanta_val        ; 
                //PHY interface
wire    [7:0]   MTxD                    ;
wire            MTxEn                   ;   
wire            MCRS                    ;
                //interface clk signals
wire            MAC_tx_clk              ;
wire            MAC_rx_clk              ;
wire            MAC_tx_clk_div          ;
wire            MAC_rx_clk_div          ;
                //reg signals   
wire    [4:0]	Tx_Hwmark				;       
wire    [4:0]	Tx_Lwmark				;       
wire    		pause_frame_send_en		;       
wire    [15:0]	pause_quanta_set		;       
wire    		MAC_tx_add_en			;       
wire    		FullDuplex         		;       
wire    [3:0]	MaxRetry	        	;       
wire    [5:0]	IFGset					;       
wire    [7:0]	MAC_tx_add_prom_data	;       
wire    [2:0]	MAC_tx_add_prom_add		;       
wire    		MAC_tx_add_prom_wr		;       
wire    		tx_pause_en				;       
wire    		xoff_cpu	        	;       
wire    		xon_cpu	            	;       
		        		//Rx host interface 	 
wire    		MAC_rx_add_chk_en		;       
wire    [7:0]	MAC_rx_add_prom_data	;       
wire    [2:0]	MAC_rx_add_prom_add		;       
wire    		MAC_rx_add_prom_wr		;       
wire    		broadcast_filter_en	    ;       
wire    [15:0]	broadcast_MAX	        ;       
wire    		RX_APPEND_CRC			;       
wire    [4:0]	Rx_Hwmark			    ;           
wire    [4:0]	Rx_Lwmark			    ;           
wire    		CRC_chk_en				;       
wire    [5:0]	RX_IFG_SET	  			;       
wire    [15:0]	RX_MAX_LENGTH 			;
wire    [6:0]	RX_MIN_LENGTH			;
		        		//RMON host interface    
wire    [5:0]	CPU_rd_addr				;
wire    		CPU_rd_apply			;
wire    		CPU_rd_grant			;
wire    [31:0]	CPU_rd_dout				;
		        		//Phy int host interface 
wire    		Line_loop_en			;
		        		//MII to CPU             
wire    [7:0] 	Divider            		;
wire    [15:0] 	CtrlData           		;
wire    [4:0] 	Rgad               		;
wire    [4:0] 	Fiad               		;
wire           	NoPre              		;
wire           	WCtrlData          		;
wire           	RStat              		;
wire           	ScanStat           		;
wire         	Busy               		;
wire         	LinkFail           		;
wire         	Nvalid             		;
wire    [15:0] 	Prsd               		;
wire         	WCtrlDataStart     		;
wire         	RStatStart         		;
wire         	UpdateMIIRX_DATAReg		;
wire    [15:0]  broadcast_bucket_depth              ;
wire    [15:0]  broadcast_bucket_interval           ;
              
//******************************************************************************
//internal signals                                                              
//******************************************************************************
MAC_rx U_MAC_rx(
.Reset                      (Reset                      ),    
.Clk_user                   (Clk_user                   ), 
.Clk                        (MAC_rx_clk_div             ), 
 //RMII interface           (//PHY interface            ),  
.MCrs_dv                    (MCrs_dv                    ),        
.MRxD                       (MRxD                       ),
.MRxErr                     (MRxErr                     ),
 //flow_control signals     (//flow_control signals     ),  
.pause_quanta               (pause_quanta               ),
.pause_quanta_val           (pause_quanta_val           ),
 //user interface           (//user interface           ),  
.Rx_mac_ra                  (Rx_mac_ra                  ),
.Rx_mac_rd                  (Rx_mac_rd                  ),
.Rx_mac_data                (Rx_mac_data                ),       
.Rx_mac_BE                  (Rx_mac_BE                  ),
.Rx_mac_pa                  (Rx_mac_pa                  ),
.Rx_mac_sop                 (Rx_mac_sop                 ),
.Rx_mac_eop                 (Rx_mac_eop                 ),
 //CPU                      (//CPU                      ),  
.MAC_rx_add_chk_en          (MAC_rx_add_chk_en          ),
.MAC_add_prom_data          (MAC_rx_add_prom_data       ),
.MAC_add_prom_add           (MAC_rx_add_prom_add        ),
.MAC_add_prom_wr            (MAC_rx_add_prom_wr         ),       
.broadcast_filter_en        (broadcast_filter_en        ),       
.broadcast_bucket_depth     (broadcast_bucket_depth     ),           
.broadcast_bucket_interval  (broadcast_bucket_interval  ),
.RX_APPEND_CRC              (RX_APPEND_CRC              ), 
.Rx_Hwmark                  (Rx_Hwmark                  ),
.Rx_Lwmark                  (Rx_Lwmark                  ),
.CRC_chk_en                 (CRC_chk_en                 ),  
.RX_IFG_SET                 (RX_IFG_SET                 ),
.RX_MAX_LENGTH              (RX_MAX_LENGTH              ),
.RX_MIN_LENGTH              (RX_MIN_LENGTH              ),
 //RMON interface           (//RMON interface           ),  
.Rx_pkt_length_rmon         (Rx_pkt_length_rmon         ),
.Rx_apply_rmon              (Rx_apply_rmon              ),
.Rx_pkt_err_type_rmon       (Rx_pkt_err_type_rmon       ),
.Rx_pkt_type_rmon           (Rx_pkt_type_rmon           )
);

MAC_tx U_MAC_tx(
.Reset                      (Reset                      ),
.Clk                        (MAC_tx_clk_div             ),
.Clk_user                   (Clk_user                   ),
 //PHY interface            (//PHY interface            ),
.TxD                        (MTxD                       ),
.TxEn                       (MTxEn                      ),
.CRS                        (MCRS                       ),
 //RMON                     (//RMON                     ),
.Tx_pkt_type_rmon           (Tx_pkt_type_rmon           ),
.Tx_pkt_length_rmon         (Tx_pkt_length_rmon         ),
.Tx_apply_rmon              (Tx_apply_rmon              ),
.Tx_pkt_err_type_rmon       (Tx_pkt_err_type_rmon       ),
 //user interface           (//user interface           ),
.Tx_mac_wa                  (Tx_mac_wa                  ),
.Tx_mac_wr                  (Tx_mac_wr                  ),
.Tx_mac_data                (Tx_mac_data                ),
.Tx_mac_BE                  (Tx_mac_BE                  ),
.Tx_mac_sop                 (Tx_mac_sop                 ),
.Tx_mac_eop                 (Tx_mac_eop                 ),
 //host interface           (//host interface           ),
.Tx_Hwmark                  (Tx_Hwmark                  ),
.Tx_Lwmark                  (Tx_Lwmark                  ),
.pause_frame_send_en        (pause_frame_send_en        ),
.pause_quanta_set           (pause_quanta_set           ),
.MAC_tx_add_en              (MAC_tx_add_en              ),
.FullDuplex                 (FullDuplex                 ),
.MaxRetry                   (MaxRetry                   ),
.IFGset                     (IFGset                     ),
.MAC_add_prom_data          (MAC_tx_add_prom_data       ),
.MAC_add_prom_add           (MAC_tx_add_prom_add        ),
.MAC_add_prom_wr            (MAC_tx_add_prom_wr         ),
.tx_pause_en                (tx_pause_en                ),
.xoff_cpu                   (xoff_cpu                   ),
.xon_cpu                    (xon_cpu                    ),
 //MAC_rx_flow              (//MAC_rx_flow              ),
.pause_quanta               (pause_quanta               ),
.pause_quanta_val           (pause_quanta_val           )
);

RMON U_RMON(
.Clk                        (Clk_reg                    ),
.Reset                      (Reset                      ),
 //Tx_RMON                  (//Tx_RMON                  ),
.Tx_pkt_type_rmon           (Tx_pkt_type_rmon           ),
.Tx_pkt_length_rmon         (Tx_pkt_length_rmon         ),
.Tx_apply_rmon              (Tx_apply_rmon              ),
.Tx_pkt_err_type_rmon       (Tx_pkt_err_type_rmon       ),
 //Tx_RMON                  (//Tx_RMON                  ),
.Rx_pkt_type_rmon           (Rx_pkt_type_rmon           ),
.Rx_pkt_length_rmon         (Rx_pkt_length_rmon         ),
.Rx_apply_rmon              (Rx_apply_rmon              ),
.Rx_pkt_err_type_rmon       (Rx_pkt_err_type_rmon       ),
 //CPU                      (//CPU                      ),
.CPU_rd_addr                (CPU_rd_addr                ),
.CPU_rd_apply               (CPU_rd_apply               ),
.CPU_rd_grant               (CPU_rd_grant               ),
.CPU_rd_dout                (CPU_rd_dout                )
);

Phy_int U_Phy_int(
.Reset                      (Reset                      ),
.MAC_rx_clk                 (MAC_rx_clk                 ),
.MAC_tx_clk                 (MAC_tx_clk                 ),
 //Rx interface             (//Rx interface             ),
.MCrs_dv                    (MCrs_dv                    ),
.MRxD                       (MRxD                       ),
.MRxErr                     (MRxErr                     ),
 //Tx interface             (//Tx interface             ),
.MTxD                       (MTxD                       ),
.MTxEn                      (MTxEn                      ),
.MCRS                       (MCRS                       ),
 //Phy interface            (//Phy interface            ),
.Tx_er                      (Tx_er                      ),
.Tx_en                      (Tx_en                      ),
.Txd                        (Txd                        ),
.Rx_er                      (Rx_er                      ),
.Rx_dv                      (Rx_dv                      ),
.Rxd                        (Rxd                        ),
.Crs                        (Crs                        ),
.Col                        (Col                        ),
 //host interface           (//host interface           ),
.Line_loop_en               (Line_loop_en               ),
.Speed                      (Speed                      )
);

Clk_ctrl U_Clk_ctrl(
.Reset                      (Reset                      ),
.Clk_125M                   (Clk_125M                   ),
 //host interface           (//host interface           ),
.Speed                      (Speed                      ),
 //Phy interface            (//Phy interface            ),
.Gtx_clk                    (Gtx_clk                    ),
.Rx_clk                     (Rx_clk                     ),
.Tx_clk                     (Tx_clk                     ),
 //interface clk            (//interface clk            ),
.MAC_tx_clk                 (MAC_tx_clk                 ),
.MAC_rx_clk                 (MAC_rx_clk                 ),
.MAC_tx_clk_div             (MAC_tx_clk_div             ),
.MAC_rx_clk_div             (MAC_rx_clk_div             )
);

eth_miim U_eth_miim(                                        
.Clk                        (Clk_reg                    ),  
.Reset                      (Reset                      ),  
.Divider                    (Divider                    ),  
.NoPre                      (NoPre                      ),  
.CtrlData                   (CtrlData                   ),  
.Rgad                       (Rgad                       ),  
.Fiad                       (Fiad                       ),  
.WCtrlData                  (WCtrlData                  ),  
.RStat                      (RStat                      ),  
.ScanStat                   (ScanStat                   ),  
.Mdio                       (Mdio                       ),  
.Mdc                        (Mdc                        ),  
.Busy                       (Busy                       ),  
.Prsd                       (Prsd                       ),  
.LinkFail                   (LinkFail                   ),  
.Nvalid                     (Nvalid                     ),  
.WCtrlDataStart             (WCtrlDataStart             ),  
.RStatStart                 (RStatStart                 ),  
.UpdateMIIRX_DATAReg        (UpdateMIIRX_DATAReg        )); 

reg_int U_reg_int(
.Reset	               		(Reset	                  	),    
.Clk_reg                  	(Clk_reg                 	), 
.CSB                        (CSB                        ),
.WRB                        (WRB                        ),
.CD_in                      (CD_in                      ),
.CD_out                     (CD_out                     ),
.CA                         (CA                         ),
 //Tx host interface        (//Tx host interface        ),
.Tx_Hwmark				    (Tx_Hwmark				    ),
.Tx_Lwmark				    (Tx_Lwmark				    ),
.pause_frame_send_en		(pause_frame_send_en		),
.pause_quanta_set		    (pause_quanta_set		    ),
.MAC_tx_add_en			    (MAC_tx_add_en			    ),
.FullDuplex         		(FullDuplex         		),
.MaxRetry	        	    (MaxRetry	        	    ),
.IFGset					    (IFGset					    ),
.MAC_tx_add_prom_data	    (MAC_tx_add_prom_data	    ),
.MAC_tx_add_prom_add		(MAC_tx_add_prom_add		),
.MAC_tx_add_prom_wr		    (MAC_tx_add_prom_wr		    ),
.tx_pause_en				(tx_pause_en				),
.xoff_cpu	        	    (xoff_cpu	        	    ),
.xon_cpu	            	(xon_cpu	            	),
 //Rx host interface 	    (//Rx host interface 	    ),
.MAC_rx_add_chk_en		    (MAC_rx_add_chk_en		    ),
.MAC_rx_add_prom_data	    (MAC_rx_add_prom_data	    ),
.MAC_rx_add_prom_add		(MAC_rx_add_prom_add		),
.MAC_rx_add_prom_wr		    (MAC_rx_add_prom_wr		    ),
.broadcast_filter_en	    (broadcast_filter_en	    ),
.broadcast_bucket_depth     (broadcast_bucket_depth     ),           
.broadcast_bucket_interval  (broadcast_bucket_interval  ),
.RX_APPEND_CRC			    (RX_APPEND_CRC			    ), 
.Rx_Hwmark       			(Rx_Hwmark					),
.Rx_Lwmark                  (Rx_Lwmark                  ),
.CRC_chk_en				    (CRC_chk_en				    ),
.RX_IFG_SET	  			    (RX_IFG_SET	  			    ),
.RX_MAX_LENGTH 			    (RX_MAX_LENGTH 			    ),
.RX_MIN_LENGTH			    (RX_MIN_LENGTH			    ),
 //RMON host interface      (//RMON host interface      ),
.CPU_rd_addr				(CPU_rd_addr				),
.CPU_rd_apply			    (CPU_rd_apply			    ),
.CPU_rd_grant			    (CPU_rd_grant			    ),
.CPU_rd_dout				(CPU_rd_dout				),
 //Phy int host interface   (//Phy int host interface   ),
.Line_loop_en			    (Line_loop_en			    ),
.Speed					    (Speed					    ),
 //MII to CPU               (//MII to CPU               ),
.Divider            		(Divider            		),
.CtrlData           		(CtrlData           		),
.Rgad               		(Rgad               		),
.Fiad               		(Fiad               		),
.NoPre              		(NoPre              		),
.WCtrlData          		(WCtrlData          		),
.RStat              		(RStat              		),
.ScanStat           		(ScanStat           		),
.Busy               		(Busy               		),
.LinkFail           		(LinkFail           		),
.Nvalid             		(Nvalid             		),
.Prsd               		(Prsd               		),
.WCtrlDataStart     		(WCtrlDataStart     		),
.RStatStart         		(RStatStart         		),
.UpdateMIIRX_DATAReg		(UpdateMIIRX_DATAReg		)
);

endmodule

















