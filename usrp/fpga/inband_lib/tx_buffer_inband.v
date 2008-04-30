module tx_buffer_inband
  ( //System
    input wire usbclk, input wire bus_reset, input wire reset, 
    input wire [15:0] usbdata, output wire have_space, input wire [3:0] channels, 
    //output transmit signals
    output wire [15:0] tx_i_0, output wire [15:0] tx_q_0, 
    output wire [15:0] tx_i_1, output wire [15:0] tx_q_1,
    output wire [15:0] tx_i_2, output wire [15:0] tx_q_2, 
    output wire [15:0] tx_i_3, output wire [15:0] tx_q_3, 
    input wire txclk, input wire txstrobe, input wire WR,
    input wire clear_status, output wire tx_empty, output wire [15:0] debugbus, 
    //command reader io
    output wire [15:0] rx_databus, output wire rx_WR, output wire rx_WR_done, 
    input wire rx_WR_enabled,
    //register io 
    output wire [1:0] reg_io_enable, output wire [31:0] reg_data_in, output wire [6:0] reg_addr,
    input wire [31:0] reg_data_out,  
    //input characteristic signals
    input wire [31:0] rssi_0, input wire [31:0] rssi_1, input wire [31:0] rssi_2, 
    input wire [31:0] rssi_3, input wire [31:0] rssi_wait, input wire [31:0] threshhold, 
    output wire [1:0] tx_underrun, 
    //system stop
    output wire stop, output wire [15:0] stop_time);
	
   parameter NUM_CHAN	 =      1 ;
    
   /* To generate channel readers */
   genvar i ;
    
   /* These will eventually be external register */
   reg                  [31:0] timestamp_clock ;
   wire                 [7:0]  txstrobe_rate [NUM_CHAN-1:0] ;
   wire			        [31:0] rssi [3:0];
   assign rssi[0] = rssi_0;
   assign rssi[1] = rssi_1;
   assign rssi[2] = rssi_2;
   assign rssi[3] = rssi_3;
   
   always @(posedge txclk)
       if (reset)
           timestamp_clock <= 0;
       else
           timestamp_clock <= timestamp_clock + 1;


    /* Connections between tx_usb_fifo_reader and
       cnannel/command processing blocks */
   wire                  [31:0] tx_data_bus ;
   wire            [NUM_CHAN:0] chan_WR ;
   wire            [NUM_CHAN:0] chan_done ;
    
    /* Connections between data block and the
       FX2/TX chains */
   wire            [NUM_CHAN:0] chan_underrun;
   wire            [NUM_CHAN:0] chan_txempty;
   
   /* Conections between tx_data_packet_fifo and
       its reader + strobe generator */
   wire                 [31:0] chan_fifodata [NUM_CHAN:0] ;
   wire                        chan_pkt_waiting [NUM_CHAN:0] ;
   wire                        chan_rdreq [NUM_CHAN:0] ;
   wire                        chan_skip [NUM_CHAN:0] ;
   wire                        chan_have_space [NUM_CHAN:0] ;

   wire		            [14:0] debug [NUM_CHAN:0];
    
   /* Outputs to transmit chains */
   wire                 [15:0] tx_i [NUM_CHAN:0] ;
   wire                 [15:0] tx_q [NUM_CHAN:0] ;

   assign tx_i[NUM_CHAN] = 0;
   assign tx_q[NUM_CHAN] = 0;
    
   assign have_space = chan_have_space[0] & chan_have_space[1];
   assign tx_empty = chan_txempty[0] & chan_txempty[1] ;

   assign tx_i_0 = chan_txempty[0] ? 16'b0 : tx_i[0] ;
   assign tx_q_0 = chan_txempty[0] ? 16'b0 : tx_q[0] ;
   assign tx_i_1 = chan_txempty[1] ? 16'b0 : tx_i[1] ;
   assign tx_q_1 = chan_txempty[1] ? 16'b0 : tx_q[1] ;
        
   assign tx_q_2 = 16'b0 ;
   assign tx_i_2 = 16'b0 ;
   assign tx_q_3 = 16'b0 ;
   assign tx_i_3 = 16'b0 ;
   assign tx_i_3 = 16'b0 ;
	
   assign debugbus = {have_space, txclk, WR, WR_final, chan_WR, chan_done, 
                      chan_pkt_waiting[0], chan_pkt_waiting[1],
                      chan_rdreq[0], chan_rdreq[1], chan_txempty[0], chan_txempty[1]};

   wire [31:0] usbdata_final;
   wire		WR_final;

   tx_packer tx_usb_packer
   (.bus_reset(bus_reset), .usbclk(usbclk), .WR_fx2(WR),
    .usbdata(usbdata), .reset(reset), .txclk(txclk),
    .usbdata_final(usbdata_final), .WR_final(WR_final));
	
   channel_demux #(NUM_CHAN) channel_demuxer
   (.usbdata_final(usbdata_final), .WR_final(WR_final),
    .reset(reset), .txclk(txclk), .WR_channel(chan_WR),
    .WR_done_channel(chan_done), .ram_data(tx_data_bus));
	
   generate for (i = 0 ; i < NUM_CHAN; i = i + 1)
     begin : generate_channel_readers
       assign tx_underrun[i] = chan_underrun[i];

       channel_ram tx_data_packet_fifo 
       (.reset(reset), .txclk(txclk), .datain(tx_data_bus),
        .WR(chan_WR[i]), .WR_done(chan_done[i]),
        .have_space(chan_have_space[i]), .dataout(chan_fifodata[i]),
        .packet_waiting(chan_pkt_waiting[i]), .RD(chan_rdreq[i]),
        .RD_done(chan_skip[i]));

       chan_fifo_reader tx_chan_reader 
       (.reset(reset), .tx_clock(txclk), .tx_strobe(txstrobe),
        .timestamp_clock(timestamp_clock), .samples_format(4'b0),          
        .tx_q(tx_q[i]), .tx_i(tx_i[i]), .underrun(chan_underrun[i]),
        .skip(chan_skip[i]), .rdreq(chan_rdreq[i]),
        .fifodata(chan_fifodata[i]), .pkt_waiting(chan_pkt_waiting[i]),
        .tx_empty(chan_txempty[i]), .rssi(rssi[i]), .debug(debug[i]),
        .threshhold(threshhold), .rssi_wait(rssi_wait));	         
    end
    endgenerate


   channel_ram tx_cmd_packet_fifo 
   (.reset(reset), .txclk(txclk), .datain(tx_data_bus), .WR(chan_WR[NUM_CHAN]),
    .WR_done(chan_done[NUM_CHAN]), .have_space(chan_have_space[NUM_CHAN]),
    .dataout(chan_fifodata[NUM_CHAN]), .packet_waiting(chan_pkt_waiting[NUM_CHAN]),
    .RD(chan_rdreq[NUM_CHAN]), .RD_done(chan_skip[NUM_CHAN]));

   cmd_reader tx_cmd_reader
   (.reset(reset), .txclk(txclk), .timestamp_clock(timestamp_clock), .skip(chan_skip[NUM_CHAN]),
    .rdreq(chan_rdreq[NUM_CHAN]), .fifodata(chan_fifodata[NUM_CHAN]),
    .pkt_waiting(chan_pkt_waiting[NUM_CHAN]), .rx_databus(rx_databus),
    .rx_WR(rx_WR), .rx_WR_done(rx_WR_done), .rx_WR_enabled(rx_WR_enabled),
    .reg_data_in(reg_data_in), .reg_data_out(reg_data_out), .reg_addr(reg_addr),
    .reg_io_enable(reg_io_enable), .debug(debug[NUM_CHAN]), .stop(stop), .stop_time(stop_time));
				   
endmodule // tx_buffer

