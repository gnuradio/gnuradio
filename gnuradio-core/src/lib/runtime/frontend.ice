/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gnuradio.ice>

[["python:package:gnuradio.ctrlport"]]
module GNURadio {
    module Frontend {

        exception NotSupported {};
        exception InvalidSetting { string msg; };
        exception ReceiverFailure { string msg; };
	exception NotExist {};
        dictionary<string, string>      TunerArgs;

        struct TunerStatus {
            int   a2dbits;
            float gain;
            bool  isInverted;
        };

        interface Tuner {
            TunerStatus configureTuner(TunerArgs args);                     //ADDED
            idempotent TunerStatus status();
            idempotent float setGain(float gain) throws NotSupported, InvalidSetting;
            idempotent bool  setInversion(bool inverted) throws NotSupported, InvalidSetting;
        };

        struct ChannelStatus {
	    string uid;
            bool active;
            float freq;
            float bandwidth;
            int payloadBits;
            bool  isComplex;
            string signalName;
        };

        interface Channel extends Component {
            idempotent ChannelStatus status();
            idempotent FeedInfo  feed();
            idempotent bool  active();
            void start();
            void stop();
            idempotent float setCenterFreq(float freq) throws NotSupported, InvalidSetting;
            idempotent float setBandwidth(float bw) throws NotSupported, InvalidSetting;
            idempotent int   setPayloadBits(int bits) throws NotSupported, InvalidSetting;
            idempotent bool  setComplex(bool complex) throws NotSupported, InvalidSetting;
            void removeChannel() throws NotSupported;
        };

        sequence<Tuner*>   TunerSeq;
        sequence<Channel*> ChannelSeq;

        struct ChannelizerStatus {
	    string uid;
            string signalName;
        };
    
        interface Channelizer extends Component {
            idempotent ChannelizerStatus status();
            idempotent Tuner*       getTuner();
            idempotent ChannelSeq   getChannels();
            idempotent ChannelSeq   getActiveChannels();
            idempotent ChannelSeq   getInactiveChannels();
            Channel* createChannel(float freq, float bw, int payloadBits, string address, int port) throws NotSupported;
        };

        sequence<Channelizer*>  ChannelizerSeq;

        interface Receiver extends AbstractReceiver {
            idempotent ChannelizerSeq getInputs();
//            idempotent ChannelizerSeq getActiveInputs();
//            idempotent ChannelizerSeq getInactiveInputs();
	    idempotent Channel* getChannelByID(string id) throws NotExist;
	    idempotent Channelizer* getChannelizerByID(string id) throws NotExist;
        };


    };
    


};
