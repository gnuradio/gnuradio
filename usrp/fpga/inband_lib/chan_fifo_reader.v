module chan_fifo_reader 
  ( input       reset,
    input       tx_clock,
    input       tx_strobe,
    input       [31:0]adc_clock,
    input       [3:0] samples_format,
    input       [15:0] fifodata,
    input       pkt_waiting,
    output  reg rdreq,
    output  reg skip,
    output  reg [15:0]tx_q,
    output  reg [15:0]tx_i,
    output  reg overrun,
    output  reg underrun) ;
    
    // Should not be needed if adc clock rate < tx clock rate
    `define JITTER        5
    
    //Samples format
    // 16 bits interleaved complex samples
    `define QI16         4'b0
    
    // States
   `define IDLE          4'd0
   `define READ          4'd1
   `define HEADER1       4'd2
   `define HEADER2       4'd3
   `define TIMESTAMP1    4'd4
   `define TIMESTAMP2    4'd5
   `define WAIT          4'd6
   `define WAITSTROBE    4'd7
   `define SENDWAIT      4'd8
   `define SEND          4'd9
   `define FEED          4'd10
   `define DISCARD       4'd11

   // State registers
   reg[3:0] reader_state;
   reg[3:0] reader_next_state;
   
   //Variables
   reg[8:0] payload_len;
   reg[8:0] read_len;
   reg[31:0] timestamp;
   reg burst;
   reg qsample;
   always @(posedge tx_clock)
   begin
       if (reset) 
          begin
             reader_state <= `IDLE;
             reader_next_state <= `IDLE;
             rdreq <= 0;
             skip <= 0;
             overrun <= 0;
             underrun <= 0;
             burst <= 0;
             qsample <= 1;
          end
       else 
		 begin
           reader_state = reader_next_state;
           case (reader_state)
               `IDLE:
                  begin
                     if (pkt_waiting == 1)
                       begin
                          reader_next_state <= `READ;
                          rdreq <= 1;
                          underrun <= 0;
                       end
                     else if (burst == 1)
                        underrun <= 1;
                  end

				// Just wait for the fifo data to arrive
               `READ: 
                  begin
                     reader_next_state <= `HEADER1;
                  end
				
				// First part of the header
               `HEADER1:
                  begin
                     reader_next_state <= `HEADER2;
                     
                     //Check Start burst flag
                     if (fifodata[3] == 1)
                        burst <= 1;
                        
                     if (fifodata[4] == 1)
                        burst <= 0;
                  end

				// Read payload length
               `HEADER2:
                  begin
                     payload_len <= (fifodata & 16'h1FF);
                     read_len <= 9'd0;
                     reader_next_state <= `TIMESTAMP1;
                  end

               `TIMESTAMP1: 
                  begin
                     timestamp <= {fifodata, 16'b0};
                     rdreq <= 0;
                     reader_next_state <= `TIMESTAMP2;
                  end
				
               `TIMESTAMP2:
                  begin
                     timestamp <= timestamp + fifodata;
                     reader_next_state <= `WAIT;
                  end
				
				// Decide if we wait, send or discard samples
               `WAIT: 
                  begin
                   // Wait a little bit more
                     if (timestamp > adc_clock + `JITTER)
                        reader_next_state <= `WAIT;
                   // Let's send it
                   else if ((timestamp < adc_clock + `JITTER 
                           && timestamp > adc_clock)
                           || timestamp == 32'hFFFFFFFF)
                      begin
                         reader_next_state <= `WAITSTROBE;
                      end
                   // Outdated
                   else if (timestamp < adc_clock)
                      begin
                         reader_next_state <= `DISCARD;
                         skip <= 1;
                     end
                 end
                 
            // Wait for the transmit chain to be ready
               `WAITSTROBE:
                  begin
                      // If end of payload...
                     if (read_len == payload_len)
                        begin
                           reader_next_state <= `DISCARD;
                           skip <= (payload_len < 508);
                        end
                          
                      if (tx_strobe == 1)
                         reader_next_state <= `SENDWAIT;
                  end
               
               `SENDWAIT:
                  begin
                     rdreq <= 1;
                     reader_next_state <= `SEND; 
                  end
               
				// Send the samples to the tx_chain
               `SEND:
                  begin
                     reader_next_state <= `WAITSTROBE; 
                     rdreq <= 0;
                     read_len <= read_len + 2;
                     case(samples_format)
                        `QI16:
                           begin
                              tx_q <= qsample ? fifodata : 16'bZ;
                              tx_i <= ~qsample ? fifodata : 16'bZ;
                              qsample <= ~ qsample;
                           end  
                        default:
                           begin
                               // Assume 16 bits complex samples by default
                              $display ("Error unknown samples format");
                              tx_q <= qsample ? fifodata : 16'bZ;
                              tx_i <= ~qsample ? fifodata : 16'bZ;
                              qsample <= ~ qsample;
                           end 
                     endcase
                  end

               `DISCARD:
                  begin
                     skip <= 0;
                     reader_next_state <= `IDLE;
                  end
               
               default:
                  begin
                     $display ("Error unknown state");
                     reader_state <= `IDLE;
                     reader_next_state <= `IDLE;
                  end
           endcase
       end
   end
 
endmodule