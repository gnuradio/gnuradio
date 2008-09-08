module demo_uart(
  Reset_n,
  Clk,

  // Interface to UART PHY (RS232 level converter)
  RXD_i,
  TXD_o,

  // Clk is divided by (Prescaler+1) to generate 16x the bitrate
  Prescaler_i,

  // Pulsed when RxData is valid
  RxValid_o,
  RxData_o,

  // Asserted when ready for a new Tx byte
  TxReady_o,

  // Pulsed when TxData is valid
  TxValid_i,
  TxData_i
);

  input        Reset_n;
  input        Clk;

  // Interface to UART PHY (RS232 level converter)
  input        RXD_i;
  output       TXD_o;

  // Clk is divided by (Prescaler+1) to generate 16x the bitrate
  input [15:0] Prescaler_i;

  // Pulsed when RxData is valid
  output       RxValid_o;
  output [7:0] RxData_o;

  // Asserted when ready for a new Tx byte
  output       TxReady_o;

  // Pulsed when TxData is valid
  input        TxValid_i;
  input [7:0]  TxData_i;

  //-------------------------------------------------------------------------
  // Local declarations
  //-------------------------------------------------------------------------

  reg          TXD_o;
  reg          RxValid_o;
  reg [7:0]    RxData_o;
  reg          TxReady_o;

  //-------------------------------------------------------------------------
  // Instantiation of sub-modules
  //-------------------------------------------------------------------------

  //--- Prescaler generating 16x bitrate clock ------------------------------

  reg        Clk_16x;
  reg [15:0] Prescaler;

  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        Prescaler <= 0;
        Clk_16x <= 0;
      end
    else
      begin
        if ( Prescaler == Prescaler_i )
          begin
            Prescaler <= 0;
            Clk_16x <= 1;
          end
        else
          begin
            Prescaler <= Prescaler + 1;
            Clk_16x <= 0;
          end
      end

  //--- Transmitter logic ---------------------------------------------------

  reg [3:0] TxCounter;
  reg       TxSendBit;

  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        TxCounter <= 0;
        TxSendBit <= 0;
      end
    else
      begin
        TxSendBit <= 0;
        if ( Clk_16x )
          begin    
            if ( TxCounter == 15 )
              begin
                TxCounter <= 0;
                TxSendBit <= 1;
              end
            else
              TxCounter <= TxCounter + 1;
          end
      end

  reg [7:0] TxData_reg;
  reg [3:0] TxBitCnt;
  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        TXD_o      <= 1;
        TxReady_o  <= 1;
        TxData_reg <= 0;
        TxBitCnt   <= 0;
      end
    else
      begin
        if ( TxReady_o )
          begin
            if ( TxValid_i )
              begin
                TxReady_o  <= 0;
                TxData_reg <= TxData_i;
                TxBitCnt   <= 0;
              end
          end
        else
          begin
            if ( TxSendBit )
              begin
                // Only do anything on bit boundaries
                casez ( TxBitCnt )
                  0: // Tx START bit
                    TXD_o <= 0;
                  10: // Tx second STOP bit
                    // Now we're done
                    TxReady_o <= 1;
                  default: // Tx data bit + first stop bit
                    begin
                      TXD_o <= TxData_reg[0];
                      TxData_reg <= { 1'b1, TxData_reg[7:1] };
                    end
                endcase
                
                TxBitCnt <= TxBitCnt+1;
              end
          end
      end

  //--- Receiver logic ------------------------------------------------------

  reg       RxHunt;
  reg [3:0] RxCounter;
  reg       RxSampleBit;
  reg       RxDone;

  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        RxCounter   <= 0;
        RxSampleBit <= 0;

        RxHunt <= 1;
      end
    else
      begin
        RxSampleBit <= 0;

        if ( RxDone )
          RxHunt <= 1;

        if ( Clk_16x )
          begin
            if ( RxHunt )
              begin
                if ( RXD_i == 0 )
                  begin
                    // Receiving start bit!
                    RxHunt <= 0;
                    // Reset 16x bit counter
                    RxCounter <= 0;
                  end
              end
            else
              begin
                RxCounter <= RxCounter + 1;
                if ( RxCounter == 7 )
                  // In middle of Rx bit in next cycle
                  RxSampleBit <= 1;
              end
          end
      end

  reg [3:0] RxBitCount;

  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        RxValid_o <= 0;
        RxData_o <= 'b0;
        RxBitCount <= 0;
        RxDone <= 0;
      end
    else
      begin
        RxValid_o <= 0;
        RxDone <= 0;

        if ( RxSampleBit )
          begin
            RxBitCount <= RxBitCount + 1;

            casez ( RxBitCount )
              0: // START bit - just ignore it
                ;
              9: // STOP bit - indicate we're ready again
                begin
                  RxDone <= 1;
                  RxBitCount <= 0;
                end
              default: // Rx Data bits
                begin
                  RxData_o <= { RXD_i, RxData_o[7:1] };
                  if ( RxBitCount == 8 )
                    // Last data bit just received
                    RxValid_o <= 1;
                end
            endcase
          end
      end

endmodule
