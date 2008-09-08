`timescale  1ns /  10ps


//
//				CY7C1356 
//			Simulatiom of Verilog model 
//
//

//
//			test bench for US vector input
//
//

//	define speed 166MHz

`define tx10 #6 
`define tx08 #4.8
`define tx05 #3
`define tx04 #2.4
`define tx02 #1.2

/*
`define tx10 #4.0		//    period
`define tx08 #3.2		//0.8 period
`define tx05 #2.0		//0.5 period  250MHZ
`define tx04 #1.6		//0.4 period
`define tx02 #0.8		//0.2 period

`define tx10 #4.4		//    period
`define tx08 #3.52		//0.8 period
`define tx05 #2.2		//0.5 period  225MHZ
`define tx04 #1.76		//0.4 period
`define tx02 #0.88		//0.2 period

`define tx10 #5			//    period
`define tx08 #4			//0.8 period
`define tx05 #2.5		//0.5 period  200MHZ
`define tx04 #2.0		//0.4 period
`define tx02 #1.0		//0.2 period
*/


module rw_test;

`define num_vectors    126
`define	impi {a[15:0],io[15:0],tsti[15:0],cenb,ce1b,ce2,ce3b,bweb,bwb,adv_lb}


reg	[57:1]	lsim_vectors	[1:`num_vectors];


reg		clk;
reg		adv_lb;
reg		ce1b;		//cs1b
reg		ce2;		//cs2
reg		ce3b;		//cs3b
reg	[1:0]	bwb;
reg		bweb;
reg		oeb;
reg		ftb;
reg		mode;		//lbob
reg		cenb;		//zz
reg		tp42;		//sclk
reg		tp39;		//se
reg		tp38;		//tm
reg	[19:0]	a;
reg	[17:0]	io;
reg	[17:0]	tsti;
reg		vddq;
reg		vssqr;
reg		iosel;


wire	[17:0]	d = iosel ? io[17:0] : 18'bz;

reg 		noti3;
reg		strb,j;
integer		vector,i;



cy1356 testram ( d, clk, a, bwb, bweb, adv_lb, ce1b, ce2, ce3b, oeb, cenb, mode);

initial
begin
  $dumpfile("dumpfile.dump");
  $dumpvars(0,rw_test);
end

initial
begin
io	= 18'bz;
ftb 	= 1;
oeb 	= 0;
a[19:16] = 4'h0;
mode	= 0;
strb 	= 0;
tp38	= 0;
tp39	= 0;
tp42	= 0;
`tx02;
forever `tx05 strb = ~strb;
end

initial
begin
clk	= 0;
forever `tx05 clk =~clk;
end

initial
 begin
 
 $readmemb("cy1356.inp", lsim_vectors);     //load input vector file

 `impi = lsim_vectors[1];         //apply 1st test vector

 for (vector = 2; vector <= `num_vectors; vector = vector + 1)
   @(posedge strb)
    begin

 `impi = lsim_vectors[vector];
 

      
      io[16:13] = io[07:04];
      io[12:09] = io[07:04];
      io[07:04] = io[03:00];
      io[03:00] = io[03:00];
      
      io[17] = io[16] ^^ io[15] ^^ io[14] ^^ io[13] ^^ io[11] ^^ io[11] ^^ io[10] ^^ io[9];
      io[8] = io[7] ^^ io[6] ^^ io[5] ^^ io[4] ^^ io[3] ^^ io[2] ^^ io[1] ^^ io[0];

      tsti[16:13] = tsti[07:04];
      tsti[12:09] = tsti[07:04];
      tsti[07:04] = tsti[03:00];
      tsti[03:00] = tsti[03:00];
      
      tsti[17] = tsti[16] ^^ tsti[15] ^^ tsti[14] ^^ tsti[13] ^^ tsti[11] ^^ tsti[11] ^^ tsti[10] ^^ tsti[9];
      tsti[8] = tsti[7] ^^ tsti[6] ^^ tsti[5] ^^ tsti[4] ^^ tsti[3] ^^ tsti[2] ^^ tsti[1] ^^ tsti[0];

      if (io === 18'hxxxxx)
       iosel = `tx05 0;
      else
       iosel = `tx05 1;

    end
   #15 $finish; // This prevents simulation beyond end of test patterns
 end

always@(posedge clk)
begin
  
  if (io !== 18'hxxxxx) 	//input cycle
  begin
    $display("NOTICE      : 001 : line = %d OK",vector -1);
  end
  else				//do the test
  begin
    if (d == tsti)
    begin
    $display("NOTICE      : 002 : line = %d OK",vector -1);
    end
    else
    begin
      j =0;
      for (i =0;i< 18; i=i+1)
      begin
        if(tsti[i] !== 1'bx)
          begin
           if (d[i] !== tsti[i]) j = 1;
          end
	else
	  j = 0;
      end
      if (j) 
    $display("ERROR   *** : 003 : line = %d data = %b test = %b",vector -1,d,tsti);
      else
    $display("NOTICE      : 003 : line = %d OK",vector -1);
    end
  end
end

endmodule




