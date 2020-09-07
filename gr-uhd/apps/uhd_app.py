#!/usr/bin/env python3
#
# Copyright 2015-2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
USRP Helper Module: Common tasks for uhd-based apps.
"""

import sys
import time
import argparse
from gnuradio import eng_arg
from gnuradio import uhd
from gnuradio import gr
from gnuradio import gru

COMMAND_DELAY = .2 # Seconds

COMPACT_TPL = "{mb_id} ({mb_serial}), {db_subdev} ({subdev}, {ant}{db_serial})"
LONG_TPL = """{prefix}  Motherboard: {mb_id} ({mb_serial})
{prefix}  Daughterboard: {db_subdev}{db_serial}
{prefix}  Subdev: {subdev}
{prefix}  Antenna: {ant}
"""

# PyLint can't reliably detect C++ exports in modules, so let's disable that
# pylint: disable=no-member

class UHDApp:
    GAIN_TYPE_GAIN = 'dB'
    GAIN_TYPE_POWER = 'power_dbm'

    " Base class for simple UHD-based applications "
    def __init__(self, prefix=None, args=None):
        self.prefix = prefix
        self.args = args
        self.verbose = args.verbose or 0
        if self.args.sync == 'auto' and len(self.args.channels) > 1:
            self.args.sync = 'pps'
        self.antenna = None
        self.gain_range = None # Can also be power range
        self.samp_rate = None
        self.has_lo_sensor = None
        self.async_msgq = None
        self.async_src = None
        self.async_rcv = None
        self.tr = None
        self.gain = None
        self.freq = None
        self.channels = None
        self.cpu_format = None
        self.spec = None
        self.clock_source = None
        self.time_source = None
        self.lo_source = None
        self.lo_export = None
        self.usrp = None
        self.lo_source_channel = None
        self.gain_type = self.GAIN_TYPE_GAIN

    def vprint(self, *args):
        """
        Print 'string' with 'prefix' prepended if self.verbose is True
        """
        if self.verbose:
            print("[{prefix}]".format(prefix=self.prefix), *args)

    def get_usrp_info_string(self, compact=False, tx_or_rx='rx', chan=0, mboard=0):
        """
        Return a nice textual description of the USRP we're using.
        """
        if tx_or_rx not in ['rx', 'tx']:
            raise ValueError("tx_or_rx argument must be one of ['rx', 'tx']")

        try:
            info_pp = {}
            if self.prefix is None:
                info_pp['prefix'] = ""
            else:
                info_pp['prefix'] = "[{prefix}] ".format(prefix=self.prefix)
            usrp_info = self.usrp.get_usrp_info(chan)
            info_pp['mb_id'] = usrp_info['mboard_id']
            info_pp['mb_serial'] = usrp_info['mboard_serial']
            if info_pp['mb_serial'] == "":
                info_pp['mb_serial'] = "no serial"
            info_pp['db_subdev'] = usrp_info["{xx}_subdev_name".format(xx=tx_or_rx)]
            info_pp['db_serial'] = ", " + usrp_info["{xx}_serial".format(xx=tx_or_rx)]
            if info_pp['db_serial'] == "":
                info_pp['db_serial'] = "no serial"
            info_pp['subdev'] = self.usrp.get_subdev_spec(mboard)
            info_pp['ant'] = self.usrp.get_antenna(chan)
            if info_pp['mb_id'] in ("B200", "B210", "E310"):
                # In this case, this is meaningless
                info_pp['db_serial'] = ""
            tpl = LONG_TPL
            if compact:
                tpl = COMPACT_TPL
            return tpl.format(**info_pp)
        except Exception:
            return "Can't establish USRP info."

    def normalize_sel(self, num_name, arg_name, num, arg):
        """
        num_name: meaningful name why we need num arguments
        arg_name: name of current argument
        num: required number of arguments
        arg: actual argument
        """
        if arg is None:
            return None
        args = [x.strip() for x in arg.split(",")]
        if len(args) == 1:
            args = args * num
        if len(args) != num:
            raise ValueError("Invalid {m} setting for {n} {b}: {a}".format(
                m=arg_name, n=num, a=arg, b=num_name
            ))

        return args

    def async_callback(self, msg):
        """
        Call this when USRP async metadata needs printing.
        """
        metadata = self.async_src.msg_to_async_metadata_t(msg)
        print("[{prefix}] Channel: {chan} Time: {t} Event: {e}".format(
            prefix=self.prefix,
            chan=metadata.channel,
            t=metadata.time_spec.get_real_secs(),
            e=metadata.event_code,
        ))

    def setup_usrp(self, ctor, args, cpu_format='fc32'):
        """
        Instantiate a USRP object; takes care of all kinds of corner cases and settings.
        Pop it and some args onto the class that calls this.
        """
        self.channels = args.channels
        self.cpu_format = cpu_format
        # Create a UHD device object:
        self.usrp = ctor(
            device_addr=args.args,
            stream_args=uhd.stream_args(
                cpu_format,
                args.otw_format,
                args=args.stream_args,
                channels=self.channels,
            )
        )
        # Set the subdevice spec:
        self.spec = self.normalize_sel("mboards", "subdev",
                                       self.usrp.get_num_mboards(), args.spec)
        if self.spec:
            for mb_idx in range(self.usrp.get_num_mboards()):
                self.usrp.set_subdev_spec(self.spec[mb_idx], mb_idx)
        # Set the clock and/or time source:
        if args.clock_source is not None:
            self.clock_source = self.normalize_sel("mboards", "clock-source",
                                                   self.usrp.get_num_mboards(), args.clock_source)
            for mb_idx in range(self.usrp.get_num_mboards()):
                self.usrp.set_clock_source(self.clock_source[mb_idx], mb_idx)
        if args.time_source is not None:
            self.time_source = self.normalize_sel("mboards", "time-source",
                                                  self.usrp.get_num_mboards(), args.time_source)
            for mb_idx in range(self.usrp.get_num_mboards()):
                self.usrp.set_time_source(self.time_source[mb_idx], mb_idx)
        # Sampling rate:
        self.usrp.set_samp_rate(args.samp_rate)
        self.samp_rate = self.usrp.get_samp_rate()
        self.vprint("Using sampling rate: {rate}".format(rate=self.samp_rate))
        # Set the antenna:
        self.antenna = self.normalize_sel("channels", "antenna", len(args.channels), args.antenna)
        if self.antenna is not None:
            for i, chan in enumerate(self.channels):
                if not self.antenna[i] in self.usrp.get_antennas(i):
                    print("[ERROR] {} is not a valid antenna name for this USRP device!".format(self.antenna[i]))
                    sys.exit(1)
                self.usrp.set_antenna(self.antenna[i], i)
                self.vprint("[{prefix}] Channel {chan}: Using antenna {ant}.".format(
                    prefix=self.prefix, chan=chan, ant=self.usrp.get_antenna(i)
                ))
        self.antenna = self.usrp.get_antenna(0)
        # Set receive daughterboard gain:
        if args.power:
            if args.gain is not None:
                print("[ERROR] Providing both --gain and --power is invalid!")
                sys.exit(1)
            self.gain_type = self.GAIN_TYPE_POWER
            if not self.usrp.has_power_reference(0):
                print("[ERROR] Device does not have power reference capabilities!")
                sys.exit(1)
            self.set_power_reference(args.power)
            self.gain_range = self.usrp.get_power_range(0)
        else:
            self.set_gain(args.gain)
            self.gain_range = self.usrp.get_gain_range(0)
            self.gain_type = self.GAIN_TYPE_GAIN
        # Set frequency (tune request takes lo_offset):
        if hasattr(args, 'lo_offset') and args.lo_offset is not None:
            treq = uhd.tune_request(args.freq, args.lo_offset)
        else:
            treq = uhd.tune_request(args.freq)
        self.has_lo_sensor = 'lo_locked' in self.usrp.get_sensor_names()
        # Set LO export and LO source operation
        if (args.lo_export is not None) and (args.lo_source is not None):
            self.lo_source = self.normalize_sel("channels", "lo-source", len(self.channels), args.lo_source)
            self.lo_export = self.normalize_sel("channels", "lo-export", len(self.channels), args.lo_export)
            self.lo_source_channel = None
            for chan, lo_source, lo_export in zip(self.channels, self.lo_source, self.lo_export):
                if (lo_source == "None") or (lo_export == "None"):
                    continue
                if lo_export == "True":
                    #If channel is LO source set frequency and store response
                    self.usrp.set_lo_export_enabled(True, uhd.ALL_LOS, chan)
                if lo_source == "internal":
                    self.lo_source_channel = chan
                    tune_resp = self.usrp.set_center_freq(treq, chan)
                self.usrp.set_lo_source(lo_source, uhd.ALL_LOS, chan)
            # Use lo source tune response to tune dsp_freq on remaining channels
            if self.lo_source_channel is not None:
                if getattr(args, 'lo_offset', None) is not None:
                    treq = uhd.tune_request(
                        target_freq=args.freq,
                        rf_freq=args.freq+args.lo_offset,
                        rf_freq_policy=uhd.tune_request.POLICY_MANUAL,
                        dsp_freq=tune_resp.actual_dsp_freq,
                        dsp_freq_policy=uhd.tune_request.POLICY_MANUAL)
                else:
                    treq = uhd.tune_request(
                        target_freq=args.freq,
                        rf_freq=args.freg,
                        rf_freq_policy=uhd.tune_request.POLICY_MANUAL,
                        dsp_freq=tune_resp.actual_dsp_freq,
                        dsp_freq_policy=uhd.tune_request.POLICY_MANUAL)
            for chan in args.channels:
                if chan == self.lo_source_channel:
                    continue
                self.usrp.set_center_freq(treq, chan)
        # Make sure tuning is synched:
        command_time_set = False
        if len(self.channels) > 1:
            if args.sync == 'pps':
                self.usrp.set_time_unknown_pps(uhd.time_spec())
            cmd_time = self.usrp.get_time_now() + uhd.time_spec(COMMAND_DELAY)
            try:
                for mb_idx in range(self.usrp.get_num_mboards()):
                    self.usrp.set_command_time(cmd_time, mb_idx)
                command_time_set = True
            except RuntimeError:
                sys.stderr.write('[{prefix}] [WARNING] Failed to set command times.\n'.format(prefix=self.prefix))
        for i, chan in enumerate(self.channels):
            self.tr = self.usrp.set_center_freq(treq, i)
            if self.tr is None:
                sys.stderr.write(
                    '[{prefix}] [ERROR] Failed to set center frequency on channel {chan}\n'
                    .format(prefix=self.prefix, chan=chan))
                sys.exit(1)
        if command_time_set:
            for mb_idx in range(self.usrp.get_num_mboards()):
                self.usrp.clear_command_time(mb_idx)
            self.vprint("Syncing channels...".format(prefix=self.prefix))
            time.sleep(COMMAND_DELAY)
        self.freq = self.usrp.get_center_freq(0)
        if args.show_async_msg:
            self.async_msgq = gr.msg_queue(0)
            self.async_src = uhd.amsg_source("", self.async_msgq)
            self.async_rcv = gru.msgq_runner(self.async_msgq, self.async_callback)

    def set_gain(self, gain):
        """
        Safe gain-setter. Catches some special cases:
        - If gain is None, set to mid-point in dB.
        - If the USRP is multi-channel, set it on all channels.
        """
        if gain is None:
            if self.args.verbose:
                self.vprint("Defaulting to mid-point gains:".format(prefix=self.prefix))
            for i, chan in enumerate(self.channels):
                self.usrp.set_normalized_gain(.5, i)
                if self.args.verbose:
                    self.vprint("Channel {chan} gain: {g} dB".format(
                        chan=chan, g=self.usrp.get_gain(i)
                    ))
        else:
            self.vprint("Setting gain to {g:.2f} dB.".format(g=gain))
            for chan in range(len(self.channels)):
                self.usrp.set_gain(gain, chan)
        self.gain = self.usrp.get_gain(0)

    def set_power_reference(self, power_dbm):
        """
        Safe power-ref-setter.
        """
        assert power_dbm is not None
        self.vprint("Setting ref power to {g:.2f} dBm.".format(g=power_dbm))
        self.usrp.set_power_reference(power_dbm)
        self.gain = self.usrp.get_power_reference(0)

    def set_freq(self, freq, skip_sync=False):
        """
        Safely tune all channels to freq.
        """
        self.vprint("Tuning all channels to {freq} MHz.".format(freq=freq / 1e6))
        # Set frequency (tune request takes lo_offset):
        if hasattr(self.args, 'lo_offset') and self.args.lo_offset is not None:
            treq = uhd.tune_request(freq, self.args.lo_offset)
        else:
            treq = uhd.tune_request(freq)
        # Special TwinRX tuning due to LO sharing
        if getattr(self, 'lo_source_channel', None) is not None:
            tune_resp = self.usrp.set_center_freq(treq, self.lo_source_channel)
            if getattr(self.args, 'lo_offset', None) is not None:
                treq = uhd.tune_request(
                    target_freq=freq,
                    rf_freq=freq+self.args.lo_offset,
                    rf_freq_policy=uhd.tune_request.POLICY_MANUAL,
                    dsp_freq=tune_resp.actual_dsp_freq,
                    dsp_freq_policy=uhd.tune_request.POLICY_MANUAL)
            else:
                treq = uhd.tune_request(
                    target_freq=freq,
                    rf_freq=freq,
                    rf_freq_policy=uhd.tune_reqest.POLICY_MANUAL,
                    dsp_freq=tune_resp.actual_dsp_freq,
                    dsp_freq_policy=uhd.tune_request.POLICY_MANUAL)
            for chan in self.channels:
                if chan == self.lo_source_channel:
                    continue
                self.usrp.set_center_freq(treq, chan)
        # Make sure tuning is synched:
        command_time_set = False
        if len(self.channels) > 1 and not skip_sync:
            cmd_time = self.usrp.get_time_now() + uhd.time_spec(COMMAND_DELAY)
            try:
                for mb_idx in range(self.usrp.get_num_mboards()):
                    self.usrp.set_command_time(cmd_time, mb_idx)
                command_time_set = True
            except RuntimeError:
                sys.stderr.write('[{prefix}] [WARNING] Failed to set command times.\n'.format(prefix=self.prefix))
        for i, chan in enumerate(self.channels):
            self.tr = self.usrp.set_center_freq(treq, i)
            if self.tr is None:
                sys.stderr.write('[{prefix}] [ERROR] Failed to set center frequency on channel {chan}\n'.format(
                    prefix=self.prefix, chan=chan
                ))
                sys.exit(1)
        if command_time_set:
            for mb_idx in range(self.usrp.get_num_mboards()):
                self.usrp.clear_command_time(mb_idx)
            self.vprint("Syncing channels...".format(prefix=self.prefix))
            time.sleep(COMMAND_DELAY)
        self.freq = self.usrp.get_center_freq(0)
        self.vprint("First channel has freq: {freq} MHz.".format(freq=self.freq / 1e6))

    @staticmethod
    def setup_argparser(
            parser=None,
            description='USRP App',
            allow_mimo=True,
            tx_or_rx="",
            skip_freq=False,
        ):
        """
        Create or amend an argument parser with typical USRP options.
        """
        def cslist(string):
            """
            For ArgParser: Turn a comma separated list into an actual list.
            """
            try:
                return [int(x.strip()) for x in string.split(",")]
            except ValueError:
                raise argparse.ArgumentTypeError("Not a comma-separated list: {string}".format(string=string))
        if parser is None:
            parser = argparse.ArgumentParser(
                description=description,
            )
        tx_or_rx = tx_or_rx.strip() + " "
        group = parser.add_argument_group('USRP Arguments')
        group.add_argument("-a", "--args", default="", help="UHD device address args")
        group.add_argument(
            "--spec",
            help="Subdevice(s) of UHD device where appropriate. "
                 "Use a comma-separated list to set different boards to different specs.")
        group.add_argument("-A", "--antenna", help="Select {xx}antenna(s) where appropriate".format(xx=tx_or_rx))
        group.add_argument("-s", "--samp-rate", type=eng_arg.eng_float, default=1e6,
                           help="Sample rate")
        group.add_argument("-g", "--gain", type=eng_arg.eng_float, default=None,
                           help="Gain (default is midpoint)")
        group.add_argument("-p", "--power", type=eng_arg.eng_float, default=None,
                           help="(Reference) power level (in dBm). "
                                "Not supported by all devices (see UHD manual). "
                                "Will fail if not supported. Precludes --gain. "
                                "Behaviour may differ between applications.")
        if not skip_freq:
            group.add_argument("-f", "--freq", type=eng_arg.eng_float, default=None, required=True,
                               help="Set carrier frequency to FREQ",
                               metavar="FREQ")
            group.add_argument("--lo-offset", type=eng_arg.eng_float, default=0.0,
                               help="Set daughterboard LO offset to OFFSET [default=hw default]")
        if allow_mimo:
            group.add_argument("-c", "--channels", default=[0,], type=cslist,
                               help="Select {xx} Channels".format(xx=tx_or_rx))
        group.add_argument(
            "--lo-export",
            help="Set TwinRX LO export {None, True, False} for each channel "
                 "with a comma-separated list. None skips a channel.")
        group.add_argument(
            "--lo-source",
            help="Set TwinRX LO source {None, internal, companion, external} "
                 "for each channel with a comma-separated list. None skips this channel.")
        group.add_argument("--otw-format", choices=['sc16', 'sc12', 'sc8'], default='sc16',
                           help="Choose over-the-wire data format")
        group.add_argument("--stream-args", default="", help="Set additional stream arguments")
        group.add_argument("-v", "--verbose", action="count", help="Use verbose console output")
        group.add_argument("--show-async-msg", action="store_true",
                           help="Show asynchronous message notifications from UHD")
        group.add_argument("--sync", choices=('default', 'pps', 'auto'),
                           default='auto', help="Set to 'pps' to sync devices to PPS")
        group.add_argument("--clock-source",
                           help="Set the clock source; typically 'internal', 'external' or 'gpsdo'")
        group.add_argument("--time-source",
                           help="Set the time source")
        return parser
