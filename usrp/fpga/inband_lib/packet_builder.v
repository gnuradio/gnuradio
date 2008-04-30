module packet_builder #(parameter NUM_CHAN = 2)(
    // System
    input rxclk,
    input reset,
	 input [31:0] timestamp_clock,
	 input [3:0] channels,
    // ADC side
    input [15:0]chan_fifodata,
    input [NUM_CHAN:0]chan_empty,
    input [9:0]chan_usedw,
    output reg [3:0]rd_select,
    output reg chan_rdreq,
    // FX2 side
    output reg WR,
    output reg [15:0]fifodata,
    input have_space, 
    input wire [31:0]rssi_0, input wire [31:0]rssi_1, input wire [31:0]rssi_2,
    input wire [31:0]rssi_3, output wire [7:0] debugbus,
    input [NUM_CHAN:0] underrun);
    
    
    // States
    `define IDLE                     3'd0
    `define HEADER1                  3'd1
    `define HEADER2                  3'd2
    `define TIMESTAMP                3'd3
    `define FORWARD                  3'd4
	
    `define MAXPAYLOAD 504
    
    `define PAYLOAD_LEN 8:0
    `define TAG 12:9
    `define MBZ 15:13
    
    `define CHAN 4:0
    `define RSSI 10:5
    `define BURST 12:11
    `define DROPPED 13
    `define UNDERRUN 14
    `define OVERRUN 15
    
    reg [NUM_CHAN:0] overrun;
    reg [2:0] state;
    reg [8:0] read_length;
    reg [8:0] payload_len;
    reg timestamp_complete;
    reg [3:0] check_next;
	
    wire [31:0] true_rssi;
    wire [4:0] true_channel;
    wire ready_to_send;

    assign debugbus = {chan_empty[0], rd_select[0], have_space, 
                       (chan_usedw >= 10'd504), (chan_usedw ==0),  
                       ready_to_send, state[1:0]};

    assign true_rssi = (rd_select[1]) ? ((rd_select[0]) ? rssi_3:rssi_2) :
							((rd_select[0]) ? rssi_1:rssi_0);
    assign true_channel = (check_next == 4'd0 ? 5'h1f : {1'd0, check_next - 4'd1});
    assign ready_to_send = (chan_usedw >= 10'd504) || (chan_usedw == 0) || 
                           ((rd_select == NUM_CHAN)&&(chan_usedw > 0));
		
    always @(posedge rxclk)
    begin
        if (reset)
          begin
            overrun <= 0;
            WR <= 0;
            rd_select <= 0;
            chan_rdreq <= 0;
            timestamp_complete <= 0;
            check_next <= 0;
            state <= `IDLE;
          end
        else case (state)
            `IDLE: begin
		chan_rdreq <= #1 0;
		//check if the channel is full
		if(~chan_empty[check_next])
		  begin
                    if (have_space)
                      begin
                        //transmit if the usb buffer have space
                       //check if we should send
                       if (ready_to_send)
                           state <= #1 `HEADER1;
						    
                       overrun[check_next] <= 0;
                      end
                  else
                    begin
                      state <= #1 `IDLE;
                      overrun[check_next] <= 1;
                    end
                  rd_select <= #1 check_next;
                end
                check_next <= #1 (check_next == channels ? 4'd0 : check_next + 4'd1);
            end
            
            `HEADER1: begin
                fifodata[`PAYLOAD_LEN] <= #1 9'd504;
                payload_len <= #1 9'd504;
                fifodata[`TAG] <= #1 0;
                fifodata[`MBZ] <= #1 0;
                WR <= #1 1;
                
                state <= #1 `HEADER2;
                read_length <= #1 0;
            end
            
            `HEADER2: begin
                fifodata[`CHAN] <= #1 true_channel;
                fifodata[`RSSI] <= #1 true_rssi[5:0];
                fifodata[`BURST] <= #1 0;
                fifodata[`DROPPED] <= #1 0;
                fifodata[`UNDERRUN] <= #1 (check_next == 0) ? 1'b0 : underrun[true_channel];
                fifodata[`OVERRUN] <= #1 (check_next == 0) ? 1'b0 : overrun[true_channel];
                state <= #1 `TIMESTAMP;
            end
            
            `TIMESTAMP: begin
                fifodata <= #1 (timestamp_complete ? timestamp_clock[31:16] : timestamp_clock[15:0]);
                timestamp_complete <= #1 ~timestamp_complete;
                
                if (~timestamp_complete)
                    chan_rdreq <= #1 1;
                
                state <= #1 (timestamp_complete ? `FORWARD : `TIMESTAMP);
            end
            
            `FORWARD: begin
                read_length <= #1 read_length + 9'd2;
                fifodata <= #1 (read_length >= payload_len ? 16'hDEAD : chan_fifodata);
                
                if (read_length >= `MAXPAYLOAD)
                  begin
                    WR <= #1 0;
                    state <= #1 `IDLE;
					chan_rdreq <= #1 0;
                  end
                else if (read_length == payload_len - 4)
                    chan_rdreq <= #1 0;
            end
            
            default: begin
				//handling error state
                state <= `IDLE;
            end
            endcase
    end
endmodule

