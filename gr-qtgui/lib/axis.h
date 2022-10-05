/*
 * axis.h
 *
 * Logic to deal with various axises
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef __FOSPHOR_AXIS_H__
#define __FOSPHOR_AXIS_H__

/*! \defgroup axis Fosphor axis-related functions
 *  @{
 */

/*! \file axis.h
 *  \brief Logic to deal with various axises
 */

struct freq_axis {
    double center;
    double span;
    double step;
    int mode;
    char abs_fmt[32];
    double abs_scale;
    char rel_fmt[32];
    double rel_step;
};

void freq_axis_build(struct freq_axis* fx, double center, double span, int n_div);
void freq_axis_render(struct freq_axis* fx, char* str, int step);

/*! @} */

#endif /* __FOSPHOR_AXIS_H__ */
