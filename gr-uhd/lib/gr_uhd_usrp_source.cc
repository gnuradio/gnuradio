/*
 * Copyright 2010-2011 Free Software Foundation, Inc.
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

#include <gr_uhd_usrp_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

static const pmt::pmt_t TIME_KEY = pmt::pmt_string_to_symbol("rx_time");

#include <uhd/convert.hpp>
inline gr_io_signature_sptr args_to_io_sig(const uhd::stream_args_t &args){
    const size_t nchan = std::max<size_t>(args.channels.size(), 1);
    #ifdef GR_UHD_USE_STREAM_API
        const size_t size = uhd::convert::get_bytes_per_item(args.cpu_format);
    #else
        size_t size = 0;
        if (args.cpu_format == "fc32") size = 8;
        if (args.cpu_format == "sc16") size = 4;
    #endif
    return gr_make_io_signature(nchan, nchan, size);
}

/***********************************************************************
 * UHD Multi USRP Source Impl
 **********************************************************************/
class uhd_usrp_source_impl : public uhd_usrp_source{
public:
    uhd_usrp_source_impl(
        const uhd::device_addr_t &device_addr,
        const uhd::stream_args_t &stream_args
    ):
        gr_sync_block(
            "gr uhd usrp source",
            gr_make_io_signature(0, 0, 0),
            args_to_io_sig(stream_args)
        ),
        _stream_args(stream_args),
        _nchan(std::max<size_t>(1, stream_args.channels.size())),
        _stream_now(_nchan == 1),
        _tag_now(false)
    {
        if (stream_args.cpu_format == "fc32") _type = boost::make_shared<uhd::io_type_t>(uhd::io_type_t::COMPLEX_FLOAT32);
        if (stream_args.cpu_format == "sc16") _type = boost::make_shared<uhd::io_type_t>(uhd::io_type_t::COMPLEX_INT16);
        std::stringstream str;
        str << name() << unique_id();
        _id = pmt::pmt_string_to_symbol(str.str());
        _dev = uhd::usrp::multi_usrp::make(device_addr);
    }

    void set_subdev_spec(const std::string &spec, size_t mboard){
        return _dev->set_rx_subdev_spec(spec, mboard);
    }

    void set_samp_rate(double rate){
        _dev->set_rx_rate(rate);
    }

    double get_samp_rate(void){
        return _dev->get_rx_rate();
    }

    uhd::meta_range_t get_samp_rates(void){
        #ifdef UHD_USRP_MULTI_USRP_GET_RATES_API
        return _dev->get_rx_rates();
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    uhd::tune_result_t set_center_freq(
        const uhd::tune_request_t tune_request, size_t chan
    ){
        return _dev->set_rx_freq(tune_request, chan);
    }

    double get_center_freq(size_t chan){
        return _dev->get_rx_freq(chan);
    }

    uhd::freq_range_t get_freq_range(size_t chan){
        return _dev->get_rx_freq_range(chan);
    }

    void set_gain(double gain, size_t chan){
        return _dev->set_rx_gain(gain, chan);
    }

    void set_gain(double gain, const std::string &name, size_t chan){
        return _dev->set_rx_gain(gain, name, chan);
    }

    double get_gain(size_t chan){
        return _dev->get_rx_gain(chan);
    }

    double get_gain(const std::string &name, size_t chan){
        return _dev->get_rx_gain(name, chan);
    }

    std::vector<std::string> get_gain_names(size_t chan){
        return _dev->get_rx_gain_names(chan);
    }

    uhd::gain_range_t get_gain_range(size_t chan){
        return _dev->get_rx_gain_range(chan);
    }

    uhd::gain_range_t get_gain_range(const std::string &name, size_t chan){
        return _dev->get_rx_gain_range(name, chan);
    }

    void set_antenna(const std::string &ant, size_t chan){
        return _dev->set_rx_antenna(ant, chan);
    }

    std::string get_antenna(size_t chan){
        return _dev->get_rx_antenna(chan);
    }

    std::vector<std::string> get_antennas(size_t chan){
        return _dev->get_rx_antennas(chan);
    }

    void set_bandwidth(double bandwidth, size_t chan){
        return _dev->set_rx_bandwidth(bandwidth, chan);
    }

    void set_dc_offset(const bool enable, size_t chan){
        #ifdef UHD_USRP_MULTI_USRP_FRONTEND_CAL_API
        return _dev->set_rx_dc_offset(enable, chan);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    void set_dc_offset(const std::complex<double> &offset, size_t chan){
        #ifdef UHD_USRP_MULTI_USRP_FRONTEND_CAL_API
        return _dev->set_rx_dc_offset(offset, chan);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    void set_iq_balance(const std::complex<double> &correction, size_t chan){
        #ifdef UHD_USRP_MULTI_USRP_FRONTEND_CAL_API
        return _dev->set_rx_iq_balance(correction, chan);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    uhd::sensor_value_t get_sensor(const std::string &name, size_t chan){
        return _dev->get_rx_sensor(name, chan);
    }

    std::vector<std::string> get_sensor_names(size_t chan){
        return _dev->get_rx_sensor_names(chan);
    }

    uhd::sensor_value_t get_mboard_sensor(const std::string &name, size_t mboard){
        return _dev->get_mboard_sensor(name, mboard);
    }

    std::vector<std::string> get_mboard_sensor_names(size_t mboard){
        return _dev->get_mboard_sensor_names(mboard);
    }

    void set_clock_config(const uhd::clock_config_t &clock_config, size_t mboard){
        return _dev->set_clock_config(clock_config, mboard);
    }

    void set_time_source(const std::string &source, const size_t mboard){
        #ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
        return _dev->set_time_source(source, mboard);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    std::string get_time_source(const size_t mboard){
        #ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
        return _dev->get_time_source(mboard);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    std::vector<std::string> get_time_sources(const size_t mboard){
        #ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
        return _dev->get_time_sources(mboard);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    void set_clock_source(const std::string &source, const size_t mboard){
        #ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
        return _dev->set_clock_source(source, mboard);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    std::string get_clock_source(const size_t mboard){
        #ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
        return _dev->get_clock_source(mboard);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    std::vector<std::string> get_clock_sources(const size_t mboard){
        #ifdef UHD_USRP_MULTI_USRP_REF_SOURCES_API
        return _dev->get_clock_sources(mboard);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    double get_clock_rate(size_t mboard){
        return _dev->get_master_clock_rate(mboard);
    }

    void set_clock_rate(double rate, size_t mboard){
        return _dev->set_master_clock_rate(rate, mboard);
    }

    uhd::time_spec_t get_time_now(size_t mboard = 0){
        return _dev->get_time_now(mboard);
    }

    uhd::time_spec_t get_time_last_pps(size_t mboard){
        return _dev->get_time_last_pps(mboard);
    }

    void set_time_now(const uhd::time_spec_t &time_spec, size_t mboard){
        return _dev->set_time_now(time_spec, mboard);
    }

    void set_time_next_pps(const uhd::time_spec_t &time_spec){
        return _dev->set_time_next_pps(time_spec);
    }

    void set_time_unknown_pps(const uhd::time_spec_t &time_spec){
        return _dev->set_time_unknown_pps(time_spec);
    }

    void set_command_time(const uhd::time_spec_t &time_spec, size_t mboard){
        #ifdef UHD_USRP_MULTI_USRP_COMMAND_TIME_API
        return _dev->set_command_time(time_spec, mboard);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    void clear_command_time(size_t mboard){
        #ifdef UHD_USRP_MULTI_USRP_COMMAND_TIME_API
        return _dev->clear_command_time(mboard);
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

    uhd::usrp::dboard_iface::sptr get_dboard_iface(size_t chan){
        return _dev->get_rx_dboard_iface(chan);
    }

    uhd::usrp::multi_usrp::sptr get_device(void){
        return _dev;
    }

/***********************************************************************
 * Work
 **********************************************************************/
    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        #ifdef GR_UHD_USE_STREAM_API
        //In order to allow for low-latency:
        //We receive all available packets without timeout.
        //This call can timeout under regular operation...
        size_t num_samps = _rx_stream->recv(
            output_items, noutput_items, _metadata, 0.0
        );

        //If receive resulted in a timeout condition:
        //We now receive a single packet with a large timeout.
        if (_metadata.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT){
            num_samps = _rx_stream->recv(
                output_items, noutput_items, _metadata, 1.0, true/*one pkt*/
            );
        }
        #else
        size_t num_samps = _dev->get_device()->recv(
            output_items, noutput_items, _metadata,
            *_type, uhd::device::RECV_MODE_FULL_BUFF, 0.0
        );
        if (_metadata.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT){
            num_samps = _dev->get_device()->recv(
                output_items, noutput_items, _metadata,
                *_type, uhd::device::RECV_MODE_ONE_PACKET, 1.0
            );
        }
        #endif

        //handle possible errors conditions
        switch(_metadata.error_code){
        case uhd::rx_metadata_t::ERROR_CODE_NONE:
            if (_tag_now){
                _tag_now = false;
                //create a timestamp pmt for the first sample
                const pmt::pmt_t val = pmt::pmt_make_tuple(
                    pmt::pmt_from_uint64(_metadata.time_spec.get_full_secs()),
                    pmt::pmt_from_double(_metadata.time_spec.get_frac_secs())
                );
                //create a timestamp tag for each channel
                for (size_t i = 0; i < _nchan; i++){
                    this->add_item_tag(i, nitems_written(0), TIME_KEY, val, _id);
                }
            }
            break;

        case uhd::rx_metadata_t::ERROR_CODE_TIMEOUT:
            //Assume that the user called stop() on the flow graph.
            //However, a timeout can occur under error conditions.
            return WORK_DONE;

        case uhd::rx_metadata_t::ERROR_CODE_OVERFLOW:
            _tag_now = true;
            //ignore overflows and try work again
            return work(noutput_items, input_items, output_items);

        default:
            std::cout << boost::format(
                "UHD source block got error code 0x%x"
            ) % _metadata.error_code << std::endl;
            return num_samps;
        }

        return num_samps;
    }

    bool start(void){
        #ifdef GR_UHD_USE_STREAM_API
        _rx_stream = _dev->get_rx_stream(_stream_args);
        _samps_per_packet = _rx_stream->get_max_num_samps();
        #endif
        //setup a stream command that starts streaming slightly in the future
        static const double reasonable_delay = 0.1; //order of magnitude over RTT
        uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        stream_cmd.stream_now = _stream_now;
        stream_cmd.time_spec = get_time_now() + uhd::time_spec_t(reasonable_delay);
        _dev->issue_stream_cmd(stream_cmd);
        _tag_now = true;
        return true;
    }

    void flush(void){
        const size_t nbytes = 4096;
        gr_vector_void_star outputs;
        std::vector<std::vector<char> > buffs(_nchan, std::vector<char>(nbytes));
        for (size_t i = 0; i < _nchan; i++){
            outputs.push_back(&buffs[i].front());
        }
        while (true){
            #ifdef GR_UHD_USE_STREAM_API
            const size_t bpi = uhd::convert::get_bytes_per_item(_stream_args.cpu_format);
            _rx_stream->recv(
                outputs, nbytes/bpi, _metadata, 0.0
            );
            #else
            _dev->get_device()->recv(
                outputs, nbytes/_type->size, _metadata,
                *_type, uhd::device::RECV_MODE_FULL_BUFF, 0.0
            );
            #endif
            if (_metadata.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) break;
        }
    }

    bool stop(void){
        _dev->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);

        this->flush();

        return true;
    }

    std::vector<std::complex<float> > finite_acquisition(const size_t nsamps){
        #ifdef GR_UHD_USE_STREAM_API
        uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
        cmd.num_samps = nsamps;
        cmd.stream_now = true;
        _dev->issue_stream_cmd(cmd);

        std::vector<std::complex<float> > samps(nsamps);
        const size_t actual_num_samps = _rx_stream->recv(
            &samps.front(), nsamps, _metadata, 0.1
        );
        samps.resize(actual_num_samps);

        return samps;
        #else
        throw std::runtime_error("not implemented in this version");
        #endif
    }

private:
    uhd::usrp::multi_usrp::sptr _dev;
    const uhd::stream_args_t _stream_args;
    boost::shared_ptr<uhd::io_type_t> _type;
    #ifdef GR_UHD_USE_STREAM_API
    uhd::rx_streamer::sptr _rx_stream;
    size_t _samps_per_packet;
    #endif
    size_t _nchan;
    bool _stream_now, _tag_now;
    uhd::rx_metadata_t _metadata;
    pmt::pmt_t _id;
};


/***********************************************************************
 * Make UHD Multi USRP Source
 **********************************************************************/
boost::shared_ptr<uhd_usrp_source> uhd_make_usrp_source(
    const uhd::device_addr_t &device_addr,
    const uhd::io_type_t &io_type,
    size_t num_channels
){
    //fill in the streamer args
    uhd::stream_args_t stream_args;
    switch(io_type.tid){
    case uhd::io_type_t::COMPLEX_FLOAT32: stream_args.cpu_format = "fc32"; break;
    case uhd::io_type_t::COMPLEX_INT16: stream_args.cpu_format = "sc16"; break;
    default: throw std::runtime_error("only complex float and shorts known to work");
    }
    stream_args.otw_format = "sc16"; //only sc16 known to work
    for (size_t chan = 0; chan < num_channels; chan++)
        stream_args.channels.push_back(chan); //linear mapping

    return uhd_make_usrp_source(device_addr, stream_args);
}

boost::shared_ptr<uhd_usrp_source> uhd_make_usrp_source(
    const uhd::device_addr_t &device_addr,
    const uhd::stream_args_t &stream_args
){
    return boost::shared_ptr<uhd_usrp_source>(
        new uhd_usrp_source_impl(device_addr, stream_args)
    );
}
