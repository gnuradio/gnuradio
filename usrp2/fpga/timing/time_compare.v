
// Top 32 bits are integer seconds, bottom 32 are clock ticks within a second

module time_compare
  (input [63:0] time_now,
   input [63:0] trigger_time,
   output now,
   output early,
   output late);
   
   wire    sec_match   = (time_now[63:32] == trigger_time[63:32]);
   wire    sec_late    = (time_now[63:32] > trigger_time[63:32]);

   wire    tick_match  = (time_now[31:0] == trigger_time[31:0]);
   wire    tick_late   = (time_now[31:0] > trigger_time[31:0]);
   
   assign now 	       = sec_match & tick_match;
   assign late 	       = sec_late | (sec_match & tick_late);
   assign early        = ~now & ~late;

endmodule // time_compare
