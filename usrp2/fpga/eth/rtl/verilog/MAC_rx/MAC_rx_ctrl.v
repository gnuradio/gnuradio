//////////////////////////////////////////////////////////////////////
////                                                              ////
////  MAC_rx_ctrl.v                                               ////
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
// $Log: MAC_rx_ctrl.v,v $
// Revision 1.4  2006/06/25 04:58:56  maverickist
// no message
//
// Revision 1.3  2006/01/19 14:07:54  maverickist
// verification is complete.
//
// Revision 1.3  2005/12/16 06:44:17  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.2  2005/12/13 12:15:37  Administrator
// no message
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//                                           

module MAC_rx_ctrl (
Reset   ,                                     
Clk     ,                                     
//RMII interface                                    
MCrs_dv ,       //
MRxD    ,       //  
MRxErr  ,       //  
//CRC_chk interface                                 
CRC_en    ,                                 
CRC_data,
CRC_init  ,
CRC_err  ,                      
//MAC_rx_add_chk interface                          
MAC_add_en          ,                         
MAC_add_data,
MAC_rx_add_chk_err  ,   
//broadcast_filter     
broadcast_ptr   ,      
broadcast_drop  ,   
//flow_control signals      
pause_quanta        ,       
pause_quanta_val    ,       
//MAC_rx_FF interface                               
Fifo_data       ,                             
Fifo_data_en    ,                             
Fifo_data_err   ,  
Fifo_data_drop  ,
Fifo_data_end   ,  
Fifo_full       ,                          
//RMON interface                
Rx_pkt_type_rmon        ,                    
Rx_pkt_length_rmon      ,                     
Rx_apply_rmon           ,                     
Rx_pkt_err_type_rmon    ,                     
//CPU                                         
RX_IFG_SET    ,
RX_MAX_LENGTH,
RX_MIN_LENGTH
);

input           Reset   ;         
input           Clk     ;         
                //RMII interface  
input           MCrs_dv ;                                       
input   [7:0]   MRxD    ;                                       
input           MRxErr  ;                                       
                //CRC_chk interface
output          CRC_en  ;       
output          CRC_init;       
output [7:0]    CRC_data;
input           CRC_err ;
                //MAC_rx_add_chk interface
output          MAC_add_en          ;
output [7:0]    MAC_add_data;
input           MAC_rx_add_chk_err  ;
                //broadcast_filter
output          broadcast_ptr           ;
input           broadcast_drop          ;
                //flow_control signals  
output  [15:0]  pause_quanta        ;   
output          pause_quanta_val    ;   
                //MAC_rx_FF interface
output  [7:0]   Fifo_data       ;
output          Fifo_data_en    ;
output          Fifo_data_err   ;
output          Fifo_data_drop  ;
output          Fifo_data_end   ;
input           Fifo_full;
                //RMON interface
output  [15:0]  Rx_pkt_length_rmon      ;
output          Rx_apply_rmon           ;
output  [2:0]   Rx_pkt_err_type_rmon    ;
output  [2:0]   Rx_pkt_type_rmon        ;
                //CPU
input   [5:0]   RX_IFG_SET    ;
input   [15:0]  RX_MAX_LENGTH   ;// 1518
input   [6:0]   RX_MIN_LENGTH   ;// 64

//******************************************************************************
//internal signals
//******************************************************************************
parameter       State_idle          =4'd00;
parameter       State_preamble      =4'd01;
parameter       State_SFD           =4'd02;
parameter       State_data          =4'd03;
parameter       State_checkCRC      =4'd04;
parameter       State_OkEnd         =4'd07;
parameter       State_DropEnd       =4'd08;
parameter       State_ErrEnd        =4'd09;
parameter       State_CRCErrEnd     =4'd10;
parameter       State_FFFullDrop    =4'd11;
parameter       State_FFFullErrEnd  =4'd12;
parameter       State_IFG           =4'd13;
parameter       State_Drop2End      =4'd14;

parameter       Pause_idle          =4'd0;   
parameter       Pause_pre_syn       =4'd1;    
parameter       Pause_quanta_hi     =4'd2;   
parameter       Pause_quanta_lo     =4'd3;   
parameter       Pause_syn           =4'd4;   
                                                  
reg [3:0]       Current_state /* synthesis syn_keep=1 */;                          
reg [3:0]       Next_state; 
reg [3:0]       Pause_current /* synthesis syn_keep=1 */; 
reg [3:0]       Pause_next;                             
reg [5:0]       IFG_counter;   
reg             Crs_dv  ;      
reg [7:0]       RxD ;
reg [7:0]       RxD_dl1 ;
reg             RxErr   ;
reg [15:0]      Frame_length_counter;
reg             Too_long;
reg             Too_short;
reg             ProcessingHeader;
//reg             Fifo_data_en;
//reg             Fifo_data_err;
//reg             Fifo_data_drop;
//reg             Fifo_data_end;
reg             CRC_en;
reg             CRC_init;
reg             Rx_apply_rmon;
reg [2:0]       Rx_pkt_err_type_rmon;
reg             MAC_add_en;
reg [2:0]       Rx_pkt_type_rmon;
reg [7:0]       pause_quanta_h      ;
reg [15:0]      pause_quanta        ;
reg             pause_quanta_val    ;
reg             pause_quanta_val_tmp;
reg             pause_frame_ptr     ;
reg             broadcast_ptr           ;
//******************************************************************************
//delay signals                                                          
//******************************************************************************
    
always @ (posedge Reset or posedge Clk)                 
    if (Reset) 
        begin  
            Crs_dv      <=0;
            RxD         <=0;                                                            
            RxErr       <=0; 
        end
    else
        begin  
            Crs_dv      <=MCrs_dv   ;
            RxD         <=MRxD      ;                                                            
            RxErr       <=MRxErr    ; 
        end

always @ (posedge Reset or posedge Clk)                 
    if (Reset) 
        RxD_dl1     <=0;
    else 
        RxD_dl1     <=RxD;
                                   
//---------------------------------------------------------------------------
// Small pre-FIFO (acutally a synchronously clearable shift-register) for
// storing the first part of a packet before writing it to the "real" FIFO
// in MAC_rx_FF. This allows a packet to be dropped safely if an error
// happens in the beginning of a packet (or if the MAC address doesn't pass
// the receive filter!)
//---------------------------------------------------------------------------

  reg           pre_fifo_data_drop;
  reg           pre_fifo_data_en;
  reg           pre_fifo_data_err;
  reg           pre_fifo_data_end;
  wire [7:0]    pre_fifo_wrdata;

  reg [8+3-1:0] pre_fifo_element_0;
  reg [8+3-1:0] pre_fifo_element_1;
  reg [8+3-1:0] pre_fifo_element_2;
  reg [8+3-1:0] pre_fifo_element_3;
  reg [8+3-1:0] pre_fifo_element_4;
  reg [8+3-1:0] pre_fifo_element_5;
  reg [8+3-1:0] pre_fifo_element_6;
  reg [8+3-1:0] pre_fifo_element_7;
  reg [8+3-1:0] pre_fifo_element_8;
  reg [8+3-1:0] pre_fifo_element_9;

  always @( posedge Reset or posedge Clk )
    if ( Reset )
      begin
        pre_fifo_element_0 <= 'b0;
        pre_fifo_element_1 <= 'b0;
        pre_fifo_element_2 <= 'b0;
        pre_fifo_element_3 <= 'b0;
        pre_fifo_element_4 <= 'b0;
        pre_fifo_element_5 <= 'b0;
        pre_fifo_element_6 <= 'b0;
        pre_fifo_element_7 <= 'b0;
        pre_fifo_element_8 <= 'b0;
        pre_fifo_element_9 <= 'b0;
      end
    else
      begin
        if ( pre_fifo_data_drop )
          begin
            pre_fifo_element_0 <= 'b0;
            pre_fifo_element_1 <= 'b0;
            pre_fifo_element_2 <= 'b0;
            pre_fifo_element_3 <= 'b0;
            pre_fifo_element_4 <= 'b0;
            pre_fifo_element_5 <= 'b0;
            pre_fifo_element_6 <= 'b0;
            pre_fifo_element_7 <= 'b0;
            pre_fifo_element_8 <= 'b0;
            pre_fifo_element_9 <= 'b0;
          end
        else
          begin
            pre_fifo_element_0 <= pre_fifo_element_1;
            pre_fifo_element_1 <= pre_fifo_element_2;
            pre_fifo_element_2 <= pre_fifo_element_3;
            pre_fifo_element_3 <= pre_fifo_element_4;
            pre_fifo_element_4 <= pre_fifo_element_5;
            pre_fifo_element_5 <= pre_fifo_element_6;
            pre_fifo_element_6 <= pre_fifo_element_7;
            pre_fifo_element_7 <= pre_fifo_element_8;
            pre_fifo_element_8 <= pre_fifo_element_9;
            pre_fifo_element_9 <= { pre_fifo_data_en,
                                    pre_fifo_data_err,
                                    pre_fifo_data_end,
                                    pre_fifo_wrdata };
          end
      end

  assign Fifo_data     = pre_fifo_element_0[7:0];
  assign Fifo_data_end = pre_fifo_element_0[8];
  assign Fifo_data_err = pre_fifo_element_0[9];
  assign Fifo_data_en  = pre_fifo_element_0[10];

  assign CRC_data     = pre_fifo_wrdata;
  assign MAC_add_data = pre_fifo_wrdata;

//******************************************************************************
//State_machine                                                           
//******************************************************************************
                                                    
always @( posedge Reset or posedge Clk )
  if ( Reset )
    Current_state <= State_idle;
  else
    Current_state <= Next_state;

always @ (*)
  case (Current_state)
    State_idle:
      if ( Crs_dv&&RxD==8'h55 )
        Next_state = State_preamble;
      else
        Next_state = Current_state;

    State_preamble:
      if ( !Crs_dv )
        Next_state = State_DropEnd;
      else if ( RxErr )
        Next_state = State_DropEnd;
      else if ( RxD==8'hd5 )
        Next_state = State_SFD;
      else if ( RxD==8'h55 )
        Next_state =Current_state;
      else
        Next_state = State_DropEnd;

    State_SFD:
      if ( !Crs_dv )
        Next_state = State_DropEnd;
      else if ( RxErr )
        Next_state = State_DropEnd;
      else
        Next_state = State_data;

    State_data:
      if ( !Crs_dv && !ProcessingHeader && !Too_short && !Too_long )
        Next_state = State_checkCRC;
      else if ( !Crs_dv && ProcessingHeader )
        Next_state = State_Drop2End;
      else if ( !Crs_dv && (Too_short | Too_long) )
        Next_state = State_ErrEnd;
       else if ( Fifo_full )
         Next_state = State_FFFullErrEnd;
       else if ( RxErr && ProcessingHeader )
         Next_state = State_Drop2End;
       else if ( RxErr || Too_long )
         Next_state = State_ErrEnd;
       else if ( MAC_rx_add_chk_err || broadcast_drop )
         Next_state = State_DropEnd;
       else
         Next_state = State_data;

    State_checkCRC:
      if ( CRC_err )
        Next_state = State_CRCErrEnd;
      else
        Next_state = State_OkEnd;

    State_OkEnd:
      Next_state = State_IFG;

    State_ErrEnd:
      Next_state = State_IFG;

    State_DropEnd:
      Next_state = State_IFG;

    State_Drop2End:
      Next_state = State_IFG;

    State_CRCErrEnd:
      Next_state = State_IFG;

    State_FFFullErrEnd:
      Next_state = State_FFFullDrop;

    State_FFFullDrop:
      if ( !Crs_dv )
        Next_state  =State_IFG;     
      else                                
        Next_state  =Current_state;                                                

    State_IFG:
      if ( IFG_counter==RX_IFG_SET-4 ) // Remove some additional time?
        Next_state = State_idle;
      else
        Next_state = Current_state;

    default:
      Next_state = State_idle;
  endcase

always @( posedge Reset or posedge Clk )
  if ( Reset )
    IFG_counter <= 0;
  else if ( Current_state!=State_IFG )
    IFG_counter <= 0;
  else
    IFG_counter <= IFG_counter + 1;

//******************************************************************************
//gen fifo interface signals                                                     
//******************************************************************************                     

assign pre_fifo_wrdata = RxD_dl1;

always @( Current_state )
  if ( Current_state==State_data )
    pre_fifo_data_en = 1;
  else
    pre_fifo_data_en = 0;
        
always @( Current_state )
  if ( (Current_state==State_ErrEnd      ) ||
       (Current_state==State_OkEnd       ) ||
       (Current_state==State_CRCErrEnd   ) ||
       (Current_state==State_FFFullErrEnd) ||
       (Current_state==State_DropEnd     ) ||
       (Current_state==State_Drop2End    ) )
    pre_fifo_data_end = 1;
  else
    pre_fifo_data_end = 0;

always @( Current_state )
  if ( (Current_state==State_ErrEnd      ) ||
       (Current_state==State_CRCErrEnd   ) ||
       (Current_state==State_FFFullErrEnd) ||
       (Current_state==State_DropEnd     ) ||
       (Current_state==State_Drop2End    ) )
    pre_fifo_data_err = 1;
  else
    pre_fifo_data_err = 0;

always @( Current_state )
  if ( (Current_state==State_DropEnd ) ||
       (Current_state==State_Drop2End) )
    pre_fifo_data_drop = 1;
  else
    pre_fifo_data_drop = 0;

  // Drop in main Rx FIFO is no longer supported!
  assign Fifo_data_drop = 0;

//******************************************************************************
//CRC_chk interface                                               
//****************************************************************************** 

always @(Current_state)
    if (Current_state==State_data)
        CRC_en  =1;
    else
        CRC_en  =0;
        
always @(Current_state)
    if (Current_state==State_SFD)
        CRC_init    =1;
    else
        CRC_init    =0;
        
//******************************************************************************
//gen rmon signals                                         
//******************************************************************************    
always @ (posedge Clk or posedge Reset)
    if (Reset)  
        Frame_length_counter        <=0;
    else if (Current_state==State_SFD)
        Frame_length_counter        <=1;
    else if (Current_state==State_data)
        Frame_length_counter        <=Frame_length_counter+ 1'b1;
        
always @( Frame_length_counter )
  if ( Frame_length_counter < 8 )
    ProcessingHeader = 1;
  else
    ProcessingHeader = 0;

always @ (Frame_length_counter or RX_MIN_LENGTH)
    if (Frame_length_counter<RX_MIN_LENGTH)
        Too_short   =1;
    else
        Too_short   =0;
        
always @ (*)
    if (Frame_length_counter>RX_MAX_LENGTH)
        Too_long    =1;
    else
        Too_long    =0;
        
assign Rx_pkt_length_rmon = Frame_length_counter-1'b1;

reg [2:0] Rx_apply_rmon_reg;

always @( posedge Clk or posedge Reset )
  if ( Reset )
    begin
      Rx_apply_rmon <= 0;
      Rx_apply_rmon_reg <= 'b0;
    end
  else
    begin
      if ( (Current_state==State_OkEnd       ) ||
           (Current_state==State_ErrEnd      ) ||
           (Current_state==State_CRCErrEnd   ) ||
           (Current_state==State_Drop2End    ) ||
           (Current_state==State_FFFullErrEnd) )
        Rx_apply_rmon <= 1;
      else
        if ( Rx_apply_rmon_reg[2] )
          Rx_apply_rmon <= 0;

      Rx_apply_rmon_reg <= { Rx_apply_rmon_reg[1:0], Rx_apply_rmon  };
    end
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        Rx_pkt_err_type_rmon    <=0;
    else if (Current_state==State_CRCErrEnd)
        Rx_pkt_err_type_rmon    <=3'b001    ;//
    else if (Current_state==State_FFFullErrEnd)
        Rx_pkt_err_type_rmon    <=3'b010    ;// 
    else if ( (Current_state==State_ErrEnd) || (Current_state==State_Drop2End) )
        Rx_pkt_err_type_rmon    <=3'b011    ;//
    else if(Current_state==State_OkEnd)
        Rx_pkt_err_type_rmon    <=3'b100    ;


        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        Rx_pkt_type_rmon        <=0;
    else if (Current_state==State_OkEnd&&pause_frame_ptr)
        Rx_pkt_type_rmon        <=3'b100    ;//
    else if(Current_state==State_SFD&&Next_state==State_data)
        Rx_pkt_type_rmon        <={1'b0,MRxD[7:6]};

always @ (posedge Clk or posedge Reset)
    if (Reset)
        broadcast_ptr   <=0;
    else if(Current_state==State_IFG)
        broadcast_ptr   <=0;
    else if(Current_state==State_SFD&&Next_state==State_data&&MRxD[7:6]==2'b11)
        broadcast_ptr   <=1;

        
    
//******************************************************************************
//MAC add checker signals                                                              
//******************************************************************************
always @ (Frame_length_counter or pre_fifo_data_en)
    if(Frame_length_counter>=1&&Frame_length_counter<=6)
        MAC_add_en  <=pre_fifo_data_en;
    else
        MAC_add_en  <=0;

//******************************************************************************
//flow control signals                                                            
//******************************************************************************
always @ (posedge Clk or posedge Reset)
    if (Reset)
        Pause_current   <=Pause_idle;
    else
        Pause_current   <=Pause_next;
        
always @ (*)
    case (Pause_current)
        Pause_idle  : 
            if(Current_state==State_SFD)
                Pause_next  =Pause_pre_syn;
            else
                Pause_next  =Pause_current;
        Pause_pre_syn:
            case (Frame_length_counter)
                16'd1:  if (RxD_dl1==8'h01)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;
                16'd2:  if (RxD_dl1==8'h80)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;            
                16'd3:  if (RxD_dl1==8'hc2)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;
                16'd4:  if (RxD_dl1==8'h00)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;
                16'd5:  if (RxD_dl1==8'h00)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;
                16'd6:  if (RxD_dl1==8'h01)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;
                16'd13: if (RxD_dl1==8'h88)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;
                16'd14: if (RxD_dl1==8'h08)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;
                16'd15: if (RxD_dl1==8'h00)
                            Pause_next  =Pause_current;
                        else
                            Pause_next  =Pause_idle;
                16'd16: if (RxD_dl1==8'h01)
                            Pause_next  =Pause_quanta_hi;
                        else
                            Pause_next  =Pause_idle;
                default:    Pause_next  =Pause_current;
            endcase
        Pause_quanta_hi :
            Pause_next  =Pause_quanta_lo;
        Pause_quanta_lo :
            Pause_next  =Pause_syn; 
        Pause_syn       :
            if (Current_state==State_IFG)
                Pause_next  =Pause_idle;
            else
                Pause_next  =Pause_current;
        default
            Pause_next  =Pause_idle;
    endcase

always @ (posedge Clk or posedge Reset)
    if (Reset)
        pause_quanta_h      <=0;
    else if(Pause_current==Pause_quanta_hi)
        pause_quanta_h      <=RxD_dl1;
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        pause_quanta        <=0;
    else if(Pause_current==Pause_quanta_lo)
        pause_quanta        <={pause_quanta_h,RxD_dl1};

   // The following 2 always blocks are a strange way of holding
   // pause_quanta_val high for 2 cycles
always @ (posedge Clk or posedge Reset)
    if (Reset)      
        pause_quanta_val_tmp    <=0;
    else if(Current_state==State_OkEnd&&Pause_current==Pause_syn)
        pause_quanta_val_tmp    <=1;
    else
        pause_quanta_val_tmp    <=0;
        
always @ (posedge Clk or posedge Reset)
    if (Reset)      
        pause_quanta_val    <=0;
    else if(Current_state==State_OkEnd&&Pause_current==Pause_syn||pause_quanta_val_tmp)
        pause_quanta_val    <=1;
    else
        pause_quanta_val    <=0;        
    
always @ (posedge Clk or posedge Reset)
    if (Reset)  
        pause_frame_ptr     <=0;
    else if(Pause_current==Pause_syn)
        pause_frame_ptr     <=1;
    else
        pause_frame_ptr     <=0;
                
endmodule
                
                                                
