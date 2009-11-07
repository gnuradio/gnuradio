/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_USRP2_SBUFF_H
#define INCLUDED_USRP2_SBUFF_H

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdio>

#define USRP2_IMPL_DEBUG 0
#if USRP2_IMPL_DEBUG
#define DEBUG_LOG(x) ::write(2, x, 1)
#else
#define DEBUG_LOG(x)
#endif

namespace usrp2 {

    /*******************************************************************
     * This smart buffer class holds a buffer and its length in bytes
     * A special callback can be passed into the sbuff as well.
     * The callback (if set) will be called on deconstruction.
     * 
     * Typically, the callback will free the memory held by the buffer.
     * But this is all up to the creator of the sbuff.
    *******************************************************************/
    class sbuff{
    public:
        //typedef for void no argument function
        typedef boost::function<void()> cb_t;
        typedef boost::shared_ptr<sbuff> sptr;
    private:
        void *d_buff;
        size_t d_len;
        cb_t d_cb;
    public:
        static sptr make(void *buff, size_t len, cb_t cb){
            return sptr(new sbuff(buff, len, cb));
        }
        static sptr make(void *buff, size_t len){
            return sptr(new sbuff(buff, len, NULL));
        }
        static sptr make(){
            return sptr(new sbuff(NULL, 0, NULL));
        }
        sbuff(void *buff, size_t len, cb_t cb)
         : d_buff(buff), d_len(len), d_cb(cb){}
        ~sbuff(){done();}
        //access methods
        void *buff(){return d_buff;}
        size_t len(){return d_len;}
        /*!
         * \brief mark this sbuff as done
         * This method allows one to explicitly tell the sbuff that its no longer needed.
         * Doing so will make the callback (if set) and zero out the other data.
         *
         * Although this method will be called automatically when the sptr calls delete,
         * it is useful for the fast-path to have the ability to call done explicitly.
         */
        void done(){
            if (d_cb) d_cb();
            d_buff = NULL;
            d_len = 0;
            d_cb = NULL;
        }

    };

}  // namespace usrp2


#endif /* INCLUDED_USRP2_SBUFF_H */
