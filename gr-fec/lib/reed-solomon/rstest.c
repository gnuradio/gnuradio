/* Test the Reed-Solomon codecs
 * for various block sizes and with random data and random error patterns
 *
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/fec/rs.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int exercise_char(void*, int);

#ifdef ALL_VERSIONS
int exercise_8(int);
int exercise_ccsds(int);
#endif

struct {
    int symsize;
    int genpoly;
    int fcs;
    int prim;
    int nroots;
    int ntrials;
} Tab[] = {
    { 2, 0x7, 1, 1, 1, 10 },
    { 3, 0xb, 1, 1, 2, 10 },
    { 4, 0x13, 1, 1, 4, 10 },
    { 5, 0x25, 1, 1, 6, 10 },
    { 6, 0x43, 1, 1, 8, 10 },
    { 7, 0x89, 1, 1, 10, 10 },
    { 8, 0x11d, 1, 1, 32, 10 },
    { 8, 0x187, 112, 11, 32, 10 }, /* Duplicates CCSDS codec */
    { 0, 0, 0, 0, 0 },
};

int main()
{
    void* handle;
    int errs, terrs;
    int i;

    terrs = 0;
    srand(time(NULL));

#ifdef ALL_VERSIONS
    printf("Testing fixed (255,223) RS codec...");
    fflush(stdout);
    errs = exercise_8(10);
    terrs += errs;
    if (errs == 0) {
        printf("OK\n");
    }
    printf("Testing CCSDS standard (255,223) RS codec...");
    fflush(stdout);
    errs = exercise_ccsds(10);
    terrs += errs;
    if (errs == 0) {
        printf("OK\n");
    }
#endif

    for (i = 0; Tab[i].symsize != 0; i++) {
        int nn, kk;

        nn = (1 << Tab[i].symsize) - 1;
        kk = nn - Tab[i].nroots;
        printf("Testing (%d,%d) RS codec...", nn, kk);
        fflush(stdout);
        if ((handle = init_rs_char(Tab[i].symsize,
                                    Tab[i].genpoly,
                                    Tab[i].fcs,
                                    Tab[i].prim,
                                    Tab[i].nroots)) == NULL) {
            printf("init_rs_char failed!\n");
            continue;
        }
        errs = exercise_char(handle, Tab[i].ntrials);
        terrs += errs;
        if (errs == 0) {
            printf("OK\n");
        }
        free_rs_char(handle);
    }
    if (terrs == 0)
        printf("All codec tests passed!\n");

    exit(0);
}