//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_device.v                                               ////
////                                                              ////
////  This file is part of the "uart16550" project                ////
////  http://www.opencores.org/projects/uart16550/                ////
////                                                              ////
////  Author(s):                                                  ////
////      - tadej@opencores.org (Tadej Markovic)                  ////
////      - igorm@opencores.org (Igor Mohor)                      ////
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
// $Log: uart_device.v,v $
// Revision 1.1  2004/03/27 03:55:16  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`include "uart_testbench_defines.v"
`include "timescale.v"

module uart_device
(
    // UART signals
    stx_i,
    srx_o,
    // Modem signals
    rts_i,
    cts_o,
    dtr_i,
    dsr_o,
    ri_o,
    dcd_o
);


// IN/OUT signals
//###############

  // UART signals
  input           stx_i;
  output          srx_o;
  // Modem signals
  input           rts_i;
  output          cts_o;
  input           dtr_i;
  output          dsr_o;
  output          ri_o;
  output          dcd_o;


// INTERNAL signals
//#################


  // Clock generation signals
  //#########################

    // Operational and transmission clock signals
    reg          rx_clk;         // RX device clock with period T_clk_period (should be equal to wb_clk_period)
    reg          tx_clk;         // TX device clock with period (T_clk_period + T_clk_delay)
    reg          tx_clk_divided; // divided TX device clock with period ((T_clk_period + T_clk_delay) * T_divisor * 16)
    // Clock enable signals
    reg          rx_clk_en = 1'b1;
    reg          tx_clk_en = 1'b1;
    reg          tx_clk_divided_en = 1'b1;
    // Clock period variables
    real         T_clk_period = 20;
    real         T_clk_delay = 0;
    integer      T_divisor = 5;


  // IN/OUT assignment signals
  //##########################

    // Modem signals
    wire         rts;
    wire         dtr;


  // UART receiver signals
  //######################

    // RX packet control signals
    wire         rx;
    reg   [3:0]  rx_length;
    reg          rx_odd_parity;
    reg          rx_even_parity;
    reg          rx_stick1_parity;
    reg          rx_stick0_parity;
    reg          rx_parity_enabled;
    reg          rx_stop_bit_1;
    reg          rx_stop_bit_1_5;
    reg          rx_stop_bit_2;
    // RX logic signals
    wire  [3:0]  rx_total_length;
    wire  [5:0]  rx_break_detection_length;
    reg          rx_packet_end;
    reg          rx_packet_end_q;
    reg          rx_clk_cnt_en;
    reg  [31:0]  rx_clk_cnt;
    reg          rx_sample_clock;
    integer      rx_bit_index;
    integer      rx_stop_bit_index;
    reg   [7:0]  rx_data;
    reg   [1:0]  rx_stop;
    reg          rx_framing_error;
    reg          rx_parity;
    reg          rx_parity_error;
    reg          rx_break_detected;
    reg          rx_break_detected_q;
    reg  [31:0]  rx_break_cnt;
    // RX events
    event        device_received_packet;
    event        device_received_last_bit;
    event        device_received_stop_bit;
    event        device_detected_rx_break;


  // UART transmitter signals
  //#########################

    // TX packet control signals
    reg          tx;
    reg   [3:0]  tx_length;
    reg          tx_odd_parity;
    reg          tx_even_parity;
    reg          tx_stick1_parity;
    reg          tx_stick0_parity;
    reg          tx_parity_enabled;
    reg          tx_parity_wrong;
    reg          tx_framing_wrong;
    // TX logic signals
    reg  [23:0]  tx_glitch_num;
    reg          start_tx_glitch_cnt;
    reg  [31:0]  tx_glitch_cnt;
    reg          tx_glitch;
    reg          tx_break_enable;
    reg  [15:0]  tx_break_num;
    reg          start_tx_break_cnt;    
    reg  [31:0]  tx_break_cnt;
    reg          tx_break;
    // TX test signals
    reg   [7:0]  sent_data;
    reg          tx_accept_next_framing_err;
    reg          tx_framing_err;
    reg          tx_framing_glitch_err;
    // TX events
    event        device_sent_packet;
    event        sent_packet_received;


// Clock generation
//#################

  // Example of TESTBENCH's task for setting UART clock period:
  //    ----------------
  //    task set_uart_clk_period;
  //      input [31:0]  clk_period;
  //    begin
  //      //@(posedge testbench.uart_device.clk);
  //      testbench.uart_device.T_clk_period = clk_period;
  //    end
  //    endtask // set_uart_clk_period
  //    ----------------
  // Example of TESTBENCH's task for setting UART clock rising edge 
  // delayed for time_delay_i after WB clock rising edge:
  //    ----------------
  //    task uart_clk_follows_wb_clk;
  //      input [31:0]  time_delay_i;
  //      integer       time_delay;
  //    begin
  //      time_delay = time_delay_i;
  //      @(posedge testbench.uart_device.clk);
  //      testbench.uart_device.clk_en = 1'b0;
  //      @(posedge wb_clk);
  //      #time_delay testbench.uart_device.clk = 1'b1;
  //      testbench.uart_device.clk_en = 1'b1;
  //    end
  //    endtask // uart_clk_follows_wb_clk
  //    ----------------

  // rx_clk rising edge
  always@(posedge rx_clk)
    if (rx_clk_en)
      #(T_clk_period / 2) rx_clk = 1'b0;
  // rx_clk falling edge
  always@(negedge rx_clk)
    if (rx_clk_en)
      #(T_clk_period / 2) rx_clk = 1'b1;

  // tx_clk rising edge
  always@(posedge tx_clk)
    if (tx_clk_en)
      #((T_clk_period + T_clk_delay) / 2) tx_clk = 1'b0;
  // tx_clk falling edge
  always@(negedge tx_clk)
    if (tx_clk_en)
      #((T_clk_period + T_clk_delay) / 2) tx_clk = 1'b1;

  // tx_clk_divided rising edge
  always@(posedge tx_clk_divided)
    if (tx_clk_divided_en)
      #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) tx_clk_divided = 1'b0;
  // tx_clk_divided falling edge
  always@(negedge tx_clk_divided)
    if (tx_clk_divided_en)
      #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) tx_clk_divided = 1'b1;

  // Inital CLK values
  initial
  begin:device
    rx_clk = 1'b0;
    tx_clk = 1'b0;
    tx_clk_divided = 1'b0;
  end


// IN/OUT assignments
//###################

  // UART output
  assign srx_o = (tx ^ tx_glitch) & ~tx_break;
  // Modem output
  assign cts_o  = 0;
  assign dsr_o  = 0;
  assign ri_o   = 0;
  assign dcd_o  = 0;
  // UART input
  assign rx = stx_i;
  // Modem input
  assign rts = rts_i;
  assign dtr = dtr_i;


// UART receiver
//##############

  // Initial values for RX
  initial
  begin
    // Default LENGTH
    rx_length = 8;
    // Default PARITY
    rx_odd_parity     = 1'b0;
    rx_even_parity    = 1'b0;
    rx_stick1_parity  = 1'b0;
    rx_stick0_parity  = 1'b0;
    rx_parity_enabled = 1'b0;
    // Default STOP
    rx_stop_bit_1   = 1'b1;
    rx_stop_bit_1_5 = 1'b0;
    rx_stop_bit_2   = 1'b0;
  end

  // Total length of RX packet (for proper generation of the rx_packet_end signal): 
  //   data length + parity + 1 stop bit + second stop bit (when enabled)
  assign rx_total_length = rx_length + rx_parity_enabled + 1 + rx_stop_bit_2;
  //   +1 is used because start bit was not included in rx_total_length.
  assign rx_break_detection_length = rx_total_length + 1;

  // Generating rx_clk_cnt_en signal.
  always@(posedge rx_clk)
  begin
    if (~rx_clk_cnt_en)
    begin
      wait (~rx);
    end
    rx_clk_cnt_en = 1;
    rx_packet_end = 0;
    wait (rx_packet_end);
    rx_clk_cnt_en = 0;
    wait (rx); // Must be high to continue, because of break condition
  end

  // Counter used in data reception
  always@(posedge rx_clk)
  begin
    if (rx_clk_cnt_en)
    begin
      if (rx_clk_cnt == (8 * T_divisor - 1) & rx) // False start bit detection
        rx_packet_end = 1;
      if (rx_clk_cnt_en) // Checking is still enabled after devisor clocks
        rx_clk_cnt <= #1 rx_clk_cnt + 1;
      else
        rx_clk_cnt <= #1 0;
    end
    else
      rx_clk_cnt <= #1 0;
  end

  // Delayed rx_packet_end signal
  always@(posedge rx_clk)
    rx_packet_end_q = rx_packet_end;

  // Generating sample clock and end of the frame (Received data is sampled with this clock)
  always@(posedge rx_clk)
  begin
    if (rx_clk_cnt == 8 * T_divisor - 1)
      rx_bit_index = 0;
    else if (rx_clk_cnt == (8 * T_divisor + 16 * T_divisor * (rx_bit_index + 1) - 1))
    begin
      rx_sample_clock = 1;
      rx_bit_index = rx_bit_index + 1;
      if (rx_bit_index == rx_total_length)
        rx_packet_end = 1;
    end
    else
      rx_sample_clock = 0;
  end

  // Sampling data (received data)
  always@(posedge rx_clk)
  begin
    if (rx_sample_clock)
    begin
      if (rx_bit_index <= rx_length) // Sampling data
      begin
        rx_stop_bit_index <= 0; // Stop bit index reset at the beginning of the data stage
//        $display("\t\t\t\t\t\t\t(rx_bit_index = %0d) Reading data bits = %0x", rx_bit_index, rx);
        rx_data[rx_bit_index - 1] = rx;
        if (rx_bit_index == rx_length)
          -> device_received_last_bit;
      end
      else
      begin
        if (rx_bit_index == (rx_length + 1))
        begin
          if (rx_parity_enabled)
          begin
//            $display("\t\t\t\t\t\t\t(rx_bit_index = %0d) Reading parity bits = %0x", rx_bit_index, rx);
          end
          else
          begin
            -> device_received_stop_bit;
            rx_stop[rx_stop_bit_index] = rx;
            rx_stop_bit_index <= rx_stop_bit_index + 1;
          end
          rx_parity = rx & rx_parity_enabled;
        end
        if (rx_bit_index >= (rx_length + 1 + rx_parity_enabled))
        begin
//          $display("\t\t\t\t\t\t\t(rx_bit_index = %0d) Reading stop bits = %0x", rx_bit_index, rx);
          rx_stop[rx_stop_bit_index] = rx;
          rx_stop_bit_index <= rx_stop_bit_index + 1;
        end
      end
    end
  
    // Filling the rest of the data with 0
    if (rx_length == 5)
      rx_data[7:5] = 0;
    if (rx_length == 6)
      rx_data[7:6] = 0;
    if (rx_length == 7)
      rx_data[7] = 0;
  
    // Framing error generation
    //   When 1 or 1.5 stop bits are used, only first stop bit is checked
    rx_framing_error = (rx_stop_bit_1 | rx_stop_bit_1_5) ? ~rx_stop[0] : ~(&rx_stop[1:0]);
  
    // Parity error generation
    if (rx_odd_parity)
      rx_parity_error = ~(^{rx_data, rx_parity});
    else if (rx_even_parity)
      rx_parity_error = ^{rx_data, rx_parity};
    else if (rx_stick0_parity)
      rx_parity_error = rx_parity;
    else if (rx_stick1_parity)
      rx_parity_error = ~rx_parity;
    else
      rx_parity_error = 0;
  end

  // Break detection
  always@(posedge rx_clk)
  begin
    rx_break_detected_q <= rx_break_detected;
    if (rx)
    begin
      rx_break_cnt = 0;         // Reseting counter
      rx_break_detected = 0;       // Clearing break detected signal
    end
    else
      rx_break_cnt = rx_break_cnt + 1;
    if (rx_break_cnt == rx_break_detection_length * 16 * T_divisor)
    begin
//      $display("\n(%0t) Break_detected.", $time);
      rx_break_detected <= 1;
      -> device_detected_rx_break;
    end
  end
  
  // Writing received data
  always@(posedge rx_clk)
  begin
    if ((rx_packet_end & ~rx_packet_end_q) | (rx_break_detected & ~rx_break_detected_q))
    begin
      wait (rx | rx_break_detected); // Waiting for "end of cycle detected" or "break to be activated"
      // rx_break_detected
      //   rx_length
      //   rx_parity_enabled
      //     rx_odd_parity | rx_even_parity | rx_stick1_parity | rx_stick0_parity
      //   rx_stop_bit_1 | rx_stop_bit_1_5 | rx_stop_bit_2
      -> device_received_packet;
    end
  end


// UART transmitter
//#################

  // Initial values for TX
  initial
  begin
    // Default LENGTH
    tx_length = 8;
    // Default PARITY
    tx_odd_parity     = 1'b0;
    tx_even_parity    = 1'b0;
    tx_stick1_parity  = 1'b0;
    tx_stick0_parity  = 1'b0;
    tx_parity_enabled = 1'b0;
    // Default CORRECT PARITY
    tx_parity_wrong = 1'b0;
    // Default CORRECT FRAME
    tx_framing_wrong = 1'b0;
    tx_framing_err        = 0;
    tx_framing_glitch_err = 0;
    // Default NO GLITCH
    tx_glitch_num = 24'h0;
    // Default NO BREAK
    tx_break_enable = 1'b0;
    tx_break_num = 16'h0;
  end

  // Counter for TX glitch generation
  always@(posedge tx_clk or posedge start_tx_glitch_cnt)
  begin
    if (start_tx_glitch_cnt)
    begin
      tx_glitch_cnt <= tx_glitch_cnt + 1;
      if (tx_glitch_cnt == ((tx_glitch_num - 1) * T_divisor))
        tx_glitch = 1'b1;
      else if (tx_glitch_cnt == (tx_glitch_num * T_divisor))
      begin
        tx_glitch = 1'b0;
        start_tx_glitch_cnt = 1'b0;
      end
    end
    else
      tx_glitch_cnt <= 0;
  end

  // Break setting & break counter
  always@(posedge tx_clk)
  begin
    if (tx_break_enable && (tx_break_cnt == (tx_break_num * T_divisor)))
    begin
      start_tx_break_cnt = 0;
    end
    else if (start_tx_break_cnt)
    begin
      tx_break_cnt = tx_break_cnt + 1;
      tx_break = 1;
    end
    else
    begin
      tx_break_cnt = 0;
      tx_break = 0;
    end
  end
  
  // Sending packets
  task send_packet;
    input          tx_random_i;
    input   [7:0]  tx_data_i;
    input          num_of_tx_data_i;
    reg     [7:0]  tx_data;
    reg            tx_parity_xor;
    integer        tx_bit_index;
    integer        num_of_tx_data;
    reg            last_tx_data;
  begin
    // SEVERE ERROR
    if (// WRONG combinations of parameters for testing 
        ((T_clk_delay != 0) && (tx_parity_wrong || tx_framing_wrong))               || 
        ((T_clk_delay != 0) && (tx_glitch_num != 0))                                || 
        ((T_clk_delay != 0) && (tx_break_enable))                                   || 
        ((tx_parity_wrong || tx_framing_wrong) && (tx_glitch_num != 0))             || 
        ((tx_parity_wrong || tx_framing_wrong) && (tx_break_enable))                || 
        ((tx_glitch_num != 0) && (tx_break_enable))                                 || 
        (tx_glitch_num > ((tx_length + 2'h2 + tx_parity_enabled) * 16 * T_divisor)) || // with STOP bit
//        (tx_glitch_num > ((tx_length + 2'h1 + tx_parity_enabled) * 16 * T_divisor)) || // without STOP bit
        // WRONG input parameters
        (num_of_tx_data_i == 0)                                                     || 
        ((num_of_tx_data_i > 1) && tx_break_enable)                                 
       )
    begin
      `SEVERE_ERROR("WRONG combination of parameters for testing UART receiver");
    end
    
    for (num_of_tx_data = 0; 
         num_of_tx_data < num_of_tx_data_i; 
         num_of_tx_data = (num_of_tx_data + 1'b1))
    begin
    
      if (num_of_tx_data == (num_of_tx_data_i - 1'b1))
        last_tx_data = 1'b1;
      else
        last_tx_data = 0;
    
      // TX data
      if (~tx_random_i)
        tx_data = tx_data_i;
      else
        tx_data = {$random}%256; // 0..255
    
      // Sending start bit
      @(posedge tx_clk_divided);
      tx = 0;
      if (tx_glitch_num > 0)
        start_tx_glitch_cnt = 1;            // enabling tx_glitch generation
      if (tx_break_enable)
        start_tx_break_cnt = 1;       // Start counter that counts break tx_length
      // Wait for almost 1 bit
      #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor);       // wait half period
      #((((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) - 2); // wait 2 less than half period
    
      // Sending tx_data bits
      for (tx_bit_index = 0; tx_bit_index < tx_length; tx_bit_index = tx_bit_index + 1)
      begin
        @(posedge tx_clk_divided);
        tx = tx_data[tx_bit_index];
      end
      // Wait for almost 1 bit
      #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor);       // wait half period
      #((((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) - 2); // wait 2 less than half period

      sent_data = tx_data;
    
      // Calculating parity
      if(tx_length == 5)
      begin
        tx_parity_xor = ^tx_data[4:0];
      end
      else if(tx_length == 6)
      begin
        tx_parity_xor = ^tx_data[5:0];
      end
      else if(tx_length == 7)
      begin
        tx_parity_xor = ^tx_data[6:0];
      end
      else if(tx_length == 8)
      begin
        tx_parity_xor = ^tx_data[7:0];
      end
      else
        $display("WRONG length of TX data packet");
    
      // Sending parity bit
      if (tx_parity_enabled)
      begin
        @(posedge tx_clk_divided);
        if (tx_odd_parity)
          tx = tx_parity_wrong ^ (~tx_parity_xor);
        else if (tx_even_parity)
          tx = tx_parity_wrong ^ tx_parity_xor;
        else if (tx_stick1_parity)
          tx = tx_parity_wrong ^ 1;
        else if (tx_stick0_parity)
          tx = tx_parity_wrong ^ 0;
        // Wait for almost 1 bit
        #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor);       // wait half period
        #((((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) - 2); // wait 2 less than half period
      end
    
      // Sending stop bit
      if (~tx_framing_wrong || 
          (tx_glitch_num != ((((tx_length + 2'h2 + tx_parity_enabled) * 2) - 1'b1) * 8 * T_divisor)))
      begin
        @(posedge tx_clk_divided);
        tx = 1;
        // Wait for almost 1 bit
        #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor);       // wait half period
        #((((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) - 2); // wait 2 less than half period
        -> device_sent_packet;
        @(sent_packet_received);
      end
      else if (~tx_framing_wrong || 
               (tx_glitch_num == ((((tx_length + 2'h2 + tx_parity_enabled) * 2) - 1'b1) * 8 * T_divisor)))
      begin
        @(posedge tx_clk_divided);
        tx = 1;
        // Wait for 1 bit
        @(posedge tx_clk_divided); // this will be like 2. stop bit
        -> device_sent_packet;
        @(sent_packet_received);
      end
      else if (tx_framing_wrong && last_tx_data)
      begin
        @(posedge tx_clk_divided);
        // Wrong stop | start bit
        tx = 0;
        @(posedge tx_clk_divided);
        -> device_sent_packet;
        @(sent_packet_received);
        tx_framing_wrong = 0;
        // TX data
        tx = 1;
        tx_data = 8'hFF;
        // Sending tx_data bits
        for (tx_bit_index = 0; tx_bit_index < tx_length; tx_bit_index = tx_bit_index + 1)
        begin
          @(posedge tx_clk_divided);
          tx = tx_data[tx_bit_index];
        end
        // Wait for almost 1 bit
        #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor);       // wait half period
        #((((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) - 2); // wait 2 less than half period
  
        sent_data = tx_data;
      
        // Calculating parity
        if(tx_length == 5)
        begin
          tx_parity_xor = ^tx_data[4:0];
        end
        else if(tx_length == 6)
        begin
          tx_parity_xor = ^tx_data[5:0];
        end
        else if(tx_length == 7)
        begin
          tx_parity_xor = ^tx_data[6:0];
        end
        else if(tx_length == 8)
        begin
          tx_parity_xor = ^tx_data[7:0];
        end
        else
          $display("WRONG length of TX data packet");
      
        // Sending parity bit
        if (tx_parity_enabled)
        begin
          @(posedge tx_clk_divided);
          if (tx_odd_parity)
            tx = tx_parity_wrong ^ (~tx_parity_xor);
          else if (tx_even_parity)
            tx = tx_parity_wrong ^ tx_parity_xor;
          else if (tx_stick1_parity)
            tx = tx_parity_wrong ^ 1;
          else if (tx_stick0_parity)
            tx = tx_parity_wrong ^ 0;
          // Wait for almost 1 bit
          #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor);       // wait half period
          #((((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) - 2); // wait 2 less than half period
        end
        
        // Stop bit
        @(posedge tx_clk_divided);
        tx = 1;
        // Wait for almost 1 bit
        #(((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor);       // wait half period
        #((((T_clk_period + T_clk_delay) / 2) * 16 * T_divisor) - 2); // wait 2 less than half period
        -> device_sent_packet;
        @(sent_packet_received);
        tx_framing_wrong = 1'b1;
      end
      else if (last_tx_data)
      begin
        @(posedge tx_clk_divided);
        -> device_sent_packet;
        @(sent_packet_received);
      end
    end
  end
  endtask // send_packet


endmodule
