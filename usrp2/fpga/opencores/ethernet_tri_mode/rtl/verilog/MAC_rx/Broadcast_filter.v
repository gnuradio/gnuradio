//////////////////////////////////////////////////////////////////////
////                                                              ////
////  Broadcast_filter.v                                          ////
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
// $Log: Broadcast_filter.v,v $
// Revision 1.3  2006/01/19 14:07:54  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:16  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//    

module Broadcast_filter (    
Reset                   ,
Clk                     ,
//MAC_rx_ctrl           ,
broadcast_ptr           ,
broadcast_drop          ,
//FromCPU               ,
broadcast_filter_en     ,
broadcast_bucket_depth    ,
broadcast_bucket_interval 
);
input           Reset                       ;
input           Clk                         ;
                //MAC_rx_ctrl               
input           broadcast_ptr               ;
output          broadcast_drop              ;
                //FromCPU                   ;
input           broadcast_filter_en         ;
input   [15:0]  broadcast_bucket_depth      ;
input   [15:0]  broadcast_bucket_interval   ;

//******************************************************************************  
//internal signals                                                                
//******************************************************************************  
reg     [15:0]  time_counter            ;
reg     [15:0]  broadcast_counter        ;
reg             broadcast_drop          ;
//******************************************************************************  
//                                                               
//****************************************************************************** 
always @ (posedge Clk or posedge Reset)
    if (Reset)
        time_counter    <=0;
    else if (time_counter==broadcast_bucket_interval)
        time_counter    <=0;
    else
        time_counter    <=time_counter+1;

always @ (posedge Clk or posedge Reset)
    if (Reset)
        broadcast_counter   <=0;
    else if (time_counter==broadcast_bucket_interval)
        broadcast_counter   <=0;
    else if (broadcast_ptr&&broadcast_counter!=broadcast_bucket_depth)
        broadcast_counter   <=broadcast_counter+1;
                
always @ (posedge Clk or posedge Reset)
    if (Reset)
        broadcast_drop      <=0;
    else if(broadcast_filter_en&&broadcast_counter==broadcast_bucket_depth)
        broadcast_drop      <=1;
    else
        broadcast_drop      <=0;

endmodule