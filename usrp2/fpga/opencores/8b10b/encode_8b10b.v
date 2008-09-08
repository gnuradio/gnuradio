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

module encode_8b10b (datain, dispin, dataout, dispout) ;
  input [8:0]   datain ;
  input 	dispin ;  // 0 = neg disp; 1 = pos disp
  output [9:0]	dataout ;
  output	dispout ;


  wire ai = datain[0] ;
  wire bi = datain[1] ;
  wire ci = datain[2] ;
  wire di = datain[3] ;
  wire ei = datain[4] ;
  wire fi = datain[5] ;
  wire gi = datain[6] ;
  wire hi = datain[7] ;
  wire ki = datain[8] ;

  wire aeqb = (ai & bi) | (!ai & !bi) ;
  wire ceqd = (ci & di) | (!ci & !di) ;
  wire l22 = (ai & bi & !ci & !di) |
	     (ci & di & !ai & !bi) |
	     ( !aeqb & !ceqd) ;
  wire l40 = ai & bi & ci & di ;
  wire l04 = !ai & !bi & !ci & !di ;
  wire l13 = ( !aeqb & !ci & !di) |
	     ( !ceqd & !ai & !bi) ;
  wire l31 = ( !aeqb & ci & di) |
	     ( !ceqd & ai & bi) ;

  // The 5B/6B encoding

  wire ao = ai ;
  wire bo = (bi & !l40) | l04 ;
  wire co = l04 | ci | (ei & di & !ci & !bi & !ai) ;
  wire do = di & ! (ai & bi & ci) ;
  wire eo = (ei | l13) & ! (ei & di & !ci & !bi & !ai) ;
  wire io = (l22 & !ei) |
	    (ei & !di & !ci & !(ai&bi)) |  // D16, D17, D18
	    (ei & l40) |
	    (ki & ei & di & ci & !bi & !ai) | // K.28
	    (ei & !di & ci & !bi & !ai) ;

  // pds16 indicates cases where d-1 is assumed + to get our encoded value
  wire pd1s6 = (ei & di & !ci & !bi & !ai) | (!ei & !l22 & !l31) ;
  // nds16 indicates cases where d-1 is assumed - to get our encoded value
  wire nd1s6 = ki | (ei & !l22 & !l13) | (!ei & !di & ci & bi & ai) ;

  // ndos6 is pds16 cases where d-1 is + yields - disp out - all of them
  wire ndos6 = pd1s6 ;
  // pdos6 is nds16 cases where d-1 is - yields + disp out - all but one
  wire pdos6 = ki | (ei & !l22 & !l13) ;


  // some Dx.7 and all Kx.7 cases result in run length of 5 case unless
  // an alternate coding is used (referred to as Dx.A7, normal is Dx.P7)
  // specifically, D11, D13, D14, D17, D18, D19.
  wire alt7 = fi & gi & hi & (ki | 
			      (dispin ? (!ei & di & l31) : (ei & !di & l13))) ;

   
  wire fo = fi & ! alt7 ;
  wire go = gi | (!fi & !gi & !hi) ;
  wire ho = hi ;
  wire jo = (!hi & (gi ^ fi)) | alt7 ;

  // nd1s4 is cases where d-1 is assumed - to get our encoded value
  wire nd1s4 = fi & gi ;
  // pd1s4 is cases where d-1 is assumed + to get our encoded value
  wire pd1s4 = (!fi & !gi) | (ki & ((fi & !gi) | (!fi & gi))) ;

  // ndos4 is pd1s4 cases where d-1 is + yields - disp out - just some
  wire ndos4 = (!fi & !gi) ;
  // pdos4 is nd1s4 cases where d-1 is - yields + disp out 
  wire pdos4 = fi & gi & hi ;

  // only legal K codes are K28.0->.7, K23/27/29/30.7
  //	K28.0->7 is ei=di=ci=1,bi=ai=0
  //	K23 is 10111
  //	K27 is 11011
  //	K29 is 11101
  //	K30 is 11110 - so K23/27/29/30 are ei & l31
  wire illegalk = ki & 
		  (ai | bi | !ci | !di | !ei) & // not K28.0->7
		  (!fi | !gi | !hi | !ei | !l31) ; // not K23/27/29/30.7

  // now determine whether to do the complementing
  // complement if prev disp is - and pd1s6 is set, or + and nd1s6 is set
  wire compls6 = (pd1s6 & !dispin) | (nd1s6 & dispin) ;

  // disparity out of 5b6b is disp in with pdso6 and ndso6
  // pds16 indicates cases where d-1 is assumed + to get our encoded value
  // ndos6 is cases where d-1 is + yields - disp out
  // nds16 indicates cases where d-1 is assumed - to get our encoded value
  // pdos6 is cases where d-1 is - yields + disp out
  // disp toggles in all ndis16 cases, and all but that 1 nds16 case

  wire disp6 = dispin ^ (ndos6 | pdos6) ;

  wire compls4 = (pd1s4 & !disp6) | (nd1s4 & disp6) ;
  assign dispout = disp6 ^ (ndos4 | pdos4) ;

  assign dataout = {(jo ^ compls4), (ho ^ compls4),
		    (go ^ compls4), (fo ^ compls4),
		    (io ^ compls6), (eo ^ compls6),
		    (do ^ compls6), (co ^ compls6),
		    (bo ^ compls6), (ao ^ compls6)} ;

endmodule
