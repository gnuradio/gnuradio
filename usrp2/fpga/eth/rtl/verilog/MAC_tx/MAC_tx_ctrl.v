//////////////////////////////////////////////////////////////////////
////                                                              ////
////  MAC_tx_ctrl.v                                               ////
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
// $Log: MAC_tx_Ctrl.v,v $
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
// Revision 1.2  2005/12/13 12:15:38  Administrator
// no message
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//                                           

module MAC_tx_ctrl (  
Reset               ,
Clk                 ,
//CRC_gen Interface  
CRC_init            ,
Frame_data          ,
Data_en             ,
CRC_rd              ,
CRC_end             ,
CRC_out             ,
//Ramdon_gen interfac
Random_init         ,
RetryCnt            ,
Random_time_meet    ,
//flow control      
pause_apply         ,
pause_quanta_sub    ,
xoff_gen            ,
xoff_gen_complete   ,
xon_gen             ,
xon_gen_complete    ,
//MAC_tx_FF          
Fifo_data           ,
Fifo_rd             ,
Fifo_eop            ,
Fifo_da             ,
Fifo_rd_finish      ,
Fifo_rd_retry       ,
Fifo_ra             ,
Fifo_data_err_empty ,
Fifo_data_err_full  ,
//RMII               
TxD                 ,
TxEn                ,
CRS                 , 
//MAC_tx_addr_add   
MAC_tx_addr_rd      ,
MAC_tx_addr_data    ,
MAC_tx_addr_init    ,
//RMON               
Tx_pkt_type_rmon    ,
Tx_pkt_length_rmon  ,
Tx_apply_rmon       ,
Tx_pkt_err_type_rmon,
//CPU  
pause_quanta_set    ,              
MAC_tx_add_en       ,  
FullDuplex          ,
MaxRetry            ,
IFGset          
);

input           Reset               ;
input           Clk                 ;
                //CRC_gen Interface 
output          CRC_init            ;
output  [7:0]   Frame_data          ;
output          Data_en             ;
output          CRC_rd              ;
input           CRC_end             ;
input   [7:0]   CRC_out             ;
                //Ramdon_gen interface
output          Random_init         ;
output  [3:0]   RetryCnt            ;
input           Random_time_meet    ;//levle hight indicate random time passed away
                //flow control
input           pause_apply         ;
output          pause_quanta_sub    ;
input           xoff_gen            ;
output          xoff_gen_complete   ;
input           xon_gen             ;
output          xon_gen_complete    ;               
                //MAC_rx_FF
input   [7:0]   Fifo_data           ;
output          Fifo_rd             ;
input           Fifo_eop            ;
input           Fifo_da             ;
output          Fifo_rd_finish      ;
output          Fifo_rd_retry       ;
input           Fifo_ra             ;
input           Fifo_data_err_empty ;
input           Fifo_data_err_full  ;
                //RMII
output  [7:0]   TxD                 ;
output          TxEn                ;   
input           CRS                 ;
                //MAC_tx_addr_add
output          MAC_tx_addr_init    ;
output          MAC_tx_addr_rd      ;
input   [7:0]   MAC_tx_addr_data    ;
                //RMON
output  [2:0]   Tx_pkt_type_rmon    ;
output  [15:0]  Tx_pkt_length_rmon  ;
output          Tx_apply_rmon       ;
output  [2:0]   Tx_pkt_err_type_rmon;   
                //CPU
input   [15:0]  pause_quanta_set    ;
input           MAC_tx_add_en       ;               
input           FullDuplex          ;
input   [3:0]   MaxRetry            ;
input   [5:0]   IFGset              ;
//******************************************************************************        
//internal signals                                                              
//******************************************************************************   
parameter       StateIdle           =4'd00;
parameter       StatePreamble       =4'd01;
parameter       StateSFD            =4'd02;
parameter       StateData           =4'd03;
parameter       StatePause          =4'd04;
parameter       StatePAD            =4'd05;
parameter       StateFCS            =4'd06;
parameter       StateIFG            =4'd07;
parameter       StateJam            =4'd08;
parameter       StateBackOff        =4'd09;
parameter       StateJamDrop        =4'd10;
parameter       StateFFEmptyDrop    =4'd11;
parameter       StateSwitchNext     =4'd12;
parameter       StateDefer          =4'd13;
parameter       StateSendPauseFrame =4'd14;

reg [3:0] 	Current_state;
reg [3:0]       Next_state;
reg [5:0]       IFG_counter;
reg [4:0]       Preamble_counter;//
reg [7:0]       TxD_tmp             ;   
reg             TxEn_tmp            ;   
reg [15:0]      Tx_pkt_length_rmon  ;
reg             Tx_apply_rmon       ;
reg [2:0]       Tx_pkt_err_type_rmon;   
reg [3:0]       RetryCnt            ;
reg             Random_init         ;
reg             Fifo_rd_finish      ;
reg             Fifo_rd_retry       ;
reg [7:0]       TxD                 ;   
reg             TxEn                ;   
reg             CRC_init            ;
reg             Data_en             ;
reg             CRC_rd              ;
reg             Fifo_rd             ;
reg             MAC_tx_addr_rd      ;
reg             MAC_header_slot     ;
reg             MAC_header_slot_tmp ;
reg [2:0]       Tx_pkt_type_rmon    ;
wire            Collision           ; 
reg             MAC_tx_addr_init    ;
reg             Src_MAC_ptr         ;
reg [7:0]       IPLengthCounter     ;//for pad append
reg [1:0]       PADCounter          ;
reg [7:0]       JamCounter          ;
reg             PktDrpEvenPtr       ;
reg [7:0]       pause_counter       ;
reg             pause_quanta_sub    ;
reg [15:0]      pause_quanta_set_dl1    ;
reg             xoff_gen_complete   ;
reg             xon_gen_complete    ;
//******************************************************************************    
//boundery signal processing                                                             
//****************************************************************************** 
always @(posedge Clk or posedge Reset)
    if (Reset)
        begin  
        pause_quanta_set_dl1            <=0;
        end
    else
        begin  
        pause_quanta_set_dl1            <=pause_quanta_set      ;
        end     
//******************************************************************************    
//state machine                                                             
//****************************************************************************** 
assign Collision=TxEn&CRS;

always @(posedge Clk or posedge Reset)
    if (Reset)
        IPLengthCounter     <=0;
    else if (Current_state==StateDefer)
        IPLengthCounter     <=0;    
    else if (IPLengthCounter!=8'hff&&(Current_state==StateData||Current_state==StateSendPauseFrame||Current_state==StatePAD))
        IPLengthCounter     <=IPLengthCounter+1;

always @(posedge Clk or posedge Reset)
    if (Reset)
        PADCounter      <=0;
    else if (Current_state!=StatePAD)
        PADCounter      <=0;
    else
        PADCounter      <=PADCounter+1;

always @(posedge Clk or posedge Reset)
    if (Reset)
        Current_state       <=StateDefer;
    else 
        Current_state       <=Next_state;    
        
always @ (*)
        case (Current_state)   
            StateDefer:
                if ((FullDuplex)||(!FullDuplex&&!CRS))
                    Next_state=StateIFG;
                else
                    Next_state=Current_state;   
            StateIFG:
                if (!FullDuplex&&CRS)
                    Next_state=StateDefer;
                else if ((FullDuplex&&IFG_counter==IFGset-4)||(!FullDuplex&&!CRS&&IFG_counter==IFGset-4))//remove some additional time
                    Next_state=StateIdle;
                else
                    Next_state=Current_state;           
            StateIdle:
                if (!FullDuplex&&CRS)
                    Next_state=StateDefer;
	        else if (xoff_gen||xon_gen)
		  Next_state=StatePreamble;
                else if (pause_apply)
                    Next_state=StatePause;          
                else if ((FullDuplex||~CRS)&&Fifo_ra)
                    Next_state=StatePreamble;
                else
                    Next_state=Current_state;   
            StatePause:
                if (pause_counter==512/8)
                    Next_state=StateDefer;
	        else if (xoff_gen||xon_gen)
		  Next_state=StateIdle;
                else
                    Next_state=Current_state;               
            StatePreamble:
                if (!FullDuplex&&Collision)
                    Next_state=StateJam;
                else if ((FullDuplex&&Preamble_counter==6)||(!FullDuplex&&!Collision&&Preamble_counter==6))
                    Next_state=StateSFD;
                else
                    Next_state=Current_state;
            StateSFD:
                if (!FullDuplex&&Collision)
                    Next_state=StateJam;
                else if (xoff_gen||xon_gen)
                    Next_state=StateSendPauseFrame;
                else 
                    Next_state=StateData;
            StateSendPauseFrame:
                if (IPLengthCounter==17)
                    Next_state=StatePAD;
                else
                    Next_state=Current_state;
            StateData:
                if (!FullDuplex&&Collision)
                    Next_state=StateJam;
                else if (Fifo_data_err_empty)
                    Next_state=StateFFEmptyDrop;                
                else if (Fifo_eop&&IPLengthCounter>=59)//IP+MAC+TYPE=60 ,start from 0
                    Next_state=StateFCS;
                else if (Fifo_eop)
                    Next_state=StatePAD;
                else 
                    Next_state=StateData;       
            StatePAD:
                if (!FullDuplex&&Collision)
                    Next_state=StateJam; 
                else if (IPLengthCounter>=59)
                    Next_state=StateFCS;        
                else 
                    Next_state=Current_state;
            StateJam:
                if (RetryCnt<=MaxRetry&&JamCounter==16) 
                    Next_state=StateBackOff;
                else if (RetryCnt>MaxRetry)
                    Next_state=StateJamDrop;
                else
                    Next_state=Current_state;
            StateBackOff:
                if (Random_time_meet)
                    Next_state  =StateDefer;
                else 
                    Next_state  =Current_state;
            StateFCS:
                if (!FullDuplex&&Collision)
                    Next_state  =StateJam;
                else if (CRC_end)
                    Next_state  =StateSwitchNext;
                else
                    Next_state  =Current_state;
            StateFFEmptyDrop:
                if (Fifo_eop)
                    Next_state  =StateSwitchNext;
                else
                    Next_state  =Current_state;             
            StateJamDrop:
                if (Fifo_eop)
                    Next_state  =StateSwitchNext;
                else
                    Next_state  =Current_state;
            StateSwitchNext:
                    Next_state  =StateDefer;            
            default:
                Next_state  =StateDefer;
        endcase

 
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        JamCounter      <=0;
    else if (Current_state!=StateJam)
        JamCounter      <=0;
    else if (Current_state==StateJam)
        JamCounter      <=JamCounter+1;
        
             
always @ (posedge Clk or posedge Reset)
    if (Reset)
        RetryCnt        <=0;
    else if (Current_state==StateSwitchNext)
        RetryCnt        <=0;
    else if (Current_state==StateJam&&Next_state==StateBackOff)
        RetryCnt        <=RetryCnt + 1;
            
always @ (posedge Clk or posedge Reset)
    if (Reset)
        IFG_counter     <=0;
    else if (Current_state!=StateIFG)
        IFG_counter     <=0;
    else 
        IFG_counter     <=IFG_counter + 1;

always @ (posedge Clk or posedge Reset)
    if (Reset)
        Preamble_counter    <=0;
    else if (Current_state!=StatePreamble)
        Preamble_counter    <=0;
    else
        Preamble_counter    <=Preamble_counter+ 1;
        
always @ (posedge Clk or posedge Reset)
    if (Reset)      
        PktDrpEvenPtr       <=0;
    else if(Current_state==StateJamDrop||Current_state==StateFFEmptyDrop)
        PktDrpEvenPtr       <=~PktDrpEvenPtr;
//******************************************************************************    
//generate output signals                                                           
//****************************************************************************** 
//CRC related
always @(Current_state)
    if (Current_state==StateSFD)
        CRC_init    =1;
    else
        CRC_init    =0;
        
assign Frame_data=TxD_tmp;

always @(Current_state)
    if (Current_state==StateData||Current_state==StateSendPauseFrame||Current_state==StatePAD)
        Data_en     =1;
    else
        Data_en     =0;
        
always @(Current_state)
    if (Current_state==StateFCS)
        CRC_rd      =1;
    else
        CRC_rd      =0;     
    
//Ramdon_gen interface
always @(Current_state or Next_state)
    if (Current_state==StateJam&&Next_state==StateBackOff)
        Random_init =1;
    else
        Random_init =0; 

//MAC_rx_FF
//data have one cycle delay after fifo read signals  
always @ (*)
    if (Current_state==StateData ||
        Current_state==StateSFD&&!(xoff_gen||xon_gen)  ||
        Current_state==StateJamDrop&&PktDrpEvenPtr||
        Current_state==StateFFEmptyDrop&&PktDrpEvenPtr )
        Fifo_rd     =1;
    else
        Fifo_rd     =0; 
        
always @ (Current_state)
    if (Current_state==StateSwitchNext)     
        Fifo_rd_finish  =1;
    else
        Fifo_rd_finish  =0;
        
always @ (Current_state)
    if (Current_state==StateJam)        
        Fifo_rd_retry   =1;
    else
        Fifo_rd_retry   =0;     
//RMII
always @(Current_state)
    if (Current_state==StatePreamble||Current_state==StateSFD||
        Current_state==StateData||Current_state==StateSendPauseFrame||
        Current_state==StateFCS||Current_state==StatePAD||Current_state==StateJam)
        TxEn_tmp    =1;
    else
        TxEn_tmp    =0;

//gen txd data      
always @(*)
    case (Current_state)
        StatePreamble:
            TxD_tmp =8'h55;
        StateSFD:
            TxD_tmp =8'hd5;
        StateData:
            if (Src_MAC_ptr&&MAC_tx_add_en)       
                TxD_tmp =MAC_tx_addr_data;
            else
                TxD_tmp =Fifo_data;
        StateSendPauseFrame:
            if (Src_MAC_ptr&&MAC_tx_add_en)       
                TxD_tmp =MAC_tx_addr_data;
            else 
                case (IPLengthCounter)
                    8'd0:   TxD_tmp =8'h01;
                    8'd1:   TxD_tmp =8'h80;
                    8'd2:   TxD_tmp =8'hc2;
                    8'd3:   TxD_tmp =8'h00;
                    8'd4:   TxD_tmp =8'h00;
                    8'd5:   TxD_tmp =8'h01;
                    8'd12:  TxD_tmp =8'h88;//type
                    8'd13:  TxD_tmp =8'h08;//
                    8'd14:  TxD_tmp =8'h00;//opcode
                    8'd15:  TxD_tmp =8'h01;
                    8'd16:  TxD_tmp =xon_gen?8'b0:pause_quanta_set_dl1[15:8];
                    8'd17:  TxD_tmp =xon_gen?8'b0:pause_quanta_set_dl1[7:0];
//                    8'd60:  TxD_tmp =8'h26;
//                    8'd61:  TxD_tmp =8'h6b;
//                    8'd62:  TxD_tmp =8'hae;
//                    8'd63:  TxD_tmp =8'h0a;
                    default:TxD_tmp =0;
                endcase
        
        StatePAD:
                TxD_tmp =8'h00; 
        StateJam:
                TxD_tmp =8'h01; //jam sequence
        StateFCS:
            TxD_tmp =CRC_out;
        default:
            TxD_tmp =2'b0;
    endcase
always @ (posedge Clk or posedge Reset)
    if (Reset)
        begin
        TxD     <=0;
        TxEn    <=0;
        end
    else
        begin
        TxD     <=TxD_tmp;
        TxEn    <=TxEn_tmp;
        end     
//RMON


always @ (posedge Clk or posedge Reset)
    if (Reset)
        Tx_pkt_length_rmon      <=0;
    else if (Current_state==StateSFD)
        Tx_pkt_length_rmon      <=0;
    else if (Current_state==StateData||Current_state==StateSendPauseFrame||Current_state==StatePAD||Current_state==StateFCS)
        Tx_pkt_length_rmon      <=Tx_pkt_length_rmon+1;
        

reg [2:0] Tx_apply_rmon_reg;

always @( posedge Clk or posedge Reset )
  if ( Reset )
    begin
      Tx_apply_rmon <= 0;
      Tx_apply_rmon_reg <= 'b0;
    end
  else
    begin
      if ( (Fifo_eop&&Current_state==StateJamDrop)     ||
           (Fifo_eop&&Current_state==StateFFEmptyDrop) ||
           CRC_end )
        Tx_apply_rmon <= 1;
      else
        if ( Tx_apply_rmon_reg[2] )
          Tx_apply_rmon <= 0;

      Tx_apply_rmon_reg <= { Tx_apply_rmon_reg[1:0], Tx_apply_rmon  };
    end

always @ (posedge Clk or posedge Reset)
    if (Reset)
        Tx_pkt_err_type_rmon    <=0;    
    else if(Fifo_eop&&Current_state==StateJamDrop)
        Tx_pkt_err_type_rmon    <=3'b001;//
    else if(Fifo_eop&&Current_state==StateFFEmptyDrop)
        Tx_pkt_err_type_rmon    <=3'b010;//underflow
    else if(Fifo_eop&&Fifo_data_err_full)
        Tx_pkt_err_type_rmon    <=3'b011;//overflow
    else if(CRC_end)
        Tx_pkt_err_type_rmon    <=3'b100;//normal
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        MAC_header_slot_tmp <=0;
    else if(Current_state==StateSFD&&Next_state==StateData)
        MAC_header_slot_tmp <=1;    
    else
        MAC_header_slot_tmp <=0;
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        MAC_header_slot     <=0;
    else 
        MAC_header_slot     <=MAC_header_slot_tmp;

always @ (posedge Clk or posedge Reset)
    if (Reset)
        Tx_pkt_type_rmon    <=0;
    else if (Current_state==StateSendPauseFrame)
        Tx_pkt_type_rmon    <=3'b100;
    else if(MAC_header_slot)
        Tx_pkt_type_rmon    <={1'b0,TxD[7:6]};

       
always @(Tx_pkt_length_rmon)
    if (Tx_pkt_length_rmon>=6&&Tx_pkt_length_rmon<=11)
        Src_MAC_ptr         =1;
    else
        Src_MAC_ptr         =0;        

//MAC_tx_addr_add  
always @ (posedge Clk or posedge Reset)
    if (Reset)
        MAC_tx_addr_rd  <=0;
    else if ((Tx_pkt_length_rmon>=4&&Tx_pkt_length_rmon<=9)&&(MAC_tx_add_en||Current_state==StateSendPauseFrame))
        MAC_tx_addr_rd  <=1;
    else
        MAC_tx_addr_rd  <=0;

   always @*
     //if ((Tx_pkt_length_rmon==3)&&Fifo_rd)
     if (Current_state==StatePreamble)
       MAC_tx_addr_init=1;
     else
       MAC_tx_addr_init=0;

//**************************************************************************************************************
// CFH: this implementation delays the time it sends an entire Ethernet frame with 512 bits for every pause
//      request of 512 bits. Actually, it should only delay the time it takes to transmit 512 bits, not counting
//      Ethernet header, CRC, Interframe Gap etc.
//      Hence the current implementation waits longer than the pause frame actually requests (~20% more)
//**************************************************************************************************************

//flow control
always @(posedge Clk or posedge Reset)
    if (Reset)
        pause_counter   <=0;
    else if (Current_state!=StatePause)
        pause_counter   <=0;
    else 
        pause_counter   <=pause_counter+1;
        
always @ (posedge Clk or posedge Reset)
    if (Reset)
        pause_quanta_sub    <=0;
    else if(pause_counter==512/8)
        pause_quanta_sub    <=1;
    else
        pause_quanta_sub    <=0;

// FIXME  The below probably won't work if the pause request comes when we are in the wrong state
   wire clear_xonxoff = (Current_state==StateSendPauseFrame) & (IPLengthCounter==17);
always @ (posedge Clk or posedge Reset)
    if (Reset) 
        xoff_gen_complete   <=0;
    else if(clear_xonxoff & xoff_gen)
        xoff_gen_complete   <=1;
    else
        xoff_gen_complete   <=0;
    
    
always @ (posedge Clk or posedge Reset)
    if (Reset) 
        xon_gen_complete    <=0;
    else if(clear_xonxoff & xon_gen)
        xon_gen_complete    <=1;
    else
        xon_gen_complete    <=0;

endmodule
