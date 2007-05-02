module tx_buffer_inband
  ( input usbclk,
    input bus_reset,  // Used here for the 257-Hack to fix the FX2 bug
    input reset,  // standard DSP-side reset
    input [15:0] usbdata,
    input wire WR,
    output wire have_space,
    output reg tx_underrun,
    input wire [3:0] channels,
    output [15:0] tx_i_0,
    output [15:0] tx_q_0,
    output [15:0] tx_i_1,
    output [15:0] tx_q_1,
    //NOT USED
    output reg [15:0] tx_i_2,
    output reg [15:0] tx_q_2,
    output reg [15:0] tx_i_3,
    output reg [15:0] tx_q_3,
    input txclk,
    input txstrobe,
    input clear_status,
    output wire tx_empty,
    output [11:0] debugbus
    );

   wire [15:0] tx_data_bus;

   wire WR_chan_0;
   wire chan_0_done;
   wire OR0;
   wire UR0;
   
   wire WR_chan_1;
   wire chan_1_done;
   wire OR1;
   wire UR1;
   
   // NOT USED yet
   wire WR_cmd;
   wire cmd_done;
   
   //EXTERNAL REGISTER
   //TODO: increment it
   reg [31:0] time_counter;
   reg [7:0] txstrobe_rate_0;
   reg [7:0] txstrobe_rate_1;
   
   
   //Usb block
   wire [15:0] tupf_fifodata;
   wire tupf_pkt_waiting;
   wire tupf_rdreq;
   wire tupf_skip;
   wire tupf_have_space;
   
   usb_packet_fifo2 tx_usb_packet_fifo 
     (  .reset         (reset),
        .usb_clock     (usbclk), 
        .fpga_clock    (txclk),
        .write_data    (usbdata),
        .write_enable  (WR),
        .read_data     (tupf_fifodata),
        .pkt_waiting   (tupf_pkt_waiting),
        .read_enable   (tupf_rdreq), 
        .skip_packet   (tupf_skip),
        .have_space    (tupf_have_space),
        .tx_empty      (tx_empty)
       );
   
	usb_fifo_reader tx_usb_packet_reader (
		.reset(reset),
		.tx_clock(txclk),
		.tx_data_bus(tx_data_bus),
      .WR_chan_0(WR_chan_0),
      .WR_chan_1(WR_chan_1),
      .WR_cmd(WR_cmd),
      .chan_0_done(chan_0_done),
      .chan_1_done(chan_1_done),
      .cmd_done(cmd_done),
      .rdreq(tupf_rdreq),
      .skip(tupf_skip),
      .pkt_waiting(tupf_pkt_waiting),
      .fifodata(tupf_fifodata)
	);


   //Channel 0 block
   wire [15:0] tdpf_fifodata_0;
   wire tdpf_pkt_waiting_0;
   wire tdpf_rdreq_0;
   wire tdpf_skip_0;
   wire tdpf_have_space_0;
   wire txstrobe_chan_0;

   data_packet_fifo tx_data_packet_fifo_0 
     (  .reset(reset),
        .clock(txclk), 
        .ram_data_in(tx_data_bus),
        .write_enable(WR_chan_0),
        .ram_data_out(tdpf_fifodata_0),
        .pkt_waiting(tdpf_pkt_waiting_0),
        .read_enable(tdpf_rdreq_0),
        .pkt_complete(chan_0_done), 
        .skip_packet(tdpf_skip_0),
        .have_space(tdpf_have_space_0)
       );
   
   strobe_gen strobe_gen_0
    (   .clock(txclk),
        .reset(reset),
        .enable(1'b1),
        .rate(txstrobe_rate_0),
        .strobe_in(txstrobe),
        .strobe(txstrobe_chan_0) 
       );
   
   chan_fifo_reader tx_chan_0_reader (
      .reset(reset),
      .tx_clock(txclk),
      .tx_strobe(txstrobe),
      //.tx_strobe(txstrobe_chan_0),
      .adc_clock(time_counter),
      .samples_format(4'b0),
      .tx_q(tx_q_0),
      .tx_i(tx_i_0),
      .overrun(OR0),
      .underrun(UR0),
      .skip(tdpf_skip_0),
      .rdreq(tdpf_rdreq_0),
      .fifodata(tdpf_fifodata_0),
      .pkt_waiting(tdpf_pkt_waiting_0)
   );	
   
   
   //Channel 1 block
   wire [15:0] tdpf_fifodata_1;
   wire tdpf_pkt_waiting_1;
   wire tdpf_rdreq_1;
   wire tdpf_skip_1;
   wire tdpf_have_space_1;
   wire txstrobe_chan_1;
   
   data_packet_fifo tx_data_packet_fifo_1 
     (  .reset(reset),
        .clock(txclk), 
        .ram_data_in(tx_data_bus),
        .write_enable(WR_chan_1),
        .ram_data_out(tdpf_fifodata_1),
        .pkt_waiting(tdpf_pkt_waiting_1),
        .read_enable(tdpf_rdreq_1),
        .pkt_complete(chan_1_done), 
        .skip_packet(tdpf_skip_1),
        .have_space(tdpf_have_space_1)
       );
   
   strobe_gen strobe_gen_1
    (   .clock(txclk),
        .reset(reset),
        .enable(1'b1),
        .rate(txstrobe_rate_1),
        .strobe_in(txstrobe),
        .strobe(txstrobe_chan_1) 
       );
   
   chan_fifo_reader tx_chan_1_reader (
      .reset(reset),
      .tx_clock(txclk),
      .tx_strobe(txstrobe),
      //.tx_strobe(txstrobe_chan_1),
      .adc_clock(time_counter),
      .samples_format(4'b0),
      .tx_q(tx_q_1),
      .tx_i(tx_i_1),
      .overrun(OR1),
      .underrun(UR1),
      .skip(tdpf_skip_1),
      .rdreq(tdpf_rdreq_1),
      .fifodata(tdpf_fifodata_1),
      .pkt_waiting(tdpf_pkt_waiting_1)
   );
   
endmodule // tx_buffer

