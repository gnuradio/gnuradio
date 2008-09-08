//////////////////////////////////////////////////////////////////////
////                                                              ////
////  spi_slave_model.v                                           ////
////                                                              ////
////  This file is part of the SPI IP core project                ////
////  http://www.opencores.org/projects/spi/                      ////
////                                                              ////
////  Author(s):                                                  ////
////      - Simon Srot (simons@opencores.org)                     ////
////                                                              ////
////  All additional information is avaliable in the Readme.txt   ////
////  file.                                                       ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2002 Authors                                   ////
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

`include "timescale.v"

module spi_slave_model (rst, ss, sclk, mosi, miso);

  input         rst;            // reset
  input         ss;             // slave select
  input         sclk;           // serial clock
  input         mosi;           // master out slave in
  output        miso;           // master in slave out

  reg           miso;

  reg           rx_negedge;     // slave receiving on negedge
  reg           tx_negedge;     // slave transmiting on negedge
  reg    [31:0] data;           // data register

  parameter     Tp = 1;

  always @(posedge(sclk && !rx_negedge) or negedge(sclk && rx_negedge) or rst)
  begin
    if (rst)
      data <= #Tp 32'b0;
    else if (!ss)
      data <= #Tp {data[30:0], mosi};
  end

  always @(posedge(sclk && !tx_negedge) or negedge(sclk && tx_negedge))
  begin
    miso <= #Tp data[31];
  end

endmodule
      
