 
module cpld_model
  (input aux_clk, input start, input mode, input done,
   output dout, output reg sclk, output detached);

   reg [7:0] rom[0:65535];

   reg [15:0] addr;
   reg [7:0]  data;
   assign     dout = data[7];

   reg [2:0]  state, bitcnt;
   
   localparam IDLE = 3'd0;
   localparam READ = 3'd1;
   localparam BIT1 = 3'd2;
   localparam BIT2 = 3'd3;
   localparam DONE = 3'd4;
   localparam DETACHED = 3'd5;
   localparam ERROR = 3'd7;
   
   integer i; 
   reg [1023:0] ROMFile;
   
   initial begin
      for (i=0;i<65536;i=i+1) begin
	 rom[i] <= 32'h0;
      end       
      if ( !$value$plusargs( "rom=%s", ROMFile ) )
        begin
           $display( "Using default ROM file, 'flash.rom'" );
           ROMFile = "flash.rom";
        end
      else
	$display( "Using %s as ROM file.", ROMFile);
      
      #1 $readmemh( ROMFile,rom );     
   end

   initial addr = 16'd0;
   initial data = 8'd0;
   initial state = IDLE;
   initial bitcnt = 3'd0;
   initial sclk = 1'b0;
   
   always @(posedge aux_clk)
     case(state)
       IDLE :
	 if(start)
	   if(~mode)
	     state <= READ;
	   else
	     state <= ERROR;
       READ :
	 if(done)
	   state <= DONE;
       	 else
	   begin
	      data <= rom[addr];
	      addr <= addr + 1;
	      bitcnt <= 3'd0;
	      if(addr==16'hFFFF)
		state <= ERROR;
	      else
		state <= BIT1;
	   end // else: !if(start)
       BIT1 :
	 begin
	    sclk <= 1'b1;
	    state <= BIT2;
	 end
       BIT2 :
	 begin
	    sclk <= 1'b0;
	    data <= {data[6:0],1'b0};
	    bitcnt <= bitcnt + 1;
	    if(bitcnt==7)
	      state <= READ;
	    else
	      state <=BIT1;
	 end
       DONE :
	 begin
	    if(start)
	      state <= ERROR;
	    else
	      state <= DETACHED;
	 end
       DETACHED :
	 if(start)
	   state <= ERROR;
     endcase // case(state)
   
   assign detached = (state == DETACHED) || (state == IDLE);
   
endmodule // cpld_model
