
module usb_fifo_writer
   #(parameter BUS_WIDTH = 16,
     parameter NUM_CHAN = 2,
     parameter FIFO_WIDTH = 32)
   (     //FX2 Side
			input bus_reset, 
			input usbclk, 
			input WR_fx2, 
			input [15:0]usbdata,
			
			// TX Side
			input reset,
			input txclk,
			output reg [NUM_CHAN:0] WR_channel,
			output reg [FIFO_WIDTH-1:0] ram_data,
			output reg [NUM_CHAN:0] WR_done_channel );
   

	reg [8:0] write_count;

	/* Fix FX2 bug */
	always @(posedge usbclk)
    	if(bus_reset)        // Use bus reset because this is on usbclk
       		write_count <= #1 0;
    	else if(WR_fx2 & ~write_count[8])
    		write_count <= #1 write_count + 9'd1;
    	else
    		write_count <= #1 WR_fx2 ? write_count : 9'b0;

	reg WR_fx2_fixed;
	reg [15:0]usbdata_fixed;
	
	always @(posedge usbclk) 
	begin
	   WR_fx2_fixed <= WR_fx2 & ~write_count[8];
	   usbdata_fixed <= usbdata;
	end

	/* Used to convert 16 bits bus_data to the 32 bits wide fifo */
    reg                             word_complete ;
    reg     [BUS_WIDTH-1:0]         usbdata_delayed ;
    reg                             writing ;
	wire	[FIFO_WIDTH-1:0]		usbdata_packed ;    
	wire							WR_packed ;
   
    always @(posedge usbclk)
    begin
        if (bus_reset)
          begin
            word_complete <= 0 ;
            writing <= 0 ;
          end
        else if (WR_fx2_fixed)
          begin
            writing <= 1 ;
            if (word_complete)
                word_complete <= 0 ;
            else
              begin
                usbdata_delayed <= usbdata_fixed ;
                word_complete <= 1 ;
              end
          end
        else
            writing <= 0 ;
	end
    
	assign usbdata_packed = {usbdata_fixed, usbdata_delayed} ;
    assign WR_packed = word_complete & writing ;

	/* Make sure data are sync with usbclk */
 	reg [31:0]usbdata_usbclk;
	reg WR_usbclk; 
    
    always @(posedge usbclk)
    begin
    	if (WR_packed)
    		usbdata_usbclk <= usbdata_packed;
        WR_usbclk <= WR_packed;
    end

	/* Cross clock boundaries */
	reg [FIFO_WIDTH-1:0] usbdata_tx ;
	reg WR_tx;
    reg WR_1;
    reg WR_2;
	reg [31:0] usbdata_final;
	reg WR_final;
   
	always @(posedge txclk) usbdata_tx <= usbdata_usbclk;

    always @(posedge txclk) 
    	if (reset)
    		WR_1 <= 0;
    	else
       		WR_1 <= WR_usbclk;

    always @(posedge txclk) 
    	if (reset)
       		WR_2 <= 0;
    	else
      		WR_2 <= WR_1;

	always @(posedge txclk)
	begin
		if (reset)
			WR_tx <= 0;
		else
		   WR_tx <= WR_1 & ~WR_2;
	end
	
	always @(posedge txclk)
	begin
	   if (reset)
	      WR_final <= 0;
	   else
	   begin
	      WR_final <= WR_tx; 
	      if (WR_tx)
	         usbdata_final <= usbdata_tx;
	   end
	end
	     
	/* Parse header and forward to ram */
	reg [3:0]reader_state;
	reg [4:0]channel ;
	reg [9:0]read_length ;
	
	parameter IDLE = 4'd0;
	parameter HEADER = 4'd1;
	parameter WAIT = 4'd2;
	parameter FORWARD = 4'd3;
	
	`define CHANNEL 20:16
	`define PKT_SIZE 512
	
	always @(posedge txclk)
	begin
	    if (reset)
	      begin
	       reader_state <= 0;
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
	            channel <= (usbdata_final[`CHANNEL] == 5'h1f ? NUM_CHAN : usbdata_final[`CHANNEL]) ;
	            WR_channel[(usbdata_final[`CHANNEL] == 5'h1f ? NUM_CHAN : usbdata_final[`CHANNEL])] <= 1;
				//channel <= usbdata_final[`CHANNEL] ;
	            //WR_channel[usbdata_final[`CHANNEL]] <= 1;
	            ram_data <= usbdata_final;
				read_length <= 10'd4 ;
				
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
	           read_length <= read_length + 10'd4;
	           
	           reader_state <= WAIT;
	        end
	       endcase
	end
endmodule  