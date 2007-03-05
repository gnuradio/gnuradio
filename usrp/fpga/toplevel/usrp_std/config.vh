// -*- verilog -*-
//
//  USRP - Universal Software Radio Peripheral
//
//  Copyright (C) 2006,2007 Matt Ettus
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

// ====================================================================
//            User control over what parts get included
//
//                  >>>> EDIT ONLY THIS SECTION <<<<
//                  Uncomment only ONE configuration
// ====================================================================

// ====================================================================
// FIXME drive configuration selection from the command line and/or gui
// ====================================================================

// Uncomment this for 1 rx channel (w/ halfband) & 1 transmit channel
//`include "../include/common_config_1rxhb_1tx.vh"

// Uncomment this for 2 rx channels (w/ halfband) & 2 transmit channels
  `include "../include/common_config_2rxhb_2tx.vh"

// Uncomment this for 4 rx channels (w/o halfband) & 0 transmit channels
//`include "../include/common_config_4rx_0tx.vh"

// Uncomment this for multi with 2 rx channels (w/ halfband) & 0 transmit channels
//`include "../include/common_config_2rxhb_0tx.vh"

// Uncomment this for multi with 2 rx channels (w/o halfband) & 0 transmit channels
//`include "../include/common_config_2rx_0tx.vh"

// Add other "known to fit" configurations here...

// ====================================================================
//  Now include the common footer
// ====================================================================
  `include "../include/common_config_bottom.vh"
