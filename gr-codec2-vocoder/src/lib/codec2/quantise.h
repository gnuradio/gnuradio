/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: quantise.h
  AUTHOR......: David Rowe                                                          
  DATE CREATED: 31/5/92                                                       
                                                                             
  Quantisation functions for the sinusoidal coder.  
                                                                             
\*---------------------------------------------------------------------------*/

/*
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

#ifndef __QUANTISE__
#define __QUANTISE__

#define WO_BITS   7
#define WO_LEVELS (1<<WO_BITS)
#define E_BITS    5
#define E_LEVELS  (1<<E_BITS)
#define E_MIN_DB -10.0
#define E_MAX_DB  40.0

void quantise_init();
float lpc_model_amplitudes(float Sn[], float w[], MODEL *model, int order,
			   int lsp,float ak[]);
void aks_to_M2(float ak[], int order, MODEL *model, float E, float *snr, 
	       int dump);

int   encode_Wo(float Wo);
float decode_Wo(int index);

void encode_lsps(int indexes[], float lsp[], int order);
void decode_lsps(float lsp[], int indexes[], int order);
void lspd_quantise(float lsp[], float lsp_[], int order);
void lspdvq_quantise(float lsp[], float lsp_[], int order); 

int encode_energy(float e);
float decode_energy(int index);

void encode_amplitudes(int    lsp_indexes[], 
		       int   *energy_index,
		       MODEL *model, 
		       float  Sn[], 
		       float  w[]);

float decode_amplitudes(MODEL *model,
			float  ak[],
			int lsp_indexes[],
			int energy_index,
			float  lsps[],
			float *e);

void pack(unsigned char * bits, unsigned int *nbit, int index, unsigned int index_bits);
int  unpack(const unsigned char * bits, unsigned int *nbit, unsigned int index_bits);

int lsp_bits(int i);

void apply_lpc_correction(MODEL *model);
float speech_to_uq_lsps(float lsp[],
			float ak[],
		        float Sn[], 
		        float w[],
		        int   order
			);
void bw_expand_lsps(float lsp[],
		    int   order
		    );
void decode_lsps(float lsp[], int indexes[], int order);

#endif
