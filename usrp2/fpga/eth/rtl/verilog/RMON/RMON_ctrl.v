//////////////////////////////////////////////////////////////////////
////                                                              ////
////  RMON_ctrl.v                                                 ////
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
// $Log: RMON_ctrl.v,v $
// Revision 1.4  2006/06/25 04:58:57  maverickist
// no message
//
// Revision 1.3  2006/01/19 14:07:55  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:19  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//  
module RMON_ctrl (
Clk             ,      
Reset           ,      
//RMON_ctrl        
Reg_apply_0     ,      
Reg_addr_0      ,      
Reg_data_0      ,      
Reg_next_0      ,      
Reg_apply_1     ,      
Reg_addr_1      ,      
Reg_data_1      ,      
Reg_next_1      ,      
//dual-port ram
Addra               ,  
Dina                ,  
Douta               ,  
Wea                 ,  
//CPU                  
CPU_rd_addr     ,  
CPU_rd_apply        ,  
CPU_rd_grant        ,
CPU_rd_dout

);
input           Clk             ;
input           Reset           ;
                //RMON_ctrl
input           Reg_apply_0     ;
input   [4:0]   Reg_addr_0      ;
input   [15:0]  Reg_data_0      ;
output          Reg_next_0      ;
input           Reg_apply_1     ;
input   [4:0]   Reg_addr_1      ;
input   [15:0]  Reg_data_1      ;
output          Reg_next_1      ;
                //dual-port ram 
                //port-a for Rmon  
output  [5:0]   Addra               ;
output  [31:0]  Dina                ;
input   [31:0]  Douta               ;
output          Wea                 ;
                //CPU
input   [5:0]   CPU_rd_addr         ;
input           CPU_rd_apply        ;
output          CPU_rd_grant        ;
output  [31:0]  CPU_rd_dout         ;




//******************************************************************************
//internal signals                                                              
//******************************************************************************

parameter       StateCPU        =4'd00;
parameter       StateMAC0       =4'd01;
parameter       StateMAC1       =4'd02;


reg [3:0]       CurrentState /* synthesys syn_keep=1 */;
reg [3:0]       NextState;
reg [3:0]       CurrentState_reg;

reg [4:0]       StepCounter;
reg [5:0]       Addra               ;
reg [31:0]      Dina;
reg             Reg_next_0      ;
reg             Reg_next_1      ;
reg             Write;
reg             Read;
reg             Pipeline;
reg [31:0]      CPU_rd_dout         ;
reg             CPU_rd_apply_reg    ;
//******************************************************************************
//State Machine                                                            
//******************************************************************************

always @(posedge Clk or posedge Reset)
    if (Reset)
        CurrentState    <=StateMAC0;
    else
        CurrentState    <=NextState;
        
always @(posedge Clk or posedge Reset)
    if (Reset)  
        CurrentState_reg    <=StateMAC0;
    else if(CurrentState!=StateCPU)
        CurrentState_reg    <=CurrentState;
                
always @(CurrentState or CPU_rd_apply_reg or Reg_apply_0 or CurrentState_reg
                                       or Reg_apply_1   
                                       or StepCounter
                                       )
    case(CurrentState)
        StateMAC0:
            if(!Reg_apply_0&&CPU_rd_apply_reg)
                NextState   =StateCPU;
            else if(!Reg_apply_0)
                NextState   =StateMAC1;
            else
                NextState   =CurrentState;
        StateMAC1:
            if(!Reg_apply_1&&CPU_rd_apply_reg)
                NextState   =StateCPU;
            else if(!Reg_apply_1)
                NextState   =StateMAC0;
            else
                NextState   =CurrentState;
        StateCPU:
            if (StepCounter==3)
                case (CurrentState_reg)
                    StateMAC0   :NextState  =StateMAC0 ;
                    StateMAC1   :NextState  =StateMAC1 ;
                    default     :NextState  =StateMAC0;
                endcase
            else
                NextState   =CurrentState;
            
        default:
                NextState   =StateMAC0;
    endcase
                


always @(posedge Clk or posedge Reset)
    if (Reset)
        StepCounter     <=0;
    else if(NextState!=CurrentState)
        StepCounter     <=0;
    else if (StepCounter!=4'hf)
        StepCounter     <=StepCounter + 1;

//******************************************************************************
//temp signals                                                            
//******************************************************************************
always @(StepCounter)
    if( StepCounter==1||StepCounter==4||
        StepCounter==7||StepCounter==10)
        Read    =1;
    else
        Read    =0;

always @(StepCounter or CurrentState)
    if( StepCounter==2||StepCounter==5||
        StepCounter==8||StepCounter==11)
        Pipeline    =1;
    else
        Pipeline    =0;
                
always @(StepCounter or CurrentState)
    if( StepCounter==3||StepCounter==6||
        StepCounter==9||StepCounter==12)
        Write   =1;
    else
        Write   =0;
        
        
//******************************************************************************
//gen output signals                                                        
//******************************************************************************    
//Addra 
always @(*)
    case(CurrentState)
        StateMAC0 :     Addra={1'd0 ,Reg_addr_0 };
        StateMAC1 :     Addra={1'd1 ,Reg_addr_1 };
        StateCPU:       Addra=CPU_rd_addr;
        default:        Addra=0;
        endcase
    
//Dina
always @(posedge Clk or posedge Reset)
    if (Reset)
        Dina    <=0;
    else 
        case(CurrentState)
            StateMAC0 :     Dina<=Douta+Reg_data_0 ;
            StateMAC1 :     Dina<=Douta+Reg_data_1 ;
            StateCPU:       Dina<=0;
            default:        Dina<=0;
        endcase
    
assign  Wea     =Write;
//Reg_next
always @(CurrentState or Pipeline)
    if(CurrentState==StateMAC0)
        Reg_next_0  =Pipeline;
    else
        Reg_next_0  =0;
    
always @(CurrentState or Pipeline)
    if(CurrentState==StateMAC1)
        Reg_next_1  =Pipeline;
    else
        Reg_next_1  =0;     


//CPU_rd_grant   
reg     CPU_rd_apply_dl1;
reg     CPU_rd_apply_dl2;
//rising edge
always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin
        CPU_rd_apply_dl1        <=0;
        CPU_rd_apply_dl2        <=0;
        end
    else
        begin
        CPU_rd_apply_dl1        <=CPU_rd_apply;
        CPU_rd_apply_dl2        <=CPU_rd_apply_dl1;
        end     

always @ (posedge Clk or posedge Reset)
    if (Reset)
        CPU_rd_apply_reg    <=0;
    else if (CPU_rd_apply_dl1&!CPU_rd_apply_dl2)
        CPU_rd_apply_reg    <=1;
    else if (CurrentState==StateCPU&&Write)
        CPU_rd_apply_reg    <=0;

assign CPU_rd_grant = CPU_rd_apply & CPU_rd_apply_dl1 & CPU_rd_apply_dl2 & !CPU_rd_apply_reg;

always @ (posedge Clk or posedge Reset)
    if (Reset)
        CPU_rd_dout     <=0;
    else if (Pipeline&&CurrentState==StateCPU)
        CPU_rd_dout     <=Douta;        

endmodule           
