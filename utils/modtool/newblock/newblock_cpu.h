/* -*- c++ -*- */
/*
 * Copyright <COPYRIGHT_YEAR> <COPYRIGHT_AUTHOR>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/newmod/newblock.h>

namespace gr {
namespace newmod {

class newblock_cpu : public virtual newblock
{
public:
    newblock_cpu(block_args args);
    work_return_code_t work(work_io& wio) override;

private:
    // private variables here
};

} // namespace newmod
} // namespace gr