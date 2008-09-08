//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_testbench.v                                            ////
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
// $Log: uart_testbench.v,v $
// Revision 1.1  2004/03/27 03:55:17  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`include "uart_defines.v"
`include "uart_testbench_defines.v"
`include "wb_model_defines.v"
`include "timescale.v"

module testbench;


parameter max_wait_cnt = 20000;

// INTERNAL signals
//#################

  // WB slave signals
  //#################

    // UART Wishbone Slave signals
    wire                         wb_int_o;
    wire [`UART_ADDR_WIDTH-1:0]  wbs_adr_i;
    wire [`UART_DATA_WIDTH-1:0]  wbs_dat_i;
    wire [`UART_DATA_WIDTH-1:0]  wbs_dat_o;
    wire                  [3:0]  wbs_sel_i;
    wire                         wbs_cyc_i;
    wire                         wbs_stb_i;
    wire                  [2:0]  wbs_cti_i;
    wire                  [1:0]  wbs_bte_i;
    wire                         wbs_we_i;
    wire                         wbs_ack_o;
    wire                         wbs_rty_o = 1'b0;
    wire                         wbs_err_o = 1'b0;

  // UART signals
  //#############

    // UART Serial Data I/O signals
    wire                         stx_pad_o;
    wire                         srx_pad_i;
    // UART Modem I/O signals
    wire                         rts_pad_o;
    wire                         cts_pad_i;
    wire                         dtr_pad_o;
    wire                         dsr_pad_i;
    wire                         ri_pad_i;
    wire                         dcd_pad_i;
  `ifdef UART_HAS_BAUDRATE_OUTPUT
    wire                         baud_o;
  `endif

  // System signals
  //###############

    // WB clock signal
    reg          wb_clk; // divided device clock with period T_wb_clk_period
    // WB clock enable signal
    reg          wb_clk_en = 1'b1;
    // WB clock period variable
    real         T_wb_clk_period = 20;
    // WB reset signal
    reg          wb_reset;
    event        reset_aserted;
    event        reset_released;
    event        int_aserted;
    event        int_released;
    // Error detection event
    event        error_detected;

  // UART register monitor
  //#########################

    // Line Status Register
      // Reading LSR register
      reg          lsr_reg_read;
      // Bit 0 - Data Ready
      reg          lsr_reg_bit0_change_allowed;
      // Bit 1 - Overrun Error
      reg          lsr_reg_bit1_change_allowed;
      // Bit 2 - Parity Error
      reg          lsr_reg_bit2_change_allowed;
      reg   [4:0]  rx_fifo_par_rd_pointer;
      integer      i2;
      // Bit 3 - Framing Error
      reg          lsr_reg_bit3_change_allowed;
      reg   [4:0]  rx_fifo_frm_rd_pointer;
      integer      i3;
      // Bit 4 - Break Interrupt
      reg          lsr_reg_bit4_change_allowed;
      reg   [4:0]  rx_fifo_brk_rd_pointer;
      integer      i4;
      // Bit 5 - Transmitter Holding Register Empty
      reg          lsr_reg_bit5_change_allowed;
      // Bit 6 - Transmitter Empty
      reg          lsr_reg_bit6_change_allowed;
      // Bit 7 - Error in RX FIFO
      reg          lsr_reg_bit7_change_allowed;

  // UART transmitter monitor
  //#########################

    // TX FIFO signals
    reg   [7:0]  tx_shift_reg;
    reg          tx_shift_reg_empty;
    reg          tx_start_bit_edge;
    reg   [7:0]  tx_fifo [0:31]; 
    reg   [4:0]  tx_fifo_wr_pointer;
    reg   [4:0]  tx_fifo_rd_pointer;
    reg   [4:0]  tx_fifo_status;

  // UART receiver monitor
  //######################

    // RX FIFO signals
    reg   [7:0]  rx_shift_reg;
    reg          rx_shift_reg_full;
    reg          rx_parity_err;
    reg          rx_framing_err;
    reg          rx_framing_glitch;
    reg          rx_break_int;
    reg          rx_overrun_err_occured;
    reg   [7:0]  rx_fifo_data [0:31]; 
    reg  [31:0]  rx_fifo_par; 
    reg  [31:0]  rx_fifo_frm; 
    reg  [31:0]  rx_fifo_brk; 
    reg   [4:0]  rx_fifo_wr_pointer;
    reg   [4:0]  rx_fifo_rd_pointer;
    reg   [4:0]  rx_fifo_status;
    reg          rx_fifo_read;

  // UART register tracker
  //######################

    // Registers
    wire  [7:0]  ier_reg;
    wire  [7:0]  iir_reg;
    wire  [7:0]  fcr_reg;
    wire  [7:0]  lcr_reg;
    wire  [7:0]  mcr_reg;
    wire  [7:0]  lsr_reg;
    wire  [7:0]  msr_reg;
    wire  [7:0]  dll_reg;
    wire  [7:0]  dlm_reg;
    // Events
    event        ier_reg_changed;
    event        iir_reg_changed;
    event        fcr_reg_changed;
    event        lcr_reg_changed;
    event        mcr_reg_changed;
    event        lsr_reg_changed;
    event        msr_reg_changed;
    event        dll_reg_changed;
    event        dlm_reg_changed;
    // Register access
    reg   [`UART_ADDR_WIDTH-1:0] reg_adr;
    reg   [`UART_DATA_WIDTH-1:0] reg_dat;
    reg          reg_dlab;
    event        reg_written;
    event        tx_reg_written;
    event        reg_read;
    event        rx_reg_read;



uart_top                #(`UART_DATA_WIDTH, `UART_ADDR_WIDTH) i_uart_top
(
    .wb_clk_i           (wb_clk),
    .wb_rst_i           (wb_reset),
    .int_o              (wb_int_o),
// WB slave signals - 2 address locations for two registers!
    .wb_cyc_i           (wbs_cyc_i),
    .wb_stb_i           (wbs_stb_i),
    .wb_we_i            (wbs_we_i),
    .wb_sel_i           (wbs_sel_i),
    .wb_adr_i           (wbs_adr_i),
    .wb_dat_i           (wbs_dat_i),
    .wb_dat_o           (wbs_dat_o),
    .wb_ack_o           (wbs_ack_o),
// UART signals
    .stx_pad_o          (stx_pad_o),
    .srx_pad_i          (srx_pad_i),
// Modem signals
    .rts_pad_o          (rts_pad_o),
    .cts_pad_i          (cts_pad_i),
    .dtr_pad_o          (dtr_pad_o),
    .dsr_pad_i          (dsr_pad_i),
    .ri_pad_i           (ri_pad_i),
    .dcd_pad_i          (dcd_pad_i)
`ifdef UART_HAS_BAUDRATE_OUTPUT
    ,
    .baud_o             (baud_o)
`endif
); 

uart_device             i_uart_device
(
// UART signals
    .stx_i              (stx_pad_o),
    .srx_o              (srx_pad_i),
// Modem signals    
    .rts_i              (rts_pad_o),
    .cts_o              (cts_pad_i),
    .dtr_i              (dtr_pad_o),
    .dsr_o              (dsr_pad_i),
    .ri_o               (ri_pad_i),
    .dcd_o              (dcd_pad_i)
);

wb_master_model         #(`UART_DATA_WIDTH, `UART_ADDR_WIDTH, 4) i_wb_master_model
(
    .wb_rst_i           (wb_reset),
    .wb_clk_i           (wb_clk),
    .wbm_cyc_o          (wbs_cyc_i),
    .wbm_cti_o          (),
    .wbm_bte_o          (),
    .wbm_stb_o          (wbs_stb_i),
    .wbm_we_o           (wbs_we_i),
    .wbm_adr_o          (wbs_adr_i),
    .wbm_sel_o          (wbs_sel_i),
    .wbm_dat_o          (wbs_dat_i),
    .wbm_dat_i          (wbs_dat_o),
    .wbm_ack_i          (wbs_ack_o),
    .wbm_err_i          (wbs_err_o), // inactive (1'b0)
    .wbm_rty_i          (wbs_rty_o)  // inactive (1'b0)
);


initial
begin:system
  // Initial system values
  wb_reset = 1'b1;
  wb_clk = 1'b0;
end


// WB clock generation (DEVICE clock is generated in uart_device.v)
//#################################################################

  // DEVICE's clock generation: 
  //    ----------------
  //    // rx_clk rising edge
  //    always@(posedge rx_clk)
  //      if (rx_clk_en)
  //        #(T_clk_period / 2) rx_clk = 1'b0;
  //    // rx_clk falling edge
  //    always@(negedge rx_clk)
  //      if (rx_clk_en)
  //        #(T_clk_period / 2) rx_clk = 1'b1;
  //    ----------------
  // DEVICE's transmit clocks generation: 
  //    ----------------
  //    // tx_clk rising edge
  //    always@(posedge tx_clk)
  //      if (tx_clk_en)
  //        #((T_clk_period / 2) * 16 * T_divisor) tx_clk = 1'b0;
  //    // tx_clk falling edge
  //    always@(negedge tx_clk)
  //      if (tx_clk_en)
  //        #((T_clk_period / 2) * 16 * T_divisor) tx_clk = 1'b1;
  //    ----------------

  // WB clock
  always@(posedge wb_clk)
    if (wb_clk_en)
      #(T_wb_clk_period / 2) wb_clk = 1'b0;
  always@(negedge wb_clk)
    if (wb_clk_en)
      #(T_wb_clk_period / 2) wb_clk = 1'b1;


// SYSTEM signals tracker
//#######################

  // Reset
  always@(posedge wb_reset)
    -> reset_aserted;
  always@(negedge wb_reset)
    -> reset_released;

  // Interrupt
  always@(posedge wb_int_o)
    -> int_aserted;
  always@(negedge wb_int_o)
    -> int_released;


// UART register tracker
//######################

  // UART registers:
  //    ----------------
  //    RBR             (R/  | ADR 0 | DLAB 0)
  //      [7:0] -RX---- "rxdata" Receiver Buffer Register
  //    ----------------
  //    THR             ( /W | ADR 0 | DLAB 0)
  //      [7:0] ----TX- "txdata" Transmitter Holding Register
  //    ----------------
  //    IER             (R/W | ADR 1 | DLAB 0) 
  //      [0]   -RX---- "1" Received Data Available & Receive Fifo Timeout
  //      [1]   ----TX- "1" Transmitter Holding Register Empty
  //      [2]   -RX---- "1" Receiver Line Status
  //      [3]   -MODEM- "1" Modem Status
  //    ----------------
  //    IIR             (R/  | ADR 2)
  //      [0]   ------- "0" Interrupt is Pending (decreasing priority level in following 3 bits)
  //      [3:1] -RX---- "011" Receiver Line Status - Overrun, Parity, Framing error or Break int. ---> READ LSR
  //      [3:1] -RX---- "010" Received Data Available - Fifo Trigger Level Reached ------------------> READ RBR (Fifo lower than trig.)
  //      [3:1] -RX---- "110" Timeout Indication - Fifo not empty & no Fifo action for 4 char times -> READ RBR
  //      [3:1] ----TX- "001" Transmitter Holding Register Empty - THR Empty ------------------------> READ IIR | WRITE THR
  //      [3:1] -MODEM- "000" Modem Status - CTS, DSR, DCD changed or RI changed from '0' to '1' ----> READ MSR
  //    ----------------
  //    FCR             ( /W | ADR 2)
  //      [1]   -RX---- "1" Clear only Receiver Fifo (not shift register)
  //      [2]   ----TX- "1" Clear only Transmitter Fifo (not shift register)
  //      [7:6] -RX---- "00"  1 BYTE  Receiver Fifo Interrupt trigger level
  //      [7:6] -RX---- "01"  4 BYTEs Receiver Fifo Interrupt trigger level
  //      [7:6] -RX---- "10"  8 BYTEs Receiver Fifo Interrupt trigger level
  //      [7:6] -RX---- "11" 14 BYTEs Receiver Fifo Interrupt trigger level
  //    ----------------
  //    LCR             (R/W | ADR 3)
  //      [1:0] -RX-TX- "00" 5 bits in each character
  //      [1:0] -RX-TX- "01" 6 bits in each character
  //      [1:0] -RX-TX- "10" 7 bits in each character
  //      [1:0] -RX-TX- "11" 8 bits in each character
  //      [2]   -RX-TX- "0" 1 stop bit
  //      [2]   -RX-TX- "1" 1.5 stop bits (when 5 bits of char.) or 2 stop bits (when 6, 7 or 8 bits of char.)
  //      [3]   -RX-TX- "1" Parity bit enabled
  //      [5:4] -RX-TX- "00" NO Stick Parity & ODD Parity bit - ODD num. of '1's is transmitted
  //      [5:4] -RX-TX- "01" NO Stick Parity & EVEN Parity bit - EVEN num. of '1's is transmitted
  //      [5:4] -RX-TX- "10" Stick Parity bit - Stick '1' as Parity bit
  //      [5:4] -RX-TX- "11" Stick Parity bit - Stick '0' as Parity bit
  //      [6]   ----TX- "1" Break Control - Output is forced to '0'
  //      [7]   ------- "1" DLAB - for access to DLL and DLM
  //    ----------------
  //    MCR             ( /W | ADR 4)
  //      [0]   -MODEM- "1" Force DTR to '0' - in LoopBack connected to DSR input
  //      [1]   -MODEM- "1" Force RTS to '0' - in LoopBack connected to CTS input
  //      [2]   -MODEM- "1" Force N.C.1 to '0' - in LoopBack connected to RI input
  //      [3]   -MODEM- "1" Force N.C.2 to '0' - in LoopBack connected to DCD input
  //      [4]   -MODEM- "1" LoopBack mode
  //    ----------------
  //    LSR             (R/  | ADR 5)
  //      [0]   -RX---- "1" Data Ready - At least 1 char. received and is in Fifo----------> READ RBR (Fifo empty)
  //      [1]   -RX---- "1" Overrun Error - Fifo full & 1 char. received in shift reg. ----> READ LSR
  //      [2]   -RX---- "1" Parity Error - top Fifo char. has invalid parity bit ----------> READ LSR
  //      [3]   -RX---- "1" Framing Error - top Fifo char. has invalid stop bit -----------> READ LSR
  //      [4]   -RX---- "1" Break Int. - top Fifo char. bits are '0' and it's ctrl. bits --> READ LSR
  //      [5]   ----TX- "1" Transmitter Holding Register Empty - transmitter Fifo empty ---> WRITE THR
  //      [6]   ----TX- "1" Transmitter EMpTy - transmitter Fifo empty & shift reg. empty -> WRITE THR
  //      [7]   -RX---- "1" At least 1 Parity Error, Framing Error or Break Int. in Fifo --> READ LSR & No More Errors in Fifo
  //    ----------------
  //    MSR             (R/  | ADR 6)
  //      [0]   -MODEM- "1" Delta CTS indicator - CTS has changed it's state --------------> READ MSR
  //      [1]   -MODEM- "1" Delta DSR indicator - DSR has changed it's state --------------> READ MSR
  //      [2]   -MODEM- "1" Trailing Edge of RI - RI has changed from '0' to '1' ----------> READ MSR
  //      [3]   -MODEM- "1" Delta DCD indicator - DCD has changed it's state --------------> READ MSR
  //      [4]   -MODEM- "x" Complement of CTS input | in LoopBack equal to RTS = MCR[1]
  //      [5]   -MODEM- "x" Complement of DSR input | in LoopBack equal to DTR = MCR[0]
  //      [6]   -MODEM- "x" Complement of RI input | in LoopBack equal to N.C.1 = MCR[2]
  //      [7]   -MODEM- "x" Complement of DCD input | in LoopBack equal to N.C.2 = MCR[3]
  //    ----------------
  //    DLL             (R/W | ADR 0 | DLAB 1)
  //      [7:0] ------- "dl[ 7:0]" LSB of DL Reg. written 2. - dl == '0' disables outputs / dl = 1/(T_wb_clk_period*16*BaudRate)
  //    ----------------
  //    DLM             (R/W | ADR 1 | DLAB 1)
  //      [7:0] ------- "dl[15:8]" MSB of DL Reg. written 1. - dl == '0' disables outputs / dl = 1/(T_wb_clk_period*16*BaudRate)
  //    ----------------

  // Transparent UART registers
  assign ier_reg[7:0] = {4'h0, testbench.i_uart_top.regs.ier      };
  assign iir_reg[7:0] = {4'hC, testbench.i_uart_top.regs.iir      };
  assign fcr_reg[7:0] = {      testbench.i_uart_top.regs.fcr, 6'h0};
  assign lcr_reg[7:0] = {      testbench.i_uart_top.regs.lcr      }; // lcr_reg[7] == DLAB !!!
  assign mcr_reg[7:0] = {3'h0, testbench.i_uart_top.regs.mcr      };
  assign lsr_reg[7:0] = {      testbench.i_uart_top.regs.lsr      };
  assign msr_reg[7:0] = {      testbench.i_uart_top.regs.msr      };
  assign dll_reg[7:0] = {      testbench.i_uart_top.regs.dl[ 7:0] };
  assign dlm_reg[7:0] = {      testbench.i_uart_top.regs.dl[15:8] };

  // Tracking changes of registers
  always@(ier_reg)
  begin
    -> ier_reg_changed;
  end
  always@(iir_reg)
  begin
    -> iir_reg_changed;
  end
  always@(fcr_reg)
  begin
    -> fcr_reg_changed;
  end
  always@(lcr_reg)
  begin
    -> lcr_reg_changed;
  end
  always@(mcr_reg)
  begin
    -> mcr_reg_changed;
  end
  always@(lsr_reg)
  begin
    -> lsr_reg_changed;
  end
  always@(msr_reg)
  begin
    -> msr_reg_changed;
  end
  always@(dll_reg)
  begin
    -> dll_reg_changed;
  end
  always@(dlm_reg)
  begin
    -> dlm_reg_changed;
  end

  // Tracking read/write access to registers
  always@(wbs_cyc_i or wbs_stb_i or wbs_we_i or wbs_sel_i or wbs_adr_i or 
          wbs_dat_i /*or wbs_ack_o*/ /*or posedge wb_clk*/)
  begin
    if (wbs_cyc_i && wbs_stb_i)
    begin
      if (wbs_we_i /*&& wbs_ack_o*/) // WRITE
      begin
        // LOG's example of  detecting of register write:
        //    ----------------
        //    case (wbs_adr_i)
        //    `UART_REG_TR: if (lcr_reg[7]) // lcr_reg[7] == DLAB !!!
        //                    -> dll_reg_written;
        //                  else
        //                    -> thr_reg_written;
        //    `UART_REG_IE: if (lcr_reg[7]) // lcr_reg[7] == DLAB !!!
        //                    -> dlm_reg_written;
        //                  else
        //                    -> ier_reg_written;
        //    `UART_REG_FC: -> fcr_reg_written;
        //    `UART_REG_LC: -> lcr_reg_written; 
        //    `UART_REG_MC: -> mcr_reg_written; 
        //    default:      -> erroneous_write_location;
        //    endcase
        //    ----------------

        reg_adr = wbs_adr_i;
        reg_dat = wbs_dat_i;
        reg_dlab = lcr_reg[7];
        -> reg_written;
        if (~reg_dlab && (reg_adr == `UART_REG_TR)) // write to FIFO
          -> tx_reg_written;
      end
    end
  end
  always@(wbs_cyc_i or wbs_stb_i or wbs_we_i or wbs_sel_i or wbs_adr_i or 
          wbs_dat_o or wbs_ack_o /*or posedge wb_clk*/)
  begin
    if (wbs_cyc_i && wbs_stb_i)
    begin
      if (~wbs_we_i && wbs_ack_o) // READ
      begin
        // LOG's example of  detecting of register read:
        //    ----------------
        //    case (wbs_adr_i)
        //    `UART_REG_RB: if (lcr_reg[7]) // lcr_reg[7] == DLAB !!!
        //                    -> dll_reg_read;
        //                  else
        //                    -> rbr_reg_read;
        //    `UART_REG_IE: if (lcr_reg[7]) // lcr_reg[7] == DLAB !!!
        //                    -> dlm_reg_read;
        //                  else
        //                    -> ier_reg_read;
        //    `UART_REG_II: -> iir_reg_read;
        //    `UART_REG_LC: -> lcr_reg_read;
        //    `UART_REG_LS: -> lsr_reg_read;
        //    `UART_REG_MS: -> msr_reg_read;
        //    default:      -> erroneous_read_location;
        //    endcase 
        //    ----------------

        reg_adr = wbs_adr_i;
        reg_dat = wbs_dat_o;
        reg_dlab = lcr_reg[7];
        -> reg_read;
        if (~reg_dlab && (reg_adr == `UART_REG_RB))
          -> rx_reg_read;
      end
    end
  end


// UART register monitor
//#######################

  // Line Status Register
    // Reading LSR register
    initial
    begin
      lsr_reg_read = 0;
      forever
      begin
        @(reg_read);
        if (reg_adr == `UART_REG_LS)
        begin
          lsr_reg_read = 1'b1;
          repeat (1) @(posedge wb_clk);
          lsr_reg_read = 0;
        end
      end
    end
    // Bit 0 - Data Ready
    initial
    begin
      lsr_reg_bit0_change_allowed = 0;
      @(reset_released);
      #10;
      fork
      begin: rx_fifo_status_changing
        forever
        begin
          if (rx_fifo_status == 0)
          begin
            wait (rx_fifo_status > 0);
            lsr_reg_bit0_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit0_change_allowed = 0;
            if (~lsr_reg[0])
            begin
              `BENCH_ERROR("Bit 0 of LSR register not '1'!");
              -> error_detected;
            end
          end
          else
          begin
            wait (rx_fifo_status == 0);
            lsr_reg_bit0_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit0_change_allowed = 0;
            if (lsr_reg[0])
            begin
              `BENCH_ERROR("Bit 0 of LSR register not '0'!");
              -> error_detected;
            end
          end
        end
      end
      begin: lsr_reg_bit0_changing
        forever
        begin
          wait (~lsr_reg_bit0_change_allowed);
          begin
            @(lsr_reg[0] or lsr_reg_bit0_change_allowed);
            if (~lsr_reg_bit0_change_allowed)
            begin
              `BENCH_ERROR("Bit 0 of LSR register should not change!");
              -> error_detected;
            end
          end
        end
      end
      join
    end
    // Bit 1 - Overrun Error
    initial
    begin
      lsr_reg_bit1_change_allowed = 0;
      @(reset_released);
      #10;
      fork
      begin: rx_overrun_err_occured_changing
        forever
        begin
          if (~rx_overrun_err_occured)
          begin
            wait (rx_overrun_err_occured);
            lsr_reg_bit1_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit1_change_allowed = 0;
            if (~lsr_reg[1])
            begin
              `BENCH_ERROR("Bit 1 of LSR register not '1'!");
              -> error_detected;
            end
          end
          else
          begin
            wait (lsr_reg_read);
            lsr_reg_bit1_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit1_change_allowed = 0;
            rx_overrun_err_occured = 0;
            if (lsr_reg[1])
            begin
              `BENCH_ERROR("Bit 1 of LSR register not '0'!");
              -> error_detected;
            end
          end
        end
      end
      begin: lsr_reg_bit1_changing
        forever
        begin
          wait (~lsr_reg_bit1_change_allowed);
          begin
            @(lsr_reg[1] or lsr_reg_bit1_change_allowed);
            if (~lsr_reg_bit1_change_allowed)
            begin
              `BENCH_ERROR("Bit 1 of LSR register should not change!");
              -> error_detected;
            end
          end
        end
      end
      join
    end
    // Bit 2 - Parity Error
    initial
    begin
      lsr_reg_bit2_change_allowed = 0;
      rx_fifo_par_rd_pointer      = 0;
      @(reset_released);
      #10;
      fork
      begin: rx_parity_err_changing
        forever
        begin
          if (~rx_fifo_par[rx_fifo_par_rd_pointer])
          begin
            wait (rx_fifo_read);
            lsr_reg_bit2_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit2_change_allowed = 0;
            rx_fifo_par_rd_pointer = rx_fifo_par_rd_pointer + 1'b1;
            // check bit
            if (~lsr_reg[2] && rx_fifo_par[rx_fifo_par_rd_pointer])
            begin
              `BENCH_ERROR("Bit 2 of LSR register not '1'!");
              -> error_detected;
            end
            else if (lsr_reg[2] && ~rx_fifo_par[rx_fifo_par_rd_pointer])
            begin
              `BENCH_ERROR("Bit 2 of LSR register not '0'!");
              -> error_detected;
            end
          end
          else
          begin
            wait (lsr_reg_read);
            lsr_reg_bit2_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit2_change_allowed = 0;
            if (rx_fifo_par_rd_pointer < rx_fifo_rd_pointer)
            begin
              for (i2 = rx_fifo_par_rd_pointer; i2 <= rx_fifo_rd_pointer; i2 = i2 + 1)
                rx_fifo_par[i2] = 0;
              rx_fifo_par_rd_pointer = rx_fifo_rd_pointer;
            end
            else if (rx_fifo_par_rd_pointer > rx_fifo_rd_pointer)
            begin
              for (i2 = rx_fifo_par_rd_pointer; i2 <= 31; i2 = i2 + 1)
                rx_fifo_par[i2] = 0;
              for (i2 = 0; i2 <= rx_fifo_rd_pointer; i2 = i2 + 1)
                rx_fifo_par[i2] = 0;
              rx_fifo_par_rd_pointer = rx_fifo_rd_pointer;
            end
            else
            begin
              rx_fifo_par = 0;
              rx_fifo_par_rd_pointer = rx_fifo_rd_pointer;
            end
            // check bit
            if (~lsr_reg[2] && rx_fifo_par[rx_fifo_par_rd_pointer])
            begin
              `BENCH_ERROR("Bit 2 of LSR register not '1'!");
              -> error_detected;
            end
            else if (lsr_reg[2] && ~rx_fifo_par[rx_fifo_par_rd_pointer])
            begin
              `BENCH_ERROR("Bit 2 of LSR register not '0'!");
              -> error_detected;
            end
          end
        end
      end
      begin: lsr_reg_bit2_changing
        forever
        begin
          wait (~lsr_reg_bit2_change_allowed);
          begin
            @(lsr_reg[2] or lsr_reg_bit2_change_allowed);
            if (~lsr_reg_bit2_change_allowed)
            begin
              `BENCH_ERROR("Bit 2 of LSR register should not change!");
              -> error_detected;
            end
          end
        end
      end
      join
    end
    // Bit 3 - Framing Error
    initial
    begin
      lsr_reg_bit3_change_allowed = 0;
      rx_fifo_frm_rd_pointer      = 0;
      @(reset_released);
      #10;
      fork
      begin: rx_framing_err_changing
        forever
        begin
          if (~rx_fifo_frm[rx_fifo_frm_rd_pointer])
          begin
            wait (rx_fifo_read);
            lsr_reg_bit3_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit3_change_allowed = 0;
            rx_fifo_frm_rd_pointer = rx_fifo_frm_rd_pointer + 1'b1;
            // check bit
            if (~lsr_reg[3] && rx_fifo_frm[rx_fifo_frm_rd_pointer])
            begin
              `BENCH_ERROR("Bit 3 of LSR register not '1'!");
              -> error_detected;
            end
            else if (lsr_reg[3] && ~rx_fifo_frm[rx_fifo_frm_rd_pointer])
            begin
              `BENCH_ERROR("Bit 3 of LSR register not '0'!");
              -> error_detected;
            end
          end
          else
          begin
            wait (lsr_reg_read);
            lsr_reg_bit3_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit3_change_allowed = 0;
            if (rx_fifo_frm_rd_pointer < rx_fifo_rd_pointer)
            begin
              for (i3 = rx_fifo_frm_rd_pointer; i3 <= rx_fifo_rd_pointer; i3 = i3 + 1)
                rx_fifo_frm[i3] = 0;
              rx_fifo_frm_rd_pointer = rx_fifo_rd_pointer;
            end
            else if (rx_fifo_frm_rd_pointer > rx_fifo_rd_pointer)
            begin
              for (i3 = rx_fifo_frm_rd_pointer; i3 <= 31; i3 = i3 + 1)
                rx_fifo_frm[i3] = 0;
              for (i3 = 0; i3 <= rx_fifo_rd_pointer; i3 = i3 + 1)
                rx_fifo_frm[i3] = 0;
              rx_fifo_frm_rd_pointer = rx_fifo_rd_pointer;
            end
            else
            begin
              rx_fifo_frm = 0;
              rx_fifo_frm_rd_pointer = rx_fifo_rd_pointer;
            end
            // check bit
            if (~lsr_reg[3] && rx_fifo_frm[rx_fifo_frm_rd_pointer])
            begin
              `BENCH_ERROR("Bit 3 of LSR register not '1'!");
              -> error_detected;
            end
            else if (lsr_reg[3] && ~rx_fifo_frm[rx_fifo_frm_rd_pointer])
            begin
              `BENCH_ERROR("Bit 3 of LSR register not '0'!");
              -> error_detected;
            end
          end
        end
      end
      begin: lsr_reg_bit3_changing
        forever
        begin
          wait (~lsr_reg_bit3_change_allowed);
          begin
            @(lsr_reg[3] or lsr_reg_bit3_change_allowed);
            if (~lsr_reg_bit3_change_allowed)
            begin
              `BENCH_ERROR("Bit 3 of LSR register should not change!");
              -> error_detected;
            end
          end
        end
      end
      join
    end
    // Bit 4 - Break Interrupt
    initial
    begin
      lsr_reg_bit4_change_allowed = 0;
      rx_fifo_brk_rd_pointer      = 0;
      @(reset_released);
      #10;
      fork
      begin: rx_break_int_changing
        forever
        begin
          if (~rx_fifo_brk[rx_fifo_brk_rd_pointer])
          begin
            wait (rx_fifo_read);
            lsr_reg_bit4_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit4_change_allowed = 0;
            rx_fifo_brk_rd_pointer = rx_fifo_brk_rd_pointer + 1'b1;
            // check bit
            if (~lsr_reg[4] && rx_fifo_brk[rx_fifo_brk_rd_pointer])
            begin
              `BENCH_ERROR("Bit 4 of LSR register not '1'!");
              -> error_detected;
            end
            else if (lsr_reg[4] && ~rx_fifo_brk[rx_fifo_brk_rd_pointer])
            begin
              `BENCH_ERROR("Bit 4 of LSR register not '0'!");
              -> error_detected;
            end
          end
          else
          begin
            wait (lsr_reg_read);
            lsr_reg_bit4_change_allowed = 1'b1;
            repeat (1) @(posedge wb_clk);
            #2;
            lsr_reg_bit4_change_allowed = 0;
            if (rx_fifo_brk_rd_pointer < rx_fifo_rd_pointer)
            begin
              for (i4 = rx_fifo_brk_rd_pointer; i4 <= rx_fifo_rd_pointer; i4 = i4 + 1)
                rx_fifo_brk[i4] = 0;
              rx_fifo_brk_rd_pointer = rx_fifo_rd_pointer;
            end
            else if (rx_fifo_brk_rd_pointer > rx_fifo_rd_pointer)
            begin
              for (i4 = rx_fifo_brk_rd_pointer; i4 <= 31; i4 = i4 + 1)
                rx_fifo_brk[i4] = 0;
              for (i4 = 0; i4 <= rx_fifo_rd_pointer; i4 = i4 + 1)
                rx_fifo_brk[i4] = 0;
              rx_fifo_brk_rd_pointer = rx_fifo_rd_pointer;
            end
            else
            begin
              rx_fifo_brk = 0;
              rx_fifo_brk_rd_pointer = rx_fifo_rd_pointer;
            end
            // check bit
            if (~lsr_reg[4] && rx_fifo_brk[rx_fifo_brk_rd_pointer])
            begin
              `BENCH_ERROR("Bit 4 of LSR register not '1'!");
              -> error_detected;
            end
            else if (lsr_reg[4] && ~rx_fifo_brk[rx_fifo_brk_rd_pointer])
            begin
              `BENCH_ERROR("Bit 4 of LSR register not '0'!");
              -> error_detected;
            end
          end
        end
      end
      begin: lsr_reg_bit4_changing
        forever
        begin
          wait (~lsr_reg_bit4_change_allowed);
          begin
            @(lsr_reg[4] or lsr_reg_bit4_change_allowed);
            if (~lsr_reg_bit4_change_allowed)
            begin
              `BENCH_ERROR("Bit 4 of LSR register should not change!");
              -> error_detected;
            end
          end
        end
      end
      join
    end
    // Bit 5 - Transmitter Holding Register Empty
    initial
    begin
      lsr_reg_bit5_change_allowed = 0;
      @(reset_released);
      #10;
      fork
      begin: tx_fifo_status_changing
        forever
        begin
          if (tx_fifo_status == 0)
          begin
//            @(tx_reg_written);
            wait (tx_fifo_status > 0);
            lsr_reg_bit5_change_allowed = 1'b1;
            repeat (3) @(posedge wb_clk);
            #2;
            lsr_reg_bit5_change_allowed = 0;
            if (lsr_reg[5])
            begin
              `BENCH_ERROR("Bit 5 of LSR register not '0'!");
              -> error_detected;
            end
          end
          else
          begin
            wait (tx_fifo_status == 0);
            lsr_reg_bit5_change_allowed = 1'b1;
            repeat (3) @(posedge wb_clk);
            #2;
            lsr_reg_bit5_change_allowed = 0;
            if (~lsr_reg[5])
            begin
              `BENCH_ERROR("Bit 5 of LSR register not '1'!");
              -> error_detected;
            end
          end
        end
      end
      begin: lsr_reg_bit5_changing
        forever
        begin
          wait (~lsr_reg_bit5_change_allowed);
          begin
            @(lsr_reg[5] or lsr_reg_bit5_change_allowed);
            if (~lsr_reg_bit5_change_allowed)
            begin
              `BENCH_ERROR("Bit 5 of LSR register should not change!");
              -> error_detected;
            end
          end
        end
      end
      join
    end
    // Bit 6 - Transmitter Empty
    initial
    begin
      lsr_reg_bit6_change_allowed = 0;
      @(reset_released);
      #10;
      fork
      begin: tx_fifo_status_and_shift_reg_changing
        forever
        begin
          if ((tx_fifo_status == 0) && tx_shift_reg_empty)
          begin
//            @(tx_reg_written);
            wait (tx_fifo_status > 0);
            lsr_reg_bit6_change_allowed = 1'b1;
            repeat (3) @(posedge wb_clk);
            #2;
            lsr_reg_bit6_change_allowed = 0;
            if (lsr_reg[6])
            begin
              `BENCH_ERROR("Bit 6 of LSR register not '0'!");
              -> error_detected;
            end
          end
          else
          begin
            wait ((tx_fifo_status == 0) && tx_shift_reg_empty);
            lsr_reg_bit6_change_allowed = 1'b1;
            repeat (3) @(posedge wb_clk);
            #2;
            lsr_reg_bit6_change_allowed = 0;
            if (~lsr_reg[6])
            begin
              `BENCH_ERROR("Bit 6 of LSR register not '1'!");
              -> error_detected;
            end
          end
        end
      end
      begin: lsr_reg_bit6_changing
        forever
        begin
          wait (~lsr_reg_bit6_change_allowed);
          begin
            @(lsr_reg[6] or lsr_reg_bit6_change_allowed);
            if (~lsr_reg_bit6_change_allowed)
            begin
              `BENCH_ERROR("Bit 6 of LSR register should not change!");
              -> error_detected;
            end
          end
        end
      end
      join
    end
    // Bit 7 - Error in RX FIFO
    initial
    begin
      lsr_reg_bit7_change_allowed = 0;
      @(reset_released);
      #10;
      fork
      begin: error_changing
        forever
        begin
          if ((rx_fifo_par == 0) && (rx_fifo_frm == 0) && (rx_fifo_brk == 0))
          begin
            wait (rx_parity_err || rx_framing_err || rx_framing_glitch || rx_break_int);
            lsr_reg_bit7_change_allowed = 1'b1;
            repeat (3) @(posedge wb_clk);
            #2;
            lsr_reg_bit7_change_allowed = 0;
            // check bit
            if (~lsr_reg[7])
            begin
              `BENCH_ERROR("Bit 7 of LSR register not '1'!");
              -> error_detected;
            end
          end
          else
          begin
            wait (lsr_reg_read && (rx_fifo_par == 0) && (rx_fifo_frm == 0) && (rx_fifo_brk == 0));
            lsr_reg_bit7_change_allowed = 1'b1;
            repeat (2) @(posedge wb_clk);
            #2;
            lsr_reg_bit7_change_allowed = 0;
            // check bit
            if (lsr_reg[7])
            begin
              `BENCH_ERROR("Bit 7 of LSR register not '0'!");
              -> error_detected;
            end
          end
        end
      end
      begin: lsr_reg_bit7_changing
        forever
        begin
          wait (~lsr_reg_bit7_change_allowed);
          begin
            @(lsr_reg[7] or lsr_reg_bit7_change_allowed);
            if (~lsr_reg_bit7_change_allowed)
            begin
              `BENCH_ERROR("Bit 7 of LSR register should not change!");
              -> error_detected;
            end
          end
        end
      end
      join
    end


// UART transmitter monitor
//#########################

  // TX FIFO status
  always@(tx_fifo_wr_pointer or tx_fifo_rd_pointer)
  begin
    if (tx_fifo_wr_pointer >= tx_fifo_rd_pointer)
      tx_fifo_status = tx_fifo_wr_pointer - tx_fifo_rd_pointer;
    else
      tx_fifo_status = (5'h1F - tx_fifo_rd_pointer) + tx_fifo_wr_pointer;
  end
  // TX FIFO and TX data
  initial
  begin
    tx_fifo_wr_pointer = 0;
    tx_fifo_rd_pointer = 0;
    tx_shift_reg_empty = 1;
    tx_fifo_status     = 0;
    tx_start_bit_edge  = 1;
    fork
    begin:write_tx_shift_reg_read_tx_fifo
      forever
      begin
        wait ((tx_fifo_status !== 0) && tx_shift_reg_empty && tx_start_bit_edge && ~stx_pad_o);
        tx_start_bit_edge  = 0;
        tx_shift_reg = tx_fifo[tx_fifo_rd_pointer];
        tx_shift_reg_empty = 0;
        @(testbench.i_uart_device.device_received_last_bit);
        repeat (16393) @(posedge wb_clk);
        tx_fifo_rd_pointer = tx_fifo_rd_pointer + 1'b1;
        @(posedge wb_clk);
        if (tx_fifo_status == 0)
        begin
          `BENCH_MSG("TX FIFO is empty!");
        end
      end
    end
    begin:write_tx_fifo
      forever
      begin
        @(tx_reg_written); // write to FIFO
        repeat (1) @(posedge wb_clk); // delay when writing into registers
        if (tx_fifo_status <= 5'h0F)
        begin
          tx_fifo[tx_fifo_wr_pointer] = reg_dat;
          tx_fifo_wr_pointer = tx_fifo_wr_pointer + 1'b1;
        end
        else // FIFO overflow
        begin
          `BENCH_WARNING("TX FIFO overflow!");
        end
      end
    end
    begin:empty_tx_fifo
      forever
      begin
        wait (fcr_reg[2]);
        tx_fifo_wr_pointer = 0;
        tx_fifo_rd_pointer = 0;
        @(posedge wb_clk);
        if (tx_fifo_status == 0)
        begin
          `BENCH_MSG("TX FIFO is empty!");
        end
      end
    end
    begin:read_tx_shift_reg
      forever
      begin
        @(testbench.i_uart_device.device_received_packet);
        // Check data
        if (tx_shift_reg != testbench.i_uart_device.rx_data)
        begin
          `BENCH_ERROR("TX data has ERROR!");
          -> error_detected;
        end
        else
          `BENCH_MSG("TX data correct!");
        if (testbench.i_uart_device.rx_parity_error)
        begin
          `BENCH_ERROR("TX data has parity ERROR!");
          -> error_detected;
        end
        else
          `BENCH_MSG("TX data parity correct!");
        if (testbench.i_uart_device.rx_framing_error)
        begin
          `BENCH_ERROR("TX data has framing ERROR!");
          -> error_detected;
        end
        else
          `BENCH_MSG("TX data framing correct!");
        // Set TX FIFO read pointer
        tx_start_bit_edge  = 1;
        repeat (7) @(wb_clk);
        if (tx_shift_reg_empty == 0)
        begin
          tx_shift_reg_empty = 1'b1;
        end
        else
        begin
          `BENCH_ERROR("TX shift register empty while transmiting data!");
          -> error_detected;
        end
      end
    end
    join
  end


// UART receiver monitor
//######################

  // RX FIFO status
  always@(rx_fifo_wr_pointer or rx_fifo_rd_pointer)
  begin
    if (rx_fifo_wr_pointer >= rx_fifo_rd_pointer)
      rx_fifo_status = rx_fifo_wr_pointer - rx_fifo_rd_pointer;
    else
      rx_fifo_status = (5'h1F - rx_fifo_rd_pointer) + rx_fifo_wr_pointer;
  end
  // RX FIFO and RX data
  initial
  begin
    rx_parity_err      = 0;
    rx_framing_err     = 0;
    rx_framing_glitch  = 0;
    rx_break_int       = 0;
    rx_overrun_err_occured = 0;
    rx_fifo_par        = 0;
    rx_fifo_frm        = 0;
    rx_fifo_brk        = 0;
    rx_shift_reg_full  = 0;
    rx_fifo_wr_pointer = 0;
    rx_fifo_rd_pointer = 0;
    rx_fifo_status     = 0;
    fork
    begin:write_rx_shift_reg
      forever
      begin
        @(testbench.i_uart_device.device_sent_packet);
        repeat (1) @(posedge wb_clk);
        rx_shift_reg   = testbench.i_uart_device.sent_data;
        rx_parity_err  = testbench.i_uart_device.tx_parity_enabled && 
                         (testbench.i_uart_device.tx_parity_wrong || 
                          (  // sample point is BIT_NUM * 2 - 1 => 3, 5, 7...
                           ((testbench.i_uart_device.tx_glitch_num == (3 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (5 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (7 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (9 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (11 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (13 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (15 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (17 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (19 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (21 * 8 * testbench.i_uart_device.T_divisor)) ||
                            (testbench.i_uart_device.tx_glitch_num == (23 * 8 * testbench.i_uart_device.T_divisor))) &&
                           (testbench.i_uart_device.tx_glitch_num[23:0] < ((testbench.i_uart_device.tx_length + 2'h1) *
                             16 * testbench.i_uart_device.T_divisor))
                         ));
        rx_framing_err = testbench.i_uart_device.tx_framing_wrong;
        rx_framing_glitch = (testbench.i_uart_device.tx_glitch_num == ((((testbench.i_uart_device.tx_length + 2'h2 +
                               testbench.i_uart_device.tx_parity_enabled) * 
                                2) - 1'b1) * 8 * testbench.i_uart_device.T_divisor));
        rx_break_int   = testbench.i_uart_device.tx_break_enable && 
                         (testbench.i_uart_device.tx_break_num[15:0] >= ((testbench.i_uart_device.tx_length + 2'h2 +
                            testbench.i_uart_device.tx_parity_enabled) * 
                           16 * testbench.i_uart_device.T_divisor));
        -> testbench.i_uart_device.sent_packet_received;
        if (rx_fifo_status > 5'h0F)
          rx_overrun_err_occured = 1'b1;
        rx_shift_reg_full = 1'b1;
      end
    end
    begin:write_rx_fifo_read_rx_shift_reg
      forever
      begin
        wait (rx_shift_reg_full);
        if (rx_fifo_status <= 5'h0F)
        begin
          rx_fifo_data[rx_fifo_wr_pointer] = testbench.i_uart_device.sent_data;
          rx_fifo_par[rx_fifo_wr_pointer]  = rx_parity_err; 
          rx_fifo_frm[rx_fifo_wr_pointer]  = rx_framing_err || rx_framing_glitch; 
          rx_fifo_brk[rx_fifo_wr_pointer]  = rx_break_int; 
          rx_fifo_wr_pointer = rx_fifo_wr_pointer + 1'b1;
        end
        else // FIFO overflow
        begin
          `BENCH_WARNING("RX FIFO overflow!");
        end
        repeat (1) @(posedge wb_clk);
        rx_shift_reg_full = 0;
      end
    end
    begin:empty_rx_fifo
      forever
      begin
        wait (fcr_reg[1]);
        rx_fifo_wr_pointer = 0;
        rx_fifo_rd_pointer = 0;
//        rx_fifo_par        = 0;
//        rx_fifo_frm        = 0;
//        rx_fifo_brk        = 0;
        @(posedge wb_clk);
        if (rx_fifo_status == 0)
        begin
          `BENCH_MSG("RX FIFO is empty!");
        end
      end
    end
    begin:read_rx_fifo
      rx_fifo_read = 0;
      forever
      begin
        @(rx_reg_read);
        if (rx_fifo_status > 0)
        begin
          rx_fifo_read = 1'b1;
          // Check data
          if (rx_fifo_data[rx_fifo_rd_pointer] != reg_dat)
          begin
            `BENCH_ERROR("RX data has ERROR!");
            -> error_detected;
          end
          else
          begin
            `BENCH_MSG("RX data correct!");
          end
          // Set RX FIFO read pointer
          repeat (1) @(posedge wb_clk);
          rx_fifo_read = 0;
          rx_fifo_rd_pointer = rx_fifo_rd_pointer + 1'b1;
        end
        else
        begin
          `BENCH_WARNING("Reading RX FIFO while RX FIFO is empty!");
        end


        if ((~rx_fifo_frm[rx_fifo_rd_pointer] && lsr_reg[3]) ||
            (rx_fifo_frm[rx_fifo_rd_pointer] && ~lsr_reg[3]))
        begin
          `BENCH_ERROR("RX data has wrong framing ERROR!");
          -> error_detected;
        end
        else
          `BENCH_MSG("RX data has correct framing error!");
        // Set RX FIFO read pointer
        repeat (1) @(posedge wb_clk);
        rx_fifo_read = 0;
        if (rx_fifo_status > 0)
        begin
//          rx_fifo_par[rx_fifo_rd_pointer] = 1'b0; 
//          rx_fifo_frm[rx_fifo_rd_pointer] = 1'b0; 
//          rx_fifo_brk[rx_fifo_rd_pointer] = 1'b0; 
          rx_fifo_rd_pointer = rx_fifo_rd_pointer + 1'b1;
        end
      end
    end
    join
  end


// UART interrupt monitor
//#######################




endmodule


