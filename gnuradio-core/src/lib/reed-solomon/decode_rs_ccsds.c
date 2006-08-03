/* This function wraps around the fixed 8-bit decoder, performing the
 * basis transformations necessary to meet the CCSDS standard
 *
 * Copyright 2002, Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */
#define FIXED 1
#include "fixed.h"
#include "ccsds.h"

int decode_rs_ccsds(unsigned char *data,int *eras_pos,int no_eras){
  int i,r;
  unsigned char cdata[NN];

  /* Convert data from dual basis to conventional */
  for(i=0;i<NN;i++)
    cdata[i] = Tal1tab[data[i]];

  r = decode_rs_8(cdata,eras_pos,no_eras);

  if(r > 0){
    /* Convert from conventional to dual basis */
    for(i=0;i<NN;i++)
      data[i] = Taltab[cdata[i]];
  }
  return r;
}
