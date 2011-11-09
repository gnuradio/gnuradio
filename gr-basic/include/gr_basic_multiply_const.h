/*
 * Copyright 2011 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_GR_BASIC_MULTIPLY_CONST_H
#define INCLUDED_GR_BASIC_MULTIPLY_CONST_H

#include <gr_basic_api.h>
#include <gr_sync_block.h>
#include <gr_basic_op_types.h>
#include <complex>

class GR_BASIC_API gr_basic_multiply_const : virtual public gr_sync_block{
public:
    typedef boost::shared_ptr<gr_basic_multiply_const> sptr;

    static sptr make(op_type type, const size_t vlen = 1);

    //! Set the value from any vector type
    template <typename type> void set_value(const std::vector<type> &val){
        std::vector<std::complex<double> > new_val;
        for (size_t i = 0; i < val.size(); i++){
            new_val.push_back(std::complex<double>(val[i]));
        }
        return this->_set_value(new_val);
    }

    //! Set the value when vlen == 1
    template <typename type> void set_value(const type &val){
        return this->set_value(std::vector<type>(1, val));
    }

private:
    virtual void _set_value(const std::vector<std::complex<double> > &val) = 0;

};

#endif /* INCLUDED_GR_BASIC_MULTIPLY_CONST_H */
