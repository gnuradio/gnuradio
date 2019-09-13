/*---------------------------------------------------------------------------*\

  FILE........: fdmdv_internal.h
  AUTHOR......: David Rowe
  DATE CREATED: April 16 2012

  Header file for FDMDV internal functions, exposed via this header
  file for testing.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2012 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __FDMDV_INTERNAL__
#define __FDMDV_INTERNAL__

#include "comp.h"
#include "codec2_fdmdv.h"
#include "kiss_fft.h"

/*---------------------------------------------------------------------------*\

                               DEFINES

\*---------------------------------------------------------------------------*/

#define PI             3.141592654
#define FS                    8000  /* sample rate in Hz                                                    */
#define T                 (1.0/FS)  /* sample period in seconds                                             */
#define RS                      50  /* symbol rate in Hz                                                    */
#define NC                      20  /* max number of data carriers (plus one pilot in the centre)           */
#define NB                       2  /* Bits/symbol for QPSK modulation                                      */
#define RB              (NC*RS*NB)  /* bit rate                                                             */
#define M                  (FS/RS)  /* oversampling factor                                                  */
#define NSYM                     6  /* number of symbols to filter over                                     */
#define NFILTER            (NSYM*M) /* size of tx/rx filters at sample rate M                               */

#define FSEP                    75  /* Default separation between carriers (Hz)                             */

#define NT                       5  /* number of symbols we estimate timing over                            */
#define P                        4  /* oversample factor used for initial rx symbol filtering               */
#define NFILTERTIMING (M+NFILTER+M) /* filter memory used for resampling after timing estimation            */

#define NPILOT_LUT                 (4*M)    /* number of pilot look up table samples                 */
#define NPILOTCOEFF                   30    /* number of FIR filter coeffs in LP filter              */
#define NPILOTBASEBAND (NPILOTCOEFF+M+M/P)  /* number of pilot baseband samples reqd for pilot LPF   */
#define NPILOTLPF                  (4*M)    /* number of samples we DFT pilot over, pilot est window */
#define MPILOTFFT                    256

#define NSYNC_MEM                6

/* averaging filter coeffs */

#define TRACK_COEFF              0.5
#define SNR_COEFF                0.9       /* SNR est averaging filter coeff */

/*---------------------------------------------------------------------------*\

                               STRUCT for States

\*---------------------------------------------------------------------------*/

struct FDMDV {

    int   Nc;
    float fsep;

    /* test data (test frame) states */

    int  ntest_bits;
    int  current_test_bit;
    int *rx_test_bits_mem;

    /* Modulator */

    int  old_qpsk_mapping;
    int  tx_pilot_bit;
    COMP prev_tx_symbols[NC+1];
    COMP tx_filter_memory[NC+1][NSYM];
    COMP phase_tx[NC+1];
    COMP freq[NC+1];

    /* Pilot generation at demodulator */

    COMP pilot_lut[NPILOT_LUT];
    int  pilot_lut_index;
    int  prev_pilot_lut_index;

    /* freq offset estimation states */

    kiss_fft_cfg fft_pilot_cfg;
    COMP pilot_baseband1[NPILOTBASEBAND];
    COMP pilot_baseband2[NPILOTBASEBAND];
    COMP pilot_lpf1[NPILOTLPF];
    COMP pilot_lpf2[NPILOTLPF];
    COMP S1[MPILOTFFT];
    COMP S2[MPILOTFFT];

    /* freq offset correction states */

    float foff;
    COMP foff_rect;
    COMP foff_phase_rect;

    /* Demodulator */

    COMP  phase_rx[NC+1];
    COMP  rx_filter_memory[NC+1][NFILTER];
    COMP  rx_filter_mem_timing[NC+1][NT*P];
    COMP  rx_baseband_mem_timing[NC+1][NFILTERTIMING];
    float rx_timing;
    COMP  phase_difference[NC+1];
    COMP  prev_rx_symbols[NC+1];

    /* sync state machine */

    int  sync_mem[NSYNC_MEM];
    int  fest_state;
    int  sync;
    int  timer;

    /* SNR estimation states */

    float sig_est[NC+1];
    float noise_est[NC+1];

    /* Buf for FFT/waterfall */

    float fft_buf[2*FDMDV_NSPEC];
    kiss_fft_cfg fft_cfg;
 };

/*---------------------------------------------------------------------------*\

                              FUNCTION PROTOTYPES

\*---------------------------------------------------------------------------*/

void bits_to_dqpsk_symbols(COMP tx_symbols[], int Nc, COMP prev_tx_symbols[], int tx_bits[], int *pilot_bit, int old_qpsk_mapping);
void tx_filter(COMP tx_baseband[NC+1][M], int Nc, COMP tx_symbols[], COMP tx_filter_memory[NC+1][NSYM]);
void fdm_upconvert(COMP tx_fdm[], int Nc, COMP tx_baseband[NC+1][M], COMP phase_tx[], COMP freq_tx[]);
void generate_pilot_fdm(COMP *pilot_fdm, int *bit, float *symbol, float *filter_mem, COMP *phase, COMP *freq);
void generate_pilot_lut(COMP pilot_lut[], COMP *pilot_freq);
float rx_est_freq_offset(struct FDMDV *f, COMP rx_fdm[], int nin);
void lpf_peak_pick(float *foff, float *max, COMP pilot_baseband[], COMP pilot_lpf[], kiss_fft_cfg fft_pilot_cfg, COMP S[], int nin);
void freq_shift(COMP rx_fdm_fcorr[], COMP rx_fdm[], float foff, COMP *foff_rect, COMP *foff_phase_rect, int nin);
void fdm_downconvert(COMP rx_baseband[NC+1][M+M/P], int Nc, COMP rx_fdm[], COMP phase_rx[], COMP freq[], int nin);
void rx_filter(COMP rx_filt[NC+1][P+1], int Nc, COMP rx_baseband[NC+1][M+M/P], COMP rx_filter_memory[NC+1][NFILTER], int nin);
float rx_est_timing(COMP  rx_symbols[], int Nc,
		   COMP  rx_filt[NC+1][P+1],
		   COMP  rx_baseband[NC+1][M+M/P],
		   COMP  rx_filter_mem_timing[NC+1][NT*P],
		   float env[],
		   COMP  rx_baseband_mem_timing[NC+1][NFILTERTIMING],
		   int   nin);
float qpsk_to_bits(int rx_bits[], int *sync_bit, int Nc, COMP phase_difference[], COMP prev_rx_symbols[], COMP rx_symbols[], int old_qpsk_mapping);
void snr_update(float sig_est[], float noise_est[], int Nc, COMP phase_difference[]);
int freq_state(int *reliable_sync_bit, int sync_bit, int *state, int *timer, int *sync_mem);
float calc_snr(int Nc, float sig_est[], float noise_est[]);

#endif
