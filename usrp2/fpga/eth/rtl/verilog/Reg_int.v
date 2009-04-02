module Reg_int (
  // Wishbone compliant core host interface
  input             CLK_I, // Wishbone interface clock (nominally 50 MHz)
  input             RST_I, // Active high (async) reset of the Wishbone interface
  input             STB_I, // Active high module-select
  input             CYC_I, // Active high cycle-enable
  input  [6:0]      ADR_I, // Module register address
  input             WE_I,  // Active high for writes, low for reads
  input  [31:0]     DAT_I, // Write data
  output reg [31:0] DAT_O, // Read data
  output reg        ACK_O, // Acknowledge output – single high pulse

  // Tx host interface 
  output [4:0]      Tx_Hwmark,
  output [4:0]      Tx_Lwmark,   
  output            MAC_tx_add_en,
  output            FullDuplex,
  output [3:0]      MaxRetry,
  output [5:0]      IFGset,
  output [7:0]      MAC_tx_add_prom_data,
  output [2:0]      MAC_tx_add_prom_add,
  output            MAC_tx_add_prom_wr,

  // Rx host interface     
  output            MAC_rx_add_chk_en,
  output [7:0]      MAC_rx_add_prom_data,
  output [2:0]      MAC_rx_add_prom_add,
  output            MAC_rx_add_prom_wr,
  output            broadcast_filter_en,
  output [15:0]     broadcast_bucket_depth,
  output [15:0]     broadcast_bucket_interval,
  output            RX_APPEND_CRC,
  output [4:0]      Rx_Hwmark,
  output [4:0]      Rx_Lwmark,
  output            CRC_chk_en,
  output [5:0]      RX_IFG_SET,
  output [15:0]     RX_MAX_LENGTH, // Default 1518
  output [6:0]      RX_MIN_LENGTH, // Default 64

  // Flow control settings
  output            pause_frame_send_en,
  output [15:0]     pause_quanta_set,
  output            tx_pause_en,
  output [15:0]     fc_hwmark,
  output [15:0]     fc_lwmark,
  output [15:0]     fc_padtime,
		
  // RMON host interface
  output [5:0]      CPU_rd_addr,
  output            CPU_rd_apply,
  input             CPU_rd_grant,
  input [31:0]      CPU_rd_dout,

  //Phy int host interface     
  output            Line_loop_en,
  output [2:0]      Speed,

  //MII to CPU 
  output [7:0]      Divider,            // Divider for the host clock
  output [15:0]     CtrlData,           // Control Data (to be written to the PHY reg.)
  output [4:0]      Rgad,               // Register Address (within the PHY)
  output [4:0]      Fiad,               // PHY Address
  output            NoPre,              // No Preamble (no 32-bit preamble)
  output            WCtrlData,          // Write Control Data operation
  output            RStat,              // Read Status operation
  output            ScanStat,           // Scan Status operation
  input             Busy,               // Busy Signal
  input             LinkFail,           // Link Integrity Signal
  input             Nvalid,             // Invalid Status (qualifier for the valid scan result)
  input [15:0]      Prsd,               // Read Status Data (data read from the PHY)
  input             WCtrlDataStart,     // This signals resets the WCTRLDATA bit in the MIIM Command register
  input             RStatStart,         // This signal resets the RSTAT BIT in the MIIM Command register
  input             UpdateMIIRX_DATAReg // Updates MII RX_DATA register with read data
);

  // New registers for controlling the MII interface
  wire [8:0]  MIIMODER;
  reg  [2:0]  MIICOMMAND;
  wire [12:0] MIIADDRESS;
  wire [15:0] MIITX_DATA;
  reg  [15:0] MIIRX_DATA;
  wire [2:0]  MIISTATUS;

  // New registers for controlling the MII interface

  // MIIMODER
  assign NoPre   = MIIMODER[8];
  assign Divider = MIIMODER[7:0];
  // MIICOMMAND
  assign WCtrlData = MIICOMMAND[2];
  assign RStat     = MIICOMMAND[1];
  assign ScanStat  = MIICOMMAND[0];
  // MIIADDRESS
  assign Rgad = MIIADDRESS[12:8];
  assign Fiad = MIIADDRESS[4:0];
  // MIITX_DATA
  assign CtrlData = MIITX_DATA[15:0];
  // MIISTATUS
  assign MIISTATUS[2:0] = { 13'b0, Nvalid, Busy, LinkFail };

  wire Wr;
  
  RegCPUData #( 5  ) U_0_000( Tx_Hwmark                    , 7'd000, 5'h09,    RST_I, CLK_I, Wr, ADR_I, DAT_I[4:0]  );
  RegCPUData #( 5  ) U_0_001( Tx_Lwmark                    , 7'd001, 5'h08,    RST_I, CLK_I, Wr, ADR_I, DAT_I[4:0]  );
  RegCPUData #( 1  ) U_0_002( pause_frame_send_en          , 7'd002, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 16 ) U_0_003( pause_quanta_set             , 7'd003, 16'h01af,    RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );
  RegCPUData #( 6  ) U_0_004( IFGset                       , 7'd004, 6'h0c,    RST_I, CLK_I, Wr, ADR_I, DAT_I[5:0]  );
  RegCPUData #( 1  ) U_0_005( FullDuplex                   , 7'd005, 1'h1,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 4  ) U_0_006( MaxRetry                     , 7'd006, 4'h2,     RST_I, CLK_I, Wr, ADR_I, DAT_I[3:0]  );
  RegCPUData #( 1  ) U_0_007( MAC_tx_add_en                , 7'd007, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 8  ) U_0_008( MAC_tx_add_prom_data         , 7'd008, 8'h00,    RST_I, CLK_I, Wr, ADR_I, DAT_I[7:0]  );
  RegCPUData #( 3  ) U_0_009( MAC_tx_add_prom_add          , 7'd009, 3'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[2:0]  );
  RegCPUData #( 1  ) U_0_010( MAC_tx_add_prom_wr           , 7'd010, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 1  ) U_0_011( tx_pause_en                  , 7'd011, 1'h1,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 16 ) U_0_012( fc_hwmark                    , 7'd012, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );
  RegCPUData #( 16 ) U_0_013( fc_lwmark                    , 7'd013, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );
  RegCPUData #( 1  ) U_0_014( MAC_rx_add_chk_en            , 7'd014, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 8  ) U_0_015( MAC_rx_add_prom_data         , 7'd015, 8'h00,    RST_I, CLK_I, Wr, ADR_I, DAT_I[7:0]  );
  RegCPUData #( 3  ) U_0_016( MAC_rx_add_prom_add          , 7'd016, 3'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[2:0]  );
  RegCPUData #( 1  ) U_0_017( MAC_rx_add_prom_wr           , 7'd017, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 1  ) U_0_018( broadcast_filter_en          , 7'd018, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 16 ) U_0_019( broadcast_bucket_depth       , 7'd019, 16'h0000, RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );
  RegCPUData #( 16 ) U_0_020( broadcast_bucket_interval    , 7'd020, 16'h0000, RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );
  RegCPUData #( 1  ) U_0_021( RX_APPEND_CRC                , 7'd021, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 5  ) U_0_022( Rx_Hwmark                    , 7'd022, 5'h1a,    RST_I, CLK_I, Wr, ADR_I, DAT_I[4:0]  );
  RegCPUData #( 5  ) U_0_023( Rx_Lwmark                    , 7'd023, 5'h10,    RST_I, CLK_I, Wr, ADR_I, DAT_I[4:0]  );
  RegCPUData #( 1  ) U_0_024( CRC_chk_en                   , 7'd024, 1'h1,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 6  ) U_0_025( RX_IFG_SET                   , 7'd025, 6'h0c,    RST_I, CLK_I, Wr, ADR_I, DAT_I[5:0]  );
  RegCPUData #( 16 ) U_0_026( RX_MAX_LENGTH                , 7'd026, 16'h2710, RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );
  RegCPUData #( 7  ) U_0_027( RX_MIN_LENGTH                , 7'd027, 7'h40,    RST_I, CLK_I, Wr, ADR_I, DAT_I[6:0]  );
  RegCPUData #( 6  ) U_0_028( CPU_rd_addr                  , 7'd028, 6'h00,    RST_I, CLK_I, Wr, ADR_I, DAT_I[5:0]  );
  RegCPUData #( 1  ) U_0_029( CPU_rd_apply                 , 7'd029, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
//RegCPUData #( 1  ) U_0_030( CPU_rd_grant                 , 7'd030, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
//RegCPUData #( 16 ) U_0_031( CPU_rd_dout_l                , 7'd031, 16'h0000, RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );
//RegCPUData #( 16 ) U_0_032( CPU_rd_dout_h                , 7'd032, 16'h0000, RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );
  RegCPUData #( 1  ) U_0_033( Line_loop_en                 , 7'd033, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[0:0]  );
  RegCPUData #( 3  ) U_0_034( Speed                        , 7'd034, 3'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[2:0]  );

  // New registers for controlling the MDIO interface
  RegCPUData #( 9  ) U_0_035( MIIMODER                     , 7'd035, 9'h064,   RST_I, CLK_I, Wr, ADR_I, DAT_I[8:0]  );
  // Reg #36 is MIICOMMAND - implemented separately below
  RegCPUData #( 13 ) U_0_037( MIIADDRESS                   , 7'd037, 13'h0000, RST_I, CLK_I, Wr, ADR_I, DAT_I[12:0] );
  RegCPUData #( 16 ) U_0_038( MIITX_DATA                   , 7'd038, 16'h0000, RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );

   // New FC register
  RegCPUData #( 16 ) U_0_041( fc_padtime                   , 7'd041, 1'h0,     RST_I, CLK_I, Wr, ADR_I, DAT_I[15:0] );

  // Asserted in first clock of 2-cycle access, negated otherwise
  wire Access = ~ACK_O & STB_I & CYC_I;

  // Asserted in first clock of 2-cycle write access, negated otherwise
  assign Wr = Access & WE_I;

  // MIICOMMAND register - needs special treatment because of auto-resetting bits
  always @ ( posedge RST_I or posedge CLK_I )
    if ( RST_I )
      MIICOMMAND <= 0;
    else
      begin
        if ( Wr & ( ADR_I == 7'd036 ) )
          // Write access
          MIICOMMAND <= DAT_I;
        else
          begin
            if ( WCtrlDataStart )
              MIICOMMAND[2] <= 0;
            if ( RStatStart )
              MIICOMMAND[1] <= 0;
          end
      end

  // MIIRX_DATA register
  always @ ( posedge RST_I or posedge CLK_I )
    if ( RST_I )
      MIIRX_DATA <= 0;
    else
      if ( UpdateMIIRX_DATAReg )
        MIIRX_DATA <= Prsd;

  // ACK_O is asserted in second clock of 2-cycle access, negated otherwise
  always @ ( posedge RST_I or posedge CLK_I )
    if ( RST_I )
      ACK_O <= 0;
    else
      ACK_O <= Access;

  always @ ( posedge RST_I or posedge CLK_I )
    if(RST_I)
      DAT_O <= 0;
    else
      begin
        DAT_O <=0;
        if ( Access & ~WE_I )
          casez ( ADR_I )
            7'd00: DAT_O <= Tx_Hwmark;
            7'd01: DAT_O <= Tx_Lwmark;
            7'd02: DAT_O <= pause_frame_send_en;
            7'd03: DAT_O <= pause_quanta_set;
            7'd04: DAT_O <= IFGset;
            7'd05: DAT_O <= FullDuplex;
            7'd06: DAT_O <= MaxRetry;
            7'd07: DAT_O <= MAC_tx_add_en;
            7'd08: DAT_O <= MAC_tx_add_prom_data;
            7'd09: DAT_O <= MAC_tx_add_prom_add;
            7'd10: DAT_O <= MAC_tx_add_prom_wr;
            7'd11: DAT_O <= tx_pause_en;
	    7'd12: DAT_O <= fc_hwmark;
	    7'd13: DAT_O <= fc_lwmark;
            7'd14: DAT_O <= MAC_rx_add_chk_en;
            7'd15: DAT_O <= MAC_rx_add_prom_data;
            7'd16: DAT_O <= MAC_rx_add_prom_add;
            7'd17: DAT_O <= MAC_rx_add_prom_wr;
            7'd18: DAT_O <= broadcast_filter_en;
            7'd19: DAT_O <= broadcast_bucket_depth;
            7'd20: DAT_O <= broadcast_bucket_interval;
            7'd21: DAT_O <= RX_APPEND_CRC;
            7'd22: DAT_O <= Rx_Hwmark;
            7'd23: DAT_O <= Rx_Lwmark;
            7'd24: DAT_O <= CRC_chk_en;
            7'd25: DAT_O <= RX_IFG_SET;
            7'd26: DAT_O <= RX_MAX_LENGTH;
            7'd27: DAT_O <= RX_MIN_LENGTH;
            7'd28: DAT_O <= CPU_rd_addr;
            7'd29: DAT_O <= CPU_rd_apply;
            7'd30: DAT_O <= CPU_rd_grant;
            7'd31: DAT_O <= CPU_rd_dout;
            //7'd32: DAT_O <= CPU_rd_dout[31:16];
            7'd33: DAT_O <= Line_loop_en;
            7'd34: DAT_O <= Speed;

            // New registers for controlling MII interface
            7'd35: DAT_O <= MIIMODER;
            7'd36: DAT_O <= MIICOMMAND;
            7'd37: DAT_O <= MIIADDRESS;
            7'd38: DAT_O <= MIITX_DATA;
            7'd39: DAT_O <= MIIRX_DATA;
            7'd40: DAT_O <= MIISTATUS;
            7'd41: DAT_O <= fc_padtime;
          endcase
      end

endmodule   

module RegCPUData(
  RegOut,
  RegAddr,
  RegInit,

  Reset,
  Clk,
  Wr,
  Addr,
  WrData
);

  parameter WIDTH = 16;

  output reg [WIDTH-1:0] RegOut;
  input [6:0]            RegAddr;
  input [WIDTH-1:0]      RegInit;

  input                  Reset;
  input                  Clk;
  input                  Wr;
  input [6:0]            Addr;
  input [WIDTH-1:0]      WrData;

  always @( posedge Reset or posedge Clk )
    if ( Reset )
      RegOut <= RegInit;
    else
      if ( Wr && ( Addr == RegAddr ) )
        RegOut <= WrData;

endmodule           
