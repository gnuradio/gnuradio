/**************************************************************************
 * Parks-McClellan algorithm for FIR filter design (C version)
 *-------------------------------------------------
 *  Copyright (c) 1995,1998  Jake Janovetz (janovetz@uiuc.edu)
 *  Copyright (c) 2004  Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
 *  There appear to be some problems with the routine search. See comments
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

#include <gnuradio/filter/pm_remez.h>
#include <gnuradio/logger.h>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace gr {
namespace filter {

// helper types
enum class filter_type { BANDPASS = 1, DIFFERENTIATOR = 2, HILBERT = 3 };
enum class symmetry_type { NEGATIVE, POSITIVE };

constexpr unsigned int MAXITERATIONS = 40;
constexpr double PI = 3.14159265358979323846;
constexpr double PI2 = 2 * PI;

#ifndef CPP20CONSTEXPR
#if __cplusplus >= 202002L
#define CPP20CONSTEXPR constexpr
#else
#define CPP20CONSTEXPR
#endif
#endif

/*******************
 * create_dense_grid
 *=================
 *
 * Creates the dense grid of frequencies from the specified bands.
 * Also creates the Desired Frequency Response function (D[]) and
 * the Weight function (W[]) on that dense grid
 *
 *
 * INPUT:
 * ------
 * unsigned int n_taps_half - 1/2 the number of filter coefficients
 * int      numtaps  - Number of taps in the resulting filter
 * int      numband  - Number of bands in user specification
 * double   bands[]  - User-specified band edges [2*numband]
 * double   des[]    - Desired response per band [2*numband]
 * double   weight[] - Weight per band [numband]
 * symmetry_type symmetry - Symmetry of filter - used for grid check
 * int      griddensity
 *
 * OUTPUT:
 * -------
 * int    gridsize   - Number of elements in the dense frequency grid
 * double Grid[]     - Frequencies (0 to 0.5) on the dense grid [gridsize]
 * double D[]        - Desired response on the dense grid [gridsize]
 * double W[]        - Weight function on the dense grid [gridsize]
 *******************/

static void create_dense_grid(unsigned int n_taps_half,
                              int numtaps,
                              int numband,
                              const std::vector<double>& bands,
                              const std::vector<double>& des,
                              const std::vector<double>& weight,
                              int gridsize,
                              std::vector<double>& Grid,
                              std::vector<double>& D,
                              std::vector<double>& W,
                              symmetry_type symmetry,
                              int griddensity)
{
    double delf = 0.5 / (griddensity * n_taps_half);
    /*
     * For differentiator, hilbert,
     *   symmetry is odd and Grid[0] = max(delf, bands[0])
     */
    double grid0 =
        (symmetry == symmetry_type::NEGATIVE) && (delf > bands[0]) ? delf : bands[0];

    for (int band = 0, j = 0; band < numband; band++) {
        double lowf = (band == 0 ? grid0 : bands[2 * band]);
        double highf = bands[2 * band + 1];
        int k = (int)((highf - lowf) / delf + 0.5); /* .5 for rounding */
        for (int i = 0; i < k; i++) {
            D[j] = des[2 * band] + i * (des[2 * band + 1] - des[2 * band]) / (k - 1);
            W[j] = weight[band];
            Grid[j] = lowf;
            lowf += delf;
            j++;
        }
        Grid[j - 1] = highf;
    }

    /*
     * Similar to above, if odd symmetry, last grid point can't be .5
     *  - but, if there are even taps, leave the last grid point at .5
     */
    if ((symmetry == symmetry_type::NEGATIVE) && (Grid[gridsize - 1] > (0.5 - delf)) &&
        (numtaps % 2)) {
        Grid[gridsize - 1] = 0.5 - delf;
    }
}


/********************
 * initial_guess
 *==============
 * Places Extremal Frequencies evenly throughout the dense grid.
 *
 *
 * INPUT:
 * ------
 * unsigned int n_taps_half - 1/2 the number of filter coefficients
 * int gridsize - Number of elements in the dense frequency grid
 *
 * OUTPUT:
 * -------
 * int ext[]    - Extremal indexes to dense frequency grid [r+1]
 ********************/

static CPP20CONSTEXPR void
initial_guess(unsigned int n_taps_half, std::vector<int>& Ext, int gridsize)
{
    for (unsigned int i = 0; i <= n_taps_half; i++)
        Ext[i] = i * (gridsize - 1) / n_taps_half;
}


/***********************
 * calc_parms
 *===========
 *
 *
 * INPUT:
 * ------
 * unsigned int n_taps_half - 1/2 the number of filter coefficients
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

static constexpr void calc_parms(unsigned int n_taps_half,
                                 const std::vector<int>& Ext,
                                 const std::vector<double>& Grid,
                                 const std::vector<double>& D,
                                 const std::vector<double>& W,
                                 std::vector<double>& ad,
                                 std::vector<double>& x,
                                 std::vector<double>& y)
{
    /*
     * Find x[]
     */
    for (unsigned int i = 0; i <= n_taps_half; i++)
        x[i] = cos(PI2 * Grid[Ext[i]]);

    /*
     * Calculate ad[]  - Oppenheim & Schafer eq 7.132
     */
    unsigned int ld = (n_taps_half - 1) / 15 + 1; /* Skips around to avoid round errors */
    for (unsigned int i = 0; i <= n_taps_half; i++) {
        double denom = 1.0;
        double xi = x[i];
        for (unsigned int j = 0; j < ld; j++) {
            for (unsigned int k = j; k <= n_taps_half; k += ld)
                if (k != i)
                    denom *= 2.0 * (xi - x[k]);
        }

        // FIXME: 0.00001 seems far too large for double and cos accuracy
        if (fabs(denom) < 0.00001)
            denom = 0.00001;
        ad[i] = 1.0 / denom;
    }

    /*
     * Calculate delta  - Oppenheim & Schafer eq 7.131
     */
    double numer = 0;
    double denom = 0;
    double sign = 1;
    for (unsigned int i = 0; i <= n_taps_half; i++) {
        numer += ad[i] * D[Ext[i]];
        denom += sign * ad[i] / W[Ext[i]];
        sign = -sign;
    }

    /*
     * Calculate y[]  - Oppenheim & Schafer eq 7.133b
     */
    double delta = numer / denom;
    sign = 1;
    for (unsigned int i = 0; i <= n_taps_half; i++) {
        y[i] = D[Ext[i]] - sign * delta / W[Ext[i]];
        sign = -sign;
    }
}


/*********************
 * compute_A
 *==========
 * Using values calculated in calc_parms, compute_A calculates the
 * actual filter response at a given frequency (freq).  Uses
 * eq 7.133a from Oppenheim & Schafer.
 *
 *
 * INPUT:
 * ------
 * double freq - Frequency (0 to 0.5) at which to calculate A
 * unsigned n_taps_half - 1/2 the number of filter coefficients
 * double ad[] - 'b' in Oppenheim & Schafer [r+1]
 * double x[]  - [r+1]
 * double y[]  - 'C' in Oppenheim & Schafer [r+1]
 *
 * OUTPUT:
 * -------
 * Returns double value of A[freq]
 *********************/

static CPP20CONSTEXPR double compute_A(double freq,
                                       unsigned int n_taps_half,
                                       const std::vector<double>& ad,
                                       const std::vector<double>& x,
                                       const std::vector<double>& y)
{
    double denom = 0;
    double numer = 0;
    const double xc = cos(PI2 * freq);
    for (unsigned int i = 0; i <= n_taps_half; i++) {
        double c = xc - x[i];
        if (fabs(c) < 1.0e-7) {
            numer = y[i];
            denom = 1;
            break;
        }
        c = ad[i] / c;
        denom += c;
        numer += c * y[i];
    }
    return numer / denom;
}


/************************
 * calc_error
 *===========
 * Calculates the Error function from the desired frequency response
 * on the dense grid (D[]), the weight function on the dense grid (W[]),
 * and the present response calculation (A[])
 *
 *
 * INPUT:
 * ------
 * unsigned n_taps_half - 1/2 the number of filter coefficients
 * double ad[]   - [r+1]
 * double x[]    - [r+1]
 * double y[]    - [r+1]
 * int gridsize  - Number of elements in the dense frequency grid
 * double Grid[] - Frequencies on the dense grid [gridsize]
 * double D[]    - Desired response on the dense grid [gridsize]
 * double W[]    - Weight function on the dense grid [gridsize]
 *
 * OUTPUT:
 * -------
 * double E[]    - Error function on dense grid [gridsize]
 ************************/

static CPP20CONSTEXPR void calc_error(unsigned int n_taps_half,
                                      const std::vector<double>& ad,
                                      const std::vector<double>& x,
                                      const std::vector<double>& y,
                                      int gridsize,
                                      const std::vector<double>& Grid,
                                      const std::vector<double>& D,
                                      const std::vector<double>& W,
                                      std::vector<double>& E)
{
    for (int i = 0; i < gridsize; i++) {
        double A = compute_A(Grid[i], n_taps_half, ad, x, y);
        E[i] = W[i] * (D[i] - A);
    }
}

/************************
 * search
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
 * unsigned n_taps_half - 1/2 the number of filter coefficients
 * int    Ext[]    - Indexes to Grid[] of extremal frequencies [r+1]
 * int    gridsize - Number of elements in the dense frequency grid
 * double E[]      - Array of error values.  [gridsize]
 * OUTPUT:
 * -------
 * int    Ext[]    - New indexes to extremal frequencies [r+1]
 ************************/
static int search(unsigned int n_taps_half,
                  std::vector<int>& Ext,
                  int gridsize,
                  const std::vector<double>& E)
{
    std::vector<int> foundExt(2 * n_taps_half);
    unsigned int k = 0;

    /*
     * Check for extremum at 0.
     */
    if (((E[0] > 0.0) && (E[0] > E[1])) || ((E[0] < 0.0) && (E[0] < E[1])))
        foundExt[k++] = 0;

    /*
     * Check for extrema inside dense grid
     */
    for (int i = 1; i < gridsize - 1; i++) {
        if (((E[i] >= E[i - 1]) && (E[i] > E[i + 1]) && (E[i] > 0.0)) ||
            ((E[i] <= E[i - 1]) && (E[i] < E[i + 1]) && (E[i] < 0.0))) {
            // PAK: we sometimes get too many extremal frequencies
            if (k >= 2 * n_taps_half) {
                return -3;
            }
            foundExt[k++] = i;
        }
    }

    /*
     * Check for extremum at 0.5
     */
    int extrempos = gridsize - 1;
    if (((E[extrempos] > 0.0) && (E[extrempos] > E[extrempos - 1])) ||
        ((E[extrempos] < 0.0) && (E[extrempos] < E[extrempos - 1]))) {
        if (k >= 2 * n_taps_half) {
            return -3;
        }
        foundExt[k++] = extrempos;
    }

    // PAK: we sometimes get not enough extremal frequencies
    if (k < n_taps_half + 1) {
        return -2;
    }

    /*
     * Remove extra extremals
     */
    int extra = k - (n_taps_half + 1);
    assert(extra >= 0);

    while (extra > 0) {
        // is first one a maximum (1) or a minimum (0)?
        int up = E[foundExt[0]] > 0.0; // C++ guarantees true == 1;

        int l = 0;
        int alt = 1;
        for (unsigned int j = 1; j < k; j++) {
            if (fabs(E[foundExt[j]]) < fabs(E[foundExt[l]]))
                l = j; /* new smallest error. */
            if ((up) && (E[foundExt[j]] < 0.0))
                up = 0; /* switch to a minima */
            else if ((!up) && (E[foundExt[j]] > 0.0))
                up = 1; /* switch to a maxima */
            else {
                alt = 0;
                // PAK: break now and you will delete the smallest overall
                // extremal.  If you want to delete the smallest of the
                // pair of non-alternating extremals, then you must do:
                //
                // if(fabs(E[foundExt[j]]) < fabs(E[foundExt[j-1]])) l=j;
                // else l=j-1;
                break; /* Ooops, found two non-alternating */
            }          /* extrema.  Delete smallest of them */
        }              /* if the loop finishes, all extrema are alternating */

        /*
         * If there's only one extremal and all are alternating,
         * delete the smallest of the first/last extremals.
         */
        if ((alt) && (extra == 1)) {
            if (fabs(E[foundExt[k - 1]]) < fabs(E[foundExt[0]]))
                /* Delete last extremal */
                l = k - 1;
            // PAK: changed from l = foundExt[k-1];
            else
                /* Delete first extremal */
                l = 0;
            // PAK: changed from l = foundExt[0];
        }

        for (unsigned int j = l; j < k - 1; j++) { /* Loop that does the deletion */
            foundExt[j] = foundExt[j + 1];
            assert(foundExt[j] < gridsize);
        }
        k--;
        extra--;
    }

    for (unsigned int i = 0; i <= n_taps_half; i++) {
        assert(foundExt[i] < gridsize);
        Ext[i] = foundExt[i]; /* Copy found extremals to Ext[] */
    }

    return 0;
}


/*********************
 * freq_sample
 *============
 * Simple frequency sampling algorithm to determine the impulse
 * response h[] from A's found in compute_A
 *
 *
 * INPUT:
 * ------
 * int      N        - Number of filter coefficients
 * double   A[]      - Sample points of desired response [N/2]
 * symmmetry_type symmetry - Symmetry of desired filter
 *
 * OUTPUT:
 * -------
 * double h[] - Impulse Response of final filter [N]
 *********************/
static CPP20CONSTEXPR void freq_sample(int N,
                                       const std::vector<double>& A,
                                       std::vector<double>& h,
                                       symmetry_type symm)
{
    double M = (N - 1.0) / 2.0;
    if (symm == symmetry_type::POSITIVE) {
        if (N % 2) {
            for (int n = 0; n < N; n++) {
                double val = A[0];
                double x = PI2 * (n - M) / N;
                for (int k = 1; k <= M; k++)
                    val += 2.0 * A[k] * cos(x * k);
                h[n] = val / N;
            }
        } else {
            for (int n = 0; n < N; n++) {
                double val = A[0];
                double x = PI2 * (n - M) / N;
                for (int k = 1; k <= (N / 2 - 1); k++)
                    val += 2.0 * A[k] * cos(x * k);
                h[n] = val / N;
            }
        }
    } else {
        if (N % 2) {
            for (int n = 0; n < N; n++) {
                double val = 0;
                double x = PI2 * (n - M) / N;
                for (int k = 1; k <= M; k++)
                    val += 2.0 * A[k] * sin(x * k);
                h[n] = val / N;
            }
        } else {
            for (int n = 0; n < N; n++) {
                double val = A[N / 2] * sin(PI * (n - M));
                double x = PI2 * (n - M) / N;
                for (int k = 1; k <= (N / 2 - 1); k++)
                    val += 2.0 * A[k] * sin(x * k);
                h[n] = val / N;
            }
        }
    }
}

/*******************
 * is_done
 *========
 * Checks to see if the error function is small enough to consider
 * the result to have converged.
 *
 * INPUT:
 * ------
 * unsigned n_taps_half - 1/2 the number of filter coefficients
 * int    Ext[] - Indexes to extremal frequencies [r+1]
 * double E[]   - Error function on the dense grid [gridsize]
 * double tolerace - threshold under which to consider being done
 *
 * OUTPUT:
 * -------
 * Returns 1 if the result converged
 * Returns 0 if the result has not converged
 ********************/
static CPP20CONSTEXPR bool is_done(int n_taps_half,
                                   const std::vector<int>& Ext,
                                   const std::vector<double>& E,
                                   double tolerance = 0.0001)
{
    // FIXME default tolerance of 1e-4 seems very high

    const double initial = E[Ext[0]];
    double min = fabs(initial);
    double max = min;

    for (int i = 1; i <= n_taps_half; i++) {
        double current = fabs(E[Ext[i]]);
        if (current < min)
            min = current;
        if (current > max)
            max = current;
    }
    return (((max - min) / max) < tolerance);
}

/********************
 * remez
 *=======
 * Calculates the optimal (in the Chebyshev/minimax sense)
 * FIR filter impulse response given a set of band edges,
 * the desired response on those bands, and the weight given to
 * the error in those bands.
 *
 * INPUT:
 * ------
 * unsigned int numtaps - Number of filter coefficients
 * unsgined int numband - Number of bands in filter specification
 * double  bands[]      - User-specified band edges [2 * numband]
 * double  response[]   - User-specified band responses [2 * numband]
 * double  weight[]     - User-specified error weights [numband]
 * filter_type type     - Type of filter
 *
 * OUTPUT:
 * -------
 * double h[]      - Impulse response of final filter [numtaps]
 * returns         - true on success, false on failure to converge
 ********************/

static int remez(std::vector<double>& h,
                 unsigned int numtaps,
                 unsigned int numband,
                 const std::vector<double>& bands,
                 const std::vector<double>& response,
                 const std::vector<double>& weight,
                 filter_type type,
                 int griddensity)
{
    symmetry_type symmetry = (type == filter_type::BANDPASS) ? symmetry_type::POSITIVE
                                                             : symmetry_type::NEGATIVE;

    int n_extrema = numtaps / 2; /* number of extrema */
    if ((numtaps % 2) && (symmetry == symmetry_type::POSITIVE))
        n_extrema++;

    /*
     * Predict dense grid size in advance for memory allocation
     *   .5 is so we round up, not truncate
     */
    unsigned int gridsize = 0;
    for (unsigned int i = 0; i < numband; i++) {
        gridsize +=
            (int)(2 * n_extrema * griddensity * (bands[2 * i + 1] - bands[2 * i]) + .5);
    }
    if (symmetry == symmetry_type::NEGATIVE) {
        gridsize--;
    }

    /*
     * Dynamically allocate memory for arrays with proper sizes
     */
    std::vector<double> Grid(gridsize);
    std::vector<double> D(gridsize);
    std::vector<double> W(gridsize);
    std::vector<double> E(gridsize);
    std::vector<int> Ext((n_extrema + 1));
    std::vector<double> taps((n_extrema + 1));
    std::vector<double> x((n_extrema + 1));
    std::vector<double> y((n_extrema + 1));
    std::vector<double> ad((n_extrema + 1));


    /*
     * Create dense frequency grid
     */
    create_dense_grid(n_extrema,
                      numtaps,
                      numband,
                      bands,
                      response,
                      weight,
                      gridsize,
                      Grid,
                      D,
                      W,
                      symmetry,
                      griddensity);
    initial_guess(n_extrema, Ext, gridsize);

    /*
     * For Differentiator: (fix grid)
     */
    if (type == filter_type::DIFFERENTIATOR) {
        for (unsigned int i = 0; i < gridsize; i++) {
            /* D[i] = D[i]*Grid[i]; */
            if (D[i] > 0.0001)
                W[i] = W[i] / Grid[i];
        }
    }

    /*
     * For odd or Negative symmetry filters, alter the
     * D[] and W[] according to Parks McClellan
     */
    if (symmetry == symmetry_type::POSITIVE) {
        if (numtaps % 2 == 0) {
            for (unsigned int i = 0; i < gridsize; i++) {
                double c = cos(PI * Grid[i]);
                D[i] /= c;
                W[i] *= c;
            }
        }
    } else {
        if (numtaps % 2) {
            for (unsigned int i = 0; i < gridsize; i++) {
                double c = sin(PI2 * Grid[i]);
                D[i] /= c;
                W[i] *= c;
            }
        } else {
            for (unsigned int i = 0; i < gridsize; i++) {
                double c = sin(PI * Grid[i]);
                D[i] /= c;
                W[i] *= c;
            }
        }
    }

    /*
     * Perform the Remez Exchange algorithm
     */
    unsigned int iter;
    for (iter = 0; iter < MAXITERATIONS; iter++) {
        calc_parms(n_extrema, Ext, Grid, D, W, ad, x, y);
        calc_error(n_extrema, ad, x, y, gridsize, Grid, D, W, E);
        int err = search(n_extrema, Ext, gridsize, E);
        if (err) {
            return err;
        }
        for (int i = 0; i <= n_extrema; i++)
            assert(Ext[i] < gridsize);
        if (is_done(n_extrema, Ext, E))
            break;
    }

    calc_parms(n_extrema, Ext, Grid, D, W, ad, x, y);

    /*
     * Find the 'taps' of the filter for use with Frequency
     * Sampling.  If odd or Negative symmetry, fix the taps
     * according to Parks McClellan
     */
    for (unsigned int i = 0; i <= numtaps / 2; i++) {
        double c;
        if (symmetry == symmetry_type::POSITIVE) {
            if (numtaps % 2)
                c = 1;
            else
                c = cos(PI * (double)i / numtaps);
        } else {
            if (numtaps % 2)
                c = sin(PI2 * (double)i / numtaps);
            else
                c = sin(PI * (double)i / numtaps);
        }
        taps[i] = compute_A((double)i / numtaps, n_extrema, ad, x, y) * c;
    }

    /*
     * Frequency sampling design with calculated taps
     */
    freq_sample(numtaps, taps, h, symmetry);

    return iter < MAXITERATIONS ? 0 : -1;
}


template <typename error_t = std::runtime_error>
static void punt(const std::string msg)
{
    gr::logger logger("pm_remez");
    logger.error("{:s}", msg);
    throw error_t(msg);
}
//////////////////////////////////////////////////////////////////////////////
//
//			    GNU Radio interface
//
//////////////////////////////////////////////////////////////////////////////

std::vector<double> pm_remez(int order,
                             const std::vector<double>& arg_bands,
                             const std::vector<double>& arg_response,
                             const std::vector<double>& arg_weight,
                             const std::string filter_type_str,
                             int grid_density) noexcept(false)
{

    int numtaps = order + 1;
    if (numtaps < 4)
        punt<std::invalid_argument>("number of taps must be >= 3");

    int numbands = arg_bands.size() / 2;
    if (numbands < 1 || arg_bands.size() % 2 == 1)
        punt<std::invalid_argument>("must have an even number of band edges");

    for (unsigned int i = 1; i < arg_bands.size(); i++) {
        if (arg_bands[i] < arg_bands[i - 1])
            punt<std::invalid_argument>("band edges must be nondecreasing");
    }

    if (arg_bands[0] < 0 || arg_bands[arg_bands.size() - 1] > 1)
        punt<std::domain_error>("band edges must be in the range [0,1]");

    std::vector<double> bands(numbands * 2);
    // Divide by 2 to fit with the implementation that uses a
    // sample rate of [0, 0.5] instead of [0, 1.0]
    for (int i = 0; i < 2 * numbands; i++)
        bands[i] = arg_bands[i] / 2;

    if (arg_response.size() != arg_bands.size())
        punt<std::invalid_argument>(
            "must have one response magnitude for each band edge");

    std::vector<double> response(numbands * 2);
    for (int i = 0; i < 2 * numbands; i++)
        response[i] = arg_response[i];

    std::vector<double> weight(numbands, 1.0);

    if (!arg_weight.empty()) {
        if ((int)arg_weight.size() != numbands)
            punt<std::invalid_argument>(
                "need one weight for each band [=length(band)/2]");
        for (int i = 0; i < numbands; i++)
            weight[i] = arg_weight[i];
    }

    filter_type filttype;
    if (filter_type_str == "bandpass")
        filttype = filter_type::BANDPASS;
    else if (filter_type_str == "differentiator")
        filttype = filter_type::DIFFERENTIATOR;
    else if (filter_type_str == "hilbert")
        filttype = filter_type::HILBERT;
    else
        punt<std::invalid_argument>("unknown ftype '" + filter_type_str + "'");

    if (grid_density < 16)
        punt<std::invalid_argument>("grid_density is too low; must be >= 16");

    std::vector<double> coeff(numtaps + 5); // FIXME why + 5?
    int err =
        remez(coeff, numtaps, numbands, bands, response, weight, filttype, grid_density);

    if (err == -1)
        punt("failed to converge");

    if (err == -2)
        punt("insufficient extremals -- cannot continue");

    if (err == -3)
        punt("too many extremals -- cannot continue");

    return std::vector<double>(&coeff[0], &coeff[numtaps]);
}

} /* namespace filter */
} /* namespace gr */
