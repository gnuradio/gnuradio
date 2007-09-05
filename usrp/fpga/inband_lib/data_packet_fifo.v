module data_packet_fifo 
  ( input       reset,
    input       clock,
    input       [31:0]ram_data_in,
    input       write_enable,
    output  reg have_space,
    output  reg [31:0]ram_data_out,
    output  reg pkt_waiting,
	output	reg	isfull,
	output	reg [1:0]usb_ram_packet_out,
	output	reg [1:0]usb_ram_packet_in,
    input       read_enable,
    input       pkt_complete,
    input       skip_packet) ;

    /* Some parameters for usage later on */
    parameter DATA_WIDTH = 32 ;
    parameter PKT_DEPTH = 128 ;
    parameter NUM_PACKETS = 4 ;

    /* Create the RAM here */
    reg [DATA_WIDTH-1:0] usb_ram [PKT_DEPTH*NUM_PACKETS-1:0] ;

    /* Create the address signals */
    reg [6:0] usb_ram_offset_out ;
    //reg [1:0] usb_ram_packet_out ;
    reg [6:0] usb_ram_offset_in ;
    //reg [1:0] usb_ram_packet_in ;

    wire [6-2+NUM_PACKETS:0] usb_ram_aout ;
    wire [6-2+NUM_PACKETS:0] usb_ram_ain ;
    //reg isfull;

    assign usb_ram_aout = {usb_ram_packet_out, usb_ram_offset_out} ;
    assign usb_ram_ain = {usb_ram_packet_in, usb_ram_offset_in} ;
    
    // Check if there is one full packet to process
    always @(usb_ram_ain, usb_ram_aout, isfull)
    begin
        if (usb_ram_ain == usb_ram_aout)
            pkt_waiting <= isfull ;
        else if (usb_ram_ain > usb_ram_aout)
            pkt_waiting <= (usb_ram_ain - usb_ram_aout) >= PKT_DEPTH;
        else
            pkt_waiting <= (usb_ram_ain + 10'b1000000000 - usb_ram_aout) >= PKT_DEPTH;
    end
 
    // Check if there is room
    always @(usb_ram_ain, usb_ram_aout, isfull)
    begin
        if (usb_ram_ain == usb_ram_aout)
            have_space <= ~isfull;   
        else if (usb_ram_ain > usb_ram_aout)
            have_space <= ((usb_ram_ain - usb_ram_aout) <= PKT_DEPTH * (NUM_PACKETS - 1))? 1'b1 : 1'b0;
        else
            have_space <= (usb_ram_aout - usb_ram_ain) >= PKT_DEPTH;
    end



    /* RAM Writing/Reading process */
    always @(posedge clock)
    begin
        if( write_enable ) 
          begin
            usb_ram[usb_ram_ain] <= ram_data_in ;
          end
		ram_data_out <= usb_ram[usb_ram_aout] ;
    end

    /* RAM Write/Read Address process */
    always @(posedge clock)
    begin
        if( reset ) 
          begin
            usb_ram_packet_out <= 0 ;
            usb_ram_offset_out <= 0 ;
			usb_ram_offset_in <= 0 ;
            usb_ram_packet_in <= 0 ;
            isfull <= 0;
          end
        else
		  begin
            if( skip_packet )
              begin
                usb_ram_packet_out <= usb_ram_packet_out + 1 ;
                usb_ram_offset_out <= 0 ;
                isfull <= 0;
              end
            else if(read_enable) 
			  begin
                if( usb_ram_offset_out == 7'b1111111 )
                  begin
                    isfull <= 0 ;
                    usb_ram_offset_out <= 0 ;
                    usb_ram_packet_out <= usb_ram_packet_out + 1 ;
                  end
                else
                    usb_ram_offset_out <= usb_ram_offset_out + 1 ;  
              end
			if( pkt_complete )
              begin
                usb_ram_packet_in <= usb_ram_packet_in + 1 ;
                usb_ram_offset_in <= 0 ;
                if ((usb_ram_packet_in + 2'b1) == usb_ram_packet_out)
                    isfull <= 1 ;
              end
            else if( write_enable ) 
              begin
                if (usb_ram_offset_in == 7'b1111111)
                    usb_ram_offset_in <= 7'b1111111 ;    
                else
                    usb_ram_offset_in <= usb_ram_offset_in + 1 ;
              end
		  end
    end

endmodule
