/**************************************************************************
 * Parks-McClellan algorithm for FIR filter design (C version)
 *-------------------------------------------------
 *  Copyright (c) 1995,1998  Jake Janovetz (janovetz@uiuc.edu)
 *  Copyright (c) 2004  Free Software Foundation, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
 *
 *
 *  Sep 1999 - Paul Kienzle (pkienzle@cs.indiana.edu)
 *      Modified for use in octave as a replacement for the matlab function
 *      remez.mex.  In particular, magnitude responses are required for all
 *      band edges rather than one per band, griddensity is a parameter,
 *      and errors are returned rather than printed directly.
 *  Mar 2000 - Kai Habel (kahacjde@linux.zrz.tu-berlin.de)
 *      Change: ColumnVector x=arg(i).vector_value();
 *      to: ColumnVector x(arg(i).vector_value());
 *  There appear to be some problems with the routine Search. See comments
 *  therein [search for PAK:].  I haven't looked closely at the rest
 *  of the code---it may also have some problems.
 *************************************************************************/

/*
 * This code was extracted from octave.sf.net, and wrapped with
 * GNU Radio glue.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gr_remez.h>
#include <cmath>
#include <assert.h>
#include <iostream>


#ifndef LOCAL_BUFFER
#include <vector>
#define LOCAL_BUFFER(T, buf, size) \
  std::vector<T> buf ## _vector (size); \
  T *buf = &(buf ## _vector[0])
#endif


#define CONST const
#define BANDPASS       1
#define DIFFERENTIATOR 2
#define HILBERT        3

#define NEGATIVE       0
#define POSITIVE       1

#define Pi             3.14159265358979323846
#define Pi2            (2*Pi)

#define GRIDDENSITY    16
#define MAXITERATIONS  40

/*******************
 * CreateDenseGrid
 *=================
 * Creates the dense grid of frequencies from the specified bands.
 * Also creates the Desired Frequency Response function (D[]) and
 * the Weight function (W[]) on that dense grid
 *
 *
 * INPUT:
 * ------
 * int      r        - 1/2 the number of filter coefficients
 * int      numtaps  - Number of taps in the resulting filter
 * int      numband  - Number of bands in user specification
 * double   bands[]  - User-specified band edges [2*numband]
 * double   des[]    - Desired response per band [2*numband]
 * double   weight[] - Weight per band [numband]
 * int      symmetry - Symmetry of filter - used for grid check
 * int      griddensity
 *
 * OUTPUT:
 * -------
 * int    gridsize   - Number of elements in the dense frequency grid
 * double Grid[]     - Frequencies (0 to 0.5) on the dense grid [gridsize]
 * double D[]        - Desired response on the dense grid [gridsize]
 * double W[]        - Weight function on the dense grid [gridsize]
 *******************/

static void 
CreateDenseGrid (int r, int numtaps, int numband, const double bands[],
		 const double des[], const double weight[], int gridsize,
		 double Grid[], double D[], double W[],
		 int symmetry, int griddensity)
{
   int i, j, k, band;
   double delf, lowf, highf, grid0;

   delf = 0.5/(griddensity*r);

/*
 * For differentiator, hilbert,
 *   symmetry is odd and Grid[0] = max(delf, bands[0])
 */
   grid0 = (symmetry == NEGATIVE) && (delf > bands[0]) ? delf : bands[0];

   j=0;
   for (band=0; band < numband; band++)
   {
      lowf = (band==0 ? grid0 : bands[2*band]);
      highf = bands[2*band + 1];
      k = (int)((highf - lowf)/delf + 0.5);   /* .5 for rounding */
      for (i=0; i<k; i++)
      {
         D[j] = des[2*band] + i*(des[2*band+1]-des[2*band])/(k-1);
         W[j] = weight[band];
         Grid[j] = lowf;
         lowf += delf;
         j++;
      }
      Grid[j-1] = highf;
   }

/*
 * Similar to above, if odd symmetry, last grid point can't be .5
 *  - but, if there are even taps, leave the last grid point at .5
 */
   if ((symmetry == NEGATIVE) &&
       (Grid[gridsize-1] > (0.5 - delf)) &&
       (numtaps % 2))
   {
      Grid[gridsize-1] = 0.5-delf;
   }
}


/********************
 * InitialGuess
 *==============
 * Places Extremal Frequencies evenly throughout the dense grid.
 *
 *
 * INPUT: 
 * ------
 * int r        - 1/2 the number of filter coefficients
 * int gridsize - Number of elements in the dense frequency grid
 *
 * OUTPUT:
 * -------
 * int Ext[]    - Extremal indexes to dense frequency grid [r+1]
 ********************/

static void
InitialGuess (int r, int Ext[], int gridsize)
{
   int i;

   for (i=0; i<=r; i++)
      Ext[i] = i * (gridsize-1) / r;
}


/***********************
 * CalcParms
 *===========
 *
 *
 * INPUT:
 * ------
 * int    r      - 1/2 the number of filter coefficients
 * int    Ext[]  - Extremal indexes to dense frequency grid [r+1]
 * double Grid[] - Frequencies (0 to 0.5) on the dense grid [gridsize]
 * double D[]    - Desired response on the dense grid [gridsize]
 * double W[]    - Weight function on the dense grid [gridsize]
 *
 * OUTPUT:
 * -------
 * double ad[]   - 'b' in Oppenheim & Schafer [r+1]
 * double x[]    - [r+1]
 * double y[]    - 'C' in Oppenheim & Schafer [r+1]
 ***********************/

static void
CalcParms (int r, int Ext[], double Grid[], double D[], double W[],
	   double ad[], double x[], double y[])
{
   int i, j, k, ld;
   double sign, xi, delta, denom, numer;

/*
 * Find x[]
 */
   for (i=0; i<=r; i++)
      x[i] = cos(Pi2 * Grid[Ext[i]]);

/*
 * Calculate ad[]  - Oppenheim & Schafer eq 7.132
 */
   ld = (r-1)/15 + 1;         /* Skips around to avoid round errors */
   for (i=0; i<=r; i++)
   {
       denom = 1.0;
       xi = x[i];
       for (j=0; j<ld; j++)
       {
          for (k=j; k<=r; k+=ld)
             if (k != i)
                denom *= 2.0*(xi - x[k]);
       }
       if (fabs(denom)<0.00001)
          denom = 0.00001;
       ad[i] = 1.0/denom;
   }

/*
 * Calculate delta  - Oppenheim & Schafer eq 7.131
 */
   numer = denom = 0;
   sign = 1;
   for (i=0; i<=r; i++)
   {
      numer += ad[i] * D[Ext[i]];
      denom += sign * ad[i]/W[Ext[i]];
      sign = -sign;
   }
   delta = numer/denom;
   sign = 1;

/*
 * Calculate y[]  - Oppenheim & Schafer eq 7.133b
 */
   for (i=0; i<=r; i++)
   {
      y[i] = D[Ext[i]] - sign * delta/W[Ext[i]];
      sign = -sign;
   }
}


/*********************
 * ComputeA
 *==========
 * Using values calculated in CalcParms, ComputeA calculates the
 * actual filter response at a given frequency (freq).  Uses
 * eq 7.133a from Oppenheim & Schafer.
 *
 *
 * INPUT:
 * ------
 * double freq - Frequency (0 to 0.5) at which to calculate A
 * int    r    - 1/2 the number of filter coefficients
 * double ad[] - 'b' in Oppenheim & Schafer [r+1]
 * double x[]  - [r+1]
 * double y[]  - 'C' in Oppenheim & Schafer [r+1]
 *
 * OUTPUT:
 * -------
 * Returns double value of A[freq]
 *********************/

static double
ComputeA (double freq, int r, double ad[], double x[], double y[])
{
   int i;
   double xc, c, denom, numer;

   denom = numer = 0;
   xc = cos(Pi2 * freq);
   for (i=0; i<=r; i++)
   {
      c = xc - x[i];
      if (fabs(c) < 1.0e-7)
      {
         numer = y[i];
         denom = 1;
         break;
      }
      c = ad[i]/c;
      denom += c;
      numer += c*y[i];
   }
   return numer/denom;
}


/************************
 * CalcError
 *===========
 * Calculates the Error function from the desired frequency response
 * on the dense grid (D[]), the weight function on the dense grid (W[]),
 * and the present response calculation (A[])
 *
 *
 * INPUT:
 * ------
 * int    r      - 1/2 the number of filter coefficients
 * double ad[]   - [r+1]
 * double x[]    - [r+1]
 * double y[]    - [r+1]
 * int gridsize  - Number of elements in the dense frequency grid
 * double Grid[] - Frequencies on the dense grid [gridsize]
 * double D[]    - Desired response on the dense grid [gridsize]
 * double W[]    - Weight function on the desnse grid [gridsize]
 *
 * OUTPUT:
 * -------
 * double E[]    - Error function on dense grid [gridsize]
 ************************/

static void
CalcError (int r, double ad[], double x[], double y[],
	   int gridsize, double Grid[],
	   double D[], double W[], double E[])
{
   int i;
   double A;

   for (i=0; i<gridsize; i++)
   {
      A = ComputeA(Grid[i], r, ad, x, y);
      E[i] = W[i] * (D[i] - A);
   }
}

/************************
 * Search
 *========
 * Searches for the maxima/minima of the error curve.  If more than
 * r+1 extrema are found, it uses the following heuristic (thanks
 * Chris Hanson):
 * 1) Adjacent non-alternating extrema deleted first.
 * 2) If there are more than one excess extrema, delete the
 *    one with the smallest error.  This will create a non-alternation
 *    condition that is fixed by 1).
 * 3) If there is exactly one excess extremum, delete the smaller
 *    of the first/last extremum
 *
 *
 * INPUT:
 * ------
 * int    r        - 1/2 the number of filter coefficients
 * int    Ext[]    - Indexes to Grid[] of extremal frequencies [r+1]
 * int    gridsize - Number of elements in the dense frequency grid
 * double E[]      - Array of error values.  [gridsize]
 * OUTPUT:
 * -------
 * int    Ext[]    - New indexes to extremal frequencies [r+1]
 ************************/
static int
Search (int r, int Ext[],
	int gridsize, double E[])
{
   int i, j, k, l, extra;     /* Counters */
   int up, alt;
   int *foundExt;             /* Array of found extremals */

/*
 * Allocate enough space for found extremals.
 */
   foundExt = (int *)malloc((2*r) * sizeof(int));
   k = 0;

/*
 * Check for extremum at 0.
 */
   if (((E[0]>0.0) && (E[0]>E[1])) ||
       ((E[0]<0.0) && (E[0]<E[1])))
      foundExt[k++] = 0;

/*
 * Check for extrema inside dense grid
 */
   for (i=1; i<gridsize-1; i++)
   {
      if (((E[i]>=E[i-1]) && (E[i]>E[i+1]) && (E[i]>0.0)) ||
          ((E[i]<=E[i-1]) && (E[i]<E[i+1]) && (E[i]<0.0))) {
	// PAK: we sometimes get too many extremal frequencies
	if (k >= 2*r) return -3;
	foundExt[k++] = i;
      }
   }

/*
 * Check for extremum at 0.5
 */
   j = gridsize-1;
   if (((E[j]>0.0) && (E[j]>E[j-1])) ||
       ((E[j]<0.0) && (E[j]<E[j-1]))) {
     if (k >= 2*r) return -3;
     foundExt[k++] = j;
   }

   // PAK: we sometimes get not enough extremal frequencies
   if (k < r+1) return -2;


/*
 * Remove extra extremals
 */
   extra = k - (r+1);
   assert(extra >= 0);

   while (extra > 0)
   {
      if (E[foundExt[0]] > 0.0)
         up = 1;                /* first one is a maxima */
      else
         up = 0;                /* first one is a minima */

      l=0;
      alt = 1;
      for (j=1; j<k; j++)
      {
         if (fabs(E[foundExt[j]]) < fabs(E[foundExt[l]]))
            l = j;               /* new smallest error. */
         if ((up) && (E[foundExt[j]] < 0.0))
            up = 0;             /* switch to a minima */
         else if ((!up) && (E[foundExt[j]] > 0.0))
            up = 1;             /* switch to a maxima */
         else
	 { 
            alt = 0;
	    // PAK: break now and you will delete the smallest overall
	    // extremal.  If you want to delete the smallest of the
	    // pair of non-alternating extremals, then you must do:
            //
	    // if (fabs(E[foundExt[j]]) < fabs(E[foundExt[j-1]])) l=j;
	    // else l=j-1;
            break;              /* Ooops, found two non-alternating */
         }                      /* extrema.  Delete smallest of them */
      }  /* if the loop finishes, all extrema are alternating */

/*
 * If there's only one extremal and all are alternating,
 * delete the smallest of the first/last extremals.
 */
      if ((alt) && (extra == 1))
      {
         if (fabs(E[foundExt[k-1]]) < fabs(E[foundExt[0]]))
	   /* Delete last extremal */
	   l = k-1;
	   // PAK: changed from l = foundExt[k-1]; 
         else
	   /* Delete first extremal */
	   l = 0;
	   // PAK: changed from l = foundExt[0];     
      }

      for (j=l; j<k-1; j++)        /* Loop that does the deletion */
      {
         foundExt[j] = foundExt[j+1];
	 assert(foundExt[j]<gridsize);
      }
      k--;
      extra--;
   }

   for (i=0; i<=r; i++)
   {
      assert(foundExt[i]<gridsize);
      Ext[i] = foundExt[i];       /* Copy found extremals to Ext[] */
   }

   free(foundExt);
   return 0;
}


/*********************
 * FreqSample
 *============
 * Simple frequency sampling algorithm to determine the impulse
 * response h[] from A's found in ComputeA
 *
 *
 * INPUT:
 * ------
 * int      N        - Number of filter coefficients
 * double   A[]      - Sample points of desired response [N/2]
 * int      symmetry - Symmetry of desired filter
 *
 * OUTPUT:
 * -------
 * double h[] - Impulse Response of final filter [N]
 *********************/
static void
FreqSample (int N, double A[], double h[], int symm)
{
   int n, k;
   double x, val, M;

   M = (N-1.0)/2.0;
   if (symm == POSITIVE)
   {
      if (N%2)
      {
         for (n=0; n<N; n++)
         {
            val = A[0];
            x = Pi2 * (n - M)/N;
            for (k=1; k<=M; k++)
               val += 2.0 * A[k] * cos(x*k);
            h[n] = val/N;
         }
      }
      else
      {
         for (n=0; n<N; n++)
         {
            val = A[0];
            x = Pi2 * (n - M)/N;
            for (k=1; k<=(N/2-1); k++)
               val += 2.0 * A[k] * cos(x*k);
            h[n] = val/N;
         }
      }
   }
   else
   {
      if (N%2)
      {
         for (n=0; n<N; n++)
         {
            val = 0;
            x = Pi2 * (n - M)/N;
            for (k=1; k<=M; k++)
               val += 2.0 * A[k] * sin(x*k);
            h[n] = val/N;
         }
      }
      else
      {
          for (n=0; n<N; n++)
          {
             val = A[N/2] * sin(Pi * (n - M));
             x = Pi2 * (n - M)/N;
             for (k=1; k<=(N/2-1); k++)
                val += 2.0 * A[k] * sin(x*k);
             h[n] = val/N;
          }
      }
   }
}

/*******************
 * isDone
 *========
 * Checks to see if the error function is small enough to consider
 * the result to have converged.
 *
 * INPUT:
 * ------
 * int    r     - 1/2 the number of filter coeffiecients
 * int    Ext[] - Indexes to extremal frequencies [r+1]
 * double E[]   - Error function on the dense grid [gridsize]
 *
 * OUTPUT:
 * -------
 * Returns 1 if the result converged
 * Returns 0 if the result has not converged
 ********************/

static bool
isDone (int r, int Ext[], double E[])
{
   int i;
   double min, max, current;

   min = max = fabs(E[Ext[0]]);
   for (i=1; i<=r; i++)
   {
      current = fabs(E[Ext[i]]);
      if (current < min)
         min = current;
      if (current > max)
         max = current;
   }
   return (((max-min)/max) < 0.0001);
}

/********************
 * remez
 *=======
 * Calculates the optimal (in the Chebyshev/minimax sense)
 * FIR filter impulse response given a set of band edges,
 * the desired reponse on those bands, and the weight given to
 * the error in those bands.
 *
 * INPUT:
 * ------
 * int     numtaps     - Number of filter coefficients
 * int     numband     - Number of bands in filter specification
 * double  bands[]     - User-specified band edges [2 * numband]
 * double  des[]       - User-specified band responses [2 * numband]
 * double  weight[]    - User-specified error weights [numband]
 * int     type        - Type of filter
 *
 * OUTPUT:
 * -------
 * double h[]      - Impulse response of final filter [numtaps]
 * returns         - true on success, false on failure to converge
 ********************/

static int
remez (double h[], int numtaps,
       int numband, const double bands[], 
       const double des[], const double weight[],
       int type, int griddensity)
{
   double *Grid, *W, *D, *E;
   int    i, iter, gridsize, r, *Ext;
   double *taps, c;
   double *x, *y, *ad;
   int    symmetry;

   if (type == BANDPASS)
      symmetry = POSITIVE;
   else
      symmetry = NEGATIVE;

   r = numtaps/2;                  /* number of extrema */
   if ((numtaps%2) && (symmetry == POSITIVE))
      r++;

/*
 * Predict dense grid size in advance for memory allocation
 *   .5 is so we round up, not truncate
 */
   gridsize = 0;
   for (i=0; i<numband; i++)
   {
      gridsize += (int)(2*r*griddensity*(bands[2*i+1] - bands[2*i]) + .5);
   }
   if (symmetry == NEGATIVE)
   {
      gridsize--;
   }

/*
 * Dynamically allocate memory for arrays with proper sizes
 */
   Grid = (double *)malloc(gridsize * sizeof(double));
   D = (double *)malloc(gridsize * sizeof(double));
   W = (double *)malloc(gridsize * sizeof(double));
   E = (double *)malloc(gridsize * sizeof(double));
   Ext = (int *)malloc((r+1) * sizeof(int));
   taps = (double *)malloc((r+1) * sizeof(double));
   x = (double *)malloc((r+1) * sizeof(double));
   y = (double *)malloc((r+1) * sizeof(double));
   ad = (double *)malloc((r+1) * sizeof(double));

/*
 * Create dense frequency grid
 */
   CreateDenseGrid(r, numtaps, numband, bands, des, weight,
                   gridsize, Grid, D, W, symmetry, griddensity);
   InitialGuess(r, Ext, gridsize);

/*
 * For Differentiator: (fix grid)
 */
   if (type == DIFFERENTIATOR)
   {
      for (i=0; i<gridsize; i++)
      {
/* D[i] = D[i]*Grid[i]; */
         if (D[i] > 0.0001)
            W[i] = W[i]/Grid[i];
      }
   }

/*
 * For odd or Negative symmetry filters, alter the
 * D[] and W[] according to Parks McClellan
 */
   if (symmetry == POSITIVE)
   {
      if (numtaps % 2 == 0)
      {
         for (i=0; i<gridsize; i++)
         {
            c = cos(Pi * Grid[i]);
            D[i] /= c;
            W[i] *= c; 
         }
      }
   }
   else
   {
      if (numtaps % 2)
      {
         for (i=0; i<gridsize; i++)
         {
            c = sin(Pi2 * Grid[i]);
            D[i] /= c;
            W[i] *= c;
         }
      }
      else
      {
         for (i=0; i<gridsize; i++)
         {
            c = sin(Pi * Grid[i]);
            D[i] /= c;
            W[i] *= c;
         }
      }
   }

/*
 * Perform the Remez Exchange algorithm
 */
   for (iter=0; iter<MAXITERATIONS; iter++)
   {
      CalcParms(r, Ext, Grid, D, W, ad, x, y);
      CalcError(r, ad, x, y, gridsize, Grid, D, W, E);
      int err = Search(r, Ext, gridsize, E);
      if (err) return err;
      for(int i=0; i <= r; i++) assert(Ext[i]<gridsize);
      if (isDone(r, Ext, E))
         break;
   }

   CalcParms(r, Ext, Grid, D, W, ad, x, y);

/*
 * Find the 'taps' of the filter for use with Frequency
 * Sampling.  If odd or Negative symmetry, fix the taps
 * according to Parks McClellan
 */
   for (i=0; i<=numtaps/2; i++)
   {
      if (symmetry == POSITIVE)
      {
         if (numtaps%2)
            c = 1;
         else
            c = cos(Pi * (double)i/numtaps);
      }
      else
      {
         if (numtaps%2)
            c = sin(Pi2 * (double)i/numtaps);
         else
            c = sin(Pi * (double)i/numtaps);
      }
      taps[i] = ComputeA((double)i/numtaps, r, ad, x, y)*c;
   }

/*
 * Frequency sampling design with calculated taps
 */
   FreqSample(numtaps, taps, h, symmetry);

/*
 * Delete allocated memory
 */
   free(Grid);
   free(W);
   free(D);
   free(E);
   free(Ext);
   free(x);
   free(y);
   free(ad);
   return iter<MAXITERATIONS?0:-1;
}

//////////////////////////////////////////////////////////////////////////////
//
//			    GNU Radio interface
//
//////////////////////////////////////////////////////////////////////////////


static void
punt (const std::string msg) 
{
  std::cerr << msg << '\n';
  throw std::runtime_error (msg);
}

std::vector<double>
gr_remez (int order,
	  const std::vector<double> &arg_bands,
	  const std::vector<double> &arg_response,
	  const std::vector<double> &arg_weight,
	  const std::string filter_type,
	  int grid_density
	  ) throw (std::runtime_error)
{
  int numtaps = order + 1;
  if (numtaps < 4)
    punt ("gr_remez: number of taps must be >= 3");

  int numbands = arg_bands.size () / 2;
  LOCAL_BUFFER (double, bands, numbands * 2);
  if (numbands < 1 || arg_bands.size () % 2 == 1)
    punt ("gr_remez: must have an even number of band edges");

  for (unsigned int i = 1; i < arg_bands.size (); i++){
    if (arg_bands[i] < arg_bands[i-1])
      punt ("gr_remez: band edges must be nondecreasing");
  }

  if (arg_bands[0] < 0 || arg_bands[arg_bands.size () - 1] > 1)
    punt ("gr_remez: band edges must be in the range [0,1]");

  // Divide by 2 to fit with the implementation that uses a
  // sample rate of [0, 0.5] instead of [0, 1.0]
  for (int i = 0; i < 2 * numbands; i++)
    bands[i] = arg_bands[i] / 2;

  LOCAL_BUFFER (double, response, numbands * 2);
  if (arg_response.size () != arg_bands.size ())
    punt ("gr_remez: must have one response magnitude for each band edge");

  for (int i = 0; i < 2 * numbands; i++)
    response[i] = arg_response[i];

  LOCAL_BUFFER (double, weight, numbands);
  for (int i = 0; i < numbands; i++)
    weight[i] = 1.0;

  if (arg_weight.size () != 0){
    if ((int) arg_weight.size () != numbands)
      punt ("gr_remez: need one weight for each band [=length(band)/2]");
    for (int i = 0; i < numbands; i++)
      weight[i] = arg_weight [i];
  }
  
  int itype = 0;
  if (filter_type == "bandpass") 
    itype = BANDPASS;
  else if (filter_type == "differentiator") 
    itype = DIFFERENTIATOR;
  else if (filter_type == "hilbert") 
    itype = HILBERT;
  else
    punt ("gr_remez: unknown ftype '" + filter_type + "'");

  if (grid_density < 16)
    punt ("gr_remez: grid_density is too low; must be >= 16");

  LOCAL_BUFFER (double, coeff, numtaps + 5);		// FIXME why + 5?
  int err = remez (coeff, numtaps, numbands,
		   bands, response, weight, itype, grid_density);

  if (err == -1)
    punt ("gr_remez: failed to converge");

  if (err == -2)
    punt ("gr_remez: insufficient extremals -- cannot continue");

  if (err == -3)
    punt ("gr_remez: too many extremals -- cannot continue");

  return std::vector<double> (&coeff[0], &coeff[numtaps]);
}



#if 0
/* == Octave interface starts here ====================================== */

DEFUN_DLD (remez, args, ,
  "b = remez(n, f, a [, w] [, ftype] [, griddensity])\n\
Parks-McClellan optimal FIR filter design.\n\
n gives the number of taps in the returned filter\n\
f gives frequency at the band edges [ b1 e1 b2 e2 b3 e3 ...]\n\
a gives amplitude at the band edges [ a(b1) a(e1) a(b2) a(e2) ...]\n\
w gives weighting applied to each band\n\
ftype is 'bandpass', 'hilbert' or 'differentiator'\n\
griddensity determines how accurately the filter will be\n\
    constructed. The minimum value is 16, but higher numbers are\n\
    slower to compute.\n\
\n\
Frequency is in the range (0, 1), with 1 being the nyquist frequency")
{
  octave_value_list retval;
  int i;

  int nargin = args.length();
  if (nargin < 3 || nargin > 6) {
    print_usage("remez");
    return retval;
  }

  int numtaps = NINT (args(0).double_value()) + 1; // #coeff = filter order+1
  if (numtaps < 4) {
    error("remez: number of taps must be an integer greater than 3");
    return retval;
  }

  ColumnVector o_bands(args(1).vector_value());
  int numbands = o_bands.length()/2;
  OCTAVE_LOCAL_BUFFER(double, bands, numbands*2);
  if (numbands < 1 || o_bands.length()%2 == 1) {
    error("remez: must have an even number of band edges");
    return retval;
  }
  for (i=1; i < o_bands.length(); i++) {
    if (o_bands(i)<o_bands(i-1)) {
      error("band edges must be nondecreasing");
      return retval;
    }
  }
  if (o_bands(0) < 0 || o_bands(1) > 1) {
    error("band edges must be in the range [0,1]");
    return retval;
  }
  for(i=0; i < 2*numbands; i++) bands[i] = o_bands(i)/2.0;

  ColumnVector o_response(args(2).vector_value());
  OCTAVE_LOCAL_BUFFER (double, response, numbands*2);
  if (o_response.length() != o_bands.length()) {
    error("remez: must have one response magnitude for each band edge");
    return retval;
  }
  for(i=0; i < 2*numbands; i++) response[i] = o_response(i);

  std::string stype = std::string("bandpass");
  int density = 16;
  OCTAVE_LOCAL_BUFFER (double, weight, numbands);
  for (i=0; i < numbands; i++) weight[i] = 1.0;
  if (nargin > 3) {
    if (args(3).is_real_matrix()) {
      ColumnVector o_weight(args(3).vector_value());
      if (o_weight.length() != numbands) {
	error("remez: need one weight for each band [=length(band)/2]");
	return retval;
      }
      for (i=0; i < numbands; i++) weight[i] = o_weight(i);
    }
    else if (args(3).is_string())
      stype = args(3).string_value();
    else if (args(3).is_real_scalar())
      density = NINT(args(3).double_value());
    else {
      error("remez: incorrect argument list");
      return retval;
    }
  }
  if (nargin > 4) {
    if (args(4).is_string() && !args(3).is_string())
      stype = args(4).string_value();
    else if (args(4).is_real_scalar() && !args(3).is_real_scalar())
      density = NINT(args(4).double_value());
    else {
      error("remez: incorrect argument list");
      return retval;
    }
  }
  if (nargin > 5) {
    if (args(5).is_real_scalar() 
	&& !args(4).is_real_scalar() 
	&& !args(3).is_real_scalar())
      density = NINT(args(4).double_value());
    else {
      error("remez: incorrect argument list");
      return retval;
    }
  }

  int itype;
  if (stype == "bandpass") 
    itype = BANDPASS;
  else if (stype == "differentiator") 
    itype = DIFFERENTIATOR;
  else if (stype == "hilbert") 
    itype = HILBERT;
  else {
    error("remez: unknown ftype '%s'", stype.data());
    return retval;
  }

  if (density < 16) {
    error("remez: griddensity is too low; must be greater than 16");
    return retval;
  }

  OCTAVE_LOCAL_BUFFER (double, coeff, numtaps+5);
  int err = remez(coeff,numtaps,numbands,bands,response,weight,itype,density);

  if (err == -1)
    warning("remez: -- failed to converge -- returned filter may be bad.");
  else if (err == -2) {
    error("remez: insufficient extremals--cannot continue");
    return retval;
  }
  else if (err == -3) {
    error("remez: too many extremals--cannot continue");
    return retval;
  }

  ColumnVector h(numtaps);
  while(numtaps--) h(numtaps) = coeff[numtaps];

  return octave_value(h);
}

/*
%!test
%! b = [
%!    0.0415131831103279
%!    0.0581639884202646
%!   -0.0281579212691008
%!   -0.0535575358002337
%!   -0.0617245915143180
%!    0.0507753178978075
%!    0.2079018331396460
%!    0.3327160895375440
%!    0.3327160895375440
%!    0.2079018331396460
%!    0.0507753178978075
%!   -0.0617245915143180
%!   -0.0535575358002337
%!   -0.0281579212691008
%!    0.0581639884202646
%!    0.0415131831103279];
%! assert(remez(15,[0,0.3,0.4,1],[1,1,0,0]),b,1e-14);

 */

#endif
