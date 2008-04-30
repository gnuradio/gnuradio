module chan_fifo_reader 
   (reset, tx_clock, tx_strobe, timestamp_clock, samples_format,
    fifodata, pkt_waiting, rdreq, skip, tx_q, tx_i,
    underrun, tx_empty, debug, rssi, threshhold, rssi_wait) ;

   input   wire                     reset ;
   input   wire                     tx_clock ;
   input   wire                     tx_strobe ; //signal to output tx_i and tx_q
   input   wire              [31:0] timestamp_clock ; //current time
   input   wire               [3:0] samples_format ;// not useful at this point
   input   wire              [31:0] fifodata ; //the data input
   input   wire                     pkt_waiting ; //signal the next packet is ready
   output  reg                      rdreq ; //actually an ack to the current fifodata
   output  reg                      skip ; //finish reading current packet
   output  reg               [15:0] tx_q ; //top 16 bit output of fifodata
   output  reg               [15:0] tx_i ; //bottom 16 bit output of fifodata
   output  reg                      underrun ; 
   output  reg                      tx_empty ; //cause 0 to be the output
   input   wire		     [31:0] rssi;
   input   wire		     [31:0] threshhold;
   input   wire		     [31:0] rssi_wait;

   output wire [14:0] debug;
   assign debug = {7'd0, rdreq, skip, reader_state, pkt_waiting, tx_strobe, tx_clock};
   
   //Samples format
   // 16 bits interleaved complex samples
   `define QI16                     4'b0
    
   // States
   parameter IDLE           =     3'd0;    
   parameter HEADER         =     3'd1;
   parameter TIMESTAMP      =     3'd2;
   parameter WAIT           =     3'd3;
   parameter WAITSTROBE     =     3'd4;
   parameter SEND           =     3'd5;

   // Header format
   `define PAYLOAD                  8:2
   `define ENDOFBURST               27
   `define STARTOFBURST             28
   `define RSSI_FLAG                26
	

   /* State registers */
   reg                        [2:0] reader_state;
   /* Local registers */  
   reg                        [6:0] payload_len;
   reg                        [6:0] read_len;
   reg                       [31:0] timestamp;
   reg                              burst;
   reg                              trash;
   reg                              rssi_flag;
   reg			     [31:0] time_wait;
   
   always @(posedge tx_clock)
     begin
       if (reset) 
         begin
           reader_state <= IDLE;
           rdreq <= 0;
           skip <= 0;
           underrun <= 0;
           burst <= 0;
           tx_empty <= 1;
           tx_q <= 0;
           tx_i <= 0;
           trash <= 0;
           rssi_flag <= 0;
           time_wait <= 0;
         end
       else 
         begin
           case (reader_state)
             IDLE:
               begin
               /*
		* reset all the variables and wait for a tx_strobe
		* it is assumed that the ram connected to this fifo_reader 
		* is a short hand fifo meaning that the header to the next packet
		* is already available to this fifo_reader when pkt_waiting is on
		*/
                 skip <=0;
                 time_wait <= 0;
                 if (pkt_waiting == 1)
                   begin
                     reader_state <= HEADER;
                     rdreq <= 1;
                     underrun <= 0;
                   end
                 if (burst == 1 && pkt_waiting == 0)
                     underrun <= 1;
                 if (tx_strobe == 1)
                     tx_empty <= 1 ;
               end

               /* Process header */
               HEADER:
                 begin
                   if (tx_strobe == 1)
                       tx_empty <= 1 ;
                   
                   rssi_flag <= fifodata[`RSSI_FLAG]&fifodata[`STARTOFBURST];
                   //Check Start/End burst flag
                   if  (fifodata[`STARTOFBURST] == 1 
                       && fifodata[`ENDOFBURST] == 1)
                       burst <= 0;
                   else if (fifodata[`STARTOFBURST] == 1)
                       burst <= 1;
                   else if (fifodata[`ENDOFBURST] == 1)
                       burst <= 0;

                   if (trash == 1 && fifodata[`STARTOFBURST] == 0)
                     begin
                       skip <= 1;
                       reader_state <= IDLE;
                       rdreq <= 0;
                     end 
                   else
                     begin   
                       payload_len <= fifodata[`PAYLOAD] ;
                       read_len <= 0;
                       rdreq <= 1;
                       reader_state <= TIMESTAMP;
                     end
                 end

               TIMESTAMP: 
                 begin
                   timestamp <= fifodata;
                   reader_state <= WAIT;
                   if (tx_strobe == 1)
                       tx_empty <= 1 ;
                   rdreq <= 0;
                 end
				
               // Decide if we wait, send or discard samples
               WAIT: 
                 begin
                   if (tx_strobe == 1)
                       tx_empty <= 1 ;
                    
                   time_wait <= time_wait + 32'd1;
                   // Outdated
                   if ((timestamp < timestamp_clock) ||
                      (time_wait >= rssi_wait && rssi_wait != 0 && rssi_flag))
                     begin
                       trash <= 1;
                       reader_state <= IDLE;
                       skip <= 1;
                     end  
                   // Let's send it					
                   else if (timestamp == timestamp_clock 
                             || timestamp == 32'hFFFFFFFF)
                     begin
                       if (rssi <= threshhold || rssi_flag == 0)
                         begin
                           trash <= 0;
                           reader_state <= WAITSTROBE; 
                         end
                       else
                         reader_state <= WAIT;
                     end
                   else
                       reader_state <= WAIT;
                 end
                 
               // Wait for the transmit chain to be ready
               WAITSTROBE:
                 begin
                   // If end of payload...
                   if (read_len == payload_len)
                     begin
                       reader_state <= IDLE;
                       skip <= 1;
                       if (tx_strobe == 1)
                           tx_empty <= 1 ;
                     end  
                   else if (tx_strobe == 1)
                     begin
                       reader_state <= SEND;
                       rdreq <= 1;
                     end
                 end
               
               // Send the samples to the tx_chain
               SEND:
                 begin
                   reader_state <= WAITSTROBE; 
                   read_len <= read_len + 7'd1;
                   tx_empty <= 0;
                   rdreq <= 0;
                   
                   case(samples_format)
                       `QI16:
                        begin
                            tx_i <= fifodata[15:0];
                            tx_q <= fifodata[31:16];
                        end
                        
                        // Assume 16 bits complex samples by default
                        default:
                        begin
                            tx_i <= fifodata[15:0];
                            tx_q <= fifodata[31:16];
                        end 
                   endcase
                 end
               
               default:
                 begin
                   //error handling
                   reader_state <= IDLE;
                 end
           endcase
       end
   end
 
endmodule
