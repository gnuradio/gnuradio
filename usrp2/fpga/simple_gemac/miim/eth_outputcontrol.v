//////////////////////////////////////////////////////////////////////
////                                                              ////
////  eth_outputcontrol.v                                         ////
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
// $Log: eth_outputcontrol.v,v $
// Revision 1.2  2005/12/13 12:54:49  maverickist
// first simulation passed
//
// Revision 1.1.1.1  2005/12/13 01:51:45  Administrator
// no message
//
// Revision 1.2  2005/04/27 15:58:46  Administrator
// no message
//
// Revision 1.1.1.1  2004/12/15 06:38:54  Administrator
// no message
//
// Revision 1.4  2002/07/09 20:11:59  mohor
// Comment removed.
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

module eth_outputcontrol(Clk, Reset, InProgress, ShiftedBit, BitCounter, WriteOp, NoPre, MdcEn_n, Mdo, MdoEn);

input         Clk;                // Host Clock
input         Reset;              // General Reset
input         WriteOp;            // Write Operation Latch (When asserted, write operation is in progress)
input         NoPre;              // No Preamble (no 32-bit preamble)
input         InProgress;         // Operation in progress
input         ShiftedBit;         // This bit is output of the shift register and is connected to the Mdo signal
input   [6:0] BitCounter;         // Bit Counter
input         MdcEn_n;            // MII Management Data Clock Enable signal is asserted for one Clk period before Mdc falls.

output        Mdo;                // MII Management Data Output
output        MdoEn;              // MII Management Data Output Enable

wire          SerialEn;

reg           MdoEn_2d;
reg           MdoEn_d;
reg           MdoEn;

reg           Mdo_2d;
reg           Mdo_d;
reg           Mdo;                // MII Management Data Output



// Generation of the Serial Enable signal (enables the serialization of the data)
assign SerialEn =  WriteOp & InProgress & ( BitCounter>31 | ( ( BitCounter == 0 ) & NoPre ) )
                | ~WriteOp & InProgress & (( BitCounter>31 & BitCounter<46 ) | ( ( BitCounter == 0 ) & NoPre ));


// Generation of the MdoEn signal
always @ (posedge Clk or posedge Reset)
begin
  if(Reset)
    begin
      MdoEn_2d <= 1'b0;
      MdoEn_d <= 1'b0;
      MdoEn <= 1'b0;
    end
  else
    begin
      if(MdcEn_n)
        begin
          MdoEn_2d <= SerialEn | InProgress & BitCounter<32;
          MdoEn_d <= MdoEn_2d;
          MdoEn <= MdoEn_d;
        end
    end
end


// Generation of the Mdo signal.
always @ (posedge Clk or posedge Reset)
begin
  if(Reset)
    begin
      Mdo_2d <= 1'b0;
      Mdo_d <= 1'b0;
      Mdo <= 1'b0;
    end
  else
    begin
      if(MdcEn_n)
        begin
          Mdo_2d <= ~SerialEn & BitCounter<32;
          Mdo_d <= ShiftedBit | Mdo_2d;
          Mdo <= Mdo_d;
        end
    end
end



endmodule
