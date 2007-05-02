module usb_fifo_reader (tx_clock, fifodata, pkt_waiting, reset,
      rdreq, skip, done_chan, WR_chan, tx_data_bus);
      
    /* Module parameters */
    parameter                       NUM_CHAN      =   2 ;
    parameter                       WIDTH         =   32 ;
    
    input   wire                    tx_clock ;
    input   wire                    reset ;
    input   wire       [WIDTH-1:0]  fifodata ;
    input   wire                    pkt_waiting ;
    output  reg                     rdreq ;
    output  reg                     skip ;
    output  reg        [NUM_CHAN:0] done_chan ;
    output  reg        [NUM_CHAN:0] WR_chan ;
    output  reg        [WIDTH-1:0]  tx_data_bus ;
     
   
   
    /* States definition */
    `define IDLE                      3'd0
    `define WAIT                      3'd1
    `define READ_HEADER               3'd2
    `define FORWARD_DATA              3'd3
    `define SKIP_REST                 3'd4
   
    /* Channel Ids */
    `define TXCHAN0                   5'h0
    `define TXCHAN1                   5'h1
    `define TXCMD                     5'h1F
   
    /* Local registers */
    reg                      [2:0]    reader_state ;
    reg                      [2:0]    reader_next_state ;
    reg                      [4:0]    channel ;
    reg                      [8:0]    pkt_length ;
    reg                      [8:0]    read_length ;
    
    /* State Machine */
    always @(posedge tx_clock)
    begin
        if (reset) 
		  begin
		      reader_state <= `IDLE ;
            reader_next_state <= `IDLE ;
            rdreq <= 0 ;
            skip <= 0 ;
            WR_chan <= {NUM_CHAN+1{1'b0}} ;
            done_chan <= {NUM_CHAN+1{1'b0}} ;
          end
        else 
		  begin
            reader_state = reader_next_state ;
            
            case(reader_state)
            `IDLE: 
				begin
				    reader_next_state <= pkt_waiting ? `WAIT : `IDLE ;
                rdreq <= pkt_waiting ;
            end
     
            /* Wait for the fifo's data to show up */
            `WAIT:
            begin
			       reader_next_state <= `READ_HEADER ;
            end
               
            `READ_HEADER: 
			   begin
                reader_next_state <= `FORWARD_DATA ;
                  
                /* Read header fields */
                channel <= (fifodata & 32'h1F0000) ;
                pkt_length <= (fifodata & 16'h1FF) + 4 ;
                read_length <= 9'd0 ;
                  
                /* Forward data */
                case (channel)
                    `TXCHAN0: WR_chan[0] <= 1 ;
                    `TXCHAN1: WR_chan[1] <= 1 ;
                    `TXCMD:   WR_chan[2] <= 1 ;
                    default:  WR_chan <= 1 ;
                endcase
                tx_data_bus <= fifodata ;
            end
               
            `FORWARD_DATA:
			   begin
                read_length <= read_length + 4 ;
                  
                // If end of payload...
                if (read_length == pkt_length)
				    begin
                    reader_next_state <= `SKIP_REST ;
                    /* If the packet is 512 bytes, don't skip */
                    skip <= pkt_length < 506 ;
                     
                    /* Data pushing done */
                    WR_chan <= {NUM_CHAN+1{1'b0}} ;
                    
                    /* Notify next block */
                    case (channel)
                       `TXCHAN0: done_chan[0] <= 1 ;
                       `TXCHAN1: done_chan[1] <= 1 ;
                       `TXCMD:   done_chan[2] <= 1 ;
                       default:  done_chan[0] <= 1 ;
                    endcase
                end
                else if (read_length == pkt_length - 4)
                    rdreq <= 0 ;
                    
                /* Forward data */
                tx_data_bus <= fifodata ;
            end
               
            `SKIP_REST: 
			   begin
			       reader_next_state <= pkt_waiting ? `READ_HEADER : `IDLE ;
                done_chan <= {NUM_CHAN+1{1'b0}} ;
                rdreq <= pkt_waiting ;
                skip <= 0 ;
            end
                
            default: 
			   begin
                reader_state <= `IDLE;
                reader_next_state <= `IDLE;
            end
            endcase
        end
    end  
endmodule
       
   
   