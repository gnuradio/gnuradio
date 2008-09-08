//////////////////////////////////////////////////////////////////////
////                                                              ////
////  User_input_sim.v                                            ////
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
// $Log: User_int_sim.v,v $
// Revision 1.3  2006/11/17 17:53:07  maverickist
// no message
//
// Revision 1.2  2006/01/19 14:07:50  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/13 12:15:35  Administrator
// no message
//
// Revision 1.1.1.1  2005/12/13 01:51:44  Administrator
// no message
// 

module User_int_sim (Reset			        ,		
		     Clk_user				,
		     CPU_init_end            ,
		     //user inputerface      ,
		     Rx_mac_ra				,
		     Rx_mac_rd				,
		     Rx_mac_data				,
		     Rx_mac_BE				,
		     Rx_mac_pa				,
		     Rx_mac_sop				,
		     Rx_mac_eop				,
		     //user inputerface      ,
		     Tx_mac_wa	        	,
		     Tx_mac_wr	        	,
		     Tx_mac_data	        	,
		     Tx_mac_BE				,
		     Tx_mac_sop	        	,
		     Tx_mac_eop				
		     );

   input			Reset					;
   input			Clk_user				;
   input 			CPU_init_end            ;
   //user inputerface 
   input			Rx_mac_ra				;
   output			Rx_mac_rd				;
   input [31:0] 		Rx_mac_data				;
   input [1:0] 			Rx_mac_BE				;
   input			Rx_mac_pa				;
   input			Rx_mac_sop				;
   input			Rx_mac_eop				;
   //user inputerface 
   input			Tx_mac_wa	        	;
   output			Tx_mac_wr	        	;
   output [31:0] 		Tx_mac_data	        	;
   output [1:0] 		Tx_mac_BE				;//big endian
   output			Tx_mac_sop	        	;
   output			Tx_mac_eop				;
   
   // ////////////////////////////////////////////////////////////////////
   // inputernal signals
   // ////////////////////////////////////////////////////////////////////
   reg [4:0] 			operation;
   reg [31:0] 			data;
   reg				Rx_mac_rd;
   reg				Start_tran;
   // ////////////////////////////////////////////////////////////////////
   //generate Tx user data
   // ////////////////////////////////////////////////////////////////////
   initial 
     begin
	operation 	=0;
	data		=0;
     end    
   
   always @ (posedge Clk_user or posedge Reset)
     if (Reset)
       Start_tran  	<=0;
     else if (Tx_mac_eop&&!Tx_mac_wa)  
       Start_tran  	<=0;     
     else if (Tx_mac_wa)
       Start_tran  	<=1;     
   
   always @ (posedge Clk_user)
     if (Tx_mac_wa&&CPU_init_end)
       $ip_32W_gen("../data/config.ini",operation,data);
     else
       begin
	  operation	<=0;
	  data		<=0;
       end
   
   assign Tx_mac_data	=data;
   assign Tx_mac_wr	=operation[4];
   assign Tx_mac_sop	=operation[3];
   assign Tx_mac_eop   =operation[2];
   assign Tx_mac_BE    =operation[1:0];
   //////////////////////////////////////////////////////////////////////
   //verify Rx user data
   //////////////////////////////////////////////////////////////////////
   always @ (posedge Clk_user or posedge Reset)
     if (Reset)
       Rx_mac_rd	<=0;
     else if(Rx_mac_ra)
       Rx_mac_rd	<=1;
     else
       Rx_mac_rd	<=0;
   
   always @ (posedge Clk_user )
     if (Rx_mac_pa)    
       $ip_32W_check(	Rx_mac_data,
			{Rx_mac_sop,Rx_mac_eop,Rx_mac_eop?Rx_mac_BE:2'b0});
   
endmodule // User_int_sim
