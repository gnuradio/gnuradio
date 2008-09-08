//////////////////////////////////////////////////////////////////////
////                                                              ////
////  MAC_rx_FF.v                                                 ////
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
// $Log: MAC_rx_FF.v,v $
// Revision 1.5  2006/06/25 04:58:56  maverickist
// no message
//
// Revision 1.4  2006/05/28 05:09:20  maverickist
// no message
//
// Revision 1.3  2006/01/19 14:07:54  maverickist
// verification is complete.
//
// Revision 1.3  2005/12/16 06:44:16  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.2  2005/12/13 12:15:37  Administrator
// no message
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//                                           

module MAC_rx_FF 
  #(parameter MAC_RX_FF_DEPTH = 9)
    (Reset,Clk_MAC,Clk_SYS,
     //MAC_rx_ctrl interface
     Fifo_data,Fifo_data_en,Fifo_full,Fifo_data_err,Fifo_data_end,
     //CPU
     Rx_Hwmark,Rx_Lwmark,RX_APPEND_CRC,
     //user interface
     Rx_mac_ra,Rx_mac_rd,Rx_mac_data,Rx_mac_BE,Rx_mac_sop,Rx_mac_pa,Rx_mac_eop);

   input Reset;
   input Clk_MAC     ;
   input Clk_SYS     ;
   //MAC_rx_ctrl interface 
   input [7:0] Fifo_data       ;
   input       Fifo_data_en    ;
   output      Fifo_full       ;
   input       Fifo_data_err   ;
   input       Fifo_data_end   ;
   //CPU
   input       RX_APPEND_CRC       ;
   input [4:0] Rx_Hwmark           ;
   input [4:0] Rx_Lwmark           ;
   //user interface 
   output      Rx_mac_ra   ;//
   input       Rx_mac_rd   ;
   output [31:0] Rx_mac_data ;
   output [1:0]  Rx_mac_BE   ;
   output 	 Rx_mac_pa   ;
   output 	 Rx_mac_sop  ;
   output 	 Rx_mac_eop  ;
   
   // ******************************************************************************
   //internal signals
   // ******************************************************************************
   parameter 	 State_byte3     =4'd0;      
   parameter 	 State_byte2     =4'd1;
   parameter 	 State_byte1     =4'd2;      
   parameter 	 State_byte0     =4'd3;
   parameter 	 State_be0       =4'd4;
   parameter 	 State_be3       =4'd5;
   parameter 	 State_be2       =4'd6;
   parameter 	 State_be1       =4'd7;
   parameter 	 State_err_end   =4'd8;
   parameter 	 State_idle      =4'd9;
   
   parameter 	 SYS_read        =3'd0;
   parameter 	 SYS_pause       =3'd1;
   parameter 	 SYS_wait_end    =3'd2;
   parameter 	 SYS_idle        =3'd3;
   parameter 	 FF_emtpy_err    =3'd4;
   
   reg [MAC_RX_FF_DEPTH-1:0] Add_wr;
   reg [MAC_RX_FF_DEPTH-1:0] Add_wr_ungray;
   reg [MAC_RX_FF_DEPTH-1:0] Add_wr_gray;
   reg [MAC_RX_FF_DEPTH-1:0] Add_wr_gray_dl1;
   reg [MAC_RX_FF_DEPTH-1:0] Add_wr_reg;
   
   reg [MAC_RX_FF_DEPTH-1:0] Add_rd;
   reg [MAC_RX_FF_DEPTH-1:0] Add_rd_gray;
   reg [MAC_RX_FF_DEPTH-1:0] Add_rd_gray_dl1;
   reg [MAC_RX_FF_DEPTH-1:0] Add_rd_ungray;
   reg [35:0] 		     Din;
   reg [35:0] 		     Din_tmp;
   reg [35:0] 		     Din_tmp_reg;
   wire [35:0] 		     Dout;
   reg 			     Wr_en;
   reg 			     Wr_en_tmp;
   reg 			     Wr_en_ptr;
   wire [MAC_RX_FF_DEPTH-1:0] Add_wr_pluse;
   wire [MAC_RX_FF_DEPTH-1:0] Add_wr_pluse4;
   wire [MAC_RX_FF_DEPTH-1:0] Add_wr_pluse3;
   wire [MAC_RX_FF_DEPTH-1:0] Add_wr_pluse2;
   reg 			      Full;
   reg 			      Almost_full;
   reg 			      Empty /* synthesis syn_keep=1 */;
   reg [3:0] 		      Current_state /* synthesis syn_keep=1 */;
   reg [3:0] 		      Next_state;
   reg [7:0] 		      Fifo_data_byte0;
   reg [7:0] 		      Fifo_data_byte1;
   reg [7:0] 		      Fifo_data_byte2;
   reg [7:0] 		      Fifo_data_byte3;
   reg 			      Fifo_data_en_dl1;
   reg [7:0] 		      Fifo_data_dl1;
   reg 			      Rx_mac_sop_tmp  ;
   reg 			      Rx_mac_sop  ;
   reg 			      Rx_mac_ra   ;
   reg 			      Rx_mac_pa   ;
   
   
   
   reg [2:0] 		      Current_state_SYS /* synthesis syn_keep=1 */;
   reg [2:0] 		      Next_state_SYS ;
   reg [5:0] 		      Packet_number_inFF /* synthesis syn_keep=1 */;
   reg 			      Packet_number_sub ;
   wire 		      Packet_number_add_edge;
   reg 			      Packet_number_add_dl1;
   reg 			      Packet_number_add_dl2;
   reg 			      Packet_number_add ;
   reg 			      Packet_number_add_tmp    ;
   reg 			      Packet_number_add_tmp_dl1;
   reg 			      Packet_number_add_tmp_dl2;
   
   reg 			      Rx_mac_sop_tmp_dl1;
   reg [35:0] 		      Dout_dl1;
   reg [4:0] 		      Fifo_data_count;
   reg 			      Rx_mac_pa_tmp       ;
   reg 			      Add_wr_jump_tmp     ;
   reg 			      Add_wr_jump_tmp_pl1 ;
   reg 			      Add_wr_jump         ;
   reg 			      Add_wr_jump_rd_pl1  ;
   reg [4:0] 		      Rx_Hwmark_pl        ;
   reg [4:0] 		      Rx_Lwmark_pl        ;
   integer 		      i                   ;
   
   // ******************************************************************************
   //domain Clk_MAC,write data to dprom.a-port for write
   // ******************************************************************************    

   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Current_state   <=State_idle;
     else
       Current_state   <=Next_state;
   
   always @(Current_state or Fifo_data_en or Fifo_data_err or Fifo_data_end)
     case (Current_state)
       State_idle:
         if (Fifo_data_en)
           Next_state  =State_byte3;
         else
           Next_state  =Current_state;                 
       State_byte3:
         if (Fifo_data_en)
           Next_state  =State_byte2;
         else if (Fifo_data_err)
           Next_state  =State_err_end;
         else if (Fifo_data_end)
           Next_state  =State_be1; 
         else
           Next_state  =Current_state;                 
       State_byte2:
         if (Fifo_data_en)
           Next_state  =State_byte1;
         else if (Fifo_data_err)
           Next_state  =State_err_end;
         else if (Fifo_data_end)
           Next_state  =State_be2; 
         else
           Next_state  =Current_state;         
       State_byte1:
         if (Fifo_data_en)
           Next_state  =State_byte0;
         else if (Fifo_data_err)
           Next_state  =State_err_end;
         else if (Fifo_data_end)
           Next_state  =State_be3; 
         else
           Next_state  =Current_state;         
       State_byte0:
         if (Fifo_data_en)
           Next_state  =State_byte3;
         else if (Fifo_data_err)
           Next_state  =State_err_end;
         else if (Fifo_data_end)
           Next_state  =State_be0; 
         else
           Next_state  =Current_state; 
       State_be1:
         Next_state      =State_idle;
       State_be2:
         Next_state      =State_idle;
       State_be3:
         Next_state      =State_idle;
       State_be0:
         Next_state      =State_idle;
       State_err_end:
         Next_state      =State_idle;
       default:
         Next_state      =State_idle;                
     endcase
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Add_wr_reg      <=0;
     else if (Current_state==State_idle)                 
       Add_wr_reg      <=Add_wr;
        
   always @ (posedge Reset or posedge Clk_MAC)
     if (Reset)
       Add_wr_gray         <=0;
     else 
       begin
	  Add_wr_gray[MAC_RX_FF_DEPTH-1]	<=Add_wr[MAC_RX_FF_DEPTH-1];
	  for (i=MAC_RX_FF_DEPTH-2;i>=0;i=i-1)
	    Add_wr_gray[i]			<=Add_wr[i+1]^Add_wr[i];
       end
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Add_rd_gray_dl1         <=0;
     else
       Add_rd_gray_dl1         <=Add_rd_gray;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Add_rd_ungray       =0;
     else        
       begin
	  Add_rd_ungray[MAC_RX_FF_DEPTH-1]	=Add_rd_gray_dl1[MAC_RX_FF_DEPTH-1];	
	  for (i=MAC_RX_FF_DEPTH-2;i>=0;i=i-1)
	    Add_rd_ungray[i]	=Add_rd_ungray[i+1]^Add_rd_gray_dl1[i];	
       end        
   assign          Add_wr_pluse=Add_wr+1;
   assign 	   Add_wr_pluse4=Add_wr+4;
   assign 	   Add_wr_pluse3=Add_wr+3;
   assign 	   Add_wr_pluse2=Add_wr+2;
   
   
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Full    <=0;
     else if (Add_wr_pluse==Add_rd_ungray)
       Full    <=1;
     else
       Full    <=0;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Almost_full	<=0;
     else if (Add_wr_pluse4==Add_rd_ungray||
	      Add_wr_pluse3==Add_rd_ungray||
	      Add_wr_pluse2==Add_rd_ungray||
	      Add_wr_pluse==Add_rd_ungray
	      )
       Almost_full	<=1;
     else
       Almost_full	<=0;		
   
   assign 	   Fifo_full =Almost_full;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Add_wr  <=0;
     else if (Current_state==State_err_end)
       Add_wr  <=Add_wr_reg;
     else if (Wr_en&&!Full)
       Add_wr  <=Add_wr +1;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Add_wr_jump_tmp <=0;
     else if (Current_state==State_err_end)
       Add_wr_jump_tmp <=1;
     else
       Add_wr_jump_tmp <=0;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Add_wr_jump_tmp_pl1 <=0;
     else
       Add_wr_jump_tmp_pl1 <=Add_wr_jump_tmp;	 
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Add_wr_jump <=0;
     else if (Current_state==State_err_end)
       Add_wr_jump <=1;
     else if (Add_wr_jump_tmp_pl1)
       Add_wr_jump <=0;	       		
   
   //
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Fifo_data_en_dl1    <=0;
     else 
       Fifo_data_en_dl1    <=Fifo_data_en;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Fifo_data_dl1   <=0;
     else 
       Fifo_data_dl1   <=Fifo_data;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Fifo_data_byte3     <=0;
     else if (Current_state==State_byte3&&Fifo_data_en_dl1)
       Fifo_data_byte3     <=Fifo_data_dl1;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Fifo_data_byte2     <=0;
     else if (Current_state==State_byte2&&Fifo_data_en_dl1)
       Fifo_data_byte2     <=Fifo_data_dl1;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Fifo_data_byte1     <=0;
     else if (Current_state==State_byte1&&Fifo_data_en_dl1)
       Fifo_data_byte1     <=Fifo_data_dl1;
   
   always @ (* )
     case (Current_state)
       State_be0:
         Din_tmp ={4'b1000,Fifo_data_byte3,Fifo_data_byte2,Fifo_data_byte1,Fifo_data_dl1};       
       State_be1:
         Din_tmp ={4'b1001,Fifo_data_byte3,24'h0};
       State_be2:
         Din_tmp ={4'b1010,Fifo_data_byte3,Fifo_data_byte2,16'h0};
       State_be3:
         Din_tmp ={4'b1011,Fifo_data_byte3,Fifo_data_byte2,Fifo_data_byte1,8'h0};
       default:
         Din_tmp ={4'b0000,Fifo_data_byte3,Fifo_data_byte2,Fifo_data_byte1,Fifo_data_dl1};
     endcase
   
   always @ (*)
     if (Current_state==State_be0||Current_state==State_be1||
	 Current_state==State_be2||Current_state==State_be3||
	 (Current_state==State_byte0&&Fifo_data_en))
       Wr_en_tmp   =1;
     else 
       Wr_en_tmp   =0; 
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Din_tmp_reg <=0;
     else if(Wr_en_tmp)
       Din_tmp_reg <=Din_tmp;  
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Wr_en_ptr   <=0;
     else if(Current_state==State_idle)
       Wr_en_ptr   <=0;    
     else if(Wr_en_tmp)
       Wr_en_ptr   <=1;
   
   //if not append FCS,delay one cycle write data and Wr_en signal to drop FCS
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       begin
          Wr_en           <=0;
          Din             <=0;
       end
     else if(RX_APPEND_CRC)
       begin
          Wr_en           <=Wr_en_tmp;
          Din             <=Din_tmp;
       end         
     else
       begin
          Wr_en           <=Wr_en_tmp&&Wr_en_ptr;
          Din             <={Din_tmp[35:32],Din_tmp_reg[31:0]};
       end                                 
   
   //this signal for read side to handle the packet number in fifo
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Packet_number_add_tmp   <=0;
     else if (Current_state==State_be0||Current_state==State_be1||
              Current_state==State_be2||Current_state==State_be3)
       Packet_number_add_tmp   <=1;
     else 
       Packet_number_add_tmp   <=0;
   
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       begin
          Packet_number_add_tmp_dl1   <=0;
          Packet_number_add_tmp_dl2   <=0;
       end
     else
       begin
          Packet_number_add_tmp_dl1   <=Packet_number_add_tmp;
          Packet_number_add_tmp_dl2   <=Packet_number_add_tmp_dl1;
       end     
   
   //Packet_number_add delay to Din[35] is needed to make sure the data have been wroten to ram.       
   //expand to two cycles long almost=16 ns
   //if the Clk_SYS period less than 16 ns ,this signal need to expand to 3 or more clock cycles       
   always @ (posedge Clk_MAC or posedge Reset)
     if (Reset)
       Packet_number_add   <=0;
     else if (Packet_number_add_tmp_dl1||Packet_number_add_tmp_dl2)
       Packet_number_add   <=1;
     else 
       Packet_number_add   <=0;
  
   // ******************************************************************************
   // domain Clk_SYS,read data from dprom.b-port for read
   // ******************************************************************************
   
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       Current_state_SYS   <=SYS_idle;
     else 
       Current_state_SYS   <=Next_state_SYS;
   
   always @ (Current_state_SYS or Rx_mac_rd or Rx_mac_ra or Dout or Empty)
     case (Current_state_SYS)
       SYS_idle:
	 if (Rx_mac_rd&&Rx_mac_ra&&!Empty)
           Next_state_SYS  =SYS_read;
	 else if(Rx_mac_rd&&Rx_mac_ra&&Empty)
	   Next_state_SYS	=FF_emtpy_err;
         else
           Next_state_SYS  =Current_state_SYS;
       SYS_read:
         if (!Rx_mac_rd)
           Next_state_SYS  =SYS_pause;
         else if (Dout[35])                
           Next_state_SYS  =SYS_wait_end;
         else if (Empty)
           Next_state_SYS  =FF_emtpy_err;
         else
           Next_state_SYS  =Current_state_SYS;
       SYS_pause:
         if (Rx_mac_rd)                            
           Next_state_SYS  =SYS_read;         
         else                                   
           Next_state_SYS  =Current_state_SYS;
       FF_emtpy_err:
         if (!Empty)
           Next_state_SYS  =SYS_read;
         else
           Next_state_SYS  =Current_state_SYS;
       SYS_wait_end:
         if (!Rx_mac_rd)
           Next_state_SYS  =SYS_idle;
         else
           Next_state_SYS  =Current_state_SYS;
       default:
         Next_state_SYS  =SYS_idle;
     endcase // case(Current_state_SYS)
   
   //gen Rx_mac_ra 
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       begin
          Packet_number_add_dl1   <=0;
          Packet_number_add_dl2   <=0;
       end
     else 
       begin
          Packet_number_add_dl1   <=Packet_number_add;
          Packet_number_add_dl2   <=Packet_number_add_dl1;
       end
   assign  Packet_number_add_edge=Packet_number_add_dl1&!Packet_number_add_dl2;
   
   always @ (Current_state_SYS or Next_state_SYS)
     if (Current_state_SYS==SYS_read&&Next_state_SYS==SYS_wait_end)
       Packet_number_sub       =1;
     else
       Packet_number_sub       =0;
   
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       Packet_number_inFF      <=0;
     else if (Packet_number_add_edge&&!Packet_number_sub)
       Packet_number_inFF      <=Packet_number_inFF + 1;
     else if (!Packet_number_add_edge&&Packet_number_sub&&Packet_number_inFF!=0)
       Packet_number_inFF      <=Packet_number_inFF - 1;
   
   always @ (posedge Clk_SYS or posedge Reset)                                                         
     if (Reset)                                                                                      
       Fifo_data_count     <=0;                                                                    
     else                                                                                            
       Fifo_data_count     <=Add_wr_ungray[MAC_RX_FF_DEPTH-1:MAC_RX_FF_DEPTH-5]-Add_rd[MAC_RX_FF_DEPTH-1:MAC_RX_FF_DEPTH-5]; 
   
   always @ (posedge Clk_SYS or posedge Reset)                                                         
     if (Reset) 
       begin
          Rx_Hwmark_pl        <=0;
          Rx_Lwmark_pl        <=0;
       end
     else
       begin
          Rx_Hwmark_pl        <=Rx_Hwmark;
          Rx_Lwmark_pl        <=Rx_Lwmark;
       end   
   
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)  
       Rx_mac_ra   <=0;
     else if (Packet_number_inFF==0&&Fifo_data_count<=Rx_Lwmark_pl)
       Rx_mac_ra   <=0;
     else if (Packet_number_inFF>=1||Fifo_data_count>=Rx_Hwmark_pl)
       Rx_mac_ra   <=1;
   
   
   //control Add_rd signal;
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       Add_rd      <=0;
     else if (Current_state_SYS==SYS_read&&!Dout[35])  
       Add_rd      <=Add_rd + 1;
   
   //
   always @ (posedge Reset or posedge Clk_SYS)
     if (Reset)
       Add_rd_gray         <=0;
     else 
       begin
	  Add_rd_gray[MAC_RX_FF_DEPTH-1]	<=Add_rd[MAC_RX_FF_DEPTH-1];
	  for (i=MAC_RX_FF_DEPTH-2;i>=0;i=i-1)
	    Add_rd_gray[i]			<=Add_rd[i+1]^Add_rd[i];
       end
   //
   
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       Add_wr_gray_dl1     <=0;
     else
       Add_wr_gray_dl1     <=Add_wr_gray;
   
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       Add_wr_jump_rd_pl1  <=0;
     else        
       Add_wr_jump_rd_pl1  <=Add_wr_jump;	
   
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       Add_wr_ungray       =0;
     else if (!Add_wr_jump_rd_pl1)       
       begin
	  Add_wr_ungray[MAC_RX_FF_DEPTH-1]	=Add_wr_gray_dl1[MAC_RX_FF_DEPTH-1];	
	  for (i=MAC_RX_FF_DEPTH-2;i>=0;i=i-1)
	    Add_wr_ungray[i]	=Add_wr_ungray[i+1]^Add_wr_gray_dl1[i];	
       end                    
   //empty signal gen  
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)      
       Empty   <=1;
     else if (Add_rd==Add_wr_ungray)
       Empty   <=1;
     else
       Empty   <=0;
      
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       Dout_dl1    <=0;
     else
       Dout_dl1    <=Dout; 
   
   assign  Rx_mac_data     =Dout_dl1[31:0];
   assign  Rx_mac_BE       =Dout_dl1[33:32];
   assign  Rx_mac_eop      =Dout_dl1[35];
   
   //aligned to Addr_rd 
   always @ (posedge Clk_SYS or posedge Reset) 
     if (Reset)
       Rx_mac_pa_tmp   <=0;    
     else if (Current_state_SYS==SYS_read&&!Dout[35])         
       Rx_mac_pa_tmp   <=1;
     else
       Rx_mac_pa_tmp   <=0;
   
   always @ (posedge Clk_SYS or posedge Reset) 
     if (Reset)
       Rx_mac_pa   <=0;
     else 
       Rx_mac_pa   <=Rx_mac_pa_tmp;
      
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       Rx_mac_sop_tmp      <=0;
     else if (Current_state_SYS==SYS_idle&&Next_state_SYS==SYS_read)
       Rx_mac_sop_tmp      <=1;
     else
       Rx_mac_sop_tmp      <=0;
      
   always @ (posedge Clk_SYS or posedge Reset)
     if (Reset)
       begin
          Rx_mac_sop_tmp_dl1  <=0;
          Rx_mac_sop          <=0;
       end
     else 
       begin
          Rx_mac_sop_tmp_dl1  <=Rx_mac_sop_tmp;
          Rx_mac_sop          <=Rx_mac_sop_tmp_dl1;
       end
   
   //******************************************************************************
   
   duram #(36,MAC_RX_FF_DEPTH)
     U_duram(.data_a         (Din        ), 
	     .wren_a         (Wr_en      ), 
	     .address_a      (Add_wr     ), 
	     .address_b      (Add_rd     ), 
	     .clock_a        (Clk_MAC    ), 
	     .clock_b        (Clk_SYS    ), 
	     .q_b            (Dout       ));
   
endmodule // MAC_rx_FF
