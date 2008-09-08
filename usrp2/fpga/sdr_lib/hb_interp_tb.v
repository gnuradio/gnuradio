module hb_interp_tb( ) ;
   
   // Parameters for instantiation
   parameter               clocks  = 8'd2 ; // Number of clocks per output
   parameter               decim   = 1 ; // Sets the filter to decimate
   parameter               rate    = 2 ; // Sets the decimation rate
   
   reg                     clock ;
   reg                     reset ;
   reg                     enable ;
   wire 		   strobe_in ;
   reg 			   signed  [17:0]  data_in ;
   wire                    strobe_out ;
   wire 		   signed  [17:0]  data_out ;
   
   initial
     begin
	$dumpfile("hb_interp_tb.vcd");
	$dumpvars(0,hb_interp_tb);
     end
   
   // Setup the clock
   initial clock = 1'b0 ;
   always #5 clock <= ~clock ;
   
   // Come out of reset after a while
   initial reset = 1'b1 ;
   initial #1000 reset = 1'b0 ;

   always @(posedge clock)
     enable <= ~reset;
   
   // Instantiate UUT
   /*
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
    */

   cic_strober #(.WIDTH(8))
     out_strober(.clock(clock),.reset(reset),.enable(enable),.rate(clocks),
		 .strobe_fast(1),.strobe_slow(strobe_out) );
   
   cic_strober #(.WIDTH(8))
     in_strober(.clock(clock),.reset(reset),.enable(enable),.rate(2),
		.strobe_fast(strobe_out),.strobe_slow(strobe_in) );
   
   hb_interp #() uut
     (.clk(clock),.rst(reset),.bypass(0),.cpo(clocks),.stb_in(strobe_in),.data_in(data_in),
      .stb_out(strobe_out),/* .output_rate(clocks), */ .data_out(data_out) );
   
   integer i, ri, ro, infile, outfile ;
   
   always @(posedge clock)
     begin
	if(strobe_out)
	  $display(data_out);
     end
   
   // Setup file IO
   initial begin
      infile = $fopen("input.dat","r") ;
      outfile = $fopen("output.dat","r") ;
      $timeformat(-9, 2, " ns", 10) ;
   end
   
   reg                 endofsim ;
   reg 		       signed  [17:0]  compare ;
   integer             noe ;
   initial             noe = 0 ;
   
   initial begin
      // Initialize inputs
      data_in <= 18'd0 ;
      
      // Wait for reset to go away
      @(negedge reset) #0 ;
      
      // While we're still simulating ...
      while( !endofsim ) begin
	 
         // Write the input from the file or 0 if EOF...
         @( negedge clock ) begin
            if(strobe_in)
              if( !$feof(infile) )
                ri <= #1 $fscanf( infile, "%d", data_in ) ;
              else
                data_in <= 18'd0 ;
         end
      end
      
      // Print out the number of errors that occured
      if( noe )
        $display( "FAILED: %d errors during simulation", noe ) ;
      else
        $display( "PASSED: Simulation successful" ) ;
      
      $finish ;
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
                  //$display( "%t: %d != %d", $realtime, data_out, compare ) ;
                  noe = noe + 1 ;
               end
            end
         end else begin
            // Signal end of simulation when no more outputs
	    if($feof(infile))
              endofsim <= 1'b1 ;
         end
      end
   end     
   
endmodule // small_hb_int_tb
