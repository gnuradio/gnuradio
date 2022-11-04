//
//  Copyright 2004,2005,2009 Free Software Foundation, Inc.
//
//  This file is part of GNU Radio
//
//  SPDX-License-Identifier: GPL-3.0-or-later
//
//

#include <gnuradio/kernel/filter/optfir.h>
#include <gnuradio/kernel/filter/pm_remez.h>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

namespace gr {
namespace kernel {
namespace filter {

double optfir::lporder(double freq1, double freq2, double delta_p, double delta_s)
{
    auto df = std::abs<double>(freq2 - freq1);
    auto ddp = log10(delta_p);
    auto dds = log10(delta_s);

    double a1 = 5.309e-3;
    double a2 = 7.114e-2;
    double a3 = -4.761e-1;
    double a4 = -2.66e-3;
    double a5 = -5.941e-1;
    double a6 = -4.278e-1;

    double b1 = 11.01217;
    double b2 = 0.5124401;

    auto t1 = a1 * ddp * ddp;
    auto t2 = a2 * ddp;
    auto t3 = a4 * ddp * ddp;
    auto t4 = a5 * ddp;

    auto dinf = ((t1 + t2 + a3) * dds) + (t3 + t4 + a6);
    auto ff = b1 + b2 * (ddp - dds);
    auto n = dinf / df - ff * df + 1;
    return n;
}
std::tuple<size_t, std::vector<double>, std::vector<double>, std::vector<double>>
optfir::remezord(std::vector<double> fcuts,
                 std::vector<double> mags,
                 std::vector<double> devs,
                 double fsamp)
{
    for (auto& f : fcuts) {
        f = float(f) / fsamp;
    }

    auto nf = fcuts.size();
    auto nm = mags.size();
    auto nd = devs.size();
    auto nbands = nm;

    if (nm != nd)
        throw std::range_error("Length of mags and devs must be equal");

    if (nf != 2 * (nbands - 1)) {
        throw std::range_error("Length of f must be 2 * len (mags) - 2");
    }

    for (size_t i = 0; i < mags.size(); i++) {
        if (mags[i] != 0) { // if not stopband, get relative deviation
            devs[i] = devs[i] / mags[i];
        }
    }

    // separate the passband and stopband edges
    std::vector<double> f1, f2;
    for (size_t i = 0; i < fcuts.size(); i += 2) {
        f1.push_back(fcuts[i]);
    }
    for (size_t i = 1; i < fcuts.size(); i += 2) {
        f2.push_back(fcuts[i]);
    }

    size_t n = 0;
    double min_delta = 2;
    for (size_t i = 0; i < f1.size(); i++) {
        if (f2[i] - f1[i] < min_delta) {
            n = i;
            min_delta = f2[i] - f1[i];
        }
    }

    double l = 0;
    if (nbands == 2) {
        // lowpass or highpass case (use formula)
        l = lporder(f1[n], f2[n], devs[0], devs[1]);
    }
    else {
        // bandpass or multipass case
        // try different lowpasses and take the worst one that
        //  goes through the BP specs
        l = 0;
        for (size_t i = 1; i < nbands - 1; i++) {
            double l1 = lporder(f1[i - 1], f2[i - 1], devs[i], devs[i - 1]);
            double l2 = lporder(f1[i], f2[i], devs[i], devs[i + 1]);
            l = std::max(l, std::max(l1, l2));
        }
    }

    n = (int)ceil(l) - 1; // need order, not length for remez

    // cook up remez compatible result
    std::vector<double> ff = fcuts;
    ff.push_back(1.0);
    ff.insert(ff.begin(), 0.0);
    for (size_t i = 1; i < ff.size() - 1; i++) {
        ff[i] *= 2;
    }


    std::vector<double> aa;
    for (auto& a : mags) {
        aa.push_back(a);
        aa.push_back(a);
        // aa = aa + [a, a]
    }

    auto max_dev = *std::max_element(devs.begin(), devs.end());
    // wts = [1] * len(devs)
    std::vector<double> wts(devs.size());
    std::fill_n(wts.begin(), wts.size(), 1.0);

    for (size_t i = 0; i < wts.size(); i++) {
        wts[i] = max_dev / devs[i];
    }

    return std::make_tuple(n, ff, aa, wts);
}
double optfir::stopband_atten_to_dev(double atten_db)
{
    return pow(10, (-atten_db / 20));
}

double optfir::passband_ripple_to_dev(double ripple_db)
{
    return ((pow(10, (ripple_db / 20)) - 1) / (pow(10, (ripple_db / 20)) + 1));
}
std::vector<double> optfir::low_pass(double gain,
                                     double Fs,
                                     double freq1,
                                     double freq2,
                                     double passband_ripple_db,
                                     double stopband_atten_db,
                                     size_t nextra_taps)
{
    auto passband_dev = passband_ripple_to_dev(passband_ripple_db);
    auto stopband_dev = stopband_atten_to_dev(stopband_atten_db);
    std::vector<double> desired_ampls{ gain, 0.0 };
    // (n, fo, ao, w)
    auto tup =
        remezord({ freq1, freq2 }, desired_ampls, { passband_dev, stopband_dev }, Fs);
    // The remezord typically under-estimates the filter order, so add 2 taps by
    // default
    auto taps = pm_remez(std::get<0>(tup) + nextra_taps,
                         std::get<1>(tup),
                         std::get<2>(tup),
                         std::get<3>(tup),
                         "bandpass");
    return taps;
}

} // namespace filter
} // namespace kernel
} // namespace gr
