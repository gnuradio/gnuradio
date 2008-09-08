//////////////////////////////////////////////////////////////////////
////                                                              ////
////  RMON_addr_gen.v                                             ////
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
// $Log: RMON_addr_gen.v,v $
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
module RMON_addr_gen(
Clk                 ,                              
Reset               ,                              
//RMON                                             
Pkt_type_rmon       ,                              
Pkt_length_rmon     ,                              
Apply_rmon          ,//pluse signal looks like eop 
Pkt_err_type_rmon   ,                              
//                                                 
Reg_apply           ,                              
Reg_addr            ,                              
Reg_data            ,                              
Reg_next            ,                              
//CPU                                              
Reg_drop_apply                                
);
input           Clk                 ;
input           Reset               ;
                //RMON
input   [2:0]   Pkt_type_rmon       ;
input   [15:0]  Pkt_length_rmon     ;
input           Apply_rmon          ;//pluse signal looks like eop
input   [2:0]   Pkt_err_type_rmon   ;
                //RMON_ctrl
output          Reg_apply           ;
output  [4:0]   Reg_addr            ;
output  [15:0]  Reg_data            ;
input           Reg_next            ;
                //CPU
output          Reg_drop_apply      ;

//******************************************************************************
//internal signals                                                              
//******************************************************************************
parameter       StateIdle       =4'd0;
parameter       StatePktLength  =4'd1;
parameter       StatePktNumber  =4'd2;
parameter       StatePktType    =4'd3;
parameter       StatePktRange   =4'd4;

reg [3:0]       CurrentState /* synthesys syn_keep=1 */;
reg [3:0]       NextState;
    
reg [2:0]       PktTypeReg      ;
reg [15:0]      PktLengthReg    ;
reg [2:0]       PktErrTypeReg   ;
    
reg             Reg_apply       ;
reg [4:0]       Reg_addr            ;
reg [15:0]      Reg_data            ;
reg             Reg_drop_apply  ;
//******************************************************************************
//register boundery signals    
                                                    
//******************************************************************************
reg             Apply_rmon_dl1;
reg             Apply_rmon_dl2;
reg             Apply_rmon_pulse;
reg [2:0]       Pkt_type_rmon_dl1       ;
reg [15:0]      Pkt_length_rmon_dl1     ;
reg [2:0]       Pkt_err_type_rmon_dl1   ;

always @(posedge Clk or posedge Reset)
    if (Reset)
        begin
        Pkt_type_rmon_dl1       <=0;
        Pkt_length_rmon_dl1     <=0;
        Pkt_err_type_rmon_dl1   <=0;
        end              
    else
        begin
        Pkt_type_rmon_dl1       <=Pkt_type_rmon     ;   
        Pkt_length_rmon_dl1     <=Pkt_length_rmon   ;
        Pkt_err_type_rmon_dl1   <=Pkt_err_type_rmon ;
        end 

always @(posedge Clk or posedge Reset)
    if (Reset)
        begin
        Apply_rmon_dl1  <=0;
        Apply_rmon_dl2  <=0;
        end
    else
        begin
        Apply_rmon_dl1  <=Apply_rmon;
        Apply_rmon_dl2  <=Apply_rmon_dl1;
        end 
    
always @(Apply_rmon_dl1 or Apply_rmon_dl2)
    if (Apply_rmon_dl1&!Apply_rmon_dl2)
        Apply_rmon_pulse    =1;
    else
        Apply_rmon_pulse    =0;


    
always @(posedge Clk or posedge Reset)
    if (Reset)
        begin
        PktTypeReg          <=0;
        PktLengthReg        <=0;
        PktErrTypeReg       <=0;    
        end
    else if (Apply_rmon_pulse&&CurrentState==StateIdle)
        begin
        PktTypeReg          <=Pkt_type_rmon_dl1     ;
        PktLengthReg        <=Pkt_length_rmon_dl1   ;
        PktErrTypeReg       <=Pkt_err_type_rmon_dl1 ;    
        end     
        

//******************************************************************************
//State Machine                                                             
//******************************************************************************
always @(posedge Clk or posedge Reset)
    if (Reset)
        CurrentState    <=StateIdle;
    else
        CurrentState    <=NextState;
        
always @(CurrentState or Apply_rmon_pulse or Reg_next)
    case (CurrentState)
        StateIdle:
            if (Apply_rmon_pulse)
                NextState   =StatePktLength;
            else
                NextState   =StateIdle;
        StatePktLength:
            if (Reg_next)
                NextState   =StatePktNumber;
            else
                NextState   =CurrentState;
        StatePktNumber:
            if (Reg_next)
                NextState   =StatePktType;
            else
                NextState   =CurrentState;
        StatePktType:
            if (Reg_next)
                NextState   =StatePktRange;
            else
                NextState   =CurrentState;
        StatePktRange:
            if (Reg_next)
                NextState   =StateIdle;
            else
                NextState   =CurrentState;
        default:
                NextState   =StateIdle;
    endcase 
            
//******************************************************************************
//gen output signals                                                            
//******************************************************************************
//Reg_apply
always @ (CurrentState)
    if (CurrentState==StatePktLength||CurrentState==StatePktNumber||
        CurrentState==StatePktType||CurrentState==StatePktRange)
        Reg_apply   =1;
    else
        Reg_apply   =0;
        
//Reg_addr
always @ (posedge Clk or posedge Reset)
    if (Reset)
        Reg_addr    <=0;
    else case (CurrentState)
            StatePktLength:
                Reg_addr    <=5'd00;
            StatePktNumber:
                Reg_addr    <=5'd01;
            StatePktType:
                case(PktTypeReg)
                    3'b011:
                        Reg_addr    <=5'd02;    //broadcast
                    3'b001:
                        Reg_addr    <=5'd03;    //multicast 
                    3'b100:
                        Reg_addr    <=5'd16;    //pause frame   
                    default:
                        Reg_addr    <=5'd04;    //unicast
                endcase
            StatePktRange:
                case(PktErrTypeReg)
                    3'b001:
                        Reg_addr    <=5'd05; 
                    3'b010:
                        Reg_addr    <=5'd06;    
                    3'b011:
                        Reg_addr    <=5'd07;    
                    3'b100:
                        if (PktLengthReg<64)    
                            Reg_addr    <=5'd08; 
                        else if (PktLengthReg==64)
                            Reg_addr    <=5'd09; 
                        else if (PktLengthReg<128)
                            Reg_addr    <=5'd10; 
                        else if (PktLengthReg<256)
                            Reg_addr    <=5'd11; 
                        else if (PktLengthReg<512)
                            Reg_addr    <=5'd12; 
                        else if (PktLengthReg<1024)
                            Reg_addr    <=5'd13; 
                        else if (PktLengthReg<1519)
                            Reg_addr    <=5'd14; 
                        else
                            Reg_addr    <=5'd15; 
                    default:
                        Reg_addr    <=5'd05;
                endcase
            default:
                    Reg_addr    <=5'd05;
        endcase
                
//Reg_data
always @ (CurrentState or PktLengthReg)
    case (CurrentState)
        StatePktLength:
            Reg_data    =PktLengthReg;
        StatePktNumber:
            Reg_data    =1;
        StatePktType:
            Reg_data =1;
        StatePktRange:
            Reg_data =1;
        default:
            Reg_data =0;
    endcase
    
//Reg_drop_apply
always @ (posedge Clk or posedge Reset)
    if (Reset)
        Reg_drop_apply  <=0;
    else if (CurrentState!=StateIdle&&Apply_rmon_pulse)
        Reg_drop_apply  <=1;
    else
        Reg_drop_apply  <=0;
                

endmodule               
            
