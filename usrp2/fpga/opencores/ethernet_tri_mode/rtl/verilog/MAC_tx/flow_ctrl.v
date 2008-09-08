//////////////////////////////////////////////////////////////////////
////                                                              ////
////  flow_ctrl.v                                                 ////
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
// $Log: flow_ctrl.v,v $
// Revision 1.3  2006/01/19 14:07:54  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:19  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//                                           

module flow_ctrl 
(
Reset               ,
Clk                 ,
//host processor    ,
tx_pause_en         ,
xoff_cpu            ,
xon_cpu             ,
//MAC_rx_flow       ,
pause_quanta        ,
pause_quanta_val    ,
//MAC_tx_ctrl       ,
pause_apply         ,
pause_quanta_sub    ,
xoff_gen            ,
xoff_gen_complete   ,
xon_gen             ,
xon_gen_complete    

);

input           Reset               ;
input           Clk                 ;
                //host processor    ;
input           tx_pause_en         ;
input           xoff_cpu            ;
input           xon_cpu             ;
                //MAC_rx_flow       ;
input   [15:0]  pause_quanta        ;
input           pause_quanta_val    ;
                //MAC_tx_ctrl       ;
output          pause_apply         ;
input           pause_quanta_sub    ;
output          xoff_gen            ;
input           xoff_gen_complete   ;
output          xon_gen             ;
input           xon_gen_complete    ;
  
//******************************************************************************
//internal signals                                                              
//******************************************************************************  
reg             xoff_cpu_dl1            ;
reg             xoff_cpu_dl2            ;
reg             xon_cpu_dl1             ;   
reg             xon_cpu_dl2             ;
reg [15:0]      pause_quanta_dl1        ;
reg             pause_quanta_val_dl1    ;
reg             pause_quanta_val_dl2    ;       
reg             pause_apply             ;
reg             xoff_gen                ;
reg             xon_gen                 ;
reg [15:0]      pause_quanta_counter    ;
reg             tx_pause_en_dl1         ;
reg             tx_pause_en_dl2         ;
//******************************************************************************  
//boundery signal processing                                                               
//******************************************************************************
always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin
        xoff_cpu_dl1        <=0;
        xoff_cpu_dl2        <=0;
        end
    else
        begin
        xoff_cpu_dl1        <=xoff_cpu;
        xoff_cpu_dl2        <=xoff_cpu_dl1;
        end

always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin
        xon_cpu_dl1     <=0;
        xon_cpu_dl2     <=0;
        end
    else
        begin
        xon_cpu_dl1     <=xon_cpu;
        xon_cpu_dl2     <=xon_cpu_dl1;
        end     
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin
        pause_quanta_dl1        <=0;
        end
    else
        begin
        pause_quanta_dl1        <=pause_quanta;
        end  
       
always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin
        pause_quanta_val_dl1    <=0;
        pause_quanta_val_dl2    <=0;
        end
    else
        begin
        pause_quanta_val_dl1    <=pause_quanta_val;
        pause_quanta_val_dl2    <=pause_quanta_val_dl1;
        end                      
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin           
        tx_pause_en_dl1     <=0;
        tx_pause_en_dl2     <=0; 
        end
    else
        begin           
        tx_pause_en_dl1     <=tx_pause_en;
        tx_pause_en_dl2     <=tx_pause_en_dl1; 
        end     

//******************************************************************************        
//gen output signals                                                            
//******************************************************************************
always @ (posedge Clk or posedge Reset)
    if (Reset)
        xoff_gen        <=0;
    else if (xoff_gen_complete)
        xoff_gen        <=0;
    else if (xoff_cpu_dl1&&!xoff_cpu_dl2)
        xoff_gen        <=1;

always @ (posedge Clk or posedge Reset)
    if (Reset)
        xon_gen     <=0;
    else if (xon_gen_complete)
        xon_gen     <=0;
    else if (xon_cpu_dl1&&!xon_cpu_dl2)
        xon_gen     <=1;                     

always @ (posedge Clk or posedge Reset)
    if (Reset)
        pause_quanta_counter    <=0;
    else if(pause_quanta_val_dl1&&!pause_quanta_val_dl2)
        pause_quanta_counter    <=pause_quanta_dl1; 
    else if(pause_quanta_sub&&pause_quanta_counter!=0)
        pause_quanta_counter    <=pause_quanta_counter-1;
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        pause_apply     <=0;
    else if(pause_quanta_counter==0)
        pause_apply     <=0;
    else if (tx_pause_en_dl2)
        pause_apply     <=1;
        
endmodule