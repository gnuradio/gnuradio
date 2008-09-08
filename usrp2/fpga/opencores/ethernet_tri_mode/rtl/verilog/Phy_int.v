//////////////////////////////////////////////////////////////////////
////                                                              ////
////  Phy_int.v                                                   ////
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
// $Log: Phy_int.v,v $
// Revision 1.3  2006/01/19 14:07:53  maverickist
// verification is complete.
//
// Revision 1.3  2005/12/16 06:44:14  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.2  2005/12/13 12:15:36  Administrator
// no message
//
// Revision 1.1.1.1  2005/12/13 01:51:44  Administrator
// no message
// 

module Phy_int (
Reset               ,
MAC_rx_clk          ,
MAC_tx_clk          ,
//Rx interface      ,
MCrs_dv             ,
MRxD                ,
MRxErr              ,
//Tx interface      ,
MTxD                ,
MTxEn               ,
MCRS                ,
//Phy interface     ,
Tx_er               ,
Tx_en               ,
Txd                 ,
Rx_er               ,
Rx_dv               ,
Rxd                 ,
Crs                 ,
Col                 ,
//host interface    ,
Line_loop_en        ,
Speed               

);
input           Reset               ;
input           MAC_rx_clk          ;
input           MAC_tx_clk          ;
                //Rx interface
output          MCrs_dv             ;       
output  [7:0]   MRxD                ;       
output          MRxErr              ;       
                //Tx interface
input   [7:0]   MTxD                ;
input           MTxEn               ;   
output          MCRS                ;
                //Phy interface
output          Tx_er               ;
output          Tx_en               ;
output  [7:0]   Txd                 ;
input           Rx_er               ;
input           Rx_dv               ;
input   [7:0]   Rxd                 ;
input           Crs                 ;
input           Col                 ;
                //host interface
input           Line_loop_en        ;
input   [2:0]   Speed               ;
//******************************************************************************
//internal signals                                                              
//******************************************************************************
reg     [7:0]   MTxD_dl1            ;
reg             MTxEn_dl1           ;
reg             Tx_odd_data_ptr     ;  
reg             Rx_odd_data_ptr     ;
reg             Tx_en               ;
reg     [7:0]   Txd                 ;
reg             MCrs_dv             ;
reg     [7:0]   MRxD                ;
reg             Rx_er_dl1           ;
reg             Rx_dv_dl1           ;
reg             Rx_dv_dl2           ;
reg     [7:0]   Rxd_dl1             ;
reg     [7:0]   Rxd_dl2             ;
reg             Crs_dl1             ;
reg             Col_dl1             ;
//******************************************************************************
//Tx control                                                              
//******************************************************************************
//reg boundery signals
always @ (posedge MAC_tx_clk or posedge Reset)
    if (Reset)
        begin
        MTxD_dl1            <=0;
        MTxEn_dl1           <=0;
        end  
    else
        begin
        MTxD_dl1            <=MTxD  ;
        MTxEn_dl1           <=MTxEn ;
        end 
     
always @ (posedge MAC_tx_clk or posedge Reset)
    if (Reset)   
        Tx_odd_data_ptr     <=0;
    else if (!MTxD_dl1)
        Tx_odd_data_ptr     <=0;
    else 
        Tx_odd_data_ptr     <=!Tx_odd_data_ptr;
        

always @ (posedge MAC_tx_clk or posedge Reset)
    if (Reset)  
        Txd                 <=0;
    else if(Speed[2]&&MTxEn_dl1)
        Txd                 <=MTxD_dl1;
    else if(MTxEn_dl1&&!Tx_odd_data_ptr)
        Txd                 <={4'b0,MTxD_dl1[3:0]};
    else if(MTxEn_dl1&&Tx_odd_data_ptr)
        Txd                 <={4'b0,MTxD_dl1[7:4]};
    else
        Txd                 <=0;

always @ (posedge MAC_tx_clk or posedge Reset)
    if (Reset)  
        Tx_en               <=0;
    else if(MTxEn_dl1)
        Tx_en               <=1;    
    else
        Tx_en               <=0;

assign Tx_er=0;

//******************************************************************************
//Rx control                                                              
//******************************************************************************
//reg boundery signals
always @ (posedge MAC_rx_clk or posedge Reset)
    if (Reset)  
        begin
        Rx_er_dl1           <=0;
        Rx_dv_dl1           <=0;
        Rx_dv_dl2           <=0 ;
        Rxd_dl1             <=0;
        Rxd_dl2             <=0;
        Crs_dl1             <=0;
        Col_dl1             <=0;
        end
    else
        begin
        Rx_er_dl1           <=Rx_er     ;
        Rx_dv_dl1           <=Rx_dv     ;
        Rx_dv_dl2           <=Rx_dv_dl1 ;
        Rxd_dl1             <=Rxd       ;
        Rxd_dl2             <=Rxd_dl1   ;
        Crs_dl1             <=Crs       ;
        Col_dl1             <=Col       ;
        end     

assign MRxErr   =Rx_er_dl1      ;
assign MCRS     =Crs_dl1        ;

always @ (posedge MAC_rx_clk or posedge Reset)
    if (Reset)  
        MCrs_dv         <=0;
    else if(Line_loop_en)
        MCrs_dv         <=Tx_en;
    else if(Rx_dv_dl2)
        MCrs_dv         <=1;
    else
        MCrs_dv         <=0;

always @ (posedge MAC_rx_clk or posedge Reset)
    if (Reset)   
        Rx_odd_data_ptr     <=0;
    else if (!Rx_dv_dl1)
        Rx_odd_data_ptr     <=0;
    else 
        Rx_odd_data_ptr     <=!Rx_odd_data_ptr;
        
always @ (posedge MAC_rx_clk or posedge Reset)
    if (Reset)  
        MRxD            <=0;
    else if(Line_loop_en)
        MRxD            <=Txd;
    else if(Speed[2]&&Rx_dv_dl2)
        MRxD            <=Rxd_dl2;
    else if(Rx_dv_dl1&&Rx_odd_data_ptr)
        MRxD            <={Rxd_dl1[3:0],Rxd_dl2[3:0]};
    
endmodule           