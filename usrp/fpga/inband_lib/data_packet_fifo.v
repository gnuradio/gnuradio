module data_packet_fifo 
  ( input       reset,
    input       clock,
    input       [15:0]ram_data_in,
    input       write_enable,
    output  reg have_space,
    output  reg [15:0]ram_data_out,
    output  reg pkt_waiting,
    input       read_enable,
    input       pkt_complete,
    input       skip_packet) ;

    /* Some parameters for usage later on */
    parameter DATA_WIDTH = 16 ;
    parameter NUM_PACKETS = 4 ;

    /* Create the RAM here */
    reg [DATA_WIDTH-1:0] usb_ram [256*NUM_PACKETS-1:0] ;

    /* Create the address signals */
    reg [7:0] usb_ram_offset_out ;
    reg [1:0] usb_ram_packet_out ;
    reg [7:0] usb_ram_offset_in ;
    reg [1:0] usb_ram_packet_in ;

    wire [7-2+NUM_PACKETS:0] usb_ram_aout ;
    wire [7-2+NUM_PACKETS:0] usb_ram_ain ;
    reg isfull;

    assign usb_ram_aout = {usb_ram_packet_out, usb_ram_offset_out} ;
    assign usb_ram_ain = {usb_ram_packet_in, usb_ram_offset_in} ;
    
    // Check if there is one full packet to process
    always @(usb_ram_ain, usb_ram_aout)
    begin
        if (reset)
            pkt_waiting <= 0;
        else if (usb_ram_ain >= usb_ram_aout)
            pkt_waiting <= usb_ram_ain - usb_ram_aout >= 256;
        else
            pkt_waiting <= (usb_ram_ain + 10'b1111111111 - usb_ram_aout) >= 256;
    end

    // Check if there is room
    always @(usb_ram_ain, usb_ram_aout)
    begin
        if (reset)
            have_space <= 1;
        else if (usb_ram_ain == usb_ram_aout)
            have_space <= ~isfull;   
        else if (usb_ram_ain > usb_ram_aout)
            have_space <= (usb_ram_ain - usb_ram_aout) <= 256 * (NUM_PACKETS - 1);
        else
            have_space <= (usb_ram_aout - usb_ram_ain) >= 256;
    end

    /* RAM Write Address process */
    always @(posedge clock)
    begin
        if( reset )
          begin
            usb_ram_offset_in <= 0 ;
            usb_ram_packet_in <= 0 ;
          end
        else
            if( pkt_complete )
              begin
                usb_ram_packet_in <= usb_ram_packet_in + 1;  
                usb_ram_offset_in <= 0;
              end
            else if( write_enable ) 
              begin
                if (usb_ram_offset_in == 8'b11111111)
                  begin
                    usb_ram_offset_in <= 0;
                    usb_ram_packet_in <= usb_ram_packet_in + 1;    
                  end
                else
                    usb_ram_offset_in <= usb_ram_offset_in + 1 ;
                    if (usb_ram_ain + 1 == usb_ram_aout)
                       isfull <= 1;
              end
    end

    /* RAM Writing process */
    always @(posedge clock)
    begin
        if( write_enable ) 
          begin
            usb_ram[usb_ram_ain] <= ram_data_in ;
          end
    end

    /* RAM Read Address process */
    always @(posedge clock)
    begin
        if( reset ) 
          begin
            usb_ram_packet_out <= 0 ;
            usb_ram_offset_out <= 0 ;
            isfull <= 0;
          end
        else
            if( skip_packet )
              begin
                usb_ram_packet_out <= usb_ram_packet_out + 1 ;
                usb_ram_offset_out <= 0 ;
              end
            else if(read_enable) begin
                if( usb_ram_offset_out == 8'b11111111 )
                  begin
                    usb_ram_offset_out <= 0 ;
                    usb_ram_packet_out <= usb_ram_packet_out + 1 ;
                  end
                else
                    usb_ram_offset_out <= usb_ram_offset_out + 1 ;  
            end 
            if (usb_ram_ain == usb_ram_aout)
               isfull <= 0;                    
    end

    /* RAM Reading Process */
    always @(posedge clock)
    begin
        ram_data_out <= usb_ram[usb_ram_aout] ;
    end

endmodule
