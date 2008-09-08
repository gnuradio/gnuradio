//////////////////////////////////////////////////////////////////////
////                                                              ////
////  MAC_tx_addr_add.v                                           ////
////                                                              ////
////  This file is part of the Ethernet IP core project           ////
////  http://www.opencores.org/projects.cgi/wr_en/ethernet_tri_mode/////
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
// $Log: MAC_tx_addr_add.v,v $
// Revision 1.3  2006/01/19 14:07:54  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:18  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//                                           

module MAC_tx_addr_add ( 
Reset               ,
Clk                 ,
MAC_tx_addr_init    ,
MAC_tx_addr_rd      ,
MAC_tx_addr_data    ,
//CPU               ,
MAC_add_prom_data   ,
MAC_add_prom_add    ,
MAC_add_prom_wr     
);

input           Reset               ;
input           Clk                 ;
input           MAC_tx_addr_rd      ;
input           MAC_tx_addr_init    ;
output  [7:0]   MAC_tx_addr_data    ;
                //CPU               ;
input   [7:0]   MAC_add_prom_data   ;
input   [2:0]   MAC_add_prom_add    ;
input           MAC_add_prom_wr     ;

//******************************************************************************   
//internal signals                                                              
//******************************************************************************
reg [2:0]       add_rd;
wire[2:0]       add_wr;
wire[7:0]       din;
wire[7:0]       dout;
wire            wr_en;
reg             MAC_add_prom_wr_dl1;
reg             MAC_add_prom_wr_dl2;
//******************************************************************************   
//write data from cpu to prom                                                              
//******************************************************************************
always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin
        MAC_add_prom_wr_dl1     <=0;
        MAC_add_prom_wr_dl2     <=0;
        end
    else
        begin
        MAC_add_prom_wr_dl1     <=MAC_add_prom_wr;
        MAC_add_prom_wr_dl2     <=MAC_add_prom_wr_dl1;
        end     
assign # 2 wr_en   =MAC_add_prom_wr_dl1&!MAC_add_prom_wr_dl2;
assign # 2 add_wr  =MAC_add_prom_add;
assign # 2 din     =MAC_add_prom_data;

//******************************************************************************   
//read data from cpu to prom                                                              
//******************************************************************************
always @ (posedge Clk or posedge Reset)
    if (Reset)
        add_rd       <=0;
    else if (MAC_tx_addr_init)
        add_rd       <=0;
    else if (MAC_tx_addr_rd)
        add_rd       <=add_rd + 1;
assign MAC_tx_addr_data=dout;      
//******************************************************************************   
//b port for read ,a port for write .
//******************************************************************************
duram #(8,3,"M512","DUAL_PORT") U_duram(           
.data_a         (din            ), 
.wren_a         (wr_en          ), 
.address_a      (add_wr         ), 
.address_b      (add_rd         ), 
.clock_a        (Clk            ), 
.clock_b        (Clk            ), 
.q_b            (dout           ));  
      

endmodule                        

