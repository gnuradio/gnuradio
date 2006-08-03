/* Include file to configure the RS codec for character symbols
 *
 * Copyright 2002, Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */

#define DTYPE unsigned char

/* Reed-Solomon codec control block */
struct rs {
  unsigned int mm;              /* Bits per symbol */
  unsigned int nn;              /* Symbols per block (= (1<<mm)-1) */
  unsigned char *alpha_to;      /* log lookup table */
  unsigned char *index_of;      /* Antilog lookup table */
  unsigned char *genpoly;       /* Generator polynomial */
  unsigned int nroots;     /* Number of generator roots = number of parity symbols */
  unsigned char fcr;        /* First consecutive root, index form */
  unsigned char prim;       /* Primitive element, index form */
  unsigned char iprim;      /* prim-th root of 1, index form */
};

static inline int modnn(struct rs *rs,int x){
  while (x >= rs->nn) {
    x -= rs->nn;
    x = (x >> rs->mm) + (x & rs->nn);
  }
  return x;
}
#define MODNN(x) modnn(rs,x)

#define MM (rs->mm)
#define NN (rs->nn)
#define ALPHA_TO (rs->alpha_to) 
#define INDEX_OF (rs->index_of)
#define GENPOLY (rs->genpoly)
#define NROOTS (rs->nroots)
#define FCR (rs->fcr)
#define PRIM (rs->prim)
#define IPRIM (rs->iprim)
#define A0 (NN)

#define ENCODE_RS encode_rs_char
#define DECODE_RS decode_rs_char
#define INIT_RS init_rs_char
#define FREE_RS free_rs_char

void ENCODE_RS(void *p,DTYPE *data,DTYPE *parity);
int DECODE_RS(void *p,DTYPE *data,int *eras_pos,int no_eras);
void *INIT_RS(unsigned int symsize,unsigned int gfpoly,unsigned int fcr,
		   unsigned int prim,unsigned int nroots);
void FREE_RS(void *p);





