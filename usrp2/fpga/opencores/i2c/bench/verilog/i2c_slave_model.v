/////////////////////////////////////////////////////////////////////
////                                                             ////
////  WISHBONE rev.B2 compliant synthesizable I2C Slave model    ////
////                                                             ////
////                                                             ////
////  Authors: Richard Herveille (richard@asics.ws) www.asics.ws ////
////           John Sheahan (jrsheahan@optushome.com.au)         ////
////                                                             ////
////  Downloaded from: http://www.opencores.org/projects/i2c/    ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2001,2002 Richard Herveille                   ////
////                         richard@asics.ws                    ////
////                                                             ////
//// This source file may be used and distributed without        ////
//// restriction provided that this copyright statement is not   ////
//// removed from the file and that any derivative work contains ////
//// the original copyright notice and the associated disclaimer.////
////                                                             ////
////     THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY     ////
//// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   ////
//// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS   ////
//// FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR      ////
//// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         ////
//// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    ////
//// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE   ////
//// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        ////
//// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  ////
//// LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT  ////
//// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  ////
//// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         ////
//// POSSIBILITY OF SUCH DAMAGE.                                 ////
////                                                             ////
/////////////////////////////////////////////////////////////////////

//  CVS Log
//
//  $Id: i2c_slave_model.v,v 1.7 2006/09/04 09:08:51 rherveille Exp $
//
//  $Date: 2006/09/04 09:08:51 $
//  $Revision: 1.7 $
//  $Author: rherveille $
//  $Locker:  $
//  $State: Exp $
//
// Change History:
//               $Log: i2c_slave_model.v,v $
//               Revision 1.7  2006/09/04 09:08:51  rherveille
//               fixed (n)ack generation
//
//               Revision 1.6  2005/02/28 11:33:48  rherveille
//               Fixed Tsu:sta timing check.
//               Added Thd:sta timing check.
//
//               Revision 1.5  2003/12/05 11:05:19  rherveille
//               Fixed slave address MSB='1' bug
//
//               Revision 1.4  2003/09/11 08:25:37  rherveille
//               Fixed a bug in the timing section. Changed 'tst_scl' into 'tst_sto'.
//
//               Revision 1.3  2002/10/30 18:11:06  rherveille
//               Added timing tests to i2c_model.
//               Updated testbench.
//
//               Revision 1.2  2002/03/17 10:26:38  rherveille
//               Fixed some race conditions in the i2c-slave model.
//               Added debug information.
//               Added headers.
//

`include "timescale.v"

module i2c_slave_model (scl, sda);

	//
	// parameters
	//
	parameter I2C_ADR = 7'b001_0000;

	//
	// input && outpus
	//
	input scl;
	inout sda;

	//
	// Variable declaration
	//
	wire debug = 1'b1;

	reg [7:0] mem [3:0]; // initiate memory
	reg [7:0] mem_adr;   // memory address
	reg [7:0] mem_do;    // memory data output

	reg sta, d_sta;
	reg sto, d_sto;

	reg [7:0] sr;        // 8bit shift register
	reg       rw;        // read/write direction

	wire      my_adr;    // my address called ??
	wire      i2c_reset; // i2c-statemachine reset
	reg [2:0] bit_cnt;   // 3bit downcounter
	wire      acc_done;  // 8bits transfered
	reg       ld;        // load downcounter

	reg       sda_o;     // sda-drive level
	wire      sda_dly;   // delayed version of sda

	// statemachine declaration
	parameter idle        = 3'b000;
	parameter slave_ack   = 3'b001;
	parameter get_mem_adr = 3'b010;
	parameter gma_ack     = 3'b011;
	parameter data        = 3'b100;
	parameter data_ack    = 3'b101;

	reg [2:0] state; // synopsys enum_state

	//
	// module body
	//

	initial
	begin
	   sda_o = 1'b1;
	   state = idle;
	end

	// generate shift register
	always @(posedge scl)
	  sr <= #1 {sr[6:0],sda};

	//detect my_address
	assign my_adr = (sr[7:1] == I2C_ADR);
	// FIXME: This should not be a generic assign, but rather
	// qualified on address transfer phase and probably reset by stop

	//generate bit-counter
	always @(posedge scl)
	  if(ld)
	    bit_cnt <= #1 3'b111;
	  else
	    bit_cnt <= #1 bit_cnt - 3'h1;

	//generate access done signal
	assign acc_done = !(|bit_cnt);

	// generate delayed version of sda
	// this model assumes a hold time for sda after the falling edge of scl.
	// According to the Phillips i2c spec, there s/b a 0 ns hold time for sda
	// with regards to scl. If the data changes coincident with the clock, the
	// acknowledge is missed
	// Fix by Michael Sosnoski
	assign #1 sda_dly = sda;


	//detect start condition
	always @(negedge sda)
	  if(scl)
	    begin
	        sta   <= #1 1'b1;
		d_sta <= #1 1'b0;
		sto   <= #1 1'b0;

	        if(debug)
	          $display("DEBUG i2c_slave; start condition detected at %t", $time);
	    end
	  else
	    sta <= #1 1'b0;

	always @(posedge scl)
	  d_sta <= #1 sta;

	// detect stop condition
	always @(posedge sda)
	  if(scl)
	    begin
	       sta <= #1 1'b0;
	       sto <= #1 1'b1;

	       if(debug)
	         $display("DEBUG i2c_slave; stop condition detected at %t", $time);
	    end
	  else
	    sto <= #1 1'b0;

	//generate i2c_reset signal
	assign i2c_reset = sta || sto;

	// generate statemachine
	always @(negedge scl or posedge sto)
	  if (sto || (sta && !d_sta) )
	    begin
	        state <= #1 idle; // reset statemachine

	        sda_o <= #1 1'b1;
	        ld    <= #1 1'b1;
	    end
	  else
	    begin
	        // initial settings
	        sda_o <= #1 1'b1;
	        ld    <= #1 1'b0;

	        case(state) // synopsys full_case parallel_case
	            idle: // idle state
	              if (acc_done && my_adr)
	                begin
	                    state <= #1 slave_ack;
	                    rw <= #1 sr[0];
	                    sda_o <= #1 1'b0; // generate i2c_ack

	                    #2;
	                    if(debug && rw)
	                      $display("DEBUG i2c_slave; command byte received (read) at %t", $time);
	                    if(debug && !rw)
	                      $display("DEBUG i2c_slave; command byte received (write) at %t", $time);

	                    if(rw)
	                      begin
	                          mem_do <= #1 mem[mem_adr];

	                          if(debug)
	                            begin
	                                #2 $display("DEBUG i2c_slave; data block read %x from address %x (1)", mem_do, mem_adr);
	                                #2 $display("DEBUG i2c_slave; memcheck [0]=%x, [1]=%x, [2]=%x", mem[4'h0], mem[4'h1], mem[4'h2]);
	                            end
	                      end
	                end

	            slave_ack:
	              begin
	                  if(rw)
	                    begin
	                        state <= #1 data;
	                        sda_o <= #1 mem_do[7];
	                    end
	                  else
	                    state <= #1 get_mem_adr;

	                  ld    <= #1 1'b1;
	              end

	            get_mem_adr: // wait for memory address
	              if(acc_done)
	                begin
	                    state <= #1 gma_ack;
	                    mem_adr <= #1 sr; // store memory address
	                    sda_o <= #1 !(sr <= 15); // generate i2c_ack, for valid address

	                    if(debug)
	                      #1 $display("DEBUG i2c_slave; address received. adr=%x, ack=%b", sr, sda_o);
	                end

	            gma_ack:
	              begin
	                  state <= #1 data;
	                  ld    <= #1 1'b1;
	              end

	            data: // receive or drive data
	              begin
	                  if(rw)
	                    sda_o <= #1 mem_do[7];

	                  if(acc_done)
	                    begin
	                        state <= #1 data_ack;
	                        mem_adr <= #2 mem_adr + 8'h1;
	                        sda_o <= #1 (rw && (mem_adr <= 15) ); // send ack on write, receive ack on read

	                        if(rw)
	                          begin
	                              #3 mem_do <= mem[mem_adr];

	                              if(debug)
	                                #5 $display("DEBUG i2c_slave; data block read %x from address %x (2)", mem_do, mem_adr);
	                          end

	                        if(!rw)
	                          begin
	                              mem[ mem_adr[3:0] ] <= #1 sr; // store data in memory

	                              if(debug)
	                                #2 $display("DEBUG i2c_slave; data block write %x to address %x", sr, mem_adr);
	                          end
	                    end
	              end

	            data_ack:
	              begin
	                  ld <= #1 1'b1;

	                  if(rw)
	                    if(sr[0]) // read operation && master send NACK
	                      begin
	                          state <= #1 idle;
	                          sda_o <= #1 1'b1;
	                      end
	                    else
	                      begin
	                          state <= #1 data;
	                          sda_o <= #1 mem_do[7];
	                      end
	                  else
	                    begin
	                        state <= #1 data;
	                        sda_o <= #1 1'b1;
	                    end
	              end

	        endcase
	    end

	// read data from memory
	always @(posedge scl)
	  if(!acc_done && rw)
	    mem_do <= #1 {mem_do[6:0], 1'b1}; // insert 1'b1 for host ack generation

	// generate tri-states
	assign sda = sda_o ? 1'bz : 1'b0;


	//
	// Timing checks
	//

	wire tst_sto = sto;
	wire tst_sta = sta;

	specify
	  specparam normal_scl_low  = 4700,
	            normal_scl_high = 4000,
	            normal_tsu_sta  = 4700,
	            normal_thd_sta  = 4000,
	            normal_tsu_sto  = 4000,
	            normal_tbuf     = 4700,

	            fast_scl_low  = 1300,
	            fast_scl_high =  600,
	            fast_tsu_sta  = 1300,
	            fast_thd_sta  =  600,
	            fast_tsu_sto  =  600,
	            fast_tbuf     = 1300;

	  $width(negedge scl, normal_scl_low);  // scl low time
	  $width(posedge scl, normal_scl_high); // scl high time

	  $setup(posedge scl, negedge sda &&& scl, normal_tsu_sta); // setup start
	  $setup(negedge sda &&& scl, negedge scl, normal_thd_sta); // hold start
	  $setup(posedge scl, posedge sda &&& scl, normal_tsu_sto); // setup stop

	  $setup(posedge tst_sta, posedge tst_sto, normal_tbuf); // stop to start time
	endspecify

endmodule


