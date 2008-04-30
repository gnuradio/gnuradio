module channel_demux
 #(parameter NUM_CHAN = 2) (     //usb Side
   input [31:0]usbdata_final,
   input WR_final, 
   // TX Side
   input reset,
   input txclk,
   output reg [NUM_CHAN:0] WR_channel,
   output reg [31:0] ram_data,
   output reg [NUM_CHAN:0] WR_done_channel );
   /* Parse header and forward to ram */
	
    reg [2:0]reader_state;
    reg [4:0]channel ;
    reg [6:0]read_length ;
	
	 // States
    parameter IDLE      =    3'd0;
    parameter HEADER    =    3'd1;
    parameter WAIT      =    3'd2;
    parameter FORWARD   =    3'd3;
	
	`define CHANNEL 20:16
	`define PKT_SIZE 127
	wire [4:0] true_channel;
	assign true_channel = (usbdata_final[`CHANNEL] == 5'h1f) ?
							NUM_CHAN : (usbdata_final[`CHANNEL]);
	
	always @(posedge txclk)
	  begin
	    if (reset)
	      begin
	       reader_state <= IDLE;
	       WR_channel <= 0;
	       WR_done_channel <= 0;
	      end
	      else
	        case (reader_state)
	        IDLE: begin
	            if (WR_final)
	                reader_state <= HEADER; 
	            end
	       
            // Store channel and forware header
	        HEADER: begin
	            channel <= true_channel;
	            WR_channel[true_channel] <= 1;
	            ram_data <= usbdata_final;
				read_length <= 7'd0 ;
				
                reader_state <= WAIT;
	        end
	       
	        WAIT: begin
	           WR_channel[channel] <= 0;
	
			   if (read_length == `PKT_SIZE)
	               reader_state <= IDLE;
	           else if (WR_final)
	               reader_state <= FORWARD;
	        end
	       
	        FORWARD: begin
	           WR_channel[channel] <= 1;
	           ram_data <= usbdata_final;
	           read_length <= read_length + 7'd1;
	           
	           reader_state <= WAIT;
	        end
	
			default:
               begin
					//error handling
                   reader_state <= IDLE;
               end
	       endcase
	   end
endmodule
