module demo(
  Reset_n,
  Clk_100M,
  Clk_125M, // GMII only

  RS232_TXD,
  RS232_RXD,

  USB_TXD,
  USB_RXD,

  //--- 10/100/1000BASE-T Ethernet PHY (MII/GMII)
  PHY_RESET_n,

  PHY_RXC,
  PHY_RXD,
  PHY_RXDV,
  PHY_RXER,

  PHY_GTX_CLK, // GMII only
  PHY_TXC,
  PHY_TXD,
  PHY_TXEN,
  PHY_TXER,

  PHY_COL,
  PHY_CRS,

  PHY_MDC,
  PHY_MDIO,

  // Misc. I/Os
  LED,
  Button
);

  input        Reset_n;
  input        Clk_100M;
  input        Clk_125M; // GMII

  output       RS232_TXD;
  input        RS232_RXD;

  output       USB_TXD;
  input        USB_RXD;

  //--- 10/100/1000BASE-T Ethernet PHY (MII/GMII)
  output       PHY_RESET_n;

  input        PHY_RXC;
  input [7:0]  PHY_RXD;
  input        PHY_RXDV;
  input        PHY_RXER;

  output       PHY_GTX_CLK; // GMII only
  input        PHY_TXC;
  output [7:0] PHY_TXD;
  output       PHY_TXEN;
  output       PHY_TXER;

  input        PHY_COL;
  input        PHY_CRS;

  output       PHY_MDC;
  inout        PHY_MDIO;

  // Misc. I/Os
  output [1:4] LED;

  input [1:4]  Button;

  //-------------------------------------------------------------------------
  // Local declarations
  //-------------------------------------------------------------------------

  // Rename to "standard" core clock name
  wire Clk = Clk_100M;

  reg [27:0] Counter;
  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      Counter <= 0;
    else
      Counter <= Counter + 1;

  assign LED[1:4] = Counter[27:24];

  //-------------------------------------------------------------------------
  // Instantiation of sub-modules
  //-------------------------------------------------------------------------

  //--- UART ----------------------------------------------------------------

  wire       UART_RXD;
  wire       UART_TXD;
  wire       UART_RxValid;
  wire [7:0] UART_RxData;
  wire       UART_TxReady;
  wire       UART_TxValid;
  wire [7:0] UART_TxData;

  demo_uart demo_uart(
    .Reset_n( Reset_n ),
    .Clk    ( Clk     ),

    // Interface to UART PHY
    .RXD_i( UART_RXD ),
    .TXD_o( UART_TXD ),

    // Clk is divided by (Prescaler+1) to generate 16x the bitrate
`ifdef EHDL_SIMULATION
    .Prescaler_i( 16'd3 ), // Corresponds to VERY FAST - for simulation only!
`else                      
    .Prescaler_i( 16'd650 ), // Corresponds to 9600 baud (assuming 100 MHz clock)
`endif
                      
    // Pulsed when RxData is valid
    .RxValid_o( UART_RxValid ),
    .RxData_o ( UART_RxData  ),

    // Asserted when ready for a new Tx byte
    .TxReady_o( UART_TxReady ),

    // Pulsed when TxData is valid
    .TxValid_i( UART_TxValid ),
    .TxData_i ( UART_TxData  )
  );

  // Transmit & receive in parallel on either RS232 or USB/RS232 interface
//  assign UART_RXD = RS232_RXD & USB_RXD; // RS232 signals are high when inactive
  assign     UART_RXD = RS232_RXD;

  assign RS232_TXD = UART_TXD;
  assign USB_TXD   = UART_TXD;

  //--- UART-to-Wishbone Master ---------------------------------------------

  wire        WB_STB_ETH;
  wire        WB_STB_PDM;
  wire        WB_STB_PG;
  wire        WB_CYC;
  wire [14:0] WB_ADR;
  wire        WB_WE;
  wire [15:0] WB_DAT_Wr;
  wire [15:0] WB_DAT_Rd;
  wire        WB_ACK;

  demo_wishbone_master demo_wishbone_master(
    .Reset_n( Reset_n ),
    .Clk    ( Clk     ),

    //--- UART interface

    // Pulsed when RxData_i is valid
    .RxValid_i( UART_RxValid ),
    .RxData_i ( UART_RxData  ),

    // Asserted when ready for a new Tx byte
    .TxReady_i( UART_TxReady ),

    // Pulsed when TxData_o is valid
    .TxValid_o( UART_TxValid ),
    .TxData_o ( UART_TxData  ),

    //--- Wishbone interface
    .STB_ETH_O( WB_STB_ETH ),
    .STB_PDM_O( WB_STB_PDM ),
    .STB_PG_O ( WB_STB_PG  ),
    .CYC_O    ( WB_CYC     ),
    .ADR_O    ( WB_ADR     ),
    .WE_O     ( WB_WE      ), 
    .DAT_O    ( WB_DAT_Wr  ),
    .DAT_I    ( WB_DAT_Rd  ),
    .ACK_I    ( WB_ACK     )
  );

  //--- Wishbone clients ----------------------------------------------------

  //--- Packet Descriptor Memory --------------------------------------------

  wire [15:0] WB_DAT_Rd_PDM;
  wire        WB_ACK_PDM;

  wire        PDM_Rd;
  wire [13:0] PDM_Addr;
  wire [31:0] PDM_RdData;

  demo_packet_descriptor_memory demo_packet_descriptor_memory(
    .Reset_n( Reset_n ),
    .Clk    ( Clk     ),

    //--- Wishbone interface
    .STB_I( WB_STB_PDM    ),
    .CYC_I( WB_CYC        ),
    .ADR_I( WB_ADR        ),
    .WE_I ( WB_WE         ), 
    .DAT_I( WB_DAT_Wr     ),
    .DAT_O( WB_DAT_Rd_PDM ),
    .ACK_O( WB_ACK_PDM    ),

    //--- Packet Generator interface
    // RdData_o is always valid exactly one clock after Addr_i changes
    // and Rd_i is asserted
    .Rd_i    ( PDM_Rd     ),
    .Addr_i  ( PDM_Addr   ),
    .RdData_o( PDM_RdData )
  );

  //--- Packet Generator ----------------------------------------------------

  wire [15:0] WB_DAT_Rd_PG;
  wire        WB_ACK_PG;

  wire        Rx_mac_ra;
  wire        Rx_mac_rd;
  wire [31:0] Rx_mac_data;
  wire [1:0]  Rx_mac_BE;
  wire        Rx_mac_pa;
  wire        Rx_mac_sop;
  wire        Rx_mac_err;
  wire        Rx_mac_eop;

  wire        Tx_mac_wa;
  wire        Tx_mac_wr;
  wire [31:0] Tx_mac_data;
  wire [1:0]  Tx_mac_BE;
  wire        Tx_mac_sop;
  wire        Tx_mac_eop;

  demo_packet_generator demo_packet_generator(
    .Reset_n( Reset_n ),
    .Clk    ( Clk     ),

    //--- Wishbone interface
    .STB_I( WB_STB_PG    ),
    .CYC_I( WB_CYC       ),
    .ADR_I( WB_ADR[1:0]  ),
    .WE_I ( WB_WE        ),
    .DAT_I( WB_DAT_Wr    ),
    .DAT_O( WB_DAT_Rd_PG ),
    .ACK_O( WB_ACK_PG    ),

    //--- Packet Descriptor Memory interface
    // RdData_i is always valid exactly one clock after Addr_o changes
    // and Rd_o is asserted
    .Rd_o    ( PDM_Rd     ),
    .Addr_o  ( PDM_Addr   ),
    .RdData_i( PDM_RdData ),

    //--- User (packet) interface
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
    .Tx_mac_eop ( Tx_mac_eop  )
  );

  //--- Simple Wishbone client ----------------------------------------------

  reg [15:0] Reg1;
  reg [15:0] Reg2;

  reg        WB_ACK_Reg;
  reg [15:0] WB_DAT_Reg;

  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        WB_ACK_Reg <= 0;
        WB_DAT_Reg <= 'b0;

        Reg1 <= 16'h1234;
        Reg2 <= 16'hABCD;
      end
    else
      begin
        WB_ACK_Reg <= 0;
        if ( WB_CYC & ~( WB_STB_ETH | WB_STB_PG | WB_STB_PDM ) )
          begin
            WB_ACK_Reg <= 1;
            if ( WB_WE )
              begin
                if ( WB_ADR[0] )
                  Reg2 <= WB_DAT_Wr;
                else
                  Reg1 <= WB_DAT_Wr;
              end
            else
              begin
                if ( WB_ADR[0] )
                  WB_DAT_Reg <= Reg2;
                else
                  WB_DAT_Reg <= Reg1;
              end
          end
      end

  //--- DUT - Ethernet Core -------------------------------------------------

  wire [15:0] WB_DAT_Rd_ETH;
  wire        WB_ACK_ETH;

  wire [2:0] Speed;

  MAC_top dut(
    // System signals
    .Clk_125M( Clk_125M ),
    .Clk_user( Clk      ),
    .Speed   ( Speed    ),

    // Wishbone compliant core host interface
    .RST_I( ~Reset_n ),
    .CLK_I( Clk     ),
    .STB_I( WB_STB_ETH    ),
    .CYC_I( WB_CYC        ),
    .ADR_I( WB_ADR[6:0]   ),
    .WE_I ( WB_WE         ),
    .DAT_I( WB_DAT_Wr     ),
    .DAT_O( WB_DAT_Rd_ETH ),
    .ACK_O( WB_ACK_ETH    ),

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
    .Gtx_clk( PHY_GTX_CLK  ), // Used only in GMII mode
    .Rx_clk ( PHY_RXC      ),
    .Tx_clk ( PHY_TXC      ),  // Used only in MII mode
    .Tx_er  ( PHY_TXER     ),
    .Tx_en  ( PHY_TXEN     ),
    .Txd    ( PHY_TXD      ),
    .Rx_er  ( PHY_RXER     ),
    .Rx_dv  ( PHY_RXDV     ),
    .Rxd    ( PHY_RXD      ),
    .Crs    ( PHY_CRS      ),
    .Col    ( PHY_COL      ),

    // MDIO interface (to PHY)
    .Mdio( PHY_MDIO ),
    .Mdc ( PHY_MDC  )
  );

  //--- Combination of Wishbone read data and acknowledge -------------------

  assign WB_DAT_Rd = ({16{WB_ACK_Reg}} & WB_DAT_Reg   ) |
                     ({16{WB_ACK_PDM}} & WB_DAT_Rd_PDM) |
                     ({16{WB_ACK_PG }} & WB_DAT_Rd_PG ) |
                     ({16{WB_ACK_ETH}} & WB_DAT_Rd_ETH);

  assign WB_ACK = WB_ACK_Reg | WB_ACK_PDM | WB_ACK_PG | WB_ACK_ETH;

  assign PHY_RESET_n = Reset_n;                 

endmodule
