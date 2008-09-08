//////////////////////////////////////////////////////////////////////
////                                                              ////
////  User_input_sim.v                                            ////
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
// $Log: User_int_sim.v,v $
// Revision 1.3  2006/11/17 17:53:07  maverickist
// no message
//
// Revision 1.2  2006/01/19 14:07:50  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/13 12:15:35  Administrator
// no message
//
// Revision 1.1.1.1  2005/12/13 01:51:44  Administrator
// no message
// 
module User_int_sim (
  Reset,
  Clk_user,
  CPU_init_end,

  Rx_mac_ra,
  Rx_mac_rd,
  Rx_mac_data,
  Rx_mac_BE,
  Rx_mac_pa,
  Rx_mac_sop,
  Rx_mac_eop,

  Tx_mac_wa,
  Tx_mac_wr,
  Tx_mac_data,
  Tx_mac_BE,
  Tx_mac_sop,
  Tx_mac_eop
);

  input         Reset;
  input         Clk_user;
  input         CPU_init_end;

  input         Rx_mac_ra;
  output        Rx_mac_rd;
  input [31:0]  Rx_mac_data;
  input [1:0]   Rx_mac_BE;
  input         Rx_mac_pa;
  input         Rx_mac_sop;
  input         Rx_mac_eop;

  input             Tx_mac_wa;
  output reg        Tx_mac_wr;
  output reg [31:0] Tx_mac_data;
  output reg [1:0]  Tx_mac_BE; // Big endian
  output reg        Tx_mac_sop;
  output reg        Tx_mac_eop;

//////////////////////////////////////////////////////////////////////
// Internal signals
//////////////////////////////////////////////////////////////////////

  reg [4:0]     operation;
  reg [31:0]    data;
  reg           Rx_mac_rd;
  reg           Start_tran;

//////////////////////////////////////////////////////////////////////
//generate Tx user data
//////////////////////////////////////////////////////////////////////

  initial
    begin
      operation = 0;
      data      = 0;
    end

  task SendPacket;
    input [15:0] Length;
    input [7:0]  StartByte;

    reg [15:0] Counter;
    reg [7:0]  TxData;

    begin
      Counter=Length;
      TxData = StartByte;
      Tx_mac_sop = 1; // First time
      while ( Counter>0 )
        begin
          while ( !Tx_mac_wa )
            begin
              Tx_mac_wr = 0;
              @( posedge Clk_user );
            end

          Tx_mac_data[31:24] = TxData;
          Tx_mac_data[23:16] = TxData+1;
          Tx_mac_data[15:8]  = TxData+2;
          Tx_mac_data[ 7:0]  = TxData+3;
          TxData = TxData+4;
          if ( Counter<=4 )
            begin
              // Indicate how many bytes are valid
              if ( Counter==4 )
                Tx_mac_BE = 2'b00;
              else
                Tx_mac_BE = Counter;
              Tx_mac_eop = 1;
            end
          Tx_mac_wr = 1;

          if ( Counter >= 4 )
            Counter = Counter - 4;
          else
            Counter = 0;
          @( posedge Clk_user );
          Tx_mac_sop = 0;
        end

      Tx_mac_eop = 0;
      Tx_mac_wr = 0;
      Tx_mac_data = 32'h0;
      Tx_mac_BE   = 2'b00;
    end
  endtask

  always @( posedge Clk_user or posedge Reset )
    if (Reset)
      Start_tran <= 0;
    else if (Tx_mac_eop && !Tx_mac_wa)  
      Start_tran <= 0;     
    else if (Tx_mac_wa)
      Start_tran <= 1;

  always @(posedge Clk_user)
    if (Tx_mac_wa && CPU_init_end)
      /* $ip_32W_gen("../data/config.ini",operation,data); */
      ;
    else
      begin
        operation <= 0;
        data      <= 0;
      end

  initial
    begin
      Tx_mac_sop = 0;
      Tx_mac_eop = 0;
      Tx_mac_wr  = 0;
      Tx_mac_data = 32'h0;
      Tx_mac_BE   = 2'b00;

      #100;
      while ( Reset )
        @( posedge Clk_user );

      @( posedge Clk_user );

      while ( !CPU_init_end )
        @( posedge Clk_user );

      SendPacket( 64, 8'h11 );
      repeat( 20 )
        @( posedge Clk_user );
      SendPacket( 1500, 8'h12 );
        
    end

//  assign Tx_mac_data = data;
//  assign Tx_mac_wr   = operation[4];
//  assign Tx_mac_sop  = operation[3];
//  assign Tx_mac_eop  = operation[2];
//  assign Tx_mac_BE   = operation[1:0];

//////////////////////////////////////////////////////////////////////
//verify Rx user data
//////////////////////////////////////////////////////////////////////

  always @ (posedge Clk_user or posedge Reset)
    if (Reset)
      Rx_mac_rd <= 0;
    else if (Rx_mac_ra)
      Rx_mac_rd <= 1;
    else
      Rx_mac_rd <= 0;

  always @ (posedge Clk_user )
    if (Rx_mac_pa)    
      /* $ip_32W_check( Rx_mac_data,
       {Rx_mac_sop,Rx_mac_eop,Rx_mac_eop?Rx_mac_BE:2'b0});
       */
      ;

endmodule
