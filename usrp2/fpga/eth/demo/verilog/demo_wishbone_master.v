module demo_wishbone_master(
  Reset_n,
  Clk,

  //--- UART interface

  // Pulsed when RxData_i is valid
  RxValid_i,
  RxData_i,

  // Asserted when ready for a new Tx byte
  TxReady_i,

  // Pulsed when TxData_o is valid
  TxValid_o,
  TxData_o,

  //--- Wishbone interface
  STB_ETH_O,
  STB_PDM_O,
  STB_PG_O,
  CYC_O,
  ADR_O,
  WE_O, 
  DAT_O,
  DAT_I,
  ACK_I
);

  input         Reset_n;
  input         Clk;

  //--- UART interface

  // Pulsed when RxData_i is valid
  input         RxValid_i;
  input [7:0]   RxData_i;

  // Asserted when ready for a new Tx byte
  input         TxReady_i;

  // Pulsed when TxData_o is valid
  output        TxValid_o;
  output [7:0]  TxData_o;

  output        STB_ETH_O;
  output        STB_PDM_O;
  output        STB_PG_O;
  output        CYC_O;
  output [14:0] ADR_O;
  output        WE_O;
  output [15:0] DAT_O;
  input [15:0]  DAT_I;
  input         ACK_I;

  //-------------------------------------------------------------------------
  // Local declarations
  //-------------------------------------------------------------------------

  reg          TxValid_o;
  reg [7:0]    TxData_o;
  reg          STB_ETH_O;
  reg          STB_PDM_O;
  reg          STB_PG_O;
  reg          CYC_O;
  reg [14:0]   ADR_O;
  reg          WE_O;
  reg [15:0]   DAT_O;

  //-------------------------------------------------------------------------
  // Instantiation of sub-modules
  //-------------------------------------------------------------------------

  //--- Transmit FSM --------------------------------------------------------

  parameter    TX_STATE_IDLE  = 0;
  parameter    TX_STATE_INIT  = 1;
  parameter    TX_STATE_OK    = 2;
  parameter    TX_STATE_ERROR = 3;
  parameter    TX_STATE_VALUE = 4;
  parameter    TX_STATE_LF    = 5;

  reg [2:0]    TxState;
  reg [3:0]    TxIndex;
  reg          TxLast;

  wire [15:0]  TxValue16;
  wire [3:0]   TxHexDigit;
  wire [7:0]   TxHexChar;
  reg          TxOK;
  reg          TxERROR;
  reg          TxValue;

  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        TxState   <= TX_STATE_INIT;
        TxIndex   <= 0;
        TxLast    <= 0;

        TxValid_o <= 0;
        TxData_o  <= 'b0;
      end
    else
      begin
        TxValid_o <= 0;

        // Don't do anything in cycle following TxValid_o being pulsed
        if ( ~TxValid_o )
          begin
            casez ( TxState )
              TX_STATE_INIT:
                casez ( TxIndex )
                  0: TxData_o <= "R";
                  1: TxData_o <= "E";
                  2: TxData_o <= "A";
                  3: TxData_o <= "D";
                  4: TxData_o <= "Y";
                  default: TxLast <= 1;
                endcase
              
              TX_STATE_OK:
                casez ( TxIndex )
                  0: TxData_o <= "O";
                  1: TxData_o <= "K";
                  default: TxLast <= 1;
                endcase

              TX_STATE_ERROR:
                casez ( TxIndex )
                  0: TxData_o <= "E";
                  1: TxData_o <= "R";
                  2: TxData_o <= "R";
                  3: TxData_o <= "O";
                  4: TxData_o <= "R";
                  default: TxLast <= 1;
                endcase

              TX_STATE_VALUE:
                casez ( TxIndex )
                  0,1,2,3: TxData_o <= TxHexChar;
                  default: TxLast <= 1;
                endcase

              TX_STATE_LF:
                ;

              default:
                begin
                  if ( TxOK )
                    TxState <= TX_STATE_OK;
                  else if ( TxERROR )
                    TxState <= TX_STATE_ERROR;
                  else if ( TxValue )
                    begin                         
                      TxState <= TX_STATE_VALUE;
                      TxIndex <= 0;
                    end
                end
            endcase

            if ( (TxState != TX_STATE_IDLE) & TxReady_i )
              begin
                TxValid_o <= 1;

                if ( TxLast )
                  begin
                    if ( TxState == TX_STATE_LF )
                      begin
                        TxData_o <= 10; // LF
                        TxState  <= TX_STATE_IDLE;
                        TxIndex  <= 0;
                        TxLast   <= 0;
                      end
                    else
                      begin
                        TxData_o <= 13; // CR
                        TxState  <= TX_STATE_LF;
                      end
                  end
                else
                  TxIndex <= TxIndex + 1;
              end
          end
      end

  assign TxHexDigit = (TxIndex==0) ? TxValue16[15:12] :
                      (TxIndex==1) ? TxValue16[11: 8] :
                      (TxIndex==2) ? TxValue16[ 7: 4] :
                                     TxValue16[ 3: 0];

  assign TxHexChar = (TxHexDigit <= 9) ? (TxHexDigit + "0") :
                                         (TxHexDigit + "A"-'hA);

  //--- Receive FSM ---------------------------------------------------------

  parameter RX_STATE_IDLE           = 0;
  parameter RX_STATE_VALUE16_FIRST  = 1;
  parameter RX_STATE_VALUE16        = 2;
  parameter RX_STATE_COMMENT        = 3;
  parameter RX_STATE_CMD            = 4;

  reg [2:0] RxState;

  wire IsWhiteSpace = ( RxData_i==" "  ) |
                      ( RxData_i=="\t" ) |
                      ( RxData_i==","  ) |
                      ( RxData_i==10   ) |
                      ( RxData_i==13   );
  wire IsHexDigit = (( RxData_i >= "0" ) & ( RxData_i <= "9" )) |
                    (( RxData_i >= "a" ) & ( RxData_i <= "f" )) |
                      (( RxData_i >= "A" ) & ( RxData_i <= "F" ));
  wire [3:0] RxHexValue =
               (( RxData_i >= "0" ) & ( RxData_i <= "9" )) ? RxData_i[3:0] :
               (( RxData_i >= "a" ) & ( RxData_i <= "f" )) ? (RxData_i-"a"+'hA) :
               (( RxData_i >= "A" ) & ( RxData_i <= "F" )) ? (RxData_i-"A"+'hA) : 0;

  reg [15:0] RxValue16;
  reg        RxWrite;
  reg        RxWrData;

  reg [15:0] RegAddr;
  reg [15:0] RegRdData;

  assign     TxValue16 = RegRdData;

  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        RxState   <= RX_STATE_IDLE;

        RxValue16 <= 16'h0;
        RxWrite   <= 0;
        RxWrData  <= 0;

        RegAddr   <= 'b0;
        RegRdData <= 'b0;

        STB_ETH_O <= 0;
        STB_PDM_O <= 0;
        STB_PG_O  <= 0;
        CYC_O     <= 0;
        ADR_O     <= 0;
        WE_O      <= 0;
        DAT_O     <= 0;

        TxOK      <= 0;
        TxERROR   <= 0;
        TxValue   <= 0;
      end
    else
      begin
        TxOK    <= 0;
        TxERROR <= 0;
        TxValue <= 0;

        if ( RxState == RX_STATE_CMD )
          begin
            STB_ETH_O <= ( RegAddr[15:12] == 4'h0 );
            STB_PG_O  <= ( RegAddr[15:12] == 4'h1 );
            STB_PDM_O <= ( RegAddr[15]    == 1'b1 );

            CYC_O <= 1;
            ADR_O <= RegAddr[14:0];
            WE_O  <= RxWrite;

            if ( ACK_I )
              begin
                // Register transaction is completing!
                CYC_O     <= 0;
                STB_ETH_O <= 0;
                STB_PDM_O <= 0;
                STB_PG_O  <= 0;

                // Latch data read in case of a read
                RegRdData <= DAT_I;

                if ( RxWrite )
                  // Transaction was a register write
                  TxOK <= 1;
                else
                  TxValue <= 1;

                RxState <= RX_STATE_IDLE;
              end
          end
        else if ( (TxState == TX_STATE_IDLE) & RxValid_i )
          begin
            // A byte has been received!

            casez ( RxState )
              RX_STATE_IDLE:
                if ( (RxData_i == "w") | (RxData_i == "W") )
                  begin
                    // Write Register Command: W rrrr dddd
                    RxState  <= RX_STATE_VALUE16_FIRST;
                    RxWrite  <= 1;
                    RxWrData <= 0;
                  end
                else if ( (RxData_i == "r") | (RxData_i == "R") )
                  begin
                    // Read Register Command: R rrrr
                    RxState <= RX_STATE_VALUE16_FIRST;
                    RxWrite <= 0;
                  end
                else if ( RxData_i == "/" )
                  begin
                    // Comment!
                    RxState <= RX_STATE_COMMENT;
                  end
                else if ( ~IsWhiteSpace )
                  // Unknown command!
                  TxERROR <= 1;

              RX_STATE_COMMENT:
                if ( (RxData_i == 13) | (RxData_i == 10) )
                  // CR or LF - end of comment
                  RxState <= RX_STATE_IDLE;

              RX_STATE_VALUE16_FIRST:
                if ( IsHexDigit )
                  begin
                    RxValue16 <= { 12'b0, RxHexValue };
                    RxState <= RX_STATE_VALUE16;
                  end
                else if ( ~IsWhiteSpace )
                  begin
                    // Unexpected character!
                    TxERROR <= 1;
                    RxState <= RX_STATE_IDLE;
                  end

              RX_STATE_VALUE16:
                if ( IsHexDigit )
                  RxValue16 <= { RxValue16[11:0], RxHexValue };
                else if ( IsWhiteSpace )
                  begin
                    // Done collecting 16-bit value
                    if ( RxWrite )
                      begin
                        // This is a register write
                        if ( RxWrData )
                          begin
                            // Second time around - just received write data
                            DAT_O   <= RxValue16;
                            RxState <= RX_STATE_CMD;
                          end
                        else
                          begin
                            // Just received register address - expecting second argument
                            RegAddr <= RxValue16;
                            RxState <= RX_STATE_VALUE16_FIRST;
                            RxWrData <= 1; // Now receive the write data
                          end
                      end
                    else
                      begin
                        // This is a register read
                        RegAddr <= RxValue16;
                        RxState <= RX_STATE_CMD;
                      end
                  end
                else
                  begin
                    // Unexpected character!
                    TxERROR <= 1;
                    RxState <= RX_STATE_IDLE;
                  end

              default:
                TxERROR <= 1;
            endcase
          end
      end
  
endmodule
