/* -*- c++ -*- */
/*
 * Copyright 2002,2007,2008,2012,2013,2018,2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/fft/window.h>
#include <gnuradio/math.h>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace gr {
namespace fft {

#define IzeroEPSILON 1E-21 /* Max error acceptable in Izero */

static double Izero(double x)
{
    double sum, u, halfx, temp;
    int n;

    sum = u = n = 1;
    halfx = x / 2.0;
    do {
        temp = halfx / (double)n;
        n += 1;
        temp *= temp;
        u *= temp;
        sum += u;
    } while (u >= IzeroEPSILON * sum);
    return (sum);
}

double midn(int ntaps) { return ntaps / 2.0; }

double midm1(int ntaps) { return (ntaps - 1.0) / 2.0; }

double midp1(int ntaps) { return (ntaps + 1.0) / 2.0; }

double freq(int ntaps) { return 2.0 * GR_M_PI / ntaps; }

double rate(int ntaps) { return 1.0 / (ntaps >> 1); }

double window::max_attenuation(win_type type, double param)
{
    switch (type) {
    case (WIN_HAMMING):
        return 53;
    case (WIN_HANN):
        return 44;
    case (WIN_BLACKMAN):
        return 74;
    case (WIN_RECTANGULAR):
        return 21;
    case (WIN_KAISER):
        // linear approximation
        return (param / 0.1102 + 8.7);
    case (WIN_BLACKMAN_hARRIS):
        return 92;
    case (WIN_BARTLETT):
        return 27;
    case (WIN_FLATTOP):
        return 93;
    case WIN_NUTTALL:
        return 114;
    case WIN_NUTTALL_CFD:
        return 112;
    case WIN_WELCH:
        return 31;
    case WIN_PARZEN:
        return 56;
    case WIN_EXPONENTIAL:
        // varies slightly depending on the decay factor, but this is a safe return value
        return 26;
    case WIN_RIEMANN:
        return 39;
    case WIN_GAUSSIAN:
        // value not meaningful for gaussian windows, but return something reasonable
        return 100;
    case WIN_TUKEY:
        // low end is a rectangular window, attenuation exponentially approaches Hann
        // piecewise linear estimate, determined empirically via curve fitting, median
        // error is less than 0.5dB and maximum error is 2.5dB; the returned value will
        // never be less than expected attenuation to ensure that window designed filters
        // are never below expected quality.
        if (param > 0.9)
            return ((param - 0.9) * 135 + 30.5);
        else if (param > 0.7)
            return ((param - 0.6) * 20 + 24);
        return (param * 5 + 21);
    default:
        throw std::out_of_range("window::max_attenuation: unknown window type provided.");
    }
}

std::vector<float> window::coswindow(int ntaps, float c0, float c1, float c2)
{
    std::vector<float> taps(ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = c0 - c1 * cosf((2.0f * GR_M_PI * n) / M) +
                  c2 * cosf((4.0f * GR_M_PI * n) / M);
    return taps;
}

std::vector<float> window::coswindow(int ntaps, float c0, float c1, float c2, float c3)
{
    std::vector<float> taps(ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = c0 - c1 * cosf((2.0f * GR_M_PI * n) / M) +
                  c2 * cosf((4.0f * GR_M_PI * n) / M) -
                  c3 * cosf((6.0f * GR_M_PI * n) / M);
    return taps;
}

std::vector<float>
window::coswindow(int ntaps, float c0, float c1, float c2, float c3, float c4)
{
    std::vector<float> taps(ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = c0 - c1 * cosf((2.0f * GR_M_PI * n) / M) +
                  c2 * cosf((4.0f * GR_M_PI * n) / M) -
                  c3 * cosf((6.0f * GR_M_PI * n) / M) +
                  c4 * cosf((8.0f * GR_M_PI * n) / M);
    return taps;
}

std::vector<float> window::rectangular(int ntaps)
{
    std::vector<float> taps(ntaps);
    for (int n = 0; n < ntaps; n++)
        taps[n] = 1;
    return taps;
}

std::vector<float> window::hamming(int ntaps)
{
    std::vector<float> taps(ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = 0.54 - 0.46 * cos((2 * GR_M_PI * n) / M);
    return taps;
}

std::vector<float> window::hann(int ntaps)
{
    std::vector<float> taps(ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = 0.5 - 0.5 * cos((2 * GR_M_PI * n) / M);
    return taps;
}

std::vector<float> window::hanning(int ntaps) { return hann(ntaps); }

std::vector<float> window::blackman(int ntaps)
{
    return coswindow(ntaps, 0.42, 0.5, 0.08);
}

std::vector<float> window::blackman2(int ntaps)
{
    return coswindow(ntaps, 0.34401, 0.49755, 0.15844);
}

std::vector<float> window::blackman3(int ntaps)
{
    return coswindow(ntaps, 0.21747, 0.45325, 0.28256, 0.04672);
}

std::vector<float> window::blackman4(int ntaps)
{
    return coswindow(ntaps, 0.084037, 0.29145, 0.375696, 0.20762, 0.041194);
}

std::vector<float> window::blackman_harris(int ntaps, int atten)
{
    switch (atten) {
    case (61):
        return coswindow(ntaps, 0.42323, 0.49755, 0.07922);
    case (67):
        return coswindow(ntaps, 0.44959, 0.49364, 0.05677);
    case (74):
        return coswindow(ntaps, 0.40271, 0.49703, 0.09392, 0.00183);
    case (92):
        return coswindow(ntaps, 0.35875, 0.48829, 0.14128, 0.01168);
    default:
        throw std::out_of_range("window::blackman_harris: unknown attenuation value "
                                "(must be 61, 67, 74, or 92)");
    }
}

std::vector<float> window::blackmanharris(int ntaps, int atten)
{
    return blackman_harris(ntaps, atten);
}

std::vector<float> window::nuttall(int ntaps)
{
    return coswindow(ntaps, 0.3635819, 0.4891775, 0.1365995, 0.0106411);
}

std::vector<float> window::blackman_nuttall(int ntaps) { return nuttall(ntaps); }
std::vector<float> window::nuttall_cfd(int ntaps)
{
    return coswindow(ntaps, 0.355768, 0.487396, 0.144232, 0.012604);
}

std::vector<float> window::flattop(int ntaps)
{
    double scale = 4.63867;
    return coswindow(
        ntaps, 1.0 / scale, 1.93 / scale, 1.29 / scale, 0.388 / scale, 0.028 / scale);
}

std::vector<float> window::kaiser(int ntaps, double beta)
{
    if (beta < 0)
        throw std::out_of_range("window::kaiser: beta must be >= 0");

    std::vector<float> taps(ntaps);

    double IBeta = 1.0 / Izero(beta);
    double inm1 = 1.0 / ((double)(ntaps - 1));
    double temp;

    /* extracting first and last element out of the loop, since
       sqrt(1.0-temp*temp) might trigger unexpected floating point behaviour
       if |temp| = 1.0+epsilon, which can happen for i==0 and
       1/i==1/(ntaps-1)==inm1 ; compare
       https://github.com/gnuradio/gnuradio/issues/1348 .
       In any case, the 0. Bessel function of first kind is 1 at point 0.
     */
    taps[0] = IBeta;
    for (int i = 1; i < ntaps - 1; i++) {
        temp = 2 * i * inm1 - 1;
        taps[i] = Izero(beta * sqrt(1.0 - temp * temp)) * IBeta;
    }
    taps[ntaps - 1] = IBeta;
    return taps;
}

std::vector<float> window::bartlett(int ntaps)
{
    std::vector<float> taps(ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps / 2; n++)
        taps[n] = 2 * n / M;
    for (int n = ntaps / 2; n < ntaps; n++)
        taps[n] = 2 - 2 * n / M;

    return taps;
}

std::vector<float> window::welch(int ntaps)
{
    std::vector<float> taps(ntaps);
    double m1 = midm1(ntaps);
    double p1 = midp1(ntaps);
    for (int i = 0; i < midn(ntaps) + 1; i++) {
        taps[i] = 1.0 - pow((i - m1) / p1, 2);
        taps[ntaps - i - 1] = taps[i];
    }
    return taps;
}

std::vector<float> window::parzen(int ntaps)
{
    std::vector<float> taps(ntaps);
    double m1 = midm1(ntaps);
    double m = midn(ntaps);
    int i;
    for (i = ntaps / 4; i < 3 * ntaps / 4; i++) {
        taps[i] = 1.0 - 6.0 * pow((i - m1) / m, 2.0) * (1.0 - fabs(i - m1) / m);
    }
    for (; i < ntaps; i++) {
        taps[i] = 2.0 * pow(1.0 - fabs(i - m1) / m, 3.0);
        taps[ntaps - i - 1] = taps[i];
    }
    return taps;
}

std::vector<float> window::exponential(int ntaps, double d)
{
    if (d < 0)
        throw std::out_of_range("window::exponential: d must be >= 0");

    double m1 = midm1(ntaps);
    double p = 1.0 / (8.69 * ntaps / (2.0 * d));
    std::vector<float> taps(ntaps);
    for (int i = 0; i < midn(ntaps) + 1; i++) {
        taps[i] = exp(-fabs(i - m1) * p);
        taps[ntaps - i - 1] = taps[i];
    }
    return taps;
}

std::vector<float> window::riemann(int ntaps)
{
    double cx;
    double sr1 = freq(ntaps);
    double mid = midn(ntaps);
    std::vector<float> taps(ntaps);
    for (int i = 0; i < mid; i++) {
        if (i == midn(ntaps)) {
            taps[i] = 1.0;
            taps[ntaps - i - 1] = 1.0;
        } else {
            cx = sr1 * (i - mid);
            taps[i] = sin(cx) / cx;
            taps[ntaps - i - 1] = sin(cx) / cx;
        }
    }
    return taps;
}

std::vector<float> window::tukey(int ntaps, float alpha)
{
    if ((alpha < 0) || (alpha > 1))
        throw std::out_of_range("window::tukey: alpha must be between 0 and 1");

    float N = static_cast<float>(ntaps - 1);

    float aN = alpha * N;
    float p1 = aN / 2.0;
    float mid = midn(ntaps);
    std::vector<float> taps(ntaps);
    for (int i = 0; i < mid; i++) {
        if (abs(i) < p1) {
            taps[i] = 0.5 * (1.0 - cos((2 * GR_M_PI * i) / (aN)));
            taps[ntaps - 1 - i] = taps[i];
        } else {
            taps[i] = 1.0;
            taps[ntaps - i - 1] = 1.0;
        }
    }
    return taps;
}

std::vector<float> window::gaussian(int ntaps, float sigma)
{
    if (sigma <= 0)
        throw std::out_of_range("window::gaussian: sigma must be > 0");

    float a = 2 * sigma * sigma;
    double m1 = midm1(ntaps);
    std::vector<float> taps(ntaps);
    for (int i = 0; i < midn(ntaps); i++) {
        float N = (i - m1);
        taps[i] = exp(-(N * N / a));
        taps[ntaps - 1 - i] = taps[i];
    }
    return taps;
}

std::vector<float>
window::build(win_type type, int ntaps, double param, const bool normalize)
{
    // If we want a normalized window, we get a non-normalized one first, then
    // normalize it here:
    if (normalize) {
        auto win = build(type, ntaps, param, false);
        const double pwr_acc = // sum(win**2) / len(win)
            std::accumulate(win.cbegin(),
                            win.cend(),
                            0.0,
                            [](const double a, const double b) { return a + b * b; }) /
            win.size();
        const float norm_fac = static_cast<float>(std::sqrt(pwr_acc));
        std::transform(win.begin(), win.end(), win.begin(), [norm_fac](const float tap) {
            return tap / norm_fac;
        });
        return win;
    }

    // Create non-normalized window:
    switch (type) {
    case WIN_RECTANGULAR:
        return rectangular(ntaps);
    case WIN_HAMMING:
        return hamming(ntaps);
    case WIN_HANN:
        return hann(ntaps);
    case WIN_BLACKMAN:
        return blackman(ntaps);
    case WIN_BLACKMAN_hARRIS:
        return blackman_harris(ntaps);
    case WIN_KAISER:
        return kaiser(ntaps, param);
    case WIN_BARTLETT:
        return bartlett(ntaps);
    case WIN_FLATTOP:
        return flattop(ntaps);
    case WIN_NUTTALL:
        return nuttall(ntaps);
    case WIN_NUTTALL_CFD:
        return nuttall_cfd(ntaps);
    case WIN_WELCH:
        return welch(ntaps);
    case WIN_PARZEN:
        return parzen(ntaps);
    case WIN_EXPONENTIAL:
        return exponential(ntaps, param);
    case WIN_RIEMANN:
        return riemann(ntaps);
    case WIN_GAUSSIAN:
        return gaussian(ntaps, param);
    case WIN_TUKEY:
        return tukey(ntaps, param);
    default:
        throw std::out_of_range("window::build: type out of range");
    }
}

} /* namespace fft */
} /* namespace gr */
