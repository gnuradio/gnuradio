/*
 * Copyright 2013 Free Software Foundation, Inc.
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

        // primitive types
		dictionary<string, string>	StrStrDict;
        dictionary<string, string>  TunerArgs;
		struct F32Range 
        {
          float min; 
          float max; 
        };

        // exception types
        exception NotSupported {};
        exception InvalidSetting { string msg; };
        exception ReceiverFailure { string msg; };
		exception NotExist {};


        // Status Types
        struct TunerStatus {
            float freq;
            float rate;
            int   a2dbits;
            float gain;
            F32Range gainrange;
            bool  isInverted;
			StrStrDict info;
        };

        struct StreamInfo {
			string uri;
			StrStrDict info;
        };

        struct ReceiverInfo {
            string uid;
            string name;
			StrStrDict info;
        };

        struct ChannelStatus {
	    	string uid;
            string name;
            bool active;
            float freq;
            float bandwidth;
            bool  isComplex;
			StrStrDict info;
        };

        struct ChannelizerStatus {
		    string uid;
            string name;
			StrStrDict info;
        };

        // Interfaces
        interface Component {
            void setName(string newName);
        };

        interface AbstractReceiver extends Component {
            idempotent ReceiverInfo   getReceiverInfo();
        };

        interface Tuner extends Component {
            TunerStatus configureTuner(TunerArgs args) throws ReceiverFailure, InvalidSetting;
            idempotent TunerStatus status();
            idempotent float setGain(float gain) throws ReceiverFailure, NotSupported, InvalidSetting;
            idempotent bool  setInversion(bool inverted) throws ReceiverFailure, NotSupported, InvalidSetting;
            idempotent float setCenterFreq(float freq) throws ReceiverFailure, NotSupported, InvalidSetting;
            idempotent float setBandwidth(float bw) throws ReceiverFailure, NotSupported, InvalidSetting;
            idempotent void  setInfo(string k, string v) throws ReceiverFailure, NotSupported, InvalidSetting;
        };

        interface Channel extends Tuner {
            void start();
            void stop();
            void destroyChannel() throws NotSupported;
            idempotent bool  active();
            idempotent ChannelStatus channelStat();
            idempotent StreamInfo    stream();
            idempotent bool setComplex(bool complex) throws ReceiverFailure, NotSupported, InvalidSetting;
            idempotent void setStreamInfo(string k, string v) throws ReceiverFailure, NotSupported, InvalidSetting;
        };

        sequence<Tuner*>   TunerSeq;
        sequence<Channel*> ChannelSeq;
    
        interface Channelizer extends AbstractReceiver {
            idempotent ChannelizerStatus status();
            idempotent Tuner*       getTuner();
            idempotent ChannelSeq   getChannels();
            idempotent ChannelSeq   getActiveChannels();
            idempotent ChannelSeq   getInactiveChannels();
            Channel* createChannel(float freq, float bw, StrStrDict args) throws NotSupported;
        };

        sequence<Channelizer*>  ChannelizerSeq;

        interface Receiver extends AbstractReceiver {
            idempotent ChannelizerSeq getInputs();
            idempotent Channel* getChannelByID(string id) throws NotExist;
            idempotent Channelizer* getChannelizerByID(string id) throws NotExist;
            idempotent void setInfo(string k, string v) throws ReceiverFailure, NotSupported, InvalidSetting;
        };

    };
    
    module Booter {
        dictionary<string, string>      WaveformArgs;
        exception WaveformRunningError {
            string waveformClass;
            float centerFrequencyHz;
        };
        exception SignalSourceError {string msg; };
        interface WaveformBooter extends Frontend::Receiver, ControlPort {
            string  launchWaveform(string waveformClass, WaveformArgs       args)
                    throws WaveformRunningError, SignalSourceError;
            WaveformArgMap  getDriverEnum();
            WaveformArgMap  getSourceInfo();
            idempotent bool     waveformRunning();
            idempotent string   getWaveformClass();
        };
    };
};
