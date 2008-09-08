//////////////////////////////////////////////////////////////////////
////                                                              ////
////  readme.txt                                                  ////
////                                                              ////
////                                                              ////
////  This file is part of the "UART 16550 compatible" project    ////
////  http://www.opencores.org/projects/uart16550/                ////
////                                                              ////
////  Documentation related to this project:                      ////
////  http://www.opencores.org/projects/uart16550/                ////
////                                                              ////
////  Projects compatibility:                                     ////
////  - WISHBONE                                                  ////
////  RS232 Protocol                                              ////
////  16550D uart (mostly supported)                              ////
////                                                              ////
////  Overview (main Features):                                   ////
////  Device interface for testing purposes                       ////
////                                                              ////
////  Known problems (limits):                                    ////
////                                                              ////
////  To Do:                                                      ////
////  Nothing.                                                    ////
////                                                              ////
////  Author(s):                                                  ////
////      - Igor Mohor (igorm@opencores.org)                      ////
////                                                              ////
////  Created and updated:   (See log for the revision history)   ////
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
// $Log: readme.txt,v $
// Revision 1.1  2002/01/25 08:54:56  mohor
// UART PHY added. Files are fully operational, working on HW.
//
//
//
//
//


Following files are making an UART16550 PHY and are used for testing:

uart_device_if_defines.v  - defines related to PHY
uart_device_if_memory.v   - Module for initializing PHY (reading commands from vapi.log file)
uart_device_if.v          - Uart PHY with additional feature for testing
vapi.log                  - File with commands (expected data, data to be send, etc.)
readme.txt                - This file




OPERATION:
uart_device_if.v is a uart PHY and connects to the uart_top.v. PHY takes commands from vapi.log
file. Depending on command it can:
- set a mode (5, 6, 7, 8-bit, parity, stop bits, etc.)
- set a frequency divider (dll)
- send a character
- receive a character and compare it to the expected one
- send a glitch (after a certain period of time)
- send a break 
- detect a break
- Check if fifo is empty/not empty (and generate an error if expected value differs from actual)
- delay (does nothing for certain number of characters)

On the other side of uart some kind of host must be connected that controls the phy.

This is the structure:


 ||||||||||||||              ||||||||||||||||              ||||||||||||||||
 |            |              |              |              |              |
 |   Host     | <----------> |    UART      | <----------> |     PHY      | 
 |            |              |              |              |              |
 ||||||||||||||              ||||||||||||||||              ||||||||||||||||
 
 
 PHY must know how host sets th UART and work in the same mode. Besides that it must know what
 host is sending or expecting to receive. Operation of the PHY must be written in the vapi.log
 file.
 
 When I was using this testing environment, I used OpenRISC1200 as a host. Everything is fully
 operational. UART was also tested in hardware (on two different boards), running uCLinux in
 both, interrupt and polling mode.

 