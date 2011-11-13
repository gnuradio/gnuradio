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

#include <gr_basic_stream_selector.h>
#include <gruel/thread.h>
#include <gr_sync_block.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <cstring> //memcpy
#include <iostream>

/***********************************************************************
 * A single selector output
 **********************************************************************/
class gr_basic_stream_selector_output : public gr_sync_block{
public:
    gr_basic_stream_selector_output(const size_t item_size):
        gr_sync_block(
            "stream selector output",
            gr_make_io_signature (0, 0, 0),
            gr_make_io_signature (1, 1, item_size)
        ),
        _item_size(item_size),
        _has_data(false)
    {
        //NOP
    }

    size_t post_output(const void *mem, const size_t len){
        //guarantees that only one caller enters at a time
        gruel::scoped_lock caller_lock(_caller_mutex);

        gruel::scoped_lock lock(_mutex);
        _mem = mem;
        _len = len;
        _has_data = true;
        _cond.notify_one();
        while (_has_data){
            _cond.wait(lock);
        }
        return _len;
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &,
        gr_vector_void_star &output_items
    ){
        gruel::scoped_lock lock(_mutex);
        while (!_has_data){
            _cond.wait(lock);
        }
        _len = std::min(_len, noutput_items*_item_size);
        std::memcpy(output_items[0], _mem, _len);
        _has_data = false;
        _cond.notify_one();
        return _len/_item_size;
    }

private:
    const size_t _item_size;
    gruel::mutex _mutex;
    gruel::mutex _caller_mutex;
    gruel::condition_variable _cond;
    const void *_mem;
    size_t _len;
    bool _has_data;
};

/***********************************************************************
 * A single selector input
 **********************************************************************/
class gr_basic_stream_selector_input : public gr_sync_block{
public:
    gr_basic_stream_selector_input(const size_t item_size):
        gr_sync_block(
            "stream selector input",
            gr_make_io_signature (1, 1, item_size),
            gr_make_io_signature (0, 0, 0)
        ),
        _item_size(item_size)
    {
        this->set_output(NULL);
    }

    void set_output(gr_basic_stream_selector_output *output, bool block = true){
        gruel::scoped_lock lock(_mutex);
        _output = output;
        _block = block;
        lock.unlock();
        _blocker.notify_one();
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &
    ){
        gruel::scoped_lock lock(_mutex);
        while (true){
            if (_output != NULL){
                return _output->post_output(input_items[0], noutput_items*_item_size)/_item_size;
            }
            if (_block){
                _blocker.wait(lock);
                continue;
            }
            else{ //consume all
                return noutput_items;
            }
        }
    }

private:
    const size_t _item_size;
    gruel::mutex _mutex;
    gruel::condition_variable _blocker;
    gr_basic_stream_selector_output *_output;
    bool _block;
};

/***********************************************************************
 * The selector implementation glue
 **********************************************************************/
class gr_basic_stream_selector_impl : public gr_basic_stream_selector{
public:
    gr_basic_stream_selector_impl(
        gr_io_signature_sptr in_sig,
        gr_io_signature_sptr out_sig
    ):
        gr_hier_block2(
            "stream selector impl",
            in_sig, out_sig
        )
    {
        //sanity check IO signatures
        if (in_sig->min_streams() != in_sig->max_streams() || in_sig->min_streams() < 1){
            throw std::invalid_argument("stream selector input signature invalid");
        }
        if (out_sig->min_streams() != out_sig->max_streams() || out_sig->min_streams() < 1){
            throw std::invalid_argument("stream selector output signature invalid");
        }

        //create and connect input blocks
        for (size_t i = 0; i < size_t(in_sig->min_streams()); i++){
            _inputs.push_back(boost::shared_ptr<gr_basic_stream_selector_input>(
                new gr_basic_stream_selector_input(in_sig->sizeof_stream_item(i))
            ));
            this->connect(this->self(), i, _inputs.back(), 0);
        }

        //create and connect output blocks
        for (size_t i = 0; i < size_t(out_sig->min_streams()); i++){
            _outputs.push_back(boost::shared_ptr<gr_basic_stream_selector_output>(
                new gr_basic_stream_selector_output(out_sig->sizeof_stream_item(i))
            ));
            this->connect(_outputs.back(), 0, this->self(), i);
        }
    }

    void set_paths(const std::vector<int> &paths){
        if (paths.size() != _inputs.size()){
            throw std::invalid_argument("stream selector set paths wrong length");
        }

        //set all inputs to block before we apply the new paths
        for (size_t i = 0; i < paths.size(); i++){
            _inputs.at(i)->set_output(NULL, true/*block*/);
        }

        //apply the new path to each input
        for (size_t i = 0; i < paths.size(); i++){
            if (paths[i] == -1){
                _inputs.at(i)->set_output(NULL, true/*block*/);
            }
            else if (paths[i] == -2){
                _inputs.at(i)->set_output(NULL, false/*consume*/);
            }
            else if (size_t(paths[i]) >= _outputs.size()){
                throw std::invalid_argument("stream selector output index invalid");
            }
            else{
                _inputs.at(i)->set_output(_outputs.at(paths[i]).get());
            }
        }
    }

private:
    std::vector<boost::shared_ptr<gr_basic_stream_selector_input> > _inputs;
    std::vector<boost::shared_ptr<gr_basic_stream_selector_output> > _outputs;
};

/***********************************************************************
 * Factory function
 **********************************************************************/
gr_basic_stream_selector::sptr gr_basic_stream_selector::make(
    gr_io_signature_sptr in_sig, gr_io_signature_sptr out_sig
){
    return sptr(new gr_basic_stream_selector_impl(in_sig, out_sig));
}
