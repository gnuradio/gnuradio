module usb_fifo_reader (
      input usbclk,
      input bus_reset, 
      input RD,
      output rdreq,
      );
      
    // FX2 Bug Fix
    reg [8:0] read_count;
    always @(negedge usbclk)
        if(bus_reset)
            read_count <= #1 9'd0;
        else if(RD & ~read_count[8])
            read_count <= #1 read_count + 9'd1;
        else
            read_count <= #1 RD ? read_count : 9'b0;
            
    assign rdreq = RD & ~read_count[8];
    
    
 
endmodule
       
   
   