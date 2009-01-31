


module giantfifo
  #(parameter WIDTH=36)
    (input clk, input rst,
     input [WIDTH-1:0] datain,
     output [WIDTH-1:0] dataout,
     input read,
     input write,
     input clear,
     output full,
     output empty,
     output [15:0] space,
     output [15:0] occupied,
     
     // External RAM
     inout [17:0] RAM_D,
     output reg [18:0] RAM_A,
     output RAM_CE1n,
     output RAM_CENn,
     output reg RAM_CLK,
     output reg RAM_WEn,
     output RAM_OEn,
     output RAM_LDn
     );

   wire [4:0] path1_occ, path2_space;
   wire [35:0] path1_dat, path2_dat;
   
   shortfifo #(.WIDTH(WIDTH)) sf1
     (.clk(clk),.rst(rst),.clear(clear),
      .datain(datain),.write(write),.full(full),
      .dataout(path1_dat),.read(path1_read),.empty(path1_empty),
      .space(),.occupied(path1_occ) );
   wire       path1_almost_empty = (path1_occ == 5'd1);
   
   shortfifo #(.WIDTH(WIDTH)) sf2
     (.clk(clk),.rst(rst),.clear(clear),
      .datain(path2_dat),.write(path2_write),.full(path2_full),
      .dataout(dataout),.read(read),.empty(empty),
      .space(path2_space),.occupied() );
   wire       path2_almost_full = (path2_space == 5'd1);
	
   assign     RAM_CE1n = 1'b0;
   assign     RAM_CENn = 1'b0;
   always @(clk)
     RAM_CLK <= #2 clk;
   assign     RAM_LDn = 1'b0;

   // State machine
   wire       write_now, read_now, idle, phase;
   reg 	      ram_full, ram_empty;
   
   reg [17:0] read_ptr, write_ptr;
   reg [2:0]  zbt_state;

   localparam ZBT_IDLE = 0;
   localparam ZBT_WRITE_UPPER = 2;
   localparam ZBT_WRITE_LOWER = 3;
   localparam ZBT_READ_UPPER = 4;
   localparam ZBT_READ_LOWER = 5;

   wire       can_write = ~ram_full & ~path1_empty;
   wire       can_write_chain = can_write & ~path1_almost_empty;

   wire       can_read = ~ram_empty & ~path2_full;
   wire       can_read_chain = can_read & ~path2_almost_full;
   
   assign     phase = zbt_state[0];

   reg [17:0] ram_occupied;
   wire       ram_almost_empty = (write_ptr == (read_ptr+1'b1));
   wire       ram_almost_full = ((write_ptr+1'b1) == read_ptr);

   always @(posedge clk)
     if(rst | clear)
       begin
	  zbt_state <= ZBT_IDLE;
	  write_ptr <= 0;
	  read_ptr <= 0;
	  ram_full <= 0;
	  ram_empty <= 1;
	  ram_occupied <= 0;
       end
     else
       case(zbt_state)
	 ZBT_IDLE : 
	   if(can_read) 
	     zbt_state <= ZBT_READ_UPPER;
	   else if(can_write)
	     zbt_state <= ZBT_WRITE_UPPER;
	 
	 ZBT_WRITE_UPPER : 
	   begin
	      zbt_state <= ZBT_WRITE_LOWER;
	      ram_occupied <= ram_occupied + 1;
	      ram_empty <= 0;
	      if(ram_occupied == 18'd10)
		ram_full <= 1;
	   end
	 ZBT_WRITE_LOWER : 
	   begin
	      write_ptr <= write_ptr + 1;
	      if(can_read_chain) 
		zbt_state <= ZBT_READ_UPPER;
	      else if(can_write_chain)
		zbt_state <= ZBT_WRITE_UPPER;
	      else
		zbt_state <= ZBT_IDLE;
	   end
	 ZBT_READ_UPPER : 
	   begin
	      zbt_state <= ZBT_READ_LOWER;
	      ram_occupied <= ram_occupied - 1;
	      ram_full <= 0;
	      if(ram_occupied == 18'd1)
		ram_empty <= 1;
	   end
	 ZBT_READ_LOWER :
	   begin
	      read_ptr <= read_ptr + 1;
	      if(can_read_chain) 
		zbt_state <= ZBT_READ_UPPER;
	      else if(can_write_chain)
		zbt_state <= ZBT_WRITE_UPPER;
	      else
		zbt_state <= ZBT_IDLE;
	   end
	 default :
	   zbt_state <= ZBT_IDLE;
       endcase // case(zbt_state)

   // Need to generate RAM_WEn, RAM_OEn, RAM_D, RAM_A;
   assign path1_read = (zbt_state == ZBT_WRITE_LOWER);
   reg 	  path2_write, delayed_read_upper, delayed_read_lower, delayed_write;

   always @(posedge clk)
     if(delayed_read_upper)
       path2_dat[35:18] <= RAM_D;
   always @(posedge clk)
     if(delayed_read_lower)
       path2_dat[17:0] <= RAM_D;

   always @(posedge clk)
     if(rst)
       begin
	  delayed_read_upper <= 0;
	  delayed_read_lower <= 0;
	  path2_write <= 0;
       end
     else 
       begin
	  delayed_read_upper <= (zbt_state == ZBT_READ_LOWER);
	  delayed_read_lower <= delayed_read_upper;
	  path2_write <= delayed_read_lower;
       end
   
   reg [17:0] RAM_D_pre2, RAM_D_pre1, RAM_D_out;
   
   always @(posedge clk)
     RAM_D_pre2 <= phase ? path1_dat[17:0] : path1_dat[35:18];

   always @(posedge clk)  RAM_D_pre1 <= RAM_D_pre2;
   always @(posedge clk)  RAM_D_out <= RAM_D_pre1;
   reg 	      wr_del_1, wr_del_2; 	      
   always @(posedge clk)
     if(rst)
       begin
	  wr_del_1 <= 0;	  
	  wr_del_2 <= 0;
	  delayed_write <= 0;
       end
     else
       begin
	  delayed_write <= wr_del_2;
	  wr_del_2 <= wr_del_1;
	  wr_del_1 <= write_now;
       end

   reg delayed_read, rd_del_1, rd_del_2;
   always @(posedge clk)
     if(rst)
       begin
	  rd_del_1 <= 0;	  
	  rd_del_2 <= 0;
	  delayed_read <= 0;
       end
     else
       begin
	  delayed_read <= rd_del_2;
	  rd_del_2 <= rd_del_1;
	  rd_del_1 <= read_now;
       end
	  
   assign     RAM_D = delayed_write ? RAM_D_out : 18'bzzzzzzzzzzzzzzzzzz;
   assign     write_now = (zbt_state == ZBT_WRITE_UPPER) || (zbt_state == ZBT_WRITE_LOWER);
   assign     read_now =  (zbt_state == ZBT_READ_UPPER) || (zbt_state == ZBT_READ_LOWER);
   
   always @(posedge clk)
     RAM_A <= write_now ? {write_ptr,phase} : {read_ptr,phase};

   always @(posedge clk)
     RAM_WEn <= ~write_now;

   assign     RAM_OEn = ~delayed_read;
   assign     RAM_OEn = 0;
   
endmodule // giantfifo
