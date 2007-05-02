`default_nettype none

module usb_packet_fifo2(reset, usb_clock, fpga_clock, write_enable, write_data, 
        read_enable, skip_packet, read_data, have_space, pkt_waiting, tx_empty) ;
    
    /* Module parameters */
    parameter                       LOG2_N          =   2 ;
    parameter                       BUS_WIDTH       =   16 ;
    parameter                       FIFO_WIDTH      =   32 ;

    input   wire                    reset;
    input   wire                    usb_clock ;
    input   wire                    fpga_clock ;
    input   wire                    write_enable ;
    input   wire    [BUS_WIDTH-1:0] write_data ;
    input   wire                    read_enable ;
    input   wire                    skip_packet ;
    output  wire   [FIFO_WIDTH-1:0] read_data ;
    output  wire                    have_space ;
    output  wire                    pkt_waiting ;
    output  wire                    tx_empty;

    
    /* Variable for generate statement */
    genvar i ;
    
    /* Local wires for FIFO connections */
    wire                      [2**LOG2_N-1:0]     fifo_resets ;
    reg                       [2**LOG2_N-1:0]     fifo_we ;
    wire                      [2**LOG2_N-1:0]     fifo_re ;
    reg                       [FIFO_WIDTH-1:0]    fifo_wdata[2**LOG2_N-1:0] ;
    wire                      [FIFO_WIDTH-1:0]    fifo_rdata[2**LOG2_N-1:0] ;
    wire                      [2**LOG2_N-1:0]     fifo_rempty ;
    wire                      [2**LOG2_N-1:0]     fifo_rfull ;
    wire                      [2**LOG2_N-1:0]     fifo_wempty ;
    wire                      [2**LOG2_N-1:0]     fifo_wfull ;
    
    /* FIFO Select for read and write ports */
    reg     [LOG2_N-1:0]            fifo_rselect ;
    reg     [LOG2_N-1:0]            fifo_wselect ;
    
    /* Used to convert 16 bits usbdata to the 32 bits wide fifo */
    reg                             word_complete ;
    reg     [BUS_WIDTH-1:0]         write_data_delayed ;
    
    /* Assign have_space to empty flag of currently selected write FIFO */
    assign have_space = fifo_wempty[fifo_wselect] ;
    
    /* Assign pkt_waiting to full flag of currently selected read FIFO */
    assign pkt_waiting = fifo_rfull[fifo_rselect] ;
    
    /* Assign the read_data to the output of the currently selected FIFO */
    assign read_data = fifo_rdata[fifo_rselect] ;
    
    /* Figure out if we're all empty */
    assign tx_empty = !(~fifo_rempty) ;
    
    /* Increment fifo_rselect here */
    always @(posedge fpga_clock)
    begin
        if (reset)
            fifo_rselect <= {2**LOG2_N{1'b0}} ;
        
        if (fifo_rempty[fifo_rselect])
            fifo_rselect <= fifo_rselect + 1 ;
            
        if (skip_packet)
            fifo_rselect <= fifo_rselect + 1 ;
    end
    
    /* Increment fifo_wselect and pack data into 32 bits block  */
    always @(posedge usb_clock, reset)
    begin
        if (reset)
          begin
            fifo_wselect <= {2**LOG2_N{1'b0}} ;
            word_complete <= 0;
          end
            
        if (fifo_wfull[fifo_wselect])
            fifo_wselect <= fifo_wselect + 1 ;
            
        if (write_enable)
          begin
            word_complete <= ~word_complete ;
            
            if (word_complete)
                fifo_wdata[fifo_wselect] <= {write_data_delayed, write_data} ;
            else
                write_data_delayed <= write_data ;
              
            /* Avoid to continue to write in the previous fifo when we have 
               just swichted to the next one */ 
            fifo_we[fifo_wselect-1] <= 0 ;
            
            fifo_we[fifo_wselect] <= write_enable & word_complete ;
          end
    end
    
    /* Generate all the single packet FIFOs */
    generate
        for( i = 0 ; i < 2**LOG2_N ; i = i + 1 )
        begin : generate_single_packet_fifos
            assign fifo_re[i] = (fifo_rselect == i) ? read_enable : 1'b0 ;
            assign fifo_resets[i] = (fifo_rselect == i) ? skip_packet : 1'b0 ;
            fifo_512 single_packet_fifo(.wrclk  ( usb_clock      ),
                                        .rdclk  ( fpga_clock     ),
                                        .aclr   ( fifo_resets[i] ), 
                                        .wrreq  ( fifo_we[i]     ),
                                        .data   ( fifo_wdata[i]  ),
                                        .rdreq  ( fifo_re[i]     ),
                                        .q      ( fifo_rdata[i]  ),
                                        .rdfull ( fifo_rfull[i]  ),
                                        .rdempty( fifo_rempty[i] ),
                                        .wrfull ( fifo_wfull[i]  ),
                                        .wrempty( fifo_wempty[i] ) ) ;
        end
    endgenerate
endmodule 