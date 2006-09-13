// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2003 Matt Ettus
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
//

module gen_sync
  ( input clock,
    input reset,
    input enable,
    input [7:0] rate,
    output wire sync );
   
//   parameter width = 8;
   
   reg [7:0] counter;
   assign sync = |(((rate+1)>>1)& counter);
      
   always @(posedge clock)
     if(reset || ~enable)
       counter <= #1 0;
     else if(counter == rate)
       counter <= #1 0;
     else 
       counter <= #1 counter + 8'd1;
   
endmodule // gen_sync

