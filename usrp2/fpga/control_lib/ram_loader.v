
// Adapted from VHDL code in spi_boot by Arnim Legauer
//  Added a full wishbone master interface (32-bit)

module ram_loader #(parameter AWIDTH=16, RAM_SIZE=16384)
  (input clk_i, input rst_i,
   // CPLD Interface
   input cfg_clk_i, input cfg_data_i,
   output start_o, output mode_o, output done_o,
   input detached_i,
   // Wishbone interface
   output wire [31:0] wb_dat_o,
   output reg [AWIDTH-1:0] wb_adr_o,
   output wb_stb_o,
   output wb_cyc_o,
   output reg [3:0] wb_sel_o,
   output reg wb_we_o,
   input wb_ack_i,
   output ram_loader_done_o);
   
   //  FSM to control start signal, clocked on main clock
   localparam FSM1_WAIT_DETACH = 2'b00;
   localparam FSM1_CHECK_NO_DONE = 2'b01;
   localparam FSM1_WAIT_DONE = 2'b10;
   
   reg [1:0]  start_fsm_q, start_fsm_s;
   reg 	      start_q, enable_q, start_s, enable_s;
   reg 	      done_q, done_s;
   
   always @(posedge clk_i or posedge rst_i)
     if(rst_i)
       begin
	  start_fsm_q <= FSM1_WAIT_DETACH;
	  start_q <= 1'b0;
	  enable_q <= 1'b0;
       end
     else
       begin
	  start_fsm_q <= start_fsm_s;
	  enable_q <= enable_s;
	  start_q <= start_s;
       end // else: !if(rst_i)
   
   always @*
     case(start_fsm_q)
       FSM1_WAIT_DETACH:
	 if(detached_i == 1'b1)
	   begin
	      start_fsm_s <= FSM1_CHECK_NO_DONE;
	      enable_s <= 1'b1;
	      start_s <= 1'b1;
	   end
	 else
	   begin
	      start_fsm_s <= FSM1_WAIT_DETACH;
	      enable_s <= enable_q;
	      start_s <= start_q;
	   end // else: !if(detached_i == 1'b1)
       FSM1_CHECK_NO_DONE:
	 if(~done_q)
	   begin
	      start_fsm_s  <= FSM1_WAIT_DONE;
	      enable_s <= enable_q;
	      start_s <= start_q;
	   end
	 else
	   begin
	      start_fsm_s  <= FSM1_CHECK_NO_DONE;
	      enable_s <= enable_q;
	      start_s <= start_q;
	   end // else: !if(~done_q)
       FSM1_WAIT_DONE:
	 if(done_q)
	   begin
	      start_fsm_s  <= FSM1_WAIT_DETACH;
	      enable_s <= 1'b0;
	      start_s <= 1'b0;
	   end
	 else
	   begin
	      start_fsm_s  <= FSM1_WAIT_DONE;
	      enable_s <= enable_q;
	      start_s <= start_q;
	   end // else: !if(done_q)
       default:
	 begin
	    start_fsm_s  <= FSM1_WAIT_DETACH;
	    enable_s <= enable_q;
	    start_s <= start_q;
	 end // else: !if(done_q)
     endcase // case(start_fsm_q)
   
   //  FSM running on data clock

   localparam FSM2_IDLE = 3'b000;
   localparam FSM2_WE_ON = 3'b001;
   localparam FSM2_WE_OFF = 3'b010;
   localparam FSM2_INC_ADDR1 = 3'b011;
   localparam FSM2_INC_ADDR2 = 3'b100;
   localparam FSM2_FINISHED = 3'b101;
   
   reg [AWIDTH-1:0] addr_q;
   reg [7:0] 	    shift_dat_q, ser_dat_q;
   reg [2:0] 	    bit_q, fsm_q, fsm_s;
   reg 		    bit_ovfl_q, ram_we_s, ram_we_q, mode_q, mode_s, inc_addr_s;
   
   always @(posedge cfg_clk_i or posedge rst_i)
     if(rst_i)
       begin
	  addr_q <= 0;
	  shift_dat_q <= 8'd0;
	  ser_dat_q <= 8'd0;
	  bit_q <= 3'd0;
	  bit_ovfl_q <= 1'b0;
	  fsm_q <= FSM2_IDLE;
	  ram_we_q <= 1'b0;
	  done_q <= 1'b0;
	  mode_q <= 1'b0;
       end
     else
       begin
	  if(inc_addr_s)
	    addr_q <= addr_q + 1;
	  if(enable_q)
	    begin
	       bit_q <= bit_q + 1;
	       bit_ovfl_q <= (bit_q == 3'd7);
	       shift_dat_q[0] <= cfg_data_i;
	       shift_dat_q[7:1] <= shift_dat_q[6:0];
	    end
	  if(bit_ovfl_q)
	    ser_dat_q <= shift_dat_q;

	  fsm_q <= fsm_s;

	  ram_we_q <= ram_we_s;

	  if(done_s)
	    done_q <= 1'b1;
	  mode_q <= mode_s;
       end // else: !if(rst_i)

   always @*
     begin
	inc_addr_s <= 1'b0;
	ram_we_s <= 1'b0;
	done_s <= 1'b0;
	fsm_s <= FSM2_IDLE;
	mode_s <= 1'b0;

	case(fsm_q)
	  FSM2_IDLE :
	    if(start_q)
	      if(bit_ovfl_q)
		fsm_s <= FSM2_WE_ON;
	  FSM2_WE_ON:
	    begin
	       ram_we_s <= 1'b1;
	       fsm_s <= FSM2_WE_OFF;
	    end
	  FSM2_WE_OFF:
	    begin
	       ram_we_s <= 1'b1;
	       fsm_s <= FSM2_INC_ADDR1;
	    end
	  FSM2_INC_ADDR1:
	    fsm_s <= FSM2_INC_ADDR2;
	  FSM2_INC_ADDR2:
	    if(addr_q == (RAM_SIZE-1))
	    //if(&addr_q)
	      begin
		 fsm_s <= FSM2_FINISHED;
		 done_s <= 1'b1;
		 mode_s <= 1'b1;
	      end
	    else
	      begin
		 inc_addr_s <= 1'b1;
		 fsm_s <= FSM2_IDLE;
	      end // else: !if(&addr_q)
	  FSM2_FINISHED:
	    begin
	       fsm_s <= FSM2_FINISHED;
	       mode_s <= 1'b1;
	    end
	endcase // case(fsm_q)
     end // always @ *
   
   assign start_o = start_q;
   assign mode_o = mode_q;
   assign done_o = start_q ? done_q : 1'b1;
   wire [AWIDTH-1:0] ram_addr = addr_q;
   wire [7:0] ram_data = ser_dat_q;
   assign ram_loader_done_o = (fsm_q == FSM2_FINISHED);
   
   // wishbone master, only writes
   reg [7:0] dat_holder;
   assign    wb_dat_o = {4{dat_holder}};
   assign    wb_stb_o = wb_we_o;
   assign    wb_cyc_o = wb_we_o;
   
   always @(posedge clk_i or posedge rst_i)
     if(rst_i)
       begin
	  dat_holder <= 8'd0;
	  wb_adr_o <= 0;
	  wb_sel_o <= 4'b0000;
	  wb_we_o <= 1'b0;
       end
     else if(ram_we_q)
       begin
	  dat_holder <= ram_data;
	  wb_adr_o <= ram_addr;
	  wb_we_o <= 1'b1;
	  case(ram_addr[1:0])   // Big Endian
	    2'b00 : wb_sel_o <= 4'b1000;
	    2'b01 : wb_sel_o <= 4'b0100;
	    2'b10 : wb_sel_o <= 4'b0010;
	    2'b11 : wb_sel_o <= 4'b0001;
	  endcase // case(ram_addr[1:0])
       end // if (ram_we_q)
     else if(wb_ack_i)
       wb_we_o <= 1'b0;
      
endmodule // ram_loader
