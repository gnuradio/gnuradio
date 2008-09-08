//////////////////////////////////////////////////////////////////////
////                                                              ////
////  wb_master_model.v                                           ////
////                                                              ////
////  This file is part of the "uart16550" project                ////
////  http://www.opencores.org/projects/uart16550/                ////
////                                                              ////
////  Author(s):                                                  ////
////      - mihad@opencores.org (Miha Dolenc)                     ////
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
// $Log: wb_master_model.v,v $
// Revision 1.1  2004/03/27 03:55:17  tadejm
// Testbench with complete selfchecking. BUG is that THRE status is set at the end of last sent bit when TX FIFO is empty instead when only TX FIFO gets empty. This causes testcases not to finish.
//
//
//


`include "wb_model_defines.v"
`include "timescale.v"

module wb_master_model
(
    wb_rst_i    ,
    wb_clk_i    ,
    wbm_cyc_o   ,
    wbm_cti_o   ,
    wbm_bte_o   ,
    wbm_stb_o   ,
    wbm_we_o    ,
    wbm_adr_o   ,
    wbm_sel_o   ,
    wbm_dat_o   ,
    wbm_dat_i   ,
    wbm_ack_i   ,
    wbm_err_i   ,
    wbm_rty_i
);

// set the parameters to impossible values, so errors will be detected at compile time
parameter wb_dat_width  =   1   ;
parameter wb_adr_width  =   1   ;
parameter wb_sel_width  =   1   ;

real    Tperiod                 ;

input   wb_rst_i    ,
        wb_clk_i    ;

output  wbm_cyc_o   ;
reg     wbm_cyc_o   ;

output  [ 2: 0] wbm_cti_o   ;
reg     [ 2: 0] wbm_cti_o   ;

output  [ 1: 0] wbm_bte_o   ;
reg     [ 1: 0] wbm_bte_o   ;

output          wbm_stb_o   ;
reg             wbm_stb_o   ;

output          wbm_we_o    ;
reg             wbm_we_o    ;

output  [wb_adr_width - 1:0]    wbm_adr_o   ;
reg     [wb_adr_width - 1:0]    wbm_adr_o   ;

output  [wb_sel_width - 1:0]    wbm_sel_o   ;
reg     [wb_sel_width - 1:0]    wbm_sel_o   ;

output  [wb_dat_width - 1:0]    wbm_dat_o   ;
reg     [wb_dat_width - 1:0]    wbm_dat_o   ;

input   [wb_dat_width - 1:0]    wbm_dat_i   ;

input   wbm_ack_i   ;

input   wbm_err_i   ;

input   wbm_rty_i   ;

event write_transfer    ;
event read_transfer     ;

reg [wb_adr_width - 1:0] write_adr ;
reg [wb_sel_width - 1:0] write_sel ;
reg [wb_dat_width - 1:0] write_dat ;

reg [wb_adr_width - 1:0] read_adr ;
reg [wb_sel_width - 1:0] read_sel ;
reg [wb_dat_width - 1:0] read_dat ;

reg [wb_adr_width - 1:0] next_write_adr  ;
reg [wb_sel_width - 1:0] next_write_sel  ;
reg [wb_dat_width - 1:0] next_write_dat  ;
reg [ 2: 0]              next_write_cti  ;
reg [ 1: 0]              next_write_bte  ;

event write_accepted ;
event write_request  ;

reg [wb_adr_width - 1:0] next_read_adr  ;
reg [wb_sel_width - 1:0] next_read_sel  ;
reg [ 2: 0]              next_read_cti  ;
reg [ 1: 0]              next_read_bte  ;

event read_accepted                     ;
event read_request                      ;

real Tsetup ;
real Thold  ;

initial Tsetup = 0.0 ;
initial Thold  = 0.0 ;

reg reset_done  ;
initial reset_done = 1'b0 ;

event   error_event ;
reg     [ 799: 0] error_message ;

initial
fork
begin
    forever
    begin
        @(wb_rst_i) ;
        if ((wb_rst_i ^ wb_rst_i) !== 1'b0)
        begin
            reset_done = 1'b0   ;
            error_message = "Invalid WISHBONE reset line value detected" ;
            `WB_MODEL_ERR_MSG(error_message) ;
            -> error_event  ;
        end
    
        if (wb_rst_i === `WB_MODEL_RST_ACTIVE)
        begin
            reset_done = 1'b0   ;
        end
    end
end
begin
    forever
    begin
        @(wb_rst_i) ;
        if (wb_rst_i === `WB_MODEL_RST_ACTIVE)
        begin
            @(posedge wb_clk_i or wb_rst_i) ;
            if (wb_rst_i !== `WB_MODEL_RST_ACTIVE)
            begin
                error_message = "Reset de-activated prior to at least one positive clock transition" ;
                `WB_MODEL_ERR_MSG(error_message) ;
                -> error_event ;
            end
            else
            begin
                reset_done = 1'b1 ;
            end
        end            
    end
end
join

always@(wb_rst_i)
begin
    if (wb_rst_i === `WB_MODEL_RST_ACTIVE)
        wbm_cyc_o <= 1'b0   ;
end

reg     access_in_progress          ;
initial access_in_progress = 1'b0   ;

task start_write ;
    input  `WBM_MODEL_WRITE_IN_TYPE    write_stim_i       ;
    output `WBM_MODEL_WRITE_OUT_TYPE   write_res_o        ;
    reg    [31: 0]                     num_of_slave_waits ;
    reg                                end_access         ;
begin:main

    write_res_o = 'h0 ;

    if (access_in_progress === 1'b1)
    begin
        error_message = "Task called when some other access was in progress" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event                   ;
        write_res_o`WBM_MODEL_WRITE_STIM_ERR = 1'b1 ;
        disable main    ;
    end

    if (reset_done !== 1'b1)
    begin
        error_message = "Task called before reset was applied to the design" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event                   ;
        write_res_o`WBM_MODEL_WRITE_STIM_ERR = 1'b1 ;
        disable main    ;
    end

    access_in_progress = 1'b1   ;
    end_access         = write_stim_i`WBM_MODEL_WRITE_LAST ;

    if (write_stim_i`WBM_MODEL_WRITE_FAST_B2B !== 1'b1)
        @(posedge wb_clk_i) ;

    wbm_cyc_o <= #(Tperiod - Tsetup) 1'b1 ;

    insert_waits(write_stim_i`WBM_MODEL_WRITE_WAITS, 'h0, num_of_slave_waits)  ;

    if ((num_of_slave_waits ^ num_of_slave_waits) === 'h0)
    begin
        error_message = "Slave responded to initial write access" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event                   ;
    end

    num_of_slave_waits = 0          ;
    
    wbm_stb_o   <=  #(Tperiod - Tsetup) 1'b1            ;
    wbm_we_o    <=  #(Tperiod - Tsetup) 1'b1            ;
    wbm_adr_o   <=  #(Tperiod - Tsetup) next_write_adr  ;
    wbm_dat_o   <=  #(Tperiod - Tsetup) get_write_dat(next_write_dat, next_write_sel)   ;
    wbm_sel_o   <=  #(Tperiod - Tsetup) next_write_sel  ;
    wbm_cti_o   <=  #(Tperiod - Tsetup) next_write_cti  ;
    wbm_bte_o   <=  #(Tperiod - Tsetup) next_write_bte  ;

    -> write_accepted ;

    @(posedge wb_clk_i) ;

    while((wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0) & (num_of_slave_waits < write_stim_i`WBM_MODEL_WRITE_ALLOWED_SLAVE_WAITS))
    begin
        num_of_slave_waits = num_of_slave_waits + 1'b1  ;
        write_adr = wbm_adr_o ;
        write_sel = wbm_sel_o ;
        write_dat = wbm_dat_o ;
        -> write_request ;
        @(posedge wb_clk_i) ;
    end

    if ((wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0))
    begin
        error_message = "Cycle terminated because allowed number of slave wait states constraint violation" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event  ;
        end_access = 1'b1   ;
        write_res_o`WBM_MODEL_WRITE_DESIGN_ERR   = 1'b1  ;
    end
    else if ((wbm_ack_i + wbm_err_i + wbm_rty_i) !== 'h1)
    begin
        error_message = "Cycle terminated because invalid slave response was received" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event  ;
        end_access = 1'b1   ;
        write_res_o`WBM_MODEL_WRITE_DESIGN_ERR   = 1'b1  ;
    end
    else
    begin
        write_res_o`WBM_MODEL_WRITE_SLAVE_WAITS  = num_of_slave_waits   ;
        write_res_o`WBM_MODEL_WRITE_SLAVE_ACK    = wbm_ack_i            ;
        write_res_o`WBM_MODEL_WRITE_SLAVE_ERR    = wbm_err_i            ;
        write_res_o`WBM_MODEL_WRITE_SLAVE_RTY    = wbm_rty_i            ;

        if (wbm_ack_i === 1'b1)
        begin
            write_adr   =   wbm_adr_o  ;
            write_dat   =   wbm_dat_o  ;
            write_sel   =   wbm_sel_o  ;
            -> write_transfer   ;
        end
    end

    if (end_access)
    begin
        wbm_cyc_o   <= #(Thold) 1'b0 ;
        wbm_stb_o   <= #(Thold) 1'bx ;
        wbm_we_o    <= #(Thold) 1'bx ;
        wbm_sel_o   <= #(Thold) 'hx  ;
        wbm_adr_o   <= #(Thold) 'hx  ;
        wbm_dat_o   <= #(Thold) 'hx  ;
        wbm_cti_o   <= #(Thold) 'hx  ;
        wbm_bte_o   <= #(Thold) 'hx  ; 
        access_in_progress = 1'b0    ;
    end
end
endtask // start_write

task subsequent_write   ;
    input  `WBM_MODEL_WRITE_IN_TYPE    write_stim_i       ;
    output `WBM_MODEL_WRITE_OUT_TYPE   write_res_o        ;
    reg    [31: 0]                     num_of_slave_waits ;
    reg                                end_access         ;
begin:main

    write_res_o = 'h0 ;

    if (access_in_progress !== 1'b1)
    begin
        error_message = "Task called when no access was in progress" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event                   ;
        write_res_o`WBM_MODEL_WRITE_STIM_ERR = 1'b1 ;
        disable main    ;
    end

    end_access  = write_stim_i`WBM_MODEL_WRITE_LAST ;

    insert_waits(write_stim_i`WBM_MODEL_WRITE_WAITS, 'h0, num_of_slave_waits)  ;

    if ((num_of_slave_waits ^ num_of_slave_waits) !== 'h0)
    begin
        num_of_slave_waits = write_stim_i`WBM_MODEL_WRITE_WAITS ;
    end
    
    wbm_stb_o   <=  #(Tperiod - Tsetup) 1'b1            ;
    wbm_we_o    <=  #(Tperiod - Tsetup) 1'b1            ;
    wbm_adr_o   <=  #(Tperiod - Tsetup) next_write_adr  ;
    wbm_dat_o   <=  #(Tperiod - Tsetup) get_write_dat(next_write_dat, next_write_sel)   ;
    wbm_sel_o   <=  #(Tperiod - Tsetup) next_write_sel  ;
    wbm_cti_o   <=  #(Tperiod - Tsetup) next_write_cti  ;
    wbm_bte_o   <=  #(Tperiod - Tsetup) next_write_bte  ;

    -> write_accepted ;

    @(posedge wb_clk_i) ;

    while((wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0) & (num_of_slave_waits < write_stim_i`WBM_MODEL_WRITE_ALLOWED_SLAVE_WAITS))
    begin
        num_of_slave_waits = num_of_slave_waits + 1'b1  ;
        write_adr = wbm_adr_o ;
        write_sel = wbm_sel_o ;
        write_dat = wbm_dat_o ;
        -> write_request ;
        @(posedge wb_clk_i) ;
    end

    if ((wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0))
    begin
        error_message = "Cycle terminated because allowed number of slave wait states constraint violation" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event  ;
        end_access = 1'b1   ;
        write_res_o`WBM_MODEL_WRITE_DESIGN_ERR   = 1'b1  ;
    end
    else if ((wbm_ack_i + wbm_err_i + wbm_rty_i) !== 'h1)
    begin
        error_message = "Cycle terminated because invalid slave response was received" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event  ;
        end_access = 1'b1   ;
        write_res_o`WBM_MODEL_WRITE_DESIGN_ERR   = 1'b1  ;
    end
    else
    begin
        write_res_o`WBM_MODEL_WRITE_SLAVE_WAITS  = num_of_slave_waits   ;
        write_res_o`WBM_MODEL_WRITE_SLAVE_ACK    = wbm_ack_i            ;
        write_res_o`WBM_MODEL_WRITE_SLAVE_ERR    = wbm_err_i            ;
        write_res_o`WBM_MODEL_WRITE_SLAVE_RTY    = wbm_rty_i            ;

        if (wbm_ack_i === 1'b1)
        begin
            write_adr   =   wbm_adr_o  ;
            write_dat   =   wbm_dat_o  ;
            write_sel   =   wbm_sel_o  ;
            -> write_transfer   ;
        end
    end

    if (end_access)
    begin
        wbm_cyc_o   <= #(Thold) 1'b0 ;
        wbm_stb_o   <= #(Thold) 1'bx ;
        wbm_we_o    <= #(Thold) 1'bx ;
        wbm_sel_o   <= #(Thold) 'hx  ;
        wbm_adr_o   <= #(Thold) 'hx  ;
        wbm_dat_o   <= #(Thold) 'hx  ;
        wbm_cti_o   <= #(Thold) 'hx  ;
        wbm_bte_o   <= #(Thold) 'hx  ; 
        access_in_progress = 1'b0    ;
    end
end
endtask // subsequent_write

task start_read ;
    input  `WBM_MODEL_READ_IN_TYPE  read_stim_i         ;
    output `WBM_MODEL_READ_OUT_TYPE read_res_o          ;
    reg    [31: 0]                  num_of_slave_waits  ;
    reg                             end_access          ;
begin:main

    read_res_o = 'h0 ;

    if (access_in_progress === 1'b1)
    begin
        error_message = "Task called when some other access was in progress" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event                   ;
        read_res_o`WBM_MODEL_READ_STIM_ERR = 1'b1 ;
        disable main    ;
    end

    if (reset_done !== 1'b1)
    begin
        error_message = "Task called before reset was applied to the design" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event                   ;
        read_res_o`WBM_MODEL_READ_STIM_ERR = 1'b1 ;
        disable main    ;
    end

    access_in_progress = 1'b1   ;
    end_access         = read_stim_i`WBM_MODEL_READ_LAST ;

    if (read_stim_i`WBM_MODEL_READ_FAST_B2B !== 1'b1)
        @(posedge wb_clk_i) ;

    wbm_cyc_o <= #(Tperiod - Tsetup) 1'b1 ;

    insert_waits(read_stim_i`WBM_MODEL_READ_WAITS, 'h0, num_of_slave_waits)  ;

    if ((num_of_slave_waits ^ num_of_slave_waits) === 'h0)
    begin
        error_message = "Slave responded to initial read access" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event                   ;
    end

    num_of_slave_waits = 0          ;
    
    wbm_stb_o   <=  #(Tperiod - Tsetup) 1'b1            ;
    wbm_we_o    <=  #(Tperiod - Tsetup) 1'b0            ;
    wbm_adr_o   <=  #(Tperiod - Tsetup) next_read_adr   ;
    wbm_sel_o   <=  #(Tperiod - Tsetup) next_read_sel   ;
    wbm_cti_o   <=  #(Tperiod - Tsetup) next_read_cti   ;
    wbm_bte_o   <=  #(Tperiod - Tsetup) next_read_bte   ;

    -> read_accepted ;

    @(posedge wb_clk_i) ;

    while((wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0) & (num_of_slave_waits < read_stim_i`WBM_MODEL_READ_ALLOWED_SLAVE_WAITS))
    begin
        num_of_slave_waits = num_of_slave_waits + 1'b1  ;
        read_adr = wbm_adr_o ;
        read_sel = wbm_sel_o ;
        -> read_request ;
        @(posedge wb_clk_i) ;
    end

    if ((wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0))
    begin
        error_message = "Cycle terminated because allowed number of slave wait states constraint violation" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event  ;
        end_access = 1'b1   ;
        read_res_o`WBM_MODEL_READ_DESIGN_ERR   = 1'b1  ;
    end
    else if ((wbm_ack_i + wbm_err_i + wbm_rty_i) !== 'h1)
    begin
        error_message = "Cycle terminated because invalid slave response was received" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event  ;
        end_access = 1'b1   ;
        read_res_o`WBM_MODEL_READ_DESIGN_ERR   = 1'b1  ;
    end
    else
    begin
        read_res_o`WBM_MODEL_READ_SLAVE_WAITS  = num_of_slave_waits   ;
        read_res_o`WBM_MODEL_READ_SLAVE_ACK    = wbm_ack_i            ;
        read_res_o`WBM_MODEL_READ_SLAVE_ERR    = wbm_err_i            ;
        read_res_o`WBM_MODEL_READ_SLAVE_RTY    = wbm_rty_i            ;

        if (wbm_ack_i === 1'b1)
        begin
            read_adr   =   wbm_adr_o  ;
            read_dat   =   wbm_dat_i  ;
            read_sel   =   wbm_sel_o  ;
            -> read_transfer   ;
        end
    end

    if (end_access)
    begin
        wbm_cyc_o   <= #(Thold) 1'b0 ;
        wbm_stb_o   <= #(Thold) 1'bx ;
        wbm_we_o    <= #(Thold) 1'bx ;
        wbm_sel_o   <= #(Thold) 'hx  ;
        wbm_adr_o   <= #(Thold) 'hx  ;
        wbm_cti_o   <= #(Thold) 'hx  ;
        wbm_bte_o   <= #(Thold) 'hx  ; 
        access_in_progress = 1'b0    ;
    end
end
endtask // start_read

task subsequent_read    ;
    input  `WBM_MODEL_READ_IN_TYPE  read_stim_i ;
    output `WBM_MODEL_READ_OUT_TYPE read_res_o  ;
    reg    [31: 0]                  num_of_slave_waits  ;
    reg                             end_access          ;
begin:main

    read_res_o = 'h0 ;

    if (access_in_progress !== 1'b1)
    begin
        error_message = "Task called when no access was in progress" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event                   ;
        read_res_o`WBM_MODEL_READ_STIM_ERR = 1'b1 ;
        disable main    ;
    end

    end_access  = read_stim_i`WBM_MODEL_READ_LAST ;

    insert_waits(read_stim_i`WBM_MODEL_READ_WAITS, 'h1, num_of_slave_waits)  ;

    if ((num_of_slave_waits ^ num_of_slave_waits) !== 'h0)
    begin
        num_of_slave_waits = read_stim_i`WBM_MODEL_READ_WAITS ;
    end
    
    wbm_stb_o   <=  #(Tperiod - Tsetup) 1'b1            ;
    wbm_we_o    <=  #(Tperiod - Tsetup) 1'b0            ;
    wbm_adr_o   <=  #(Tperiod - Tsetup) next_read_adr   ;
    wbm_sel_o   <=  #(Tperiod - Tsetup) next_read_sel   ;
    wbm_cti_o   <=  #(Tperiod - Tsetup) next_read_cti   ;
    wbm_bte_o   <=  #(Tperiod - Tsetup) next_read_bte   ;

    -> read_accepted ;

    @(posedge wb_clk_i) ;

    while((wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0) & (num_of_slave_waits < read_stim_i`WBM_MODEL_READ_ALLOWED_SLAVE_WAITS))
    begin
        num_of_slave_waits = num_of_slave_waits + 1'b1  ;
        read_adr = wbm_adr_o ;
        read_sel = wbm_sel_o ;
        -> read_request ;
        @(posedge wb_clk_i) ;
    end

    if ((wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0))
    begin
        error_message = "Cycle terminated because allowed number of slave wait states constraint violation" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event  ;
        end_access = 1'b1   ;
        read_res_o`WBM_MODEL_READ_DESIGN_ERR   = 1'b1  ;
    end
    else if ((wbm_ack_i + wbm_err_i + wbm_rty_i) !== 'h1)
    begin
        error_message = "Cycle terminated because invalid slave response was received" ;
        `WB_MODEL_ERR_MSG(error_message) ;
        -> error_event  ;
        end_access = 1'b1   ;
        read_res_o`WBM_MODEL_WRITE_DESIGN_ERR   = 1'b1  ;
    end
    else
    begin
        read_res_o`WBM_MODEL_READ_SLAVE_WAITS   = num_of_slave_waits   ;
        read_res_o`WBM_MODEL_READ_SLAVE_ACK     = wbm_ack_i            ;
        read_res_o`WBM_MODEL_READ_SLAVE_ERR     = wbm_err_i            ;
        read_res_o`WBM_MODEL_READ_SLAVE_RTY     = wbm_rty_i            ;

        if (wbm_ack_i === 1'b1)
        begin
            read_adr    =   wbm_adr_o  ;
            read_dat    =   wbm_dat_i  ;
            read_sel    =   wbm_sel_o  ;
            -> read_transfer   ;
        end
    end

    if (end_access)
    begin
        wbm_cyc_o   <= #(Thold) 1'b0 ;
        wbm_stb_o   <= #(Thold) 1'bx ;
        wbm_we_o    <= #(Thold) 1'bx ;
        wbm_sel_o   <= #(Thold) 'hx  ;
        wbm_adr_o   <= #(Thold) 'hx  ;
        wbm_cti_o   <= #(Thold) 'hx  ;
        wbm_bte_o   <= #(Thold) 'hx  ; 
        access_in_progress = 1'b0    ;
    end
end
endtask // subsequent_read

task insert_waits ;
    input   [31: 0] num_of_waits_i      ;
    input           read_req_on_wait_i  ;
    output  [31: 0] num_of_slave_waits  ;
    reg   [31: 0] cur_num_of_waits  ;
begin
    num_of_slave_waits = 'hx    ;

    for (cur_num_of_waits = 0 ; cur_num_of_waits < num_of_waits_i ; cur_num_of_waits = cur_num_of_waits + 1'b1)
    begin
        wbm_stb_o <= #(Thold) 1'b0    ;
        wbm_adr_o <= #(Thold) 'hx     ;
        wbm_sel_o <= #(Thold) 'hx     ;
        wbm_we_o  <= #(Thold) 'hx     ;
        wbm_dat_o <= #(Thold) 'hx     ;
        wbm_cti_o <= #(Thold) 'hx     ;
        wbm_bte_o <= #(Thold) 'hx     ;
        
        @(posedge wb_clk_i) ;

        if (read_req_on_wait_i)
        begin
            if ( (wbm_ack_i === 1'b0) & (wbm_err_i === 1'b0) & (wbm_rty_i === 1'b0) )
            begin
                if ( (next_read_cti === 'h1) | (next_read_cti === 'h2) | (next_read_cti === 'h7) )
                begin
                    read_adr = next_read_adr ;
                    read_sel = next_read_sel ;
                    -> read_request ;
                end
            end
        end

        if ((num_of_slave_waits ^ num_of_slave_waits) !== 'h0)
        begin
            if ((wbm_ack_i !== 1'b0) | (wbm_err_i !== 1'b0) | (wbm_rty_i !== 1'b0))
                num_of_slave_waits = cur_num_of_waits   ;
        end
    end
end
endtask

always@(posedge wb_clk_i)
begin:wb_monitoring_blk
    reg burst_in_progress ;
    reg ack_prev ;
    reg rty_prev ;
    reg err_prev ;
    reg stb_prev ;
    reg cyc_prev ;
    reg [wb_dat_width - 1:0]    sdat_prev   ;

    ack_prev    <= wbm_ack_i ;
    rty_prev    <= wbm_rty_i ;
    err_prev    <= wbm_err_i ;
    stb_prev    <= wbm_stb_o ;
    cyc_prev    <= wbm_cyc_o ;
    sdat_prev   <= wbm_dat_i ;

    if (wb_rst_i === `WB_MODEL_RST_ACTIVE)
    begin
        if (wbm_ack_i !== 1'b0)
        begin
            error_message = "ACK input signal was not de-asserted while reset was asserted" ;
            `WB_MODEL_ERR_MSG(error_message) ;
            -> error_event  ;
        end

        if (wbm_err_i !== 1'b0)
        begin
            error_message = "ERR input signal was not de-asserted while reset was asserted" ;
            `WB_MODEL_ERR_MSG(error_message) ;
            -> error_event  ;
        end

        if (wbm_rty_i !== 1'b0)
        begin
            error_message = "RTY input signal was not de-asserted while reset was asserted" ;
            `WB_MODEL_ERR_MSG(error_message) ;
            -> error_event  ;
        end

        burst_in_progress <= 1'b0 ;
    end
    else
    begin
        if (wbm_cyc_o !== 1'b1)
        begin
            if (wbm_ack_i !== 1'b0)
            begin
                error_message = "ACK input signal was asserted while no cycle was in progress" ;
                `WB_MODEL_ERR_MSG(error_message) ;
                -> error_event  ;
            end
    
            if (wbm_err_i !== 1'b0)
            begin
                error_message = "ERR input signal was asserted while no cycle was in progress" ;
                `WB_MODEL_ERR_MSG(error_message) ;
                -> error_event  ;
            end
    
            if (wbm_rty_i !== 1'b0)
            begin
                error_message = "RTY input signal was asserted while no cycle was in progress" ;
                `WB_MODEL_ERR_MSG(error_message) ;
                -> error_event  ;
            end
        end
        else
        begin
            if (burst_in_progress !== 1'b1)
            begin
                if ((wbm_ack_i !== 1'b0) & (wbm_stb_o !== 1'b1))
                begin
                    error_message = "ACK input signal was asserted while STB was de-asserted and no burst was in progress" ;
                    `WB_MODEL_ERR_MSG(error_message) ;
                    -> error_event  ;
                end
            
                if ((wbm_err_i !== 1'b0) & (wbm_stb_o !== 1'b1))
                begin
                    error_message = "ERR input signal was asserted while STB was de-asserted and no burst was in progress" ;
                    `WB_MODEL_ERR_MSG(error_message) ;
                    -> error_event  ;
                end
            
                if ((wbm_rty_i !== 1'b0) & (wbm_stb_o !== 1'b1))
                begin
                    error_message = "RTY input signal was asserted while STB was de-asserted and no burst was in progress" ;
                    `WB_MODEL_ERR_MSG(error_message) ;
                    -> error_event  ;
                end
            end
            else
            begin
                if ((ack_prev !== 1'b0) & (stb_prev !== 1'b1))
                begin
                    if (wbm_ack_i !== 1'b1)
                    begin
                        error_message = "Slave de-asserted ACK signal during burst cycle without receiving STB asserted" ;
                        `WB_MODEL_ERR_MSG(error_message) ;
                        -> error_event  ;
                    end

                    if (wbm_we_o !== 'b1)
                    begin
                        if (sdat_prev !== wbm_dat_i)
                        begin
                            error_message = "Slave changed the value of data output bus during burst cycle without receiving STB asserted" ;
                            `WB_MODEL_ERR_MSG(error_message) ;
                            -> error_event  ;
                        end
                    end
                end

                if ((rty_prev !== 1'b0) & (stb_prev !== 1'b1) & (wbm_rty_i !== 1'b1))
                begin
                    error_message = "Slave de-asserted RTY signal during burst cycle without receiving STB asserted" ;
                    `WB_MODEL_ERR_MSG(error_message) ;
                    -> error_event  ;
                end

                if ((err_prev !== 1'b0) & (stb_prev !== 1'b1) & (wbm_err_i !== 1'b1))
                begin
                    error_message = "Slave de-asserted ERR signal during burst cycle without receiving STB asserted" ;
                    `WB_MODEL_ERR_MSG(error_message) ;
                    -> error_event  ;
                end
            end

            if (wbm_stb_o === 1'b1)
            begin
                case (wbm_cti_o)
                3'b000:burst_in_progress <= 1'b0 ;
                3'b001:burst_in_progress <= 1'b1 ;
                3'b010:burst_in_progress <= 1'b1 ;
                3'b011:burst_in_progress <= 1'b0 ;
                3'b100:burst_in_progress <= 1'b0 ;
                3'b101:burst_in_progress <= 1'b0 ;
                3'b110:burst_in_progress <= 1'b0 ;
                3'b111:if (wbm_ack_i === 1'b1) burst_in_progress <= 1'b0 ;
                default:
                    begin
                        error_message = "WISHBONE master sent invalid cycle type identifier" ;
                        burst_in_progress <= 1'bx ;
                        `WB_MODEL_ERR_MSG(error_message) ;
                        -> error_event  ;
                    end
                endcase

                if (wbm_err_i === 1'b1)
                    burst_in_progress <= 1'b0 ;

                if (wbm_rty_i === 1'b1)
                    burst_in_progress <= 1'b0 ;

            end
        end
    end
end

function [wb_dat_width - 1:0] get_write_dat ;
    input [wb_dat_width - 1:0] dat_i    ;
    input [wb_sel_width - 1:0] sel_i    ;

    integer cur_bit ;
    reg [wb_dat_width - 1:0] dat_o ;
begin
    for (cur_bit = 0 ; cur_bit < wb_dat_width ; cur_bit = cur_bit + 1'b1)
    begin
        if (sel_i[cur_bit >> 3] === 1'b1)
            dat_o[cur_bit] = dat_i[cur_bit] ;
        else
            dat_o[cur_bit] = 1'bx           ;
    end

    get_write_dat = dat_o ;
end
endfunction // get_write_dat

endmodule

