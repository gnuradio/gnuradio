//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_device_utilities.v                                     ////
////                                                              ////
////  This file is part of the "uart16550" project                ////
////  http://www.opencores.org/projects/uart16550/                ////
////                                                              ////
////  Author(s):                                                  ////
////      - tadej@opencores.org (Tadej Markovic)                  ////
////                                                              ////
////  All additional information is avaliable in the README.txt   ////
////  file.                                                       ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2000 - 2004 authors                            ////
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
// $Log: uart_device_utilities.v,v $
// Revision 1.1  2004/03/27 03:55:16  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`include "uart_defines.v"
`include "uart_testbench_defines.v"
`include "wb_model_defines.v"
`include "timescale.v"

module uart_device_utilities;

// UART receiver setting TASKs
//############################

  // Set RX length
  task set_rx_length;
    input   [3:0]  len;
  begin
    `UTILS_MSG("SETTING RX CHAR length.");
    testbench.i_uart_device.rx_length = len;
    `UTILS_VAL1("Length:", len);
  end
  endtask // set_rx_length

  // Enable RX odd parity
  task enable_rx_odd_parity;
  begin
    `UTILS_MSG("ENABLING RX CHAR odd parity.");
    testbench.i_uart_device.rx_odd_parity     = 1'b1;
    testbench.i_uart_device.rx_even_parity    = 1'b0;
    testbench.i_uart_device.rx_stick1_parity  = 1'b0;
    testbench.i_uart_device.rx_stick0_parity  = 1'b0;
    testbench.i_uart_device.rx_parity_enabled = 1'b1;
  end
  endtask // enable_rx_odd_parity

  // Enable RX even parity
  task enable_rx_even_parity;
  begin
    `UTILS_MSG("ENABLING RX CHAR even parity.");
    testbench.i_uart_device.rx_odd_parity     = 1'b0;
    testbench.i_uart_device.rx_even_parity    = 1'b1;
    testbench.i_uart_device.rx_stick1_parity  = 1'b0;
    testbench.i_uart_device.rx_stick0_parity  = 1'b0;
    testbench.i_uart_device.rx_parity_enabled = 1'b1;
  end
  endtask // enable_rx_even_parity

  // Enable RX stick1 parity
  task enable_rx_stick1_parity;
  begin
    `UTILS_MSG("ENABLING RX CHAR stick1 parity.");
    testbench.i_uart_device.rx_odd_parity     = 1'b0;
    testbench.i_uart_device.rx_even_parity    = 1'b0;
    testbench.i_uart_device.rx_stick1_parity  = 1'b1;
    testbench.i_uart_device.rx_stick0_parity  = 1'b0;
    testbench.i_uart_device.rx_parity_enabled = 1'b1;
  end
  endtask // enable_rx_stick1_parity

  // Enable RX stick0 parity
  task enable_rx_stick0_parity;
  begin
    `UTILS_MSG("ENABLING RX CHAR stick0 parity.");
    testbench.i_uart_device.rx_odd_parity     = 1'b0;
    testbench.i_uart_device.rx_even_parity    = 1'b0;
    testbench.i_uart_device.rx_stick1_parity  = 1'b0;
    testbench.i_uart_device.rx_stick0_parity  = 1'b1;
    testbench.i_uart_device.rx_parity_enabled = 1'b1;
  end
  endtask // enable_rx_stick0_parity

  // Disable RX parity
  task disable_rx_parity;
  begin
    `UTILS_MSG("DISABLING RX CHAR parity.");
    testbench.i_uart_device.rx_odd_parity     = 1'b0;
    testbench.i_uart_device.rx_even_parity    = 1'b0;
    testbench.i_uart_device.rx_stick1_parity  = 1'b0;
    testbench.i_uart_device.rx_stick0_parity  = 1'b0;
    testbench.i_uart_device.rx_parity_enabled = 1'b0;
  end
  endtask // disable_rx_parity

  // Set 1 or 2 (1.5) RX stop bits
  task set_rx_second_stop_bit;
    input          second_stop_bit;
  begin
    if (~second_stop_bit)
    begin
      `UTILS_MSG("SETTING RX CHAR 1 stop bit.");
    end
    else if (second_stop_bit && (testbench.i_uart_device.rx_length == 5))
    begin
      `UTILS_MSG("SETTING RX CHAR 1.5 stop bit.");
    end
    else
    begin
      `UTILS_MSG("SETTING RX CHAR 2 stop bits.");
    end
    testbench.i_uart_device.rx_stop_bit_1   = ~second_stop_bit;
    testbench.i_uart_device.rx_stop_bit_1_5 =  second_stop_bit & (testbench.i_uart_device.rx_length == 5);
    testbench.i_uart_device.rx_stop_bit_2   =  second_stop_bit & (testbench.i_uart_device.rx_length != 5);
  end
  endtask // set_rx_second_stop_bit

// UART transmitter setting TASKs
//###############################

  // Set TX length
  task set_tx_length;
    input   [3:0]  len;
  begin
    `UTILS_MSG("SETTING TX CHAR length.");
    testbench.i_uart_device.tx_length = len;
    `UTILS_VAL1("Length:", len);
  end
  endtask // set_tx_length

  // Enable TX odd parity
  task enable_tx_odd_parity;
  begin
    `UTILS_MSG("ENABLING TX CHAR odd parity.");
    testbench.i_uart_device.tx_odd_parity     = 1'b1;
    testbench.i_uart_device.tx_even_parity    = 1'b0;
    testbench.i_uart_device.tx_stick1_parity  = 1'b0;
    testbench.i_uart_device.tx_stick0_parity  = 1'b0;
    testbench.i_uart_device.tx_parity_enabled = 1'b1;
  end
  endtask // enable_tx_odd_parity

  // Enable TX even parity
  task enable_tx_even_parity;
  begin
    `UTILS_MSG("ENABLING TX CHAR even parity.");
    testbench.i_uart_device.tx_odd_parity     = 1'b0;
    testbench.i_uart_device.tx_even_parity    = 1'b1;
    testbench.i_uart_device.tx_stick1_parity  = 1'b0;
    testbench.i_uart_device.tx_stick0_parity  = 1'b0;
    testbench.i_uart_device.tx_parity_enabled = 1'b1;
  end
  endtask // enable_tx_even_parity

  // Enable TX stick1 parity
  task enable_tx_stick1_parity;
  begin
    `UTILS_MSG("ENABLING TX CHAR stick1 parity.");
    testbench.i_uart_device.tx_odd_parity     = 1'b0;
    testbench.i_uart_device.tx_even_parity    = 1'b0;
    testbench.i_uart_device.tx_stick1_parity  = 1'b1;
    testbench.i_uart_device.tx_stick0_parity  = 1'b0;
    testbench.i_uart_device.tx_parity_enabled = 1'b1;
  end
  endtask // enable_tx_stick1_parity

  // Enable TX stick0 parity
  task enable_tx_stick0_parity;
  begin
    `UTILS_MSG("ENABLING TX CHAR stick0 parity.");
    testbench.i_uart_device.tx_odd_parity     = 1'b0;
    testbench.i_uart_device.tx_even_parity    = 1'b0;
    testbench.i_uart_device.tx_stick1_parity  = 1'b0;
    testbench.i_uart_device.tx_stick0_parity  = 1'b1;
    testbench.i_uart_device.tx_parity_enabled = 1'b1;
  end
  endtask // enable_tx_stick0_parity

  // Disable TX parity
  task disable_tx_parity;
  begin
    `UTILS_MSG("DISABLING TX CHAR parity.");
    testbench.i_uart_device.tx_odd_parity     = 1'b0;
    testbench.i_uart_device.tx_even_parity    = 1'b0;
    testbench.i_uart_device.tx_stick1_parity  = 1'b0;
    testbench.i_uart_device.tx_stick0_parity  = 1'b0;
    testbench.i_uart_device.tx_parity_enabled = 1'b0;
  end
  endtask // disable_tx_parity
  
  // Correct TX parity
  task correct_tx_parity;
  begin
    `UTILS_MSG("DISABLING WRONG parity generation.");
    testbench.i_uart_device.tx_parity_wrong = 1'b0;
  end
  endtask // correct_tx_parity
  
  // Wrong TX parity
  task wrong_tx_parity;
  begin
    `UTILS_MSG("ENABLING WRONG parity generation.");
    testbench.i_uart_device.tx_parity_wrong = 1'b1;
  end
  endtask // wrong_tx_parity

  // Correct TX frame
  task correct_tx_frame;
  begin
    `UTILS_MSG("DISABLING WRONG frame generation.");
    testbench.i_uart_device.tx_framing_wrong = 1'b0;
  end
  endtask // correct_tx_frame

  // Wrong TX frame
  task wrong_tx_frame;
  begin
    `UTILS_MSG("ENABLING WRONG frame generation.");
    testbench.i_uart_device.tx_framing_wrong = 1'b1;
  end
  endtask // wrong_tx_frame

  // Generate TX glitch
  task generate_tx_glitch;
    input  [23:0]  generate_glitch_num;
  begin
    if (generate_glitch_num == 0)
    begin
      `UTILS_MSG("DISABLING 1 TIME glitch generation with CLKs delay.");
    end
    else
    begin
      `UTILS_MSG("ENABLING 1 TIME glitch generation with CLKs delay.");
    end
    testbench.i_uart_device.tx_glitch_num = generate_glitch_num;
    `UTILS_VAL1("CLKs delay from start bit edge:", generate_glitch_num);
  end
  endtask // generate_tx_glitch

  // Enable TX break
  task enable_tx_break;
    input  [15:0]  break_num;
  begin
    `UTILS_MSG("ENABLING brake generation with each TX CHAR with brake length.");
    testbench.i_uart_device.tx_break_enable = 1'b1;
    testbench.i_uart_device.tx_break_num = break_num;
    `UTILS_VAL1("Brake bit length:", break_num);
  end
  endtask // enable_tx_break
  
  // Disable TX break
  task disable_tx_break;
  begin
    `UTILS_MSG("DISABLING brake generation with each TX CHAR.");
    testbench.i_uart_device.tx_break_enable = 1'b0;
  end
  endtask // disable_tx_break

// UART transmitter send TASKs
//############################

  // Send character
  task send_char;
    input  [7:0]  char;
  begin
    testbench.i_uart_device.send_packet(1'b0, char, 1);
  end
  endtask // Send character
  
  // Send random character
  task send_rnd_char;
  begin
    testbench.i_uart_device.send_packet(1'b1, 8'h0, 1);
  end
  endtask // send_rnd_char
  
  // Send burst random character
  task send_burst_rnd_char;
    input  [31:0] num_of_char;
    integer       i;
  begin
    testbench.i_uart_device.send_packet(1'b1, 8'h0, num_of_char);
  end
  endtask // send_burst_rnd_char


endmodule