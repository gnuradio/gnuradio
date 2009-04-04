//////////////////////////////////////////////////////////////////////
////                                                              ////
////  eth_shiftreg.v                                              ////
////                                                              ////
////  This file is part of the Ethernet IP core project           ////
////  http://www.opencores.org/projects/ethmac/                   ////
////                                                              ////
////  Author(s):                                                  ////
////      - Igor Mohor (igorM@opencores.org)                      ////
////                                                              ////
////  All additional information is avaliable in the Readme.txt   ////
////  file.                                                       ////
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
// $Log: eth_shiftreg.v,v $
// Revision 1.2  2005/12/13 12:54:49  maverickist
// first simulation passed
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//
// Revision 1.2  2005/04/27 15:58:47  Administrator
// no message
//
// Revision 1.1.1.1  2004/12/15 06:38:54  Administrator
// no message
//
// Revision 1.5  2002/08/14 18:16:59  mohor
// LinkFail signal was not latching appropriate bit.
//
// Revision 1.4  2002/03/02 21:06:01  mohor
// LinkFail signal was not latching appropriate bit.
//
// Revision 1.3  2002/01/23 10:28:16  mohor
// Link in the header changed.
//
// Revision 1.2  2001/10/19 08:43:51  mohor
// eth_timescale.v changed to timescale.v This is done because of the
// simulation of the few cores in a one joined project.
//
// Revision 1.1  2001/08/06 14:44:29  mohor
// A define FPGA added to select between Artisan RAM (for ASIC) and Block Ram (For Virtex).
// Include files fixed to contain no path.
// File names and module names changed ta have a eth_ prologue in the name.
// File eth_timescale.v is used to define timescale
// All pin names on the top module are changed to contain _I, _O or _OE at the end.
// Bidirectional signal MDIO is changed to three signals (Mdc_O, Mdi_I, Mdo_O
// and Mdo_OE. The bidirectional signal must be created on the top level. This
// is done due to the ASIC tools.
//
// Revision 1.1  2001/07/30 21:23:42  mohor
// Directory structure changed. Files checked and joind together.
//
// Revision 1.3  2001/06/01 22:28:56  mohor
// This files (MIIM) are fully working. They were thoroughly tested. The testbench is not updated.
//
//

module eth_shiftreg(Clk, Reset, MdcEn_n, Mdi, Fiad, Rgad, CtrlData, WriteOp, ByteSelect, 
                    LatchByte, ShiftedBit, Prsd, LinkFail);


input       Clk;              // Input clock (Host clock)
input       Reset;            // Reset signal
input       MdcEn_n;          // Enable signal is asserted for one Clk period before Mdc falls.
input       Mdi;              // MII input data
input [4:0] Fiad;             // PHY address
input [4:0] Rgad;             // Register address (within the selected PHY)
input [15:0]CtrlData;         // Control data (data to be written to the PHY)
input       WriteOp;          // The current operation is a PHY register write operation
input [3:0] ByteSelect;       // Byte select
input [1:0] LatchByte;        // Byte select for latching (read operation)

output      ShiftedBit;       // Bit shifted out of the shift register
output[15:0]Prsd;             // Read Status Data (data read from the PHY)
output      LinkFail;         // Link Integrity Signal

reg   [7:0] ShiftReg;         // Shift register for shifting the data in and out
reg   [15:0]Prsd;
reg         LinkFail;




// ShiftReg[7:0] :: Shift Register Data
always @ (posedge Clk or posedge Reset) 
begin
  if(Reset)
    begin
      ShiftReg[7:0] <= 8'h0;
      Prsd[15:0] <= 16'h0;
      LinkFail <= 1'b0;
    end
  else
    begin
      if(MdcEn_n)
        begin 
          if(|ByteSelect)
            begin
              case (ByteSelect[3:0])
                4'h1 :    ShiftReg[7:0] <= {2'b01, ~WriteOp, WriteOp, Fiad[4:1]};
                4'h2 :    ShiftReg[7:0] <= {Fiad[0], Rgad[4:0], 2'b10};
                4'h4 :    ShiftReg[7:0] <= CtrlData[15:8];
                4'h8 :    ShiftReg[7:0] <= CtrlData[7:0];
                default : ShiftReg[7:0] <= 8'h0;
              endcase
            end 
          else
            begin
              ShiftReg[7:0] <= {ShiftReg[6:0], Mdi};
              if(LatchByte[0])
                begin
                  Prsd[7:0] <= {ShiftReg[6:0], Mdi};
                  if(Rgad == 5'h01)
                    LinkFail <= ~ShiftReg[1];  // this is bit [2], because it is not shifted yet
                end
              else
                begin
                  if(LatchByte[1])
                    Prsd[15:8] <= {ShiftReg[6:0], Mdi};
                end
            end
        end
    end
end


assign ShiftedBit = ShiftReg[7];


endmodule
