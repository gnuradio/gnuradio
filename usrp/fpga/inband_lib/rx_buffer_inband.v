//`include "../../firmware/include/fpga_regs_common.v"
//`include "../../firmware/include/fpga_regs_standard.v"
module rx_buffer_inband
  ( input usbclk,
    input bus_reset,
    input reset,  // DSP side reset (used here), do not reset registers
    input reset_regs, //Only reset registers
    output [15:0] usbdata,
    input RD,
    output wire have_pkt_rdy,
    output reg rx_overrun,
    input wire [3:0] channels,
    input wire [15:0] ch_0,
    input wire [15:0] ch_1,
    input wire [15:0] ch_2,
    input wire [15:0] ch_3,
    input wire [15:0] ch_4,
    input wire [15:0] ch_5,
    input wire [15:0] ch_6,
    input wire [15:0] ch_7,
    input rxclk,
    input rxstrobe,
    input clear_status,
    input [6:0] serial_addr, 
    input [31:0] serial_data, 
    input serial_strobe,
    output wire [15:0] debugbus,
	
    //Connection with tx_inband
    input rx_WR,
    input [15:0] rx_databus,
    input rx_WR_done,
    output reg rx_WR_enabled,
    //signal strength
    input wire [31:0] rssi_0, input wire [31:0] rssi_1,
    input wire [31:0] rssi_2, input wire [31:0] rssi_3,
    input wire [1:0] tx_underrun
    );
    
    parameter NUM_CHAN = 1;
    genvar i ;
    
    // FX2 Bug Fix
    reg [8:0] read_count;
    always @(negedge usbclk)
        if(bus_reset)
            read_count <= #1 9'd0;
        else if(RD & ~read_count[8])
            read_count <= #1 read_count + 9'd1;
        else
            read_count <= #1 RD ? read_count : 9'b0;
       
	// Time counter
	reg [31:0] timestamp_clock;
	always @(posedge rxclk)
		if (reset)
			timestamp_clock <= 0;
		else
			timestamp_clock <= timestamp_clock + 1;
     
  // USB side fifo
  wire [11:0] rdusedw;
  wire [11:0] wrusedw;
  wire [15:0] fifodata;
  wire [15:0] fifodata_il[0:NUM_CHAN];
  wire WR;
  wire have_space;
  reg sel;
  reg wr;

  always@(posedge rxclk)
    begin
      if(reset)
        begin
          sel<=1;
          wr<=0;
        end
      else if(rxstrobe)
        begin
          sel<=0;
          wr<=1;
        end
      else if(wr&~sel)
          sel<=1;
      else if(wr&sel)
          wr<=0;
      else
          wr<=0;
    end

  assign fifodata_il[0] = (sel)?ch_1:ch_0;
  assign fifodata_il[1] = (sel)?ch_3:ch_2;

  fifo_4kx16_dc	rx_usb_fifo (
    .aclr ( reset ),
    .data ( fifodata ),
    .rdclk ( ~usbclk ),
    .rdreq ( RD & ~read_count[8] ),
    .wrclk ( rxclk ),
    .wrreq ( WR ),
    .q ( usbdata ),
    .rdempty (  ),
    .rdusedw ( rdusedw ),
    .wrfull (  ),
    .wrusedw ( wrusedw ) );
    
  assign have_pkt_rdy = (rdusedw >= 12'd256);
  assign have_space = (wrusedw < 12'd760);
	 
  // Rx side fifos
  // These are of size [NUM_CHAN:0] because the extra channel is used for the
  // RX command channel.  If there were no command channel, they would be
  // NUM_CHAN-1.
  wire chan_rdreq;
  wire [15:0] chan_fifodata;
  wire [9:0] chan_usedw;
  wire [NUM_CHAN:0] chan_empty;
  wire [3:0] rd_select;
  wire [NUM_CHAN:0] rx_full;
	 
  packet_builder #(NUM_CHAN) rx_pkt_builer (
    .rxclk ( rxclk ),
    .reset ( reset ),
    .timestamp_clock ( timestamp_clock ),
    .channels ( NUM_CHAN ),
    .chan_rdreq ( chan_rdreq ),
    .chan_fifodata ( chan_fifodata ),
    .chan_empty ( chan_empty ),
    .rd_select ( rd_select ),
    .chan_usedw ( chan_usedw ),
    .WR ( WR ),
    .fifodata ( fifodata ),
    .have_space ( have_space ),
      .rssi_0(rssi_0), .rssi_1(rssi_1),
      .rssi_2(rssi_2),.rssi_3(rssi_3), .debugbus(debug),
      .underrun(tx_underrun));
	 
  // Detect overrun
  always @(posedge rxclk)
    if(reset)
      rx_overrun <= 1'b0;
    else if(rx_full[0])
      rx_overrun <= 1'b1;
    else if(clear_status)
      rx_overrun <= 1'b0;

		
  // FIXME: what is the purpose of these two lines?
  wire [15:0]ch[NUM_CHAN:0];
  assign ch[0] = ch_0;
	
  wire cmd_empty;
	
  always @(posedge rxclk)
    if(reset)
      rx_WR_enabled <= 1;
    else if(cmd_empty)
      rx_WR_enabled <= 1;
    else if(rx_WR_done)
      rx_WR_enabled <= 0;


  // Of Size 0:NUM_CHAN due to extra command channel.
  wire [15:0] dataout [0:NUM_CHAN];
  wire [9:0]  usedw	[0:NUM_CHAN];
  wire empty[0:NUM_CHAN];
	
  generate for (i = 0 ; i < NUM_CHAN; i = i + 1)
    begin : generate_channel_fifos

      wire rdreq;

      assign rdreq = (rd_select == i) & chan_rdreq;

      fifo_1kx16 rx_chan_fifo (
      .aclr ( reset ),
      .clock ( rxclk ),
      .data ( fifodata_il[i] ),
      .rdreq ( rdreq ),
      .wrreq ( ~rx_full[i] & wr),
      .empty (empty[i]),
      .full (rx_full[i]),
      .q ( dataout[i]),
      .usedw ( usedw[i]),
      .almost_empty(chan_empty[i])
      );
    end
  endgenerate
	
  wire [7:0] debug;
	 
  fifo_1kx16 rx_cmd_fifo (
    .aclr ( reset ),
    .clock ( rxclk ),
    .data ( rx_databus ),
    .rdreq ( (rd_select == NUM_CHAN) & chan_rdreq ),
    .wrreq ( rx_WR & rx_WR_enabled),
    .empty ( cmd_empty),
    .full ( rx_full[NUM_CHAN] ),
    .q ( dataout[NUM_CHAN]),
    .usedw ( usedw[NUM_CHAN] )
  );
	
  assign chan_empty[NUM_CHAN] = cmd_empty | rx_WR_enabled;
  assign chan_fifodata = dataout[rd_select];
  assign chan_usedw = usedw[rd_select];
  assign debugbus = {4'd0, rxclk, rxstrobe, rx_full[0], rx_full[1], sel, wr};

endmodule
