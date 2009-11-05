
// Heavily modified by M. Ettus, 2009, little original code remains
// Modified by M. Ettus, 2008 for 32 bit width

/////////////////////////////////////////////////////////////////////
////                                                             ////
////  OpenCores         Simple Programmable Interrupt Controller ////
////                                                             ////
////  Author: Richard Herveille                                  ////
////          richard@asics.ws                                   ////
////          www.asics.ws                                       ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2002 Richard Herveille                        ////
////                    richard@asics.ws                         ////
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
//
// This is a simple Programmable Interrupt Controller.
// The number of interrupts is depending on the databus size.
// There's one interrupt input per databit (i.e. 16 interrupts for a 16
// bit databus).
// All attached devices share the same CPU priority level.
//
//
//
// Registers:
//
// 0x00: EdgeEnable Register
//       bits 7:0 R/W  Edge Enable '1' = edge triggered interrupt source
//                                 '0' = level triggered interrupt source
// 0x01: PolarityRegister
//       bits 7:0 R/W Polarity     '1' = high level / rising edge
//                                 '0' = low level / falling edge
// 0x02: MaskRegister
//       bits 7:0 R/W Mask         '1' = interrupt masked (disabled)
//                                 '0' = interrupt not masked (enabled)
// 0x03: PendingRegister
//       bits 7:0 R/W Pending      '1' = interrupt pending
//                                 '0' = no interrupt pending
//
// A CPU interrupt is generated when an interrupt is pending and its
// MASK bit is cleared.
//
//
//
// HOWTO:
//
// Clearing pending interrupts:
// Writing a '1' to a bit in the interrupt pending register clears the
// interrupt. Make sure to clear the interrupt at the source before
// writing to the interrupt pending register. Otherwise the interrupt
// will be set again.
//
// Priority based interrupts:
// Upon reception of an interrupt, check the interrupt register and
// determine the highest priority interrupt. Mask all interrupts from the
// current level to the lowest level. This negates the interrupt line, and
// makes sure only interrupts with a higher level are triggered. After
// completion of the interrupt service routine, clear the interrupt source,
// the interrupt bit in the pending register, and restore the MASK register
// to it's previous state.
//
// Addapt the core for fewer interrupt sources:
// If less than 8 interrupt sources are required, than the 'is' parameter
// can be set to the amount of required interrupts. Interrupts are mapped
// starting at the LSBs. So only the 'is' LSBs per register are valid. All
// other bits (i.e. the 8-'is' MSBs) are set to zero '0'.
// Codesize is approximately linear to the amount of interrupts. I.e. using
// 4 instead of 8 interrupt sources reduces the size by approx. half.
//


module pic
  (input clk_i, input rst_i, input cyc_i, input stb_i,
   input [2:0] adr_i, 
   input we_i, 
   input [31:0] dat_i, 
   output reg [31:0] dat_o, 
   output reg ack_o, 
   output reg int_o,
   input [31:0] irq
   );

   reg [31:0] 	pol, edgen, pending, mask;   // register bank
   reg [31:0] 	lirq, dirq;                  // latched irqs, delayed latched irqs
   
   // latch interrupt inputs
   always @(posedge clk_i)
     lirq <=  irq;
   
   // generate delayed latched irqs
   always @(posedge clk_i)
     dirq <=  lirq;

   // generate actual triggers
   function trigger;
      input 	edgen, pol, lirq, dirq;
      reg 	edge_irq, level_irq;
      begin
	 edge_irq  = pol ? (lirq & ~dirq) : (dirq & ~lirq);
	 level_irq = pol ? lirq : ~lirq;
	 trigger = edgen ? edge_irq : level_irq;
      end
   endfunction
   
   reg  [31:0] irq_event;
   integer     n;
   always @(posedge clk_i)
     for(n = 0; n < 32; n = n+1)
       irq_event[n] <=  trigger(edgen[n], pol[n], lirq[n], dirq[n]);

   // generate wishbone register bank writes
   wire        wb_acc = cyc_i & stb_i;                   // WISHBONE access
   wire        wb_wr  = wb_acc & we_i;                   // WISHBONE write access

   always @(posedge clk_i)
     if (rst_i)
       begin
          pol   <=  0;              // clear polarity register
          edgen <=  0;              // clear edge enable register
          mask  <=  0;              // mask all interrupts
       end
     else if(wb_wr)                               // wishbone write cycle??
       case (adr_i) // synopsys full_case parallel_case
         3'd0 : edgen <=  dat_i;        // EDGE-ENABLE register
         3'd1 : pol   <=  dat_i;        // POLARITY register
         3'd2 : mask  <=  dat_i;        // MASK register
         3'd3 : ;                       // PENDING register is a special case (see below)
	 3'd4 : ;                       // Priority encoded live (pending & ~mask)
       endcase

   // pending register is a special case
   always @(posedge clk_i)
     if (rst_i)
       pending <=  0;            // clear all pending interrupts
     else if ( wb_wr & (adr_i == 3'd3) )
       pending <=  (pending & ~dat_i) | irq_event;
     else
       pending <=  pending | irq_event;

   wire [31:0] live_enc;
   priority_enc priority_enc ( .in(pending & ~mask), .out(live_enc) );
   
   always @(posedge clk_i)
      case (adr_i) // synopsys full_case parallel_case
        3'd0 : dat_o <= edgen;
        3'd1 : dat_o <= pol;
        3'd2 : dat_o <= mask;
        3'd3 : dat_o <= pending;
	3'd4 : dat_o <= live_enc;
      endcase

   always @(posedge clk_i)
     ack_o <=  wb_acc & !ack_o;

   always @(posedge clk_i)
     if(rst_i)
       int_o <=  0;
     else
       int_o <=  |(pending & ~mask);
   
endmodule
