module halfband_tb( ) ;
    
    // Parameters for instantiation
    parameter               clocks  = 2 ; // Number of clocks per input
    parameter               decim   = 0 ; // Sets the filter to decimate
    parameter               rate    = 2 ; // Sets the decimation rate

    reg                     clock ;
    reg                     reset ;
    reg                     enable ;
    reg                     strobe_in ;
    reg     signed  [17:0]  data_in ;
    wire                    strobe_out ;
    wire    signed  [17:0]  data_out ;

    // Setup the clock
    initial clock = 1'b0 ;
    always #5 clock <= ~clock ;

    // Come out of reset after a while
    initial reset = 1'b1 ;
    initial #100 reset = 1'b0 ;

    // Enable the entire system
    initial enable = 1'b1 ;

    // Instantiate UUT
    halfband_ideal 
      #(
        .decim      ( decim         ),
        .rate       ( rate          )
      ) uut(
        .clock      ( clock         ),
        .reset      ( reset         ),
        .enable     ( enable        ),
        .strobe_in  ( strobe_in     ),
        .data_in    ( data_in       ),
        .strobe_out ( strobe_out    ),
        .data_out   ( data_out      )
      ) ;
    
    integer i, ri, ro, infile, outfile ;

    // Setup file IO
    initial begin
        infile = $fopen("input.dat","r") ;
        outfile = $fopen("output.dat","r") ;
        $timeformat(-9, 2, " ns", 10) ;
    end

    reg                 endofsim ;
    reg signed  [17:0]  compare ;
    integer             noe ;
    initial             noe = 0 ;

    initial begin
        // Initialize inputs
        strobe_in <= 1'd0 ;
        data_in <= 18'd0 ;

        // Wait for reset to go away
        @(negedge reset) #0 ;
        
        // While we're still simulating ...
        while( !endofsim ) begin

            // Write the input from the file or 0 if EOF...
            @( posedge clock ) begin
                #1 ;
                strobe_in <= 1'b1 ;
                if( !$feof(infile) )
                    ri = $fscanf( infile, "%d", data_in ) ;
                else
                    data_in <= 18'd0 ;
            end

            // Clocked in - set the strobe to 0 if the number of
            // clocks per sample is greater than 1
            if( clocks > 1 ) begin
                @(posedge clock) begin
                    strobe_in <= 1'b0  ;
                end

                // Wait for the specified number of cycles
                for( i = 0 ; i < (clocks-2) ; i = i + 1 ) begin
                    @(posedge clock) #1 ;
                end
            end
        end

        // Print out the number of errors that occured
        if( noe )
            $display( "FAILED: %d errors during simulation", noe ) ;
        else
            $display( "PASSED: Simulation successful" ) ;

        $stop ;
    end

    // Output comparison of simulated values versus known good values
    always @ (posedge clock) begin
        if( reset )
            endofsim <= 1'b0 ;
        else begin
            if( !$feof(outfile) ) begin
                if( strobe_out ) begin
                    ro = $fscanf( outfile, "%d\n", compare ) ;
                    if( compare != data_out ) begin
                        $display( "%t: %d != %d", $realtime, data_out, compare ) ;
                        noe = noe + 1 ;
                    end
                end
            end else begin
                // Signal end of simulation when no more outputs
                endofsim <= 1'b1 ;
            end
        end
    end     

endmodule
