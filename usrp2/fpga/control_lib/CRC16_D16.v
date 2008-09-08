///////////////////////////////////////////////////////////////////////
// File:  CRC16_D16.v                             
// Date:  Sun Jun 17 06:42:55 2007                                                      
//                                                                     
// Copyright (C) 1999-2003 Easics NV.                 
// This source file may be used and distributed without restriction    
// provided that this copyright statement is not removed from the file 
// and that any derivative work contains the original copyright notice
// and the associated disclaimer.
//
// THIS SOURCE FILE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
// WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Purpose: Verilog module containing a synthesizable CRC function
//   * polynomial: (0 5 12 16)
//   * data width: 16
//                                                                     
// Info: tools@easics.be
//       http://www.easics.com                                  
///////////////////////////////////////////////////////////////////////


module CRC16_D16
  (input [15:0] Data,
   input [15:0] CRC,
   output [15:0] NewCRC);

   assign 	 NewCRC = nextCRC16_D16(Data,CRC);
   
  // polynomial: (0 5 12 16)
  // data width: 16
  // convention: the first serial data bit is D[15]
  function [15:0] nextCRC16_D16;

    input [15:0] Data;
    input [15:0] CRC;

    reg [15:0] D;
    reg [15:0] C;
    reg [15:0] NewCRC;

  begin

    D = Data;
    C = CRC;

    NewCRC[0] = D[12] ^ D[11] ^ D[8] ^ D[4] ^ D[0] ^ C[0] ^ C[4] ^ 
                C[8] ^ C[11] ^ C[12];
    NewCRC[1] = D[13] ^ D[12] ^ D[9] ^ D[5] ^ D[1] ^ C[1] ^ C[5] ^ 
                C[9] ^ C[12] ^ C[13];
    NewCRC[2] = D[14] ^ D[13] ^ D[10] ^ D[6] ^ D[2] ^ C[2] ^ C[6] ^ 
                C[10] ^ C[13] ^ C[14];
    NewCRC[3] = D[15] ^ D[14] ^ D[11] ^ D[7] ^ D[3] ^ C[3] ^ C[7] ^ 
                C[11] ^ C[14] ^ C[15];
    NewCRC[4] = D[15] ^ D[12] ^ D[8] ^ D[4] ^ C[4] ^ C[8] ^ C[12] ^ 
                C[15];
    NewCRC[5] = D[13] ^ D[12] ^ D[11] ^ D[9] ^ D[8] ^ D[5] ^ D[4] ^ 
                D[0] ^ C[0] ^ C[4] ^ C[5] ^ C[8] ^ C[9] ^ C[11] ^ C[12] ^ 
                C[13];
    NewCRC[6] = D[14] ^ D[13] ^ D[12] ^ D[10] ^ D[9] ^ D[6] ^ D[5] ^ 
                D[1] ^ C[1] ^ C[5] ^ C[6] ^ C[9] ^ C[10] ^ C[12] ^ 
                C[13] ^ C[14];
    NewCRC[7] = D[15] ^ D[14] ^ D[13] ^ D[11] ^ D[10] ^ D[7] ^ D[6] ^ 
                D[2] ^ C[2] ^ C[6] ^ C[7] ^ C[10] ^ C[11] ^ C[13] ^ 
                C[14] ^ C[15];
    NewCRC[8] = D[15] ^ D[14] ^ D[12] ^ D[11] ^ D[8] ^ D[7] ^ D[3] ^ 
                C[3] ^ C[7] ^ C[8] ^ C[11] ^ C[12] ^ C[14] ^ C[15];
    NewCRC[9] = D[15] ^ D[13] ^ D[12] ^ D[9] ^ D[8] ^ D[4] ^ C[4] ^ 
                C[8] ^ C[9] ^ C[12] ^ C[13] ^ C[15];
    NewCRC[10] = D[14] ^ D[13] ^ D[10] ^ D[9] ^ D[5] ^ C[5] ^ C[9] ^ 
                 C[10] ^ C[13] ^ C[14];
    NewCRC[11] = D[15] ^ D[14] ^ D[11] ^ D[10] ^ D[6] ^ C[6] ^ C[10] ^ 
                 C[11] ^ C[14] ^ C[15];
    NewCRC[12] = D[15] ^ D[8] ^ D[7] ^ D[4] ^ D[0] ^ C[0] ^ C[4] ^ C[7] ^ 
                 C[8] ^ C[15];
    NewCRC[13] = D[9] ^ D[8] ^ D[5] ^ D[1] ^ C[1] ^ C[5] ^ C[8] ^ C[9];
    NewCRC[14] = D[10] ^ D[9] ^ D[6] ^ D[2] ^ C[2] ^ C[6] ^ C[9] ^ C[10];
    NewCRC[15] = D[11] ^ D[10] ^ D[7] ^ D[3] ^ C[3] ^ C[7] ^ C[10] ^ 
                 C[11];

    nextCRC16_D16 = NewCRC;

  end

  endfunction

endmodule

