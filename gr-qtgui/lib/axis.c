/*
 * axis.c
 *
 * Logic to deal with various axes
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/*! \addtogroup axis
 *  @{
 */

/*! \file axis.c
 *  \brief Logic to deal with various axes
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "axis.h"


#define FX_MODE_COUNT 0
#define FX_MODE_RELATIVE 1
#define FX_MODE_ABSOLUTE 2


static double _si_scaling(double val, int max_num, char* prefix, int* exp)
{
    const char prefixes[5] = { ' ', 'k', 'M', 'G', 'T' };
    int exponent = log10f(fabs(val));
    int d = exponent >= max_num ? ((exponent - max_num + 3) / 3) : 0;

    if (prefix)
        *prefix = prefixes[d];

    if (exp)
        *exp = d * 3;

    return val / powf(10.0, d * 3);
}

static int _num_decimals(double val)
{
    int c;
    double v;

    for (c = 0; c < 22; c++) {
        v = val * pow(10, c);
        if (fabs(v - round(v)) == 0.0)
            return c;
    }

    return -1;
}


void freq_axis_build(struct freq_axis* fx, double center, double span, int n_div)
{
    /* Basic info */
    fx->center = center;
    fx->span = span;
    fx->step = span / n_div;

    /* Select mode of operation */
    if (span == 0.0) {
        fx->mode = FX_MODE_COUNT;
    } else if (center == 0.0) {
        fx->mode = FX_MODE_RELATIVE;
    } else if (floor(log10f(fx->step)) < (floor(log10f(fx->center)) - 4)) {
        fx->mode = FX_MODE_RELATIVE;
    } else {
        fx->mode = FX_MODE_ABSOLUTE;
    }

    /* Select display format for absolute frequencies */
    if (center != 0.0) {
        double min_freq, max_freq, big_freq;
        char prefix[2] = { 0, 0 };
        int exp, x, y, z;

        max_freq = fx->center + (span / 2.0);
        min_freq = fx->center - (span / 2.0);

        big_freq = (fabs(max_freq) > fabs(min_freq)) ? fabs(max_freq) : fabs(min_freq);

        _si_scaling(big_freq, 4, prefix, &exp);

        if (prefix[0] == ' ')
            prefix[0] = '\0';

        fx->abs_scale = 1.0 / powf(10.0, exp);

        x = (int)floor(log10f(big_freq)) - exp + 1;
        y = _num_decimals(fx->center * fx->abs_scale);
        z = _num_decimals(fx->step * fx->abs_scale);

        if (z > y)
            y = z;

        if (x + y > 6)
            y = 6 - x;

        sprintf(fx->abs_fmt, "%%.%dlf%s", y, prefix);
    }

    /* Select display format for relative mode */
    if (fx->mode == FX_MODE_RELATIVE) {
        double max_dev = fx->step * 5;
        char prefix[2] = { 0, 0 };
        int exp, x, y;

        _si_scaling(max_dev, 3, prefix, &exp);

        if (prefix[0] == ' ')
            prefix[0] = '\0';

        fx->rel_step = fx->step / powf(10.0, exp);

        x = (int)floor(log10f(max_dev)) - exp + 1;
        y = _num_decimals(fx->rel_step);

        if (x + y > 4)
            y = 4 - x;

        sprintf(fx->rel_fmt, "%%+.%dlf%s", y, prefix);
    }
}

void freq_axis_render(struct freq_axis* fx, char* str, int step)
{
    /* Special case: count mode */
    if (step && (fx->mode == FX_MODE_COUNT)) {
        sprintf(str, "%+d", step);
        return;
    }

    /* Special case: no center frequency */
    if (!step && (fx->center == 0.0)) {
        sprintf(str, "0");
        return;
    }

    /* Relative case */
    if (step && (fx->mode == FX_MODE_RELATIVE)) {
        sprintf(str, fx->rel_fmt, step * fx->rel_step);
        return;
    }

    /* Normal full absolute frequency */
    sprintf(str, fx->abs_fmt, (fx->center + step * fx->step) * fx->abs_scale);
}

/*! @} */
