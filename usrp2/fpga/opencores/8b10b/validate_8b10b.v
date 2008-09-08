// Chuck Benz, Hollis, NH   Copyright (c)2002
//
// The information and description contained herein is the
// property of Chuck Benz.
//
// Permission is granted for any reuse of this information
// and description as long as this copyright notice is
// preserved.  Modifications may be made as long as this
// notice is preserved.

// 11-OCT-2002: updated with clearer messages, and checking decodeout

`timescale 1ns / 1ns
module test_8b10b ;
   reg [29:0]	code8b10b [0:267] ;
   reg [8:0] 	testin ;
   reg 		dispin ;
   reg [10:0] 	i ;
   wire [9:0] 	testout ;
   wire 	dispout, decodedisp, decodeerr, disperr ;
   wire [8:0] 	decodeout ;
   // My data file is 30 columns. Column 1 becomes [29], 2 becomes [28], etc..
   // code[0] is last Column (30)
   // First column, [29] is K indication
   // columns 2:9, [28:21], are data byte, aka 'm' and 'n' of Dm.n
   // columns 10:19, [20:11] are 10b symbol if starting disparity was negative, 0
   // columns 20:29, [10:1] are 10b symbol if starting disparity was positive, 1
   // column 30, [0], is a 1 if symbol results in a disparity flip
   //   0 for a balanced symbol (5 '1's, 5 '0's).

   wire [29:0] 	code = code8b10b[i] ;
   wire [9:0] 	expect_0_disp = {code[11], code[12], code[13], code[14], code[15],
				 code[16], code[17], code[18], code[19], code[20]} ;
   wire [9:0] 	expect_1_disp = {code[1], code[2], code[3], code[4], code[5],
				 code[6], code[7], code[8], code[9], code[10]} ;

   reg [1023:0] legal ;  // mark every used 10b symbol as legal, leave rest marked as not
   reg [2047:0] okdisp ; // now mark every used combination of symbol and starting disparity
   reg [8:0] 	mapcode [1023:0] ;
   reg [10:0] 	codedisp0, codedisp1 ;
   reg [9:0] 	decodein ;
   reg 		decdispin ;
   integer 	errors ;

   encode_8b10b DUTE (testin, dispin, testout, dispout) ;
   decode_8b10b DUTD (decodein, decdispin, decodeout, decodedisp, decodeerr, disperr) ;

   always @ (code) testin = code[29:21] ;
   
   initial begin 
      errors = 0 ;
      $readmemb ("8b10b_a.mem", code8b10b) ;
      //$vcdpluson ;
      $dumpvars (0);
      $display ("\n\nFirst, test by trying all 268 (256 Dx.y and 12 Kx.y)") ;
      $display ("valid inputs, with both + and - starting disparity.");
      $display ("We check that the encoder output and ending disparity is correct.");
      $display ("We also check that the decoder matches.");
      for (i = 0 ; i < 268 ; i = i + 1) begin
	 // testin = code[29:21] ;
	 dispin = 0 ;
	 #1
	   decodein = testout ;
	 decdispin = dispin ;
	 #1
//	   $display ("%b %b %b %b *%b*", dispin, testin, testout, {dispout, DUTD.disp6a, DUTD.disp6a2, DUTD.disp6a0, DUTD.disp6a2}, decodeout,, decodedisp,, DUTD.k28,, DUTD.disp6b) ;
	 if (testout != expect_0_disp) 
	   $display ("bad code0 %b %b %b %b %b", dispin, testin,  dispout, testout, expect_0_disp) ;
	 if (dispout != (dispin ^ code[0]))
	   $display ("bad disp0 %b %b %b %b %b", dispin, testin, dispout, testout, (dispin ^ code[0])) ;
	 if (0 != (9'b1_1111_1111 & (testin ^ decodeout)))
	   $display ("diff in abcdefghk decode, %b %b %b %b %b", dispin, testin,  dispout, testout, decodeout) ;
	 if (decodedisp != dispout)
	   $display ("diff in decoder disp out, %b %b %b %b %b", dispin, testin,  dispout, testout, decodeout) ;
	 if (decodeerr) $display ("decode error asserted improperly, %b %b %b %b %b", dispin, testin,  dispout, testout, decodeout) ;
	 if ((testout != expect_0_disp) | decodeerr |
	     (dispout != (dispin ^ code[0])) | (decodedisp != dispout))
	   errors = errors + 1 ;

	 dispin = 1 ;
	 #1
	 decodein = testout ;
	 decdispin = dispin ;
	 #1
//	   $display ("%b %b %b %b *%b*", dispin, testin, testout, {dispout, DUTD.disp6a, DUTD.disp6a2, DUTD.disp6a0, DUTD.disp6a2, DUTD.fghjp31, DUTD.feqg, DUTD.heqj, DUTD.fghj22, DUTD.fi, DUTD.gi, DUTD.hi, DUTD.ji, DUTD.dispout}, decodeout,, decodedisp,, DUTD.k28,, DUTD.disp6b) ;
	 if (testout != expect_1_disp) 
	   $display ("bad code1 %b %b %b %b %b", dispin, testin, dispout, testout, expect_1_disp) ;
	 if (dispout != (dispin ^ code[0]))
	   $display ("bad disp1 %b %b %b %b %b", dispin, testin,  dispout, testout, (dispin ^ code[0])) ;
	 if (0 != (9'b1_1111_1111 & (testin ^ decodeout)))
	   $display ("diff in abcdefghk decode, %b %b %b %b %b", dispin, testin,  dispout, testout, decodeout) ;
	 if (decodedisp != dispout)
	   $display ("diff in decoder disp out, %b %b %b %b %b", dispin, testin,  dispout, testout, decodeout) ;
	 if (decodeerr) $display ("decode error asserted improperly, %b %b %b %b %b", dispin, testin,  dispout, testout, decodeout) ;
	 if ((testout != expect_1_disp) | decodeerr |
	     (dispout != (dispin ^ code[0])) | (decodedisp != dispout))
	   errors = errors + 1 ;
      end
      $display ("%d errors in that testing.\n", errors) ;

      // Now, having verified all legal codes, lets run some illegal codes
      // at the decoder... how to figure illegal codes ?  2048 possible cases,
      // lets mark the OK ones...
      legal = 0 ;
      okdisp = 0 ;
      for (i = 0 ; i < 268 ; i = i + 1) begin
	 #1
//	   $display ("i=%d: %b %b %d %d %x %x", i, expect_0_disp, expect_1_disp, expect_0_disp, expect_1_disp, expect_0_disp, expect_1_disp) ;
	 legal[expect_0_disp] = 1 ;
	 legal[expect_1_disp] = 1 ;
	 codedisp0 = expect_0_disp ;
	 codedisp1 = {1'b1, expect_1_disp} ;
	 okdisp[codedisp0] = 1 ;
	 okdisp[codedisp1] = 1 ;
	 mapcode[expect_0_disp] = code[29:21] ;
	 mapcode[expect_1_disp] = code[29:21] ;
      end

      $display ("Now lets test all (legal and illegal) codes into the decoder.");
      $display ("checking all possible decode inputs") ;
      for (i = 0 ; i < 1024 ; i = i + 1) begin
	 decodein = i ;
	 decdispin = 0 ;
	 codedisp1 = 1024 | i ;
	 #1
	 if (((legal[i] == 0) & (decodeerr != 1)) |
	     (legal[i] & (mapcode[i] != decodeout)) |
	     (legal[i] & (disperr != !okdisp[i])))
	   $display ("10b:%b start disp:%b 8b:%b end disp:%b codevio:%b dispvio:%b known code:%b used disp:", 
		     decodein, decdispin, decodeout, decodedisp, decodeerr, disperr, legal[i], okdisp[i]) ;
	 if ((legal[i] == 0) & (decodeerr != 1)) $display ("ERR: decoderr should be 1") ;
	 if (legal[i] & (mapcode[i] != decodeout)) $display ("ERR: decode output incorrect") ;
	 if (legal[i] & (disperr != 1) & !okdisp[i]) $display ("ERR: disp err should be asserted") ;
	 else if (legal[i] & (disperr != 0) & okdisp[i])
	   $display ("ERR: disp err should not be asserted") ;

	 if (((legal[i] == 0) & (decodeerr != 1)) |
	     (legal[i] & !disperr & !okdisp[i]) |
	     (legal[i] & (mapcode[i] != decodeout)) |
	     (legal[i] & disperr & okdisp[i]))
	   errors = errors + 1 ;

	 decdispin = 1 ;
	 #1
	 if (((legal[i] == 0) & (decodeerr != 1)) |
	     (legal[i] & (mapcode[i] != decodeout)) |
	     (legal[i] & (disperr != !okdisp[i|1024])))
	   $display ("10b:%b start disp:%b 8b:%b end disp:%b codevio:%b dispvio:%b known code:%b used disp:", 
		     decodein, decdispin, decodeout, decodedisp, decodeerr, disperr, legal[i], okdisp[i|1024]) ;
	 if ((legal[i] == 0) & (decodeerr != 1)) $display ("ERR: decoderr should be 1") ;
	 if (legal[i] & (mapcode[i] != decodeout)) $display ("ERR: decode output incorrect") ;
	 if (legal[i] & (disperr != 1) & !okdisp[i|1024]) $display ("ERR: disp err should be asserted") ;
	 else if (legal[i] & (disperr != 0) & okdisp[i|1024])
	   $display ("ERR: disp err should not be asserted") ;
	 if (((legal[i] == 0) & (decodeerr != 1)) |
	     (legal[i] & !disperr & !okdisp[i|1024]) |
	     (legal[i] & (mapcode[i] != decodeout)) |
	     (legal[i] & disperr & okdisp[i|1024]))
	   errors = errors + 1 ;
      end // for (i = 0 ; i < 1024 ; i = i + 1)

      $display ("\nDone testing decoder.\n") ;
      $display ("Total error count: %d", errors);
      if (errors == 0) $display ("Congratulations!\n");
      $finish ;
   end // initial begin
   
endmodule
