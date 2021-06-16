/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

class TimePrecisionClass
{
public:
    TimePrecisionClass(const int timePrecision) { d_timePrecision = timePrecision; }
    virtual ~TimePrecisionClass() = default;
    virtual unsigned int getTimePrecision() const { return d_timePrecision; }
    virtual void setTimePrecision(const unsigned int newPrecision)
    {
        d_timePrecision = newPrecision;
    }

protected:
    unsigned int d_timePrecision;
};
