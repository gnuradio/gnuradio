/* Reed-Solomon encoder
 * Copyright 2002, Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include <string.h>

#ifdef FIXED
#include "fixed.h"
#elif defined(BIGSYM)
#include "int.h"
#else
#include "char.h"
#endif

void ENCODE_RS(
#ifndef FIXED
void *p,
#endif
DTYPE *data, DTYPE *bb){
#ifndef FIXED
  struct rs *rs = (struct rs *)p;
#endif
  unsigned int i, j;
  DTYPE feedback;

  memset(bb,0,NROOTS*sizeof(DTYPE));

  for(i=0;i<NN-NROOTS;i++){
    feedback = INDEX_OF[data[i] ^ bb[0]];
    if(feedback != A0){      /* feedback term is non-zero */
#ifdef UNNORMALIZED
      /* This line is unnecessary when GENPOLY[NROOTS] is unity, as it must
       * always be for the polynomials constructed by init_rs()
       */
      feedback = MODNN(NN - GENPOLY[NROOTS] + feedback);
#endif
      for(j=1;j<NROOTS;j++)
#ifdef FIXED
	bb[j] ^= ALPHA_TO[MODNN(feedback + GENPOLY[NROOTS-j])];
#elif defined(BIGSYM)
        // Same as above; keeping as a separate line in case these change.
	bb[j] ^= ALPHA_TO[MODNN(feedback + GENPOLY[NROOTS-j])];
#else
	bb[j] ^= ALPHA_TO[rs->modnn_table[feedback + GENPOLY[NROOTS-j]]];
#endif
    }
    /* Shift */
    memmove(&bb[0],&bb[1],sizeof(DTYPE)*(NROOTS-1));
    if(feedback != A0)
#ifdef FIXED
      bb[NROOTS-1] = ALPHA_TO[MODNN(feedback + GENPOLY[0])];
#elif defined(BIGSYM)
      // Same as above; keeping as a separate line in case these change.
      bb[NROOTS-1] = ALPHA_TO[MODNN(feedback + GENPOLY[0])];
#else
      bb[NROOTS-1] = ALPHA_TO[rs->modnn_table[feedback + GENPOLY[0]]];
#endif
    else
      bb[NROOTS-1] = 0;
  }
}
