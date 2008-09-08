//////////////////////////////////////////////////////////////////////
////                                                              ////
////  MAC_rx_add_chk.v                                            ////
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
// $Log: MAC_rx_add_chk.v,v $
// Revision 1.3  2006/01/19 14:07:54  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:17  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//                                           

module MAC_rx_add_chk (     
Reset               ,                                
Clk                 ,                                
Init                ,                                
data                ,                                
MAC_add_en          ,                                
MAC_rx_add_chk_err  ,                                
//From CPU                                         
MAC_rx_add_chk_en   ,                                
MAC_add_prom_data   ,       
MAC_add_prom_add    ,       
MAC_add_prom_wr             

);
input           Reset               ;
input           Clk                 ;
input           Init                ;
input   [7:0]   data                ;
input           MAC_add_en          ;
output          MAC_rx_add_chk_err  ;
                //From CPU
input           MAC_rx_add_chk_en   ;   
input   [7:0]   MAC_add_prom_data   ;   
input   [2:0]   MAC_add_prom_add    ;   
input           MAC_add_prom_wr     ;   

//******************************************************************************   
//internal signals                                                              
//******************************************************************************
reg [2:0]   addr_rd;
wire[2:0]   addr_wr;
wire[7:0]   din;
wire[7:0]   dout;
wire        wr_en;

reg         MAC_rx_add_chk_err;
reg         MAC_add_prom_wr_dl1;
reg         MAC_add_prom_wr_dl2;
reg [7:0]   data_dl1                ;
reg         MAC_add_en_dl1          ;
//******************************************************************************   
//write data from cpu to prom                                                              
//******************************************************************************
always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin
        data_dl1            <=0;
        MAC_add_en_dl1      <=0;
        end
    else
        begin
        data_dl1            <=data;
        MAC_add_en_dl1      <=MAC_add_en;
        end        

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
         
assign wr_en      =MAC_add_prom_wr_dl1&!MAC_add_prom_wr_dl2;
assign addr_wr    =MAC_add_prom_add;
assign din        =MAC_add_prom_data;

//******************************************************************************   
//mac add verify                                                             
//******************************************************************************
always @ (posedge Clk or posedge Reset)
    if (Reset)
        addr_rd       <=0;
    else if (Init)
        addr_rd       <=0;
    else if (MAC_add_en)
        addr_rd       <=addr_rd + 1;
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        MAC_rx_add_chk_err  <=0;
    else if (Init)
        MAC_rx_add_chk_err  <=0;
    else if (MAC_rx_add_chk_en&&MAC_add_en_dl1&&dout!=data_dl1)
        MAC_rx_add_chk_err  <=1;
        

//******************************************************************************   
//a port for read ,b port for write .
//******************************************************************************     
duram #(8,3,"M512","DUAL_PORT") U_duram(
.data_a         (din       ),
.wren_a         (wr_en        ),
.address_a      (addr_wr      ),
.address_b      (addr_rd      ),
.clock_a        (Clk        ),
.clock_b        (Clk        ),
.q_b            (dout      ));

endmodule
