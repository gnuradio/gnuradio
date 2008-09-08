//////////////////////////////////////////////////////////////////////
////                                                              ////
////  reg_int_sim.v                                               ////
////                                                              ////
////  This file is part of the Ethernet IP core project           ////
////  http://www.opencores.org/projects.cgi/web/ethernet_tri_mode/////
////                                                              ////
////  Author(s):                                                  ////
////      - Jon Gao (gaojon@yahoo.com)                      	  ////
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
// $Log: reg_int_sim.v,v $
// Revision 1.1  2005/12/13 12:54:38  maverickist
// first simulation passed
//

module reg_int_sim (
input			Reset					,
input			Clk_reg					,
				//Tx host interface 
output	[4:0]	Tx_Hwmark				,
output	[4:0]	Tx_Lwmark				,	
output			pause_frame_send_en		,				
output	[15:0]	pause_quanta_set		,
output			MAC_tx_add_en			,				
output			FullDuplex         		,
output	[3:0]	MaxRetry	        	,
output	[5:0]	IFGset					,
output	[7:0]	MAC_tx_add_prom_data	,
output	[2:0]	MAC_tx_add_prom_add		,
output			MAC_tx_add_prom_wr		,
output			tx_pause_en				,
output			xoff_cpu	        	,
output			xon_cpu	            	,
				//Rx host interface 	
output			MAC_rx_add_chk_en		,	
output	[7:0]	MAC_rx_add_prom_data	,	
output	[2:0]	MAC_rx_add_prom_add		,   
output			MAC_rx_add_prom_wr		,   
output			broadcast_filter_en	    ,
output	[15:0]	broadcast_MAX	        ,				        
output			RX_APPEND_CRC			,
output			CRC_chk_en				,				
output	[5:0]	RX_IFG_SET	  			,
output	[15:0]	RX_MAX_LENGTH 			,//	1518
output	[6:0]	RX_MIN_LENGTH			,//	64
				//RMON host interface
output	[5:0]	CPU_rd_addr				,
output			CPU_rd_apply			,
input			CPU_rd_grant			,
input	[31:0]	CPU_rd_dout				,
				//Phy int host interface     
output			Line_loop_en			,
output	[2:0]	Speed					,
				//MII to CPU 
output  [7:0] 	Divider            		,// Divider for the host clock
output  [15:0] 	CtrlData           		,// Control Data (to be written to the PHY reg.)
output  [4:0] 	Rgad               		,// Register Address (within the PHY)
output  [4:0] 	Fiad               		,// PHY Address
output         	NoPre              		,// No Preamble (no 32-bit preamble)
output         	WCtrlData          		,// Write Control Data operation
output         	RStat              		,// Read Status operation
output         	ScanStat           		,// Scan Status operation
input        	Busy               		,// Busy Signal
input        	LinkFail           		,// Link Integrity Signal
input        	Nvalid             		,// Invalid Status (qualifier for the valid scan result)
input 	[15:0] 	Prsd               		,// Read Status Data (data read from the PHY)
input        	WCtrlDataStart     		,// This signals resets the WCTRLDATA bit in the MIIM Command register
input        	RStatStart         		,// This signal resets the RSTAT BIT in the MIIM Command register
input        	UpdateMIIRX_DATAReg		,// Updates MII RX_DATA register with read data
);

   assign 	Tx_Hwmark				    	=5'h1e;
   assign 	Tx_Lwmark				        =5'h19;
   assign 	pause_frame_send_en		        =0;
   assign 	pause_quanta_set		            =0;
   assign 	MAC_tx_add_en			        =0;
   assign 	FullDuplex         		        =1;
   assign 	MaxRetry	        	            =2;
   assign 	IFGset					        =10;
   assign 	MAC_tx_add_prom_data	            =0;
   assign 	MAC_tx_add_prom_add		        =0;
   assign 	MAC_tx_add_prom_wr		        =0;
   assign 	tx_pause_en				        =0;
   assign 	xoff_cpu	        	            =0;
   assign 	xon_cpu	                        =0;
   
   assign 	MAC_rx_add_chk_en				=0;
   assign 	MAC_rx_add_prom_data             =0;
   assign 	MAC_rx_add_prom_add	            =0;
   assign 	MAC_rx_add_prom_wr	            =0;
   assign 	broadcast_filter_en	            =0;
   assign 	broadcast_MAX	                =10;
   assign 	RX_APPEND_CRC		            =0;
   assign 	CRC_chk_en			            =1;
   assign 	RX_IFG_SET	  		            =10;
   assign 	RX_MAX_LENGTH 		            =1518;
   assign 	RX_MIN_LENGTH		            =64;
   
   assign 	CPU_rd_addr						=0;
   assign 	CPU_rd_apply	                    =0;
   
   assign 	Line_loop_en						=0;		
   assign 	Speed					        =3'b001;
   
endmodule // reg_int_sim
