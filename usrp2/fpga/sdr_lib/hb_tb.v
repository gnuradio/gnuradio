
module hb_tb();

   localparam SWIDTH = 17;
   localparam CWIDTH = 18;
   localparam TWIDTH = 20;
   localparam ACC_WIDTH = 40;
   
   reg clk = 0, rst = 1;
   wire strobe_in, strobe_out;
   reg [SWIDTH-1:0] sample_in;
   wire signed [SWIDTH:0] sample_out;

   reg 	       set_stb;
   reg [7:0]   set_addr;
   reg [31:0]  set_data;

   localparam  DECIM = 3;
   
   initial $dumpfile("hb_tb.vcd");
   initial $dumpvars(0,hb_tb);

   always #5 clk <= ~clk;
   initial 
     begin
	@(posedge clk);
	@(negedge clk);
	rst <= 0;
     end

   reg [7:0] stb_counter;
   always @(posedge clk)
     if(rst)
       stb_counter <= 0;
     else
       if(stb_counter == 0)
	 stb_counter <= DECIM;
       else
	 stb_counter <= stb_counter - 1;
   assign    strobe_in = (stb_counter == 0);
   
   hb_decim #(.SWIDTH(SWIDTH),.CWIDTH(CWIDTH),
	      .TWIDTH(TWIDTH),.ACC_WIDTH(ACC_WIDTH)) hb_decim
     (.clk(clk), .rst(rst),
      .set_stb(set_stb), .set_addr(set_addr), .set_data(set_data),
      .sample_in(sample_in),
      .strobe_in(strobe_in),
      .sample_out(sample_out),
      .strobe_out(strobe_out)
      );

   initial
     begin : load_coeffs
	@(negedge rst);
	@(posedge clk);
	set_addr <= 124;   // load coeffs
	set_stb <= 1;
	set_data <= -18'd49;
	@(posedge clk);
	set_data <= 18'd165;
	@(posedge clk);
	set_data <= -18'd412;
	@(posedge clk);
	set_data <= 18'd873;
	@(posedge clk);
	set_data <= -18'd1681;
	@(posedge clk);
	set_data <= 18'd3135;
	@(posedge clk);
	set_data <= -18'd6282;
	@(posedge clk);
	set_data <= 18'd20628;
	@(posedge clk);
	set_addr <=125;  // load table
	// { stb_out, accum, load_accum, done, even_addr, odd_addr_a, odd_addr_b, coeff_addr }
	set_data <= {1'b1,1'b1,1'b0,1'b1,4'd15,4'd15,4'd0,4'd0}; // Phase 8
	@(posedge clk);
	set_data <= {1'b0,1'b1,1'b0,1'b0,4'd15,4'd14,4'd1,4'd1}; // Phase 7
	@(posedge clk);
	set_data <= {1'b0,1'b1,1'b0,1'b0,4'd15,4'd13,4'd2,4'd2}; // Phase 6
	@(posedge clk);
	set_data <= {1'b0,1'b1,1'b0,1'b0,4'd15,4'd12,4'd3,4'd3}; // Phase 5
	@(posedge clk);
	set_data <= {1'b0,1'b1,1'b0,1'b0,4'd15,4'd11,4'd4,4'd4}; // Phase 4
	@(posedge clk);
	set_data <= {1'b0,1'b1,1'b0,1'b0,4'd15,4'd10,4'd5,4'd5}; // Phase 3
	@(posedge clk);
	set_data <= {1'b0,1'b1,1'b0,1'b0,4'd15,4'd9,4'd6,4'd6};  // Phase 2
	@(posedge clk);
	set_data <= {1'b0,1'b0,1'b1,1'b0,4'd15,4'd8,4'd7,4'd7};   // Phase 1
	@(posedge clk);
	set_data <= {1'b0,1'b0,1'b0,1'b0,4'd15,4'd8,4'd7,4'd7};   // Phase 0
	@(posedge clk);
	set_stb <= 0;
     end // block: load_coeffs
   
   initial
     begin
	sample_in <= 0;
	repeat(40)
	  @(posedge strobe_in);
	$display("EVEN");
	sample_in <= 0;
	repeat(10)
	  @(posedge strobe_in);
	sample_in <= 1;
	@(posedge strobe_in);
	sample_in <= 0;
	repeat(40)
	  @(posedge strobe_in);
	sample_in <= 1;
	repeat(40)
	  @(posedge strobe_in);
	sample_in <= 0;
	repeat(60)
	  @(posedge strobe_in);
	sample_in <= 1;
	repeat(2)
	  @(posedge strobe_in);
	sample_in <= 0;
	repeat(60)
	  @(posedge strobe_in);
	$display("ODD");
	sample_in <= 0;
	repeat(10)
	  @(posedge strobe_in);
	sample_in <= 1;
	@(posedge strobe_in);
	sample_in <= 0;
	repeat(40)
	  @(posedge strobe_in);
	sample_in <= 1;
	repeat(40)
	  @(posedge strobe_in);
	sample_in <= 0;
	repeat(60)
	  @(posedge strobe_in);
	sample_in <= 1;
	repeat(2)
	  @(posedge strobe_in);
	sample_in <= 0;
	repeat(60)
	  @(posedge strobe_in);
	$finish;
     end

   always @(posedge clk)
     if(strobe_in)
       $display(sample_in);
   
      always @(posedge clk)
	if(strobe_out)
	  $display("\t",sample_out);

endmodule // hb_tb
