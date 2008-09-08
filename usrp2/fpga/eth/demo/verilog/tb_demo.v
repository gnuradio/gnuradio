`timescale 1ns / 1ns

module tb_demo;

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

  reg        Reset_n;
  reg        Clk_100M;
  reg        Clk_125M;

  wire       RS232_TXD;
  wire       RS232_RXD;

  wire       USB_TXD;
  wire       USB_RXD;

  //--- 10/100/1000BASE-T Ethernet PHY (MII/GMII)
  wire       PHY_RESET_n;

  wire       PHY_RXC;
  wire [7:0] PHY_RXD;
  wire       PHY_RXDV;
  wire       PHY_RXER;

  wire       PHY_GTX_CLK; // GMII only
  wire       PHY_TXC;
  wire [7:0] PHY_TXD;
  wire       PHY_TXEN;
  wire       PHY_TXER;

  wire       PHY_COL = 0;
  wire       PHY_CRS = 0;

  wire       PHY_MDC;
  wire       PHY_MDIO;

  wire [1:4] LED;

  reg [1:4]  Button = 4'b0000;

  //-------------------------------------------------------------------------
  // Local declarations
  //-------------------------------------------------------------------------

  //-------------------------------------------------------------------------
  // Instantiation of sub-modules
  //-------------------------------------------------------------------------

  //--- DUT

  demo demo(
    .Reset_n ( Reset_n  ),
    .Clk_100M( Clk_100M ),
    .Clk_125M( Clk_125M ),

    .RS232_TXD( RS232_TXD ),
    .RS232_RXD( RS232_RXD ),

    .USB_TXD( USB_TXD ),
    .USB_RXD( USB_RXD ),

    //--- 10/100/1000BASE-T Ethernet PHY (MII/GMII)
    .PHY_RESET_n( PHY_RESET_n ),

    .PHY_RXC ( PHY_RXC  ),
    .PHY_RXD ( PHY_RXD  ),
    .PHY_RXDV( PHY_RXDV ),
    .PHY_RXER( PHY_RXER ),

    .PHY_GTX_CLK( PHY_GTX_CLK ), // GMII only
    .PHY_TXC    ( PHY_TXC  ),
    .PHY_TXD    ( PHY_TXD  ),
    .PHY_TXEN   ( PHY_TXEN ),
    .PHY_TXER   ( PHY_TXER ),

    .PHY_COL( PHY_COL ),
    .PHY_CRS( PHY_CRS ),

    .PHY_MDC ( PHY_MDC  ),
    .PHY_MDIO( PHY_MDIO ),

    // Misc. I/Os
    .LED   ( LED    ),
    .Button( Button )
  );

  //-------------------------------------------------------------------------
  // MII/GMII Ethernet PHY model

  reg [2:0]  Speed = 3'b000;

  Phy_sim U_Phy_sim(
    .Gtx_clk( PHY_GTX_CLK ),
    .Rx_clk ( PHY_RXC  ),
    .Tx_clk ( PHY_TXC  ),
    .Tx_er  ( PHY_TXER ),
    .Tx_en  ( PHY_TXEN ),
    .Txd    ( PHY_TXD  ),
    .Rx_er  ( PHY_RXER ),
    .Rx_dv  ( PHY_RXDV ),
    .Rxd    ( PHY_RXD  ),
    .Crs    ( PHY_CRS  ),
    .Col    ( PHY_COL  ),
    .Speed  ( Speed    ),
    .Done   ( Done     )
  );

  //-------------------------------------------------------------------------
  // Generate all clocks & reset
  //-------------------------------------------------------------------------

  // Core master clock (100 MHz)
  initial 
    begin
      #10;
      while ( !Done )
        begin
          #5 Clk_100M = 0;
          #5 Clk_100M = 1;
        end
    end

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

  initial
    begin
      Reset_n = 0;

      #103;
      Reset_n = 1;
    end

  //--- Emulate UART Transmitter --------------------------------------------

  parameter    PRESCALER_16X = 3;
  integer      Prescaler;
  integer      TxLen = 0;
  reg [2:0]    TxState;
  integer      TxBit;
  reg [1023:0] TxMsg;
  reg          TXD;
  reg          TxDone;

  always @( negedge Reset_n or posedge Clk_100M )
    if ( ~Reset_n )
      begin
        Prescaler <= 0;
        TxState   = 0;
        TXD       = 1;
        TxBit     = 0;
        TxDone    <= 0;
      end
    else
      begin
        TxDone <= 0;

        if ( Prescaler == ((PRESCALER_16X + 1)*16 -1) )
          Prescaler <= 0;
        else
          Prescaler <= Prescaler + 1;

        if ( Prescaler==0 )
          begin
            casez ( TxState )
              0: // IDLE
                begin
                  if ( TxLen != 0 )
                    begin // Send start bit!
                      TxBit = (TxLen-1)*8;
                      TxLen = TxLen - 1;
                      TXD = 0;
                      TxState = 1;
                    end
                end

              1: // Send next data bit
                begin
                  // Send next data bit
                  TXD = TxMsg[ TxBit ];
                  TxBit = TxBit + 1;
                  if ( (TxBit % 8)==0 )
                    // Next send two stop bits
                    TxState = 2;
                end

              2: // First of two stop bits
                begin
                  TXD = 1;
                  TxState = 3;
                end

              3: // Second of two stop bits
                begin
                  TXD = 1;
                  TxState = 0;
                  if ( TxLen == 0 )
                    // Done with transmission!
                    TxDone <= 1;
                end
            endcase
          end
      end

  assign RS232_RXD = TXD;
  assign USB_RXD = 1;

  //--- Send commands to the DUT --------------------------------------------

  initial
    begin
      #10;
      while ( ~Reset_n ) #10;

      // Wait a couple of clock edges before continuing to allow
      // internal logic to get out of reset
      repeat ( 5 )
        @( posedge Clk_100M );

      // Wait for the "READY" message to complete transmission
      #60000;

      // Select 100 Mbps
      Speed = 3'b010;
      TxMsg = "W 0022 0002 ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "W 8000 8003 ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "W 8001 0011 ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "W 8002 1234 ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "W 8003 5678 ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "W 8004 9ABC ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "W 8005 DEF0 ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "W 8006 C5C0 ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "W 8007 BABE ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      TxMsg = "R 8006 ";
      TxLen = 7;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      // Enable PG!
      TxMsg = "W 1000 0001 ";
      TxLen = 12;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      // Read back that PG has been enabled!
      TxMsg = "R 1000 ";
      TxLen = 7;
      while ( ~TxDone )
        @( posedge Clk_100M );

      #50000;

      #50000;

      Done = 1; #10;

      $stop;
    end

  //--- Directly accesses a register on the internal Wishbone bus, bypassing the UART interface

  task WrReg;
    input [15:0] Reg;
    input [15:0] Data;

    begin
    end
  endtask

endmodule
