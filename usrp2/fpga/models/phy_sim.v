//////////////////////////////////////////////////////////////////////
////                                                              ////
////  Phy_sim.v                                                   ////
////                                                              ////
////  This file is part of the Ethernet IP core project           ////
////  http://www.opencores.org/projects.cgi/web/ethernet_tri_mode/////
////                                                              ////
////  Author(s):                                                  ////
////      - Jon Gao (gaojon@yahoo.com)                            ////
////                                                              ////
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
// $Log: Phy_sim.v,v $
// Revision 1.3  2006/11/17 17:53:07  maverickist
// no message
//
// Revision 1.2  2006/01/19 14:07:50  maverickist
// verification is complete.
//
// Revision 1.1.1.1  2005/12/13 01:51:44  Administrator
// no message
// 

`timescale 1ns/100ps 

module phy_sim(
  input        Gtx_clk, // Used only in GMII mode
  output       Rx_clk,
  output       Tx_clk, // Used only in MII mode
  input        Tx_er,
  input        Tx_en,
  input  [7:0] Txd,
  output       Rx_er,
  output       Rx_dv,
  output [7:0] Rxd,
  output       Crs,
  output       Col,
  input  [2:0] Speed,
  input        Done
);

//////////////////////////////////////////////////////////////////////
// this file used to simulate Phy.
// generate clk and loop the Tx data to Rx data
// full duplex mode can be verified on loop mode.
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// internal signals
//////////////////////////////////////////////////////////////////////
reg Clk_25m;  // Used for 100 Mbps mode
reg Clk_2_5m; // Used for 10 Mbps mode

//wire Rx_clk;
//wire Tx_clk; // Used only in MII mode

  initial 
    begin
      #10;
      while ( !Done )
        begin
          #20 Clk_25m = 0;
          #20 Clk_25m = 1;
        end
    end

  initial 
    begin
      #10;
      while ( !Done )
        begin
          #200 Clk_2_5m = 0;
          #200 Clk_2_5m = 1;
        end
    end

  assign Rx_clk = Speed[2] ? Gtx_clk : Speed[1] ? Clk_25m : Speed[0] ? Clk_2_5m : 0;        
  assign Tx_clk = Speed[2] ? Gtx_clk : Speed[1] ? Clk_25m : Speed[0] ? Clk_2_5m : 0;

  assign Rx_dv = Tx_en;
  assign Rxd   = Txd;
  assign Rx_er = Tx_er;
  assign Crs   = Tx_en;
  assign Col   = 0;

endmodule
