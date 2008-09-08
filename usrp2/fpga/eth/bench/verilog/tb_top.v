`timescale 1 ns / 1 ps

//////////////////////////////////////////////////////////////////////
////                                                              ////
////  tb_top.v                                                    ////
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
// $Log: tb_top.v,v $
// Revision 1.3  2006/01/19 14:07:51  maverickist
// verification is complete.
//
// Revision 1.2  2005/12/16 06:44:13  Administrator
// replaced tab with space.
// passed 9.6k length frame test.
//
// Revision 1.1.1.1  2005/12/13 01:51:44  Administrator
// no message
// 

module tb_top;

  //-------------------- Instantiate Xilinx glbl module ----------------------
  // - this is needed to get ModelSim to work because e.g. I/O buffer models
  //   refer directly to glbl.GTS and similar signals

  wire GSR;
  wire GTS;
  xlnx_glbl glbl( .GSR( GSR ), .GTS( GTS ) );

  reg  VLOG_ExitSignal = 0;
  reg  Done = 0;
  reg  Error = 0;

  //-------------------------------------------------------------------------

  // System signals
  wire        Reset;
  reg         Clk_125M;
  reg         Clk_user;

  reg         RST_I;
  reg         CLK_I;
  reg         STB_I;
  reg         CYC_I;
  reg [6:0]   ADR_I;
  reg         WE_I;
  reg [15:0]  DAT_I;
  wire [15:0] DAT_O;
  wire        ACK_O;

  // User interface (Rx)
  wire        Rx_mac_ra;
  reg         Rx_mac_rd = 0;
  wire [31:0] Rx_mac_data;
  wire [1:0]  Rx_mac_BE;
  wire        Rx_mac_pa;
  wire        Rx_mac_sop;
  wire        Rx_mac_err;
  wire        Rx_mac_eop;

  // User interface (Tx)
  wire        Tx_mac_wa;
  reg         Tx_mac_wr = 0;
  reg  [31:0] Tx_mac_data = 32'bx;
  reg  [1:0]  Tx_mac_BE = 2'bx;
  reg         Tx_mac_sop = 1'bx;
  reg         Tx_mac_eop = 1'bx;

  // PHY interface (GMII/MII)
  wire        Gtx_clk;
  wire        Rx_clk;
  wire        Tx_clk;
  wire        Tx_er;
  wire        Tx_en;
  wire [7:0]  Txd;
  wire        Rx_er;
  wire        Rx_dv;
  wire [7:0]  Rxd;
  wire        Crs;
  wire        Col;

  // PHY int host interface

  wire        Line_loop_en;
  wire [2:0]  Speed;

  // MDIO interface
  wire        Mdio; // MII Management Data In
  wire        Mdc;  // MII Management Data Clock

  //-------------------------------------------------------------------------

  // Generate generic reset signal from Wishbone specific one
  assign Reset = RST_I;

  MAC_top U_MAC_top(
    // System signals
    .Clk_125M    ( Clk_125M ),
    .Clk_user    ( Clk_user ),
    .Speed       ( Speed    ),

    // Wishbone compliant core host interface
    .RST_I      ( RST_I ),
    .CLK_I      ( CLK_I ),
    .STB_I      ( STB_I ),
    .CYC_I      ( CYC_I ),
    .ADR_I      ( ADR_I ),
    .WE_I       ( WE_I  ),
    .DAT_I      ( DAT_I ),
    .DAT_O      ( DAT_O ),
    .ACK_O      ( ACK_O ),

    // User (packet) interface
    .Rx_mac_ra  ( Rx_mac_ra   ),    
    .Rx_mac_rd  ( Rx_mac_rd   ),
    .Rx_mac_data( Rx_mac_data ),
    .Rx_mac_BE  ( Rx_mac_BE   ),
    .Rx_mac_pa  ( Rx_mac_pa   ),
    .Rx_mac_sop ( Rx_mac_sop  ),
    .Rx_mac_err ( Rx_mac_err  ),
    .Rx_mac_eop ( Rx_mac_eop  ),

    .Tx_mac_wa  ( Tx_mac_wa   ),
    .Tx_mac_wr  ( Tx_mac_wr   ),
    .Tx_mac_data( Tx_mac_data ),
    .Tx_mac_BE  ( Tx_mac_BE   ),
    .Tx_mac_sop ( Tx_mac_sop  ),
    .Tx_mac_eop ( Tx_mac_eop  ),

    // PHY interface (GMII/MII)
    .Gtx_clk    ( Gtx_clk ),
    .Rx_clk     ( Rx_clk  ),
    .Tx_clk     ( Tx_clk  ),
    .Tx_er      ( Tx_er   ),
    .Tx_en      ( Tx_en   ),
    .Txd        ( Txd     ),
    .Rx_er      ( Rx_er   ),
    .Rx_dv      ( Rx_dv   ),
    .Rxd        ( Rxd     ),
    .Crs        ( Crs     ),
    .Col        ( Col     ),

    // MDIO interface (to PHY)
    .Mdio       ( Mdio ),
    .Mdc        ( Mdc  )
  );

  reg [15:0] InjectError;
  reg        InjectErrorDone;
  reg [15:0] TxError;
  wire       Tx_er_Modified;
  wire       Tx_en_Modified;
  wire [7:0] Txd_Modified;

  Phy_sim U_Phy_sim(
    .Gtx_clk( Gtx_clk ),
    .Rx_clk ( Rx_clk  ),
    .Tx_clk ( Tx_clk  ),
    .Tx_er  ( Tx_er_Modified ),
    .Tx_en  ( Tx_en_Modified ),
    .Txd    ( Txd_Modified   ),
    .Rx_er  ( Rx_er   ),
    .Rx_dv  ( Rx_dv   ),
    .Rxd    ( Rxd     ),
    .Crs    ( Crs     ),
    .Col    ( Col     ),
    .Speed  ( Speed   ),
    .Done   ( Done    )
  );

  integer TxTrackPreAmble;

  always @( posedge Reset or posedge Tx_clk )
    if ( Reset )
      TxTrackPreAmble <= 0;
    else
      if ( ~Tx_en )
        TxTrackPreAmble <= 0;
      else
        TxTrackPreAmble <= TxTrackPreAmble + 1;

  // Asserted after the Destination MAC address in the packet
  wire TxInPayload = Tx_en & (TxTrackPreAmble > (7+6));
  assign Tx_er_Modified = Tx_er ^ ( TxError[9] & TxInPayload );
  assign Tx_en_Modified = Tx_en ^ ( TxError[8] & TxInPayload );
  assign Txd_Modified = Txd ^ ( TxError[7:0] & {8{TxInPayload}} );

  always @( posedge Reset or posedge Tx_clk )
    if ( Reset )
      begin
        InjectError <= 0;
        InjectErrorDone <= 0;
        TxError <= 'b0;
      end
    else
      if ( InjectError )
        begin
          TxError <= InjectError;
          InjectError <= 0;
          InjectErrorDone <= TxInPayload;
        end
      else if ( TxInPayload || InjectErrorDone )
        begin
          TxError <= 8'h00;
          InjectErrorDone <= 0;
        end

  //-------------------------------------------------------------------------
  // Track pause on Tx interface

  reg TxEnSeenOnce;  
  integer TxTrackPause;

  always @( posedge Reset or posedge Tx_clk )
    if ( Reset )
      begin
        TxEnSeenOnce <= 0;
        TxTrackPause <= 0;
      end
    else
      if ( Tx_en )
        begin
          if ( TxEnSeenOnce && (TxTrackPause >= 64) ) // 512 bits
            $display( "IDLE period on Tx interface ended after %0d Tx clocks (%0d bits, tick ~ %0d)",
                      TxTrackPause,
                      (Speed == 4) ? TxTrackPause*8     : TxTrackPause*4,
                      (Speed == 4) ? TxTrackPause*8/512 : TxTrackPause*4/512 );
          TxEnSeenOnce <= 1;
          TxTrackPause <= 0;
        end
      else
        TxTrackPause <= TxTrackPause + 1;

  //-------------------------------------------------------------------------
  // Host access routines (register read & write)
  //-------------------------------------------------------------------------

  task HostInit;
    begin
      RST_I <= 1;

      STB_I <= 0;
      CYC_I <= 0;
      ADR_I <= 'b0;
      WE_I  <= 0;
      DAT_I <= 'b0;

      #100 RST_I <= 0;

      // Wait a couple of clock edges before continuing to allow
      // internal logic to get out of reset
      repeat( 2 )
        @( posedge CLK_I );
    end
  endtask
  
  task HostWriteReg;
    input [6:0]  Addr;
    input [15:0] Data;
    begin
      @( posedge CLK_I );
      ADR_I <= Addr;
      DAT_I <= Data;
      WE_I  <= 1;
      STB_I <= 1;
      CYC_I <= 1;

      @( posedge CLK_I );

      while ( ~ACK_O )
        @( posedge CLK_I );

      STB_I <= 0;
      CYC_I <= 0;
    end
  endtask
  
  task HostReadReg;
    input [6:0]   Addr;
    output [15:0] Data;
    begin
      @( posedge CLK_I );
      ADR_I <= Addr;
      WE_I  <= 0;
      STB_I <= 1;
      CYC_I <= 1;

      @( posedge CLK_I );

      while ( ~ACK_O )
        @( posedge CLK_I );

      Data = DAT_O;
      STB_I <= 0;
      CYC_I <= 0;
    end
  endtask

  //-------------------------------------------------------------------------
  // User interface access routines (packet Tx and Rx)
  //-------------------------------------------------------------------------

  `define FIFOSIZE 10000

  integer FIFO_WrPtr = 0;
  integer FIFO_RdPtr = 0;
  integer FIFO_ElementCount = 0;
  reg [35:0] FIFO_Data[0:`FIFOSIZE];

  function FIFO_Empty;
    input Dummy;
    begin
      if ( FIFO_ElementCount > 0 )
        FIFO_Empty = 0;
      else
        FIFO_Empty = 1;
    end
  endfunction

  function FIFO_Full;
    input Dummy;
    begin
      if ( FIFO_ElementCount < `FIFOSIZE )
        FIFO_Full = 0;
      else
        FIFO_Full = 1;
    end
  endfunction

  task FIFO_Wr;
    input [35:0] Data;

    begin
      if ( !FIFO_Full(0) )
        begin
          FIFO_Data[ FIFO_WrPtr ] = Data;
          FIFO_WrPtr = (FIFO_WrPtr + 1) % `FIFOSIZE;
          FIFO_ElementCount = FIFO_ElementCount + 1;
        end
      else
        begin
          $display( "ERROR: FIFO_Wr() - FIFO overflow!" );
          Error = 1;
          $finish;
        end
    end
  endtask

  task FIFO_Rd;
    output [35:0] Data;

    begin
      if ( !FIFO_Empty(0) )
        begin
          Data = FIFO_Data[ FIFO_RdPtr ];
          FIFO_RdPtr = (FIFO_RdPtr + 1) % `FIFOSIZE;
          FIFO_ElementCount = FIFO_ElementCount - 1;
        end
      else
        begin
          $display( "ERROR: FIFO_Rd() - Reading from empty FIFO!" );
          Error = 1;
          $finish;
        end
    end
  endtask

  //-------------------------------------------------------------------------

  `define TXDATALEN 8000
  reg [7:0] TxData[0:`TXDATALEN-1];
  reg [7:0] TxAltData[0:`TXDATALEN-1];

  // By default change payload after Ethernet Header
  reg [15:0] TxHeaderLength = 14;

  real    TxStartTime;
  integer TxPacketCount = 0;
  integer TxByteCount;

  task SendPacket;
    input [15:0] Length;
    // 0: Don't write to FIFO, 1: Write to FIFO, 2: Write Alternate to FIFO, 3: Write IGNORE to FIFO
    input [1:0]  Wr2FIFO;

    reg [15:0] Counter;
    integer    TxIndex;
    integer    i;

    reg [31:0] Tx_fifo_data;

    begin
      @( posedge Clk_user ); #1;

      TxPacketCount = TxPacketCount + 1;
      TxByteCount = TxByteCount + Length;

      Counter=Length;
      TxIndex = 0;
      Tx_mac_sop = 1; // First time

      if ( TxStartTime == 0 )
        TxStartTime = $realtime;

      while ( Counter>0 )
        begin
          while ( !Tx_mac_wa )
            begin
              Tx_mac_wr = 0;
              @( posedge Clk_user ); #1;
            end

          Tx_mac_data[31:24] = TxData[ TxIndex   ];
          Tx_mac_data[23:16] = TxData[ TxIndex+1 ];
          Tx_mac_data[15:8]  = TxData[ TxIndex+2 ];
          Tx_mac_data[ 7:0]  = TxData[ TxIndex+3 ];

          // Default take data from regular tx buffer
          Tx_fifo_data = Tx_mac_data;
          if ( Wr2FIFO==2 )
            begin
              // Put content of Alternate Tx buffer on Rx expectancy queue
              if ( (TxIndex+0)<TxHeaderLength )
                Tx_fifo_data[31:24] = TxAltData[ TxIndex ];
              if ( (TxIndex+1)<TxHeaderLength )
                Tx_fifo_data[23:16] = TxAltData[ TxIndex+1 ];
              if ( (TxIndex+2)<TxHeaderLength )
                Tx_fifo_data[15:8]  = TxAltData[ TxIndex+2 ];
              if ( (TxIndex+3)<TxHeaderLength )
                Tx_fifo_data[ 7:0]  = TxAltData[ TxIndex+3 ];
            end

          for ( i=0; i<4; i=i+1 )
            begin
              if ( TxIndex >= TxHeaderLength )
                TxData[ TxIndex ] = TxData[ TxIndex ] + 1;
              TxIndex = TxIndex+1;
            end

          if ( Counter<=4 )
            begin
              // Indicate how many bytes are valid
              if ( Counter==4 )
                Tx_mac_BE = 2'b00;
              else
                Tx_mac_BE = Counter;
              Tx_mac_eop = 1;
            end
          else
            begin
              Tx_mac_BE = 2'b00;
              Tx_mac_eop = 0;
            end

          casez ( Wr2FIFO )
            1,
            2: FIFO_Wr( { Tx_mac_sop, Tx_mac_eop, Tx_mac_BE, Tx_fifo_data } );
            3: // Ignore
              begin
                FIFO_Wr( { 2'b11, 2'b00, 32'h00000000 } );
                Wr2FIFO = 0;
              end
          endcase

          Tx_mac_wr = 1;

          if ( Counter >= 4 )
            Counter = Counter - 4;
          else
            Counter = 0;
          @( posedge Clk_user ); #1;
          Tx_mac_sop = 0;
        end

      Tx_mac_sop = 1'bx;
      Tx_mac_eop = 1'bx;
      Tx_mac_wr = 0;
      Tx_mac_data = 32'bx;
      Tx_mac_BE   = 2'bx;
    end
  endtask

  //-------------------------------------------------------------------------

  reg Negate_Rx_mac_rd;

  always @( posedge Clk_user or posedge Reset )
    if ( Reset )
      Rx_mac_rd <= 0;
    else if ( Rx_mac_ra & ~Negate_Rx_mac_rd )
      Rx_mac_rd <= 1;
    else
      Rx_mac_rd <= 0;

  real    RxStartTime;
  integer RxPacketCount;
  integer RxByteCount;

  reg     InPacket;
  integer RxPacketLength;
  reg     IgnoreUntilNextERR;

   always @( posedge Clk_user or posedge Reset )
    if ( Reset )
      begin
        InPacket = 0;
        RxPacketCount = 0;
        Negate_Rx_mac_rd <= 0;
        IgnoreUntilNextERR = 0;
      end
    else
      begin
        Negate_Rx_mac_rd <= 0;

        if ( Rx_mac_pa )
          begin : RxWord
            reg [35:0] RxData;
            reg [35:0] Expected;
            reg [35:0] Mask;

            RxData = { Rx_mac_sop, Rx_mac_eop, Rx_mac_BE, Rx_mac_data };
            casez ( Rx_mac_BE )
              2'b01:   Mask = 36'hfff000000;
              2'b10:   Mask = 36'hfffff0000;
              2'b11:   Mask = 36'hfffffff00;
              default: Mask = 36'hfffffffff;
            endcase

            // Retrieve expected packet data

            if ( !IgnoreUntilNextERR )
              begin
                FIFO_Rd( Expected );
                if ( Expected[35] & Expected[34] )
                  begin
                    // Both SOP & EOP are asserted in expectancy data
                    // - this means that we should ignore all data received until next EOP
                    $display( "The payload of this packet will be IGNORED - and an ERROR must be signalled!" );
                    IgnoreUntilNextERR = 1;
                  end
              end
            if ( IgnoreUntilNextERR )
              Mask = 36'h000000000;

            //$display( "DEBUG: RxData=0x%0x, Expected=0x%0x", RxData, Expected );

            if ( (RxData & Mask) !== (Expected & Mask) )
              begin
                $display( "ERROR: Receiving unexpected packet data: Got 0x%0x, expected 0x%0x (Mask=0x%0x)",
                          RxData, Expected, Mask );
                Error = 1;
              end

            if ( InPacket )
              begin
                if ( Rx_mac_eop )
                  begin
                    // Ensure Rx_mac_rd is negated for one clock
                    Negate_Rx_mac_rd <= 1;
                    if ( Rx_mac_BE==2'b00 )
                      RxPacketLength = RxPacketLength + 4;
                    else
                      RxPacketLength = RxPacketLength + Rx_mac_BE;
                    $display( "Rx packet #%0d of length %0d ends",
                              RxPacketCount,
                              RxPacketLength );
                    RxPacketCount = RxPacketCount + 1;
                    RxByteCount = RxByteCount + RxPacketLength;
                    InPacket = 0;
                  end
                else
                  RxPacketLength = RxPacketLength + 4;
              end
            else
              begin
                if ( Rx_mac_sop )
                  begin
                    RxPacketLength = 4;
                    $display( "Rx packet #%0d begins: 0x%08x", RxPacketCount, Rx_mac_data );
                    InPacket = 1;
                    if ( RxStartTime == 0 )
                      RxStartTime = $realtime;
                  end
                else
                  begin
                    $display( "ERROR: Unexpectedly reading from Rx FIFO while not receiving a packet!" );
                    Error = 1;
                  end
              end

            if ( Rx_mac_err )
              begin
                if ( !Rx_mac_eop )
                  begin
                    $display( "ERROR: Rx_mac_err was asserted without Rx_mac_eop also being asserted!" );
                    Error = 1;
                  end
                if ( IgnoreUntilNextERR )
                  $display( "Info: Rx_mac_err was asserted as expected!" );
                else
                  begin
                    $display( "ERROR: Rx_mac_err was unexpectedly asserted!" );
                    Error = 1;
                  end
                IgnoreUntilNextERR = 0;
              end
          end
      end

  //-------------------------------------------------------------------------
  // Script handling
  //-------------------------------------------------------------------------

  integer PC;

  task ScriptWriteReg;
    input [15:0] Addr;
    input [15:0] Data;

    begin
      $display( "WriteReg( 0x%04x, 0x%04x )", Addr, Data );
      HostWriteReg( Addr, Data );
    end
  endtask

  task ScriptReadReg;
    input [15:0] Addr;

    reg [15:0] Data;

    begin
      $write( "ReadReg( 0x%04x ): ", Addr );
      HostReadReg( Addr, Data );
      $display( "0x%04x", Data );
    end
  endtask

  task ScriptReadRegAndMatch;
    input [15:0] Addr;
    input [15:0] Data;
    input [15:0] Mask;

    reg [15:0] Read;

    begin
      $write( "ReadRegAndMatch( 0x%04x, 0x%04x, 0x%04x ): ", Addr, Data, Mask );

      HostReadReg( Addr, Read );
      $display( "0x%04x, masked=0x%04x", Read, Read & Mask );

      if ( Data !== (Read & Mask) )
        begin
          $display( "Error: Unexpected data read" );
          Error = 1;
        end
    end
  endtask

  integer RxExpectPacketCount = 0;

  task ScriptSendPacket;
    input [15:0] Length;
    // 0: Don't receive, 1: Receive & match, 2: Receive & match alternate, 3: Receive & ignore
    input [1:0]  ExpectToRx;

    begin
      $display( "ScriptSendPacket( 0x%04x, %0d )", Length, ExpectToRx );
      SendPacket( Length, ExpectToRx );
      if ( ExpectToRx != 0 )
        RxExpectPacketCount = RxExpectPacketCount + 1;
    end
  endtask

  `define SCRIPTLEN 10000
  integer i;
  reg [7:0] Script[0:`SCRIPTLEN-1];

  function [15:0] Get16bit;
    input Dummy;

    reg [15:0] Data;

    begin
      Data[15:8] = Script[PC];
      Data[7:0]  = Script[PC+1];
      PC = PC+2;

      Get16bit = Data;
    end
  endfunction

  task ExecuteScript;

    reg [7:0] OpCode;
    reg [15:0] Addr;
    reg [15:0] Data;
    reg [15:0] Length;
    reg [15:0] Count;
    reg [15:0] Mask;

    reg ScriptDone;

    begin
      ScriptDone = 0;
      Error = 0;
      PC = 0;
      
      while ( !ScriptDone )
        begin
          OpCode = Script[PC];
          //$write( "PC=%0d, OpCode=%02x: ", PC, OpCode );
          PC = PC+1;
          
          casez ( OpCode )
            8'h00: // NOP
              begin
//                $display( "NOP" );
                #10;
              end
            8'h01: // Write
              begin
                Addr = Get16bit(i);
                Data = Get16bit(i);
                ScriptWriteReg( Addr, Data );
              end
            8'h02: // Read
              begin
                Addr = Get16bit(i);
                ScriptReadReg( Addr );
              end
            8'h03: // Read & match
              begin
                Addr = Get16bit(i);
                Data = Get16bit(i);
                Mask = Get16bit(i);
                ScriptReadRegAndMatch( Addr, Data, Mask );
              end

            8'h0f: // Delay
              begin
                Count = Get16bit(i);
                $display( "Delay %0d", Count );
                while ( Count > 0 )
                  begin
                    #10;
                    Count = Count - 1;
                  end
              end

            8'h10: // Setup Tx Data
              begin
                Addr   = Get16bit(i);
                Length = Get16bit(i);
                $write( "TxData( 0x%04x ), length=%0d: ", Addr, Length );
                while ( Length != 0 )
                  begin
                    TxData[Addr] = Script[PC];
                    $write( " 0x%02x", Script[PC] );
                    PC = PC + 1;
                    Addr = Addr + 1;
                    Length = Length - 1;
                  end
                $display( "" );
              end

            8'h11: // Setup Alternative Tx Data
              begin
                Addr   = Get16bit(i);
                Length = Get16bit(i);
                $write( "TxAltData( 0x%04x ), length=%0d: ", Addr, Length );
                while ( Length != 0 )
                  begin
                    TxAltData[Addr] = Script[PC];
                    $write( " 0x%02x", Script[PC] );
                    PC = PC + 1;
                    Addr = Addr + 1;
                    Length = Length - 1;
                  end
                $display( "" );
              end

            8'h20: // Transmit packet - and put it on Rx expectancy queue
              begin
                Length = Get16bit(i); // Length in bytes
                Count  = Get16bit(i); // Number of times
                while ( Count != 0 )
                  begin
                    ScriptSendPacket( Length, 1 );
                    Count = Count - 1;
                  end
              end

            8'h21: // Transmit packet - but DON'T put it on Rx expectancy queue
              begin
                Length = Get16bit(i); // Length in bytes
                Count  = Get16bit(i); // Number of times
                while ( Count != 0 )
                  begin
                    ScriptSendPacket( Length, 0 );
                    Count = Count - 1;
                  end
              end

            8'h22: // Wait
              begin : OpCode22
                reg NoTimeOut;
                Count = Get16bit(i); // Timeout in ns
                if ( Count==0 )
                  NoTimeOut = 1;
                else
                  NoTimeOut = 0;

                $display( "Waiting for # of Rx packets = # of Tx packets..." );
                $display( "Timeout = %0d ns - Current # Rx =%0d, Expected=%0d",
                          Count, RxPacketCount, RxExpectPacketCount );

                while( (NoTimeOut || (Count != 0)) && ( RxExpectPacketCount != RxPacketCount ) && !Error )
                  begin
                    #1;
                    if ( !NoTimeOut )
                      Count = Count - 1;
                    //$display( "NoTimeOut=%0d, Count=%0d", NoTimeOut, Count );
                  end

                if ( !Error )
                  if ( RxExpectPacketCount != RxPacketCount )
                    begin
                      $display( "ERROR: Timeout waiting for Rx packet(s)!" );
                      ScriptDone = 1;
                      Error = 1;
                    end
                  else
                    $display( "...Done waiting (time remaining = %0d ns)!", Count );
              end

            8'h23: // Inject bit error in Tx packet
              begin
                InjectError = Get16bit(i); // Get bit error pattern
                $display( "Injecting a single bit-error in Tx packet: TxEr=%0d, TxEn=%0d, TxD=0x%02h (0x%03h)",
                          InjectError[9], InjectError[8], InjectError[7:0], InjectError );
              end

            8'h24: // Store internally generated PAUSE frame in Rx expect queue
              begin
                Count = Get16bit(i); // Timeout in ns
                $display( "Generating PAUSE frame (tick=%0d) on Rx expect queue", Count );
                RxExpectPacketCount = RxExpectPacketCount + 1;
                FIFO_Wr( { 1'b1, 1'b0, 2'b00, 32'h0180c200 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 16'h0001, 16'h0000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h88080001 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, Count, 16'h0000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b0, 2'b00, 32'h00000000 } );
                FIFO_Wr( { 1'b0, 1'b1, 2'b00, 32'h00000000 } );
              end

            8'h25: // Transmit packet - and indicate that it must be IGNORED upon reception
              begin
                Length = Get16bit(i); // Length in bytes
                Count  = Get16bit(i); // Number of times
                while ( Count != 0 )
                  begin
                    ScriptSendPacket( Length, 3 );
                    Count = Count - 1;
                  end
              end

            8'h26: // Transmit packet - and put it on expectancy queue with Alternate header!
              begin
                Length = Get16bit(i); // Length in bytes
                Count  = Get16bit(i); // Number of times
                while ( Count != 0 )
                  begin
                    ScriptSendPacket( Length, 2 );
                    Count = Count - 1;
                  end
              end

            8'hff: // Halt
              begin
                $display( "HALT" );
                ScriptDone = 1;
                Done = 1;
              end

            default: // Unknown opcode
              begin
                $display( "Unknown instruction encountered @ PC=%0d: OpCode=0x%02x", PC-1, OpCode );
                Error = 1;
              end

          endcase

          if ( Error )
            begin
              ScriptDone = 1;
              Done = 1;
            end
        end

      if ( Error )
        $display( "ERROR: Test failed!");
      else
        begin : ScriptSuccess
          real TxTimeElapsed;
          real RxTimeElapsed;
          real ReferenceTime;

          ReferenceTime = $realtime;
          #1;
          ReferenceTime = $realtime - ReferenceTime;

          TxTimeElapsed = $realtime - TxStartTime;
          RxTimeElapsed = $realtime - RxStartTime;

          $display( "TxStartTime=%0e, Now=%0e", TxStartTime, $realtime );
          $display( "RxStartTime=%0e, Now=%0e", RxStartTime, $realtime );
          
          $display( "Tx stats: %0d packet(s) send, total of %0d bytes in %0e ns ~ %1.2f Mbps",
                    TxPacketCount, TxByteCount, TxTimeElapsed, TxByteCount*8*1e3/TxTimeElapsed );
          $display( "Rx stats: %0d packet(s) received, total of %0d bytes in %0e ns ~ %1.2f Mbps",
                    RxPacketCount, RxByteCount, RxTimeElapsed, RxByteCount*8*1e3/RxTimeElapsed );
          $display( "Test succeeded!");
        end

    end
  endtask

  //-------------------------------------------------------------------------
  // Generate all clocks
  //-------------------------------------------------------------------------

  // GMII master clock (125 MHz)
  initial 
    begin
      #10;
      while ( !Done )
        begin
          #4 Clk_125M = 0;
          #4 Clk_125M = 1;
        end
    end

  // User (packet) interface clock (100 MHz)
  initial 
    begin
      #10;
      while ( !Done )
        begin
          #5 Clk_user = 0;
          #5 Clk_user = 1;
        end
    end

  // Wishbone host interface clock (50 MHz)
  initial
    begin
      #10;
      while ( !Done )
        begin
          #10 CLK_I = 0;
          #10 CLK_I = 1;
        end
    end

  //-------------------------------------------------------------------------

  initial
    begin
      if ( $test$plusargs( "vcd" ) )
        begin
          $display( "Turning VCD data dump on" );
          $dumpfile();
          $dumpvars( 0 ); // Dump all signals in entire design
        end
    end

  //-------------------------------------------------------------------------

  reg [1023:0] ScriptFile;

  initial
    begin
      HostInit;

      TxStartTime = 0;
      RxStartTime = 0;
      TxByteCount = 0;
      RxByteCount = 0;

      for ( i=0; i<`TXDATALEN; i=i+1 )
        TxData[i] = (i & 8'hff);

      // Fill script memory with HALTs
      for ( i=0; i<`SCRIPTLEN; i=i+1 )
        Script[i] = 8'hff;

      if ( !$value$plusargs( "script=%s", ScriptFile ) )
        begin
          $display( "Using default script file" );
          ScriptFile = "test.scr";
        end

      $readmemh( ScriptFile, Script );

//      for ( i=0; i<40; i=i+1 )
//        $display( "Script[%0d]=0x%02x", i, Script[i] );

      #10;

      ExecuteScript;
    end

endmodule
