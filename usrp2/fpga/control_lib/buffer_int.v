
// FIFO Interface to the 2K buffer RAMs
// Read port is read-acknowledge
// FIXME do we want to be able to interleave reads and writes?

module buffer_int
  #(parameter BUFF_NUM = 0)
    (// Control Interface
     input clk,
     input rst,
     input [31:0] ctrl_word,
     input go,
     output done,
     output error,
     output idle,
     
     // Buffer Interface
     output en_o,
     output we_o,
     output reg [8:0] addr_o,
     output [31:0] dat_to_buf,
     input [31:0] dat_from_buf,
     
     // Write FIFO Interface
     input [31:0] wr_dat_i,
     input wr_write_i,
     input wr_done_i,
     input wr_error_i,
     output reg wr_ready_o,
     output reg wr_full_o,
     
     // Read FIFO Interface
     output [31:0] rd_dat_o,
     input rd_read_i,
     input rd_done_i,
     input rd_error_i,
     output reg rd_sop_o,
     output reg rd_eop_o
     );
   
   reg [31:0] ctrl_reg;
   reg 	      go_reg;
   
   always @(posedge clk)
     go_reg <= go;
   
   always @(posedge clk)
     if(rst)
       ctrl_reg <= 0;
     else
       if(go & (ctrl_word[31:28] == BUFF_NUM))
	 ctrl_reg <= ctrl_word;
   
   wire [8:0] firstline = ctrl_reg[8:0];
   wire [8:0] lastline = ctrl_reg[17:9];
   wire [3:0] step = ctrl_reg[21:18];
   wire       read = ctrl_reg[22];
   wire       write = ctrl_reg[23];
   wire       clear = ctrl_reg[24];
   //wire [2:0] port = ctrl_reg[27:25];  // Ignored in this block
   //wire [3:0] buff_num = ctrl_reg[31:28];  // Ignored here ?
   
   assign     dat_to_buf = wr_dat_i;
   assign     rd_dat_o = dat_from_buf;
   
   localparam IDLE = 3'd0;
   localparam PRE_READ = 3'd1;
   localparam READING = 3'd2;
   localparam WRITING = 3'd3;
   localparam ERROR = 3'd4;
   localparam DONE = 3'd5;
   
   reg [2:0]  state;
   
   always @(posedge clk)
     if(rst)
       begin
	  state <= IDLE;
	  rd_sop_o <= 0;
	  rd_eop_o <= 0;
	  wr_ready_o <= 0;
	  wr_full_o <= 0;
       end
     else
       if(clear)
	 begin
	    state <= IDLE;
	    rd_sop_o <= 0;
	    rd_eop_o <= 0;
	    wr_ready_o <= 0;
	    wr_full_o <= 0;
	 end
       else 
	 case(state)
	   IDLE :
	     if(go_reg & read)
	       begin
		  addr_o <= firstline;
		  state <= PRE_READ;
	       end
	     else if(go_reg & write)
	       begin
		  addr_o <= firstline;
		  state <= WRITING;
		  wr_ready_o <= 1;
	       end
	   
	   PRE_READ :
	     begin
		state <= READING;
		addr_o <= addr_o + 1;
		rd_sop_o <= 1;
	     end
	   
	   READING :
	     if(rd_error_i)
	       state <= ERROR;
	     else if(rd_done_i)
	       state <= DONE;
	     else if(rd_read_i)
	       begin
		  rd_sop_o <= 0;
		  addr_o <= addr_o + 1;
		  if(addr_o == lastline)
		    rd_eop_o <= 1;
		  else
		    rd_eop_o <= 0;
		  if(rd_eop_o)
		    state <= DONE;
	       end
	   
	   WRITING :
	     begin
		if(wr_error_i)
		  begin
		     state <= ERROR;
		     wr_ready_o <= 0;
		  end
		else
		  begin
		     if(wr_write_i)
		       begin
			  wr_ready_o <= 0;
			  addr_o <= addr_o + 1;
			  if(addr_o == (lastline-1))
			    wr_full_o <= 1;
			  if(addr_o == lastline)
			    state <= DONE;
		       end
		     if(wr_done_i)
		       begin
			  state <= DONE;
			  wr_ready_o <= 0;
		       end
		  end // else: !if(wr_error_i)
	     end // case: WRITING

	   DONE :
	     begin
		rd_eop_o <= 0;
		rd_sop_o <= 0;
		wr_ready_o <= 0;
		wr_full_o <= 0;
	     end
	   
	 endcase // case(state)
   
   // FIXME ignores step for now

   assign     we_o = (state == WRITING) && wr_write_i;  // FIXME potential critical path
                   // IF this is a timing problem, we could always write when in this state
   assign     en_o = ~((state==READING)& ~rd_read_i);   // FIXME potential critical path
   
   assign     done = (state == DONE);
   assign     error = (state == ERROR);
   assign     idle = (state == IDLE);
endmodule // buffer_int

// Unused old code
   //assign     rd_empty_o = (state != READING); // && (state != PRE_READ);
   //assign     rd_empty_o = rd_empty_reg;         // timing fix?
   //assign     rd_ready_o = (state == READING);
   //assign     rd_ready_o = ~rd_empty_reg;        // timing fix?
   
   //wire       rd_en = (state == PRE_READ) || ((state == READING) && rd_read_i);
   //wire       wr_en = (state == WRITING) && wr_write_i;  // IF this is a timing problem, we could always enable when in this state
   //assign     en_o = rd_en | wr_en;   
   
   // assign     wr_full_o = (state != WRITING);
   // assign     wr_ready_o = (state == WRITING);
   
