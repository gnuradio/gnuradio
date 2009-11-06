/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "transport.h"
#include <stdexcept>
#include <cstdio>

usrp2::transport::transport(const std::string &type_str){
    d_cb = NULL;
    d_type_str = type_str;
    d_running = false;
}

usrp2::transport::~transport(){
    if (d_running) stop();
}

void usrp2::transport::start(){
    if (not d_cb){
        throw std::runtime_error("usrp2::transport for" + d_type_str + " has no callback\n");
    }
    if (d_running){
        throw std::runtime_error("usrp2::transport for" + d_type_str + " already started\n");
    }
    d_running = true;
    d_thread = new boost::thread(boost::bind(&usrp2::transport::run, this));
}

void usrp2::transport::stop(){
    if (not d_running){
        throw std::runtime_error("usrp2::transport for" + d_type_str + " already stopped\n");
    }
    d_running = false;
    d_thread->join();
}

void usrp2::transport::run(){
    init();
    while (d_running){
        try{
            // call recv to get a new sbuffer
            // pass the buffer into the callback
            std::vector<sbuff::sptr> sbs = recv();
            if (sbs.size()) d_cb(sbs);
        //catch thread interrupts from join, sleep, etc
        //the running condition will be re-checked
        }catch(boost::thread_interrupted const &){}
    }
}
