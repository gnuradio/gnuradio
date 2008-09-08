module demo_packet_generator(
  Reset_n,
  Clk,

  //--- Wishbone interface
  STB_I,
  CYC_I,
  ADR_I,
  WE_I,
  DAT_I,
  DAT_O,
  ACK_O,

  //--- Packet Descriptor Memory interface
  // RdData_i is always valid exactly one clock after Addr_o changes
  // and Rd_o is asserted
  Rd_o,
  Addr_o,
  RdData_i,

  //--- User (packet) interface
  Rx_mac_ra,
  Rx_mac_rd,
  Rx_mac_data,
  Rx_mac_BE,
  Rx_mac_pa,
  Rx_mac_sop,
  Rx_mac_err,
  Rx_mac_eop,

  Tx_mac_wa,
  Tx_mac_wr,
  Tx_mac_data,
  Tx_mac_BE,
  Tx_mac_sop,
  Tx_mac_eop
);

  input         Reset_n;
  input         Clk;

  //--- Wishbone interface
  input         STB_I;
  input         CYC_I;
  input [1:0]   ADR_I;
  input         WE_I;
  input [15:0]  DAT_I;
  output [15:0] DAT_O;
  output        ACK_O;

  //--- Packet Generator interface
  // RdData_o is always valid exactly one clock after Addr_o changes
  // and Rd_o is asserted
  output        Rd_o;
  output [13:0] Addr_o;
  input [31:0]  RdData_i;

  //--- User (packet) interface
  input         Rx_mac_ra;
  output        Rx_mac_rd;
  input [31:0]  Rx_mac_data;
  input [1:0]   Rx_mac_BE;
  input         Rx_mac_pa;
  input         Rx_mac_sop;
  input         Rx_mac_err;
  input         Rx_mac_eop;

  input         Tx_mac_wa;
  output        Tx_mac_wr;
  output [31:0] Tx_mac_data;
  output [1:0]  Tx_mac_BE;
  output        Tx_mac_sop;
  output        Tx_mac_eop;

  //-------------------------------------------------------------------------
  // Local declarations
  //-------------------------------------------------------------------------

  reg        ACK_O;
  reg [15:0] DAT_O;

  reg        Rd_o;
  reg        Tx_mac_wr;
  reg [1:0]  Tx_mac_BE;
  reg        Tx_mac_sop;
  reg        Tx_mac_eop;

  //--- Wishbone interface --------------------------------------------------

  reg [1:0] PG_CFG;
  wire      PG_Enable = PG_CFG[0];
  
  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        ACK_O <= 0;
        DAT_O <= 'b0;

        PG_CFG <= 2'h0;
      end
    else
      begin
        ACK_O <= 0;
        if ( CYC_I & STB_I )
          begin
            ACK_O <= ~ACK_O; // Generate single cycle pulse!
            if ( WE_I )
              begin
                PG_CFG <= DAT_I;
              end
            else
              begin
                DAT_O[1:0] <= PG_CFG;
              end
          end
      end

  //--- Packet Generator FSM ------------------------------------------------

  parameter  PG_FSM_STATE_IDLE               = 3'h0;
  parameter  PG_FSM_STATE_LD_DESC_1          = 3'h1;
  parameter  PG_FSM_STATE_LD_DESC_2          = 3'h2;
  parameter  PG_FSM_STATE_RD_HEADER          = 3'h3;
  parameter  PG_FSM_STATE_PAYLOAD_SEQ_NUMBER = 3'h4;
  parameter  PG_FSM_STATE_PAYLOAD            = 3'h5;
  parameter  PG_FSM_STATE_DONE               = 3'h6;
  reg [2:0]  PG_FSM_State;

  reg [9:0] DescHigh; // Selects currente descriptor
  reg [3:0] DescLow; // Index into a single descriptor (16 entries)

  reg        PDM_CFG1_LAST;
  reg [3:0]  PDM_CFG1_REPEAT;
  reg [3:0]  PDM_CFG1_HDRLEN;
  reg [15:0] PDM_CFG2_PAYLDLEN;

  reg [31:0] Tx_mac_data_reg;
  reg        WriteHeader;
  reg [15:0] PayloadRemaining;
  reg [31:0] PacketSequenceNumber;
  reg [31:0] Payload;
  
  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        PG_FSM_State <= PG_FSM_STATE_IDLE;

        Rd_o <= 0;

        DescHigh <= 10'b0;
        DescLow  <= 4'b0;

        Tx_mac_wr  <= 0;
        Tx_mac_sop <= 0;
        Tx_mac_eop <= 0;
        Tx_mac_BE  <= 2'b00;

        Tx_mac_data_reg <= 32'b0;
        WriteHeader <= 0;
        PayloadRemaining <= 16'd0;

        PacketSequenceNumber <= 32'd0;
        Payload <= 32'h0;

        { PDM_CFG1_HDRLEN, PDM_CFG1_REPEAT, PDM_CFG1_LAST, PDM_CFG2_PAYLDLEN } <= 'b0;
      end
    else
      begin
        casez ( PG_FSM_State )
          PG_FSM_STATE_IDLE:
            if ( PG_Enable )
              begin
                PG_FSM_State <= PG_FSM_STATE_LD_DESC_1;
                Rd_o <= 1;
              end
            else
              begin
                DescHigh <= 10'b0;
                DescLow  <= 4'b0;
              end

          PG_FSM_STATE_LD_DESC_1:
            begin
              PG_FSM_State <= PG_FSM_STATE_LD_DESC_2;

              DescLow <= DescLow + 1;
            end

          PG_FSM_STATE_LD_DESC_2:
            begin
              PG_FSM_State <= PG_FSM_STATE_RD_HEADER;

              { PDM_CFG1_LAST, PDM_CFG1_REPEAT, PDM_CFG1_HDRLEN, PDM_CFG2_PAYLDLEN } <=
                { RdData_i[31], RdData_i[23:20], RdData_i[19:16], RdData_i[15:0] };
            end

          PG_FSM_STATE_RD_HEADER:
            begin
              Tx_mac_wr <= 0;
              if ( Tx_mac_wa )
                begin
                  // Space in Tx FIFO - write next header word
                  DescLow <= DescLow + 1;
                  Tx_mac_wr <= 1;
                  Tx_mac_sop <= ( DescLow == 1 ); // Assert SOP on first header word
                  WriteHeader <= 1;
                  if ( DescLow == PDM_CFG1_HDRLEN )
                    begin
                      // The requested number of header words has been read
                      // - proceed to generate packet payload
                      PG_FSM_State <= PG_FSM_STATE_PAYLOAD_SEQ_NUMBER;
                      PayloadRemaining <= PDM_CFG2_PAYLDLEN;
                    end
                end
            end

          PG_FSM_STATE_PAYLOAD_SEQ_NUMBER:
            begin
              WriteHeader <= 0;
              Tx_mac_data_reg <= PacketSequenceNumber;
              Tx_mac_wr <= 0;
              Tx_mac_sop <= 0;
              if ( Tx_mac_wa )
                begin
                  Tx_mac_wr <= 1;
                  PG_FSM_State <= PG_FSM_STATE_PAYLOAD;
                  Payload <= 32'h01020304;
                  PayloadRemaining <= PayloadRemaining - 4;
                end
            end

          PG_FSM_STATE_PAYLOAD:
            begin
              Tx_mac_data_reg <= Payload;
              Tx_mac_wr <= 0;
              if ( Tx_mac_wa )
                begin
                  Tx_mac_wr <= 1;
                  Tx_mac_data_reg <= Payload;
                  Payload[31:24] <= Payload[31:24] + 8'h04;
                  Payload[23:16] <= Payload[23:16] + 8'h04;
                  Payload[15: 8] <= Payload[15: 8] + 8'h04;
                  Payload[ 7: 0] <= Payload[ 7: 0] + 8'h04;
                  PayloadRemaining <= PayloadRemaining - 4;
                  if ( PayloadRemaining <= 4 )
                    begin
                      PG_FSM_State <= PG_FSM_STATE_DONE;

                      Tx_mac_eop <= 1;
                      // Indicate how many bytes are valid in this last transfer
                      Tx_mac_BE <= PayloadRemaining[1:0];
                    end
                end
            end

          PG_FSM_STATE_DONE:
            begin
              // TBD: Add support for REPEAT, NEXT & LAST!
              Tx_mac_wr <= 0;
              Tx_mac_eop <= 0;
            end
        endcase
      end

  //-------------------------------------------------------------------------

  assign Tx_mac_data = WriteHeader ?
                       RdData_i : Tx_mac_data_reg;

  assign Addr_o = { DescHigh, DescLow };

  assign Rx_mac_rd = 0;

endmodule
