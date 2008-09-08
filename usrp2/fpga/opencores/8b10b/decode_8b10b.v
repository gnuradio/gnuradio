// Chuck Benz, Hollis, NH   Copyright (c)2002
//
// The information and description contained herein is the
// property of Chuck Benz.
//
// Permission is granted for any reuse of this information
// and description as long as this copyright notice is
// preserved.  Modifications may be made as long as this
// notice is preserved.

// per Widmer and Franaszek

module decode_8b10b (datain, dispin, dataout, dispout, code_err, disp_err) ;
  input [9:0]   datain ;
  input		dispin ;
  output [8:0]	dataout ;
  output	dispout ;
  output	code_err ;
  output	disp_err ;

  wire ai = datain[0] ;
  wire bi = datain[1] ;
  wire ci = datain[2] ;
  wire di = datain[3] ;
  wire ei = datain[4] ;
  wire ii = datain[5] ;
  wire fi = datain[6] ;
  wire gi = datain[7] ;
  wire hi = datain[8] ;
  wire ji = datain[9] ;

  wire aeqb = (ai & bi) | (!ai & !bi) ;
  wire ceqd = (ci & di) | (!ci & !di) ;
  wire p22 = (ai & bi & !ci & !di) |
	     (ci & di & !ai & !bi) |
	     ( !aeqb & !ceqd) ;
  wire p13 = ( !aeqb & !ci & !di) |
	     ( !ceqd & !ai & !bi) ;
  wire p31 = ( !aeqb & ci & di) |
	     ( !ceqd & ai & bi) ;

  wire p40 = ai & bi & ci & di ;
  wire p04 = !ai & !bi & !ci & !di ;

  wire disp6a = p31 | (p22 & dispin) ; // pos disp if p22 and was pos, or p31.
   wire disp6a2 = p31 & dispin ;  // disp is ++ after 4 bits
   wire disp6a0 = p13 & ! dispin ; // -- disp after 4 bits
    
  wire disp6b = (((ei & ii & ! disp6a0) | (disp6a & (ei | ii)) | disp6a2 |
		  (ei & ii & di)) & (ei | ii | di)) ;

  // The 5B/6B decoding special cases where ABCDE != abcde

  wire p22bceeqi = p22 & bi & ci & (ei == ii) ;
  wire p22bncneeqi = p22 & !bi & !ci & (ei == ii) ;
  wire p13in = p13 & !ii ;
  wire p31i = p31 & ii ;
  wire p13dei = p13 & di & ei & ii ;
  wire p22aceeqi = p22 & ai & ci & (ei == ii) ;
  wire p22ancneeqi = p22 & !ai & !ci & (ei == ii) ;
  wire p13en = p13 & !ei ;
  wire anbnenin = !ai & !bi & !ei & !ii ;
  wire abei = ai & bi & ei & ii ;
  wire cdei = ci & di & ei & ii ;
  wire cndnenin = !ci & !di & !ei & !ii ;

  // non-zero disparity cases:
  wire p22enin = p22 & !ei & !ii ;
  wire p22ei = p22 & ei & ii ;
  //wire p13in = p12 & !ii ;
  //wire p31i = p31 & ii ;
  wire p31dnenin = p31 & !di & !ei & !ii ;
  //wire p13dei = p13 & di & ei & ii ;
  wire p31e = p31 & ei ;

  wire compa = p22bncneeqi | p31i | p13dei | p22ancneeqi | 
		p13en | abei | cndnenin ;
  wire compb = p22bceeqi | p31i | p13dei | p22aceeqi | 
		p13en | abei | cndnenin ;
  wire compc = p22bceeqi | p31i | p13dei | p22ancneeqi | 
		p13en | anbnenin | cndnenin ;
  wire compd = p22bncneeqi | p31i | p13dei | p22aceeqi |
		p13en | abei | cndnenin ;
  wire compe = p22bncneeqi | p13in | p13dei | p22ancneeqi | 
		p13en | anbnenin | cndnenin ;

  wire ao = ai ^ compa ;
  wire bo = bi ^ compb ;
  wire co = ci ^ compc ;
  wire do = di ^ compd ;
  wire eo = ei ^ compe ;

  wire feqg = (fi & gi) | (!fi & !gi) ;
  wire heqj = (hi & ji) | (!hi & !ji) ;
  wire fghj22 = (fi & gi & !hi & !ji) |
		(!fi & !gi & hi & ji) |
		( !feqg & !heqj) ;
  wire fghjp13 = ( !feqg & !hi & !ji) |
		 ( !heqj & !fi & !gi) ;
  wire fghjp31 = ( (!feqg) & hi & ji) |
		 ( !heqj & fi & gi) ;

  wire dispout = (fghjp31 | (disp6b & fghj22) | (hi & ji)) & (hi | ji) ;

  wire ko = ( (ci & di & ei & ii) | ( !ci & !di & !ei & !ii) |
		(p13 & !ei & ii & gi & hi & ji) |
		(p31 & ei & !ii & !gi & !hi & !ji)) ;

  wire alt7 =   (fi & !gi & !hi & // 1000 cases, where disp6b is 1
		 ((dispin & ci & di & !ei & !ii) | ko |
		  (dispin & !ci & di & !ei & !ii))) |
		(!fi & gi & hi & // 0111 cases, where disp6b is 0
		 (( !dispin & !ci & !di & ei & ii) | ko |
		  ( !dispin & ci & !di & ei & ii))) ;

  wire k28 = (ci & di & ei & ii) | ! (ci | di | ei | ii) ;
  // k28 with positive disp into fghi - .1, .2, .5, and .6 special cases
  wire k28p = ! (ci | di | ei | ii) ;
  wire fo = (ji & !fi & (hi | !gi | k28p)) |
	    (fi & !ji & (!hi | gi | !k28p)) |
	    (k28p & gi & hi) |
	    (!k28p & !gi & !hi) ;
  wire go = (ji & !fi & (hi | !gi | !k28p)) |
	    (fi & !ji & (!hi | gi |k28p)) |
	    (!k28p & gi & hi) |
	    (k28p & !gi & !hi) ;
  wire ho = ((ji ^ hi) & ! ((!fi & gi & !hi & ji & !k28p) | (!fi & gi & hi & !ji & k28p) | 
			    (fi & !gi & !hi & ji & !k28p) | (fi & !gi & hi & !ji & k28p))) |
	    (!fi & gi & hi & ji) | (fi & !gi & !hi & !ji) ;

  wire disp6p = (p31 & (ei | ii)) | (p22 & ei & ii) ;
  wire disp6n = (p13 & ! (ei & ii)) | (p22 & !ei & !ii) ;
  wire disp4p = fghjp31 ;
  wire disp4n = fghjp13 ;

  assign code_err = p40 | p04 | (fi & gi & hi & ji) | (!fi & !gi & !hi & !ji) |
		    (p13 & !ei & !ii) | (p31 & ei & ii) | 
		    (ei & ii & fi & gi & hi) | (!ei & !ii & !fi & !gi & !hi) | 
		    (ei & !ii & gi & hi & ji) | (!ei & ii & !gi & !hi & !ji) |
		    (!p31 & ei & !ii & !gi & !hi & !ji) |
		    (!p13 & !ei & ii & gi & hi & ji) |
		    (((ei & ii & !gi & !hi & !ji) | 
		      (!ei & !ii & gi & hi & ji)) &
		     ! ((ci & di & ei) | (!ci & !di & !ei))) |
		    (disp6p & disp4p) | (disp6n & disp4n) |
		    (ai & bi & ci & !ei & !ii & ((!fi & !gi) | fghjp13)) |
		    (!ai & !bi & !ci & ei & ii & ((fi & gi) | fghjp31)) |
		    (fi & gi & !hi & !ji & disp6p) |
		    (!fi & !gi & hi & ji & disp6n) |
		    (ci & di & ei & ii & !fi & !gi & !hi) |
		    (!ci & !di & !ei & !ii & fi & gi & hi) ;

  assign dataout = {ko, ho, go, fo, eo, do, co, bo, ao} ;

  // my disp err fires for any legal codes that violate disparity, may fire for illegal codes
   assign disp_err = ((dispin & disp6p) | (disp6n & !dispin) |
		      (dispin & !disp6n & fi & gi) |
		      (dispin & ai & bi & ci) |
		      (dispin & !disp6n & disp4p) |
		      (!dispin & !disp6p & !fi & !gi) |
		      (!dispin & !ai & !bi & !ci) |
		      (!dispin & !disp6p & disp4n) |
		      (disp6p & disp4p) | (disp6n & disp4n)) ;

endmodule
