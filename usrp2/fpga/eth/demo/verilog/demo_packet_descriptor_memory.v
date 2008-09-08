module demo_packet_descriptor_memory(
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

  //--- Packet Generator interface
  // RdData_o is always valid exactly one clock after Addr_i changes
  // and Rd_i is asserted
  Rd_i,
  Addr_i,
  RdData_o
);

  input         Reset_n;
  input         Clk;

  //--- Wishbone interface
  input         STB_I;
  input         CYC_I;
  input [14:0]  ADR_I;
  input         WE_I;
  input [15:0]  DAT_I;
  output [15:0] DAT_O;
  output        ACK_O;

  //--- Packet Generator interface
  // RdData_o is always valid exactly one clock after Addr_i changes
  // and Rd_i is asserted
  input         Rd_i;
  input [13:0]  Addr_i;
  output [31:0] RdData_o;

  //-------------------------------------------------------------------------
  // Local declarations
  //-------------------------------------------------------------------------

  reg ACK_O;

  //-------------------------------------------------------------------------

  wire [15:0] WrDataA = DAT_I;
  wire [15:0] RdDataA;
  wire [31:0] RdDataB;

  assign DAT_O = RdDataA;
  assign RdData_o = RdDataB;

  wire WB_Access = STB_I & CYC_I;
  wire WB_AccessClock1;
  reg  WB_AccessClock2;

  assign WB_AccessClock1 = WB_Access & ~WB_AccessClock2;

  always @( negedge Reset_n or posedge Clk )
    if ( ~Reset_n )
      begin
        WB_AccessClock2 <= 0;
        ACK_O <= 0;
      end
    else
      begin
        WB_AccessClock2 <= WB_Access;
        ACK_O <= WB_AccessClock1;
      end

  //-------------------------------------------------------------------------
  // Instantiation of sub-modules
  //-------------------------------------------------------------------------

  //--- Instantiation of Xilinx 16 Kbit Dual Port Memory --------------------

  RAMB16_S1_S2 RAMB16_S1_S2_bit0 (
    .DOA( RdDataA[0] ),
    .DOB( { RdDataB[0], RdDataB[16+0] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[0] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit1 (
    .DOA( RdDataA[1] ),
    .DOB( { RdDataB[1], RdDataB[16+1] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[1] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit2 (
    .DOA( RdDataA[2] ),
    .DOB( { RdDataB[2], RdDataB[16+2] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[2] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit3 (
    .DOA( RdDataA[3] ),
    .DOB( { RdDataB[3], RdDataB[16+3] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[3] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit4 (
    .DOA( RdDataA[4] ),
    .DOB( { RdDataB[4], RdDataB[16+4] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[4] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit5 (
    .DOA( RdDataA[5] ),
    .DOB( { RdDataB[5], RdDataB[16+5] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[5] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit6 (
    .DOA( RdDataA[6] ),
    .DOB( { RdDataB[6], RdDataB[16+6] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[6] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit7 (
    .DOA( RdDataA[7] ),
    .DOB( { RdDataB[7], RdDataB[16+7] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[7] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit8 (
    .DOA( RdDataA[8] ),
    .DOB( { RdDataB[8], RdDataB[16+8] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[8] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit9 (
    .DOA( RdDataA[9] ),
    .DOB( { RdDataB[9], RdDataB[16+9] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[9] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit10 (
    .DOA( RdDataA[10] ),
    .DOB( { RdDataB[10], RdDataB[16+10] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[10] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit11 (
    .DOA( RdDataA[11] ),
    .DOB( { RdDataB[11], RdDataB[16+11] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[11] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit12 (
    .DOA( RdDataA[12] ),
    .DOB( { RdDataB[12], RdDataB[16+12] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[12] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit13 (
    .DOA( RdDataA[13] ),
    .DOB( { RdDataB[13], RdDataB[16+13] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[13] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit14 (
    .DOA( RdDataA[14] ),
    .DOB( { RdDataB[14], RdDataB[16+14] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[14] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

  RAMB16_S1_S2 RAMB16_S1_S2_bit15 (
    .DOA( RdDataA[15] ),
    .DOB( { RdDataB[15], RdDataB[16+15] } ),

    .ADDRA( ADR_I[13:0]     ),
    .DIA  ( WrDataA[15] ),
    .ENA  ( WB_AccessClock1 ),
    .CLKA ( Clk             ),
    .WEA  ( WE_I            ),
    .SSRA ( 1'b0            ),

    .ADDRB( Addr_i[12:0] ),
    .DIB  ( 2'b00        ),
    .ENB  ( Rd_i         ),
    .CLKB ( Clk          ),
    .WEB  ( 1'b0         ),
    .SSRB ( 1'b0         )
  );

endmodule
