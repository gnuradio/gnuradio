"""
Copyright 2010-2011,2014,2018 Free Software Foundation, Inc.

This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import sys

MAIN_TMPL = """\
id: uhd_usrp_${sourk}
label: 'UHD: USRP ${sourk.title()}'
flags: throttle

parameters:
-   id: type
    label: ${direction.title()}put Type
    dtype: enum
    options: [fc32, sc16, item32]
    option_labels: [Complex float32, Complex int16, VITA word32]
    option_attributes:
        type: [fc32, sc16, s32]
    hide: part
-   id: otw
    label: Wire Format
    dtype: enum
    options: ['', sc16, sc12, sc8]
    option_labels: [Automatic, Complex int16, Complex int12, Complex int8]
    hide: ${'$'}{ 'none' if otw else 'part'}
-   id: stream_args
    label: Stream args
    dtype: string
    options: ['', peak=0.003906]
    option_labels: ['', peak=0.003906]
    hide: ${'$'}{ 'none' if stream_args else 'part'}
-   id: stream_chans
    label: Stream channels
    dtype: int_vector
    default: '[]'
    hide: ${'$'}{ 'none' if stream_chans else 'part'}
-   id: dev_addr
    label: Device Address
    dtype: string
    default: '""'
    hide: ${'$'}{ 'none' if dev_addr else 'part'}
-   id: dev_args
    label: Device Arguments
    dtype: string
    default: '""'
    hide: ${'$'}{ 'none' if dev_args else 'part'}
-   id: sync
    label: Sync
    dtype: enum
    options: [sync, pc_clock, none]
    option_labels: [Unknown PPS, PC Clock, No Sync]
    hide: ${'$'}{ 'none' if sync else 'part'}
-   id: clock_rate
    label: Clock Rate (Hz)
    dtype: real
    default: 0e0
    options: [0e0, 200e6, 184.32e6, 120e6, 30.72e6]
    option_labels: [Default, 200 MHz, 184.32 MHz, 120 MHz, 30.72 MHz]
    hide: ${'$'}{ 'none' if clock_rate else 'part' }
-   id: num_mboards
    label: Num Mboards
    dtype: int
    default: 1
    options: [1, 2, 3, 4, 5, 6, 7, 8]
    hide: part
% for m in range(max_mboards):
-   id: clock_source${m}
    label: 'Mb${m}: Clock Source'
    dtype: string
    options: ['', internal, external, mimo, gpsdo]
    option_labels: [Default, Internal, External, MIMO Cable, O/B GPSDO]
    hide: ${'$'}{ 'all' if not (num_mboards > ${m}) else ( 'none' if clock_source${m} else 'part')}
-   id: time_source${m}
    label: 'Mb${m}: Time Source'
    dtype: string
    options: ['', external, mimo, gpsdo]
    option_labels: [Default, External, MIMO Cable, O/B GPSDO]
    hide: ${'$'}{ 'all' if not (num_mboards > ${m}) else ('none' if time_source${m} else 'part')}
-   id: sd_spec${m}
    label: 'Mb${m}: Subdev Spec'
    dtype: string
    hide: ${'$'}{ 'all' if not (num_mboards > ${m}) else ('none' if sd_spec${m} else 'part')}
% endfor
-   id: nchan
    label: Num Channels
    dtype: int
    default: 1
    options: [ ${", ".join([str(n) for n in range(1, max_nchan+1)])} ]
    hide: part
-   id: samp_rate
    label: Samp rate (Sps)
    dtype: real
    default: samp_rate
${params}

inputs:
-   domain: message
    id: command
    optional: true
% if sourk == 'source':

outputs:
% endif
-   domain: stream
    dtype: ${'$'}{type.type}
    multiplicity: ${'$'}{nchan}
% if sourk == 'sink':

outputs:
-   domain: message
    id: async_msgs
    optional: true
% endif

templates:
    imports: |-
        from gnuradio import uhd
        import time
    make: |
        uhd.usrp_${sourk}(
            ",".join((${'$'}{dev_addr}, ${'$'}{dev_args})),
            uhd.stream_args(
                cpu_format="${'$'}{type}",
                ${'%'} if otw:
                otw_format="${'$'}{otw}",
                ${'%'} endif
                ${'%'} if stream_args:
                args=${'$'}{stream_args},
                ${'%'} endif
                ${'%'} if stream_chans:
                channels=${'$'}{stream_chans},
                ${'%'} else:
                channels=range(${'$'}{nchan}),
                ${'%'} endif
            ),
            ${'%'} if len_tag_name:
            ${'$'}{len_tag_name},
            ${'%'} endif
        )
        % for m in range(max_mboards):
        ${'%'} if context.get('num_mboards')() > ${m}:
        ${'%'} if context.get('sd_spec${m}')():
        self.${'$'}{id}.set_subdev_spec(${'$'}{${'sd_spec' + str(m)}}, ${m})
        ${'%'} endif
        ${'%'} if context.get('time_source${m}')():
        self.${'$'}{id}.set_time_source(${'$'}{${'time_source' + str(m)}}, ${m})
        ${'%'} endif
        ${'%'} if context.get('clock_source${m}')():
        self.${'$'}{id}.set_clock_source(${'$'}{${'clock_source' + str(m)}}, ${m})
        ${'%'} endif
        ${'%'} endif
        % endfor
        % for n in range(max_nchan):
        ${'%'} if context.get('nchan')() > ${n}:
        self.${'$'}{id}.set_center_freq(${'$'}{${'center_freq' + str(n)}}, ${n})
        ${'%'} if bool(eval(context.get('norm_gain' + '${n}')())):
        self.${'$'}{id}.set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} else:
        self.${'$'}{id}.set_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} endif
        ${'%'} if context.get('ant${n}')():
        self.${'$'}{id}.set_antenna(${'$'}{${'ant' + str(n)}}, ${n})
        ${'%'} endif
        ${'%'} if context.get('bw${n}')():
        self.${'$'}{id}.set_bandwidth(${'$'}{${'bw' + str(n)}}, ${n})
        ${'%'} endif
        ${'%'} if context.get('show_lo_controls')():
        self.${'$'}{id}.set_lo_source(${'$'}{${'lo_source' + str(n)}}, uhd.ALL_LOS, ${n})
        self.${'$'}{id}.set_lo_export_enabled(${'$'}{${'lo_export' + str(n)}}, uhd.ALL_LOS, ${n})
        ${'%'} endif
        ${'%'} endif
        % endfor
        ${'%'} if clock_rate():
        self.${'$'}{id}.set_clock_rate(${'$'}{clock_rate}, uhd.ALL_MBOARDS)
        ${'%'} endif
        self.${'$'}{id}.set_samp_rate(${'$'}{samp_rate})
        ${'%'} if sync == 'sync':
        self.${'$'}{id}.set_time_unknown_pps(uhd.time_spec())
        ${'%'} elif sync == 'pc_clock':
        self.${'$'}{id}.set_time_now(uhd.time_spec(time.time()), uhd.ALL_MBOARDS)
        ${'%'} else:
        # No synchronization enforced.
        ${'%'} endif
    callbacks:
    -   set_samp_rate(${'$'}{samp_rate})
    % for n in range(max_nchan):
    -   set_center_freq(${'$'}{${'center_freq' + str(n)}}, ${n})
    -   self.${'$'}{id}.set_${'$'}{'normalized_' if bool(eval(context.get('norm_gain${n}')())) else ''}gain(${'$'}{${'gain' + str(n)}}, ${n})
    -   ${'$'}{'set_lo_source(' + lo_source${n} + ', uhd.ALL_LOS, ${n})' if show_lo_controls else ''}
    -   ${'$'}{'set_lo_export_enabled(' + lo_export${n} + ', uhd.ALL_LOS, ${n})' if show_lo_controls else ''}
    -   set_antenna(${'$'}{${'ant' + str(n)}}, ${n})
    -   set_bandwidth(${'$'}{${'bw' + str(n)}}, ${n})
    % endfor


documentation: |-
    The UHD USRP ${sourk.title()} Block:

    Device Address:
    The device address is a delimited string used to locate UHD devices on your system. \\
    If left blank, the first UHD device found will be used. \\
    Use the device address to specify a specific device or list of devices.
    USRP1 Example: serial=12345678
    USRP2 Example: addr=192.168.10.2
    USRP2 Example: addr0=192.168.10.2, addr1=192.168.10.3

    ${direction.title()} Type:
    This parameter controls the data type of the stream in gnuradio.

    Wire Format:
    This parameter controls the form of the data over the bus/network. \
    Complex bytes may be used to trade off precision for bandwidth. \
    Not all formats are supported on all devices.

    Stream Args:
    Optional arguments to be passed in the UHD streamer object. \
    Streamer args is a list of key/value pairs; usage is determined by the implementation.
    Ex: the scalar key affects the scaling between 16 and 8 bit integers in sc8 wire format.

    Num Motherboards:
    Selects the number of USRP motherboards in this device configuration.

    Reference Source:
    Where the motherboard should sync its time and clock references.
    If source and sink blocks reference the same device,
    it is only necessary to set the reference source on one of the blocks.

    Subdevice specification:
    Each motherboard should have its own subdevice specification \\
    and all subdevice specifications should be the same length. \\
    Select the subdevice or subdevices for each channel using a markup string. \\
    The markup string consists of a list of dboard_slot:subdev_name pairs (one pair per channel). \\
    If left blank, the UHD will try to select the first subdevice on your system. \\
    See the application notes for further details.
    Single channel example: :AB
    Dual channel example: :A :B

    Num Channels:
    Selects the total number of channels in this multi-USRP configuration.
    Ex: 4 motherboards with 2 channels per board = 8 channels total

    Sample rate:
    The sample rate is the number of samples per second input by this  block. \\
    The UHD device driver will try its best to match the requested sample rate. \\
    If the requested rate is not possible, the UHD block will print an error at runtime.

    Center frequency:
    The center frequency is the overall frequency of the RF chain. \\
    For greater control of how the UHD tunes elements in the RF chain, \\
    pass a tune_request object rather than a simple target frequency.
    Tuning with an LO offset example: uhd.tune_request(freq, lo_off)
    Tuning without DSP: uhd.tune_request(target_freq, dsp_freq=0, \\
    dsp_freq_policy=uhd.tune_request.POLICY_MANUAL)

    Antenna:
    For subdevices with only one antenna, this may be left blank. \\
    Otherwise, the user should specify one of the possible antenna choices. \\
    See the daughterboard application notes for the possible antenna choices.

    Bandwidth:
    To use the default bandwidth filter setting, this should be zero. \\
    Only certain subdevices have configurable bandwidth filters. \\
    See the daughterboard application notes for possible configurations.

    Length tag key (Sink only):
    When a nonempty string is given, the USRP sink will look for length tags \\
    to determine transmit burst lengths.

    See the UHD manual for more detailed documentation:
    http://uhd.ettus.com

file_format: 1
"""

PARAMS_TMPL = """
-   id: center_freq${n}
    label: 'Ch${n}: Center Freq (Hz)'
    category: RF Options
    dtype: real
    default: '0'
    hide: ${'$'}{ 'none' if (nchan > ${n}) else 'all' }
-   id: gain${n}
    label: 'Ch${n}: Gain Value'
    category: RF Options
    dtype: float
    default: '0'
    hide: ${'$'}{ 'none' if nchan > ${n} else 'all' }
-   id: norm_gain${n}
    label: 'Ch${n}: Gain Type'
    category: RF Options
    dtype: string
    default: 'False'
    options: ['False', 'True']
    option_labels: [Absolute (dB), Normalized]
    hide: ${'$'}{ 'all' if nchan <= ${n} else ('none' if bool(eval('norm_gain' + str(${n}))) else 'part')}
-   id: ant${n}
    label: 'Ch${n}: Antenna'
    category: RF Options
    dtype: string
% if sourk == 'source':
    options: [TX/RX, RX2, RX1]
    option_labels: [TX/RX, RX2, RX1]
    default: RX2
% else:
    options: [TX/RX]
    option_labels: [TX/RX]
% endif
    hide: ${'$'}{ 'all' if not nchan > ${n} else ('none' if eval('ant' + str(${n})) else 'part')}
-   id: bw${n}
    label: 'Ch${n}: Bandwidth (Hz)'
    category: RF Options
    dtype: real
    default: '0'
    hide: ${'$'}{ 'all' if not nchan > ${n} else ('none' if eval('bw' + str(${n})) else 'part')}
-   id: lo_source${n}
    label: 'Ch${n}: LO Source'
    category: RF Options
    dtype: string
    default: internal
    options: [internal, external, companion]
    hide: ${'$'}{ 'all' if not nchan > ${n} else ('none' if show_lo_controls else 'all')}
-   id: lo_export${n}
    label: 'Ch${n}: LO Export'
    category: RF Options
    dtype: bool
    default: 'False'
    options: ['True', 'False']
    hide: ${'$'}{ 'all' if not nchan > ${n} else ('none' if show_lo_controls else 'all')}
% if sourk == 'source':
-   id: dc_offs_enb${n}
    label: 'Ch${n}: Enable DC Offset Correction'
    category: FE Corrections
    dtype: raw
    default: '""'
    hide: ${'$'}{ 'all' if not nchan > ${n} else 'part'}
-   id: iq_imbal_enb${n}
    label: 'Ch${n}: Enable IQ Imbalance Correction'
    category: FE Corrections
    dtype: raw
    default: '""'
    hide: ${'$'}{ 'all' if not nchan > ${n} else 'part'}
% endif
"""

SHOW_LO_CONTROLS_PARAM = """
-   id: show_lo_controls
    label: Show LO Controls
    category: Advanced
    dtype: bool
    default: 'False'
    hide: part
"""

TSBTAG_PARAM = """
-   id: len_tag_name
    label: TSB tag name
    dtype: string
    hide: ${ 'none' if len(str(len_tag_name)) else 'part'}
"""

TSBTAG_ARG = """
${'%'} if len_tag_name():
${'$'}{len_tag_name},
${'%'} endif
"""

def parse_tmpl(_tmpl, **kwargs):
    """ Render _tmpl using the kwargs. """
    from mako.template import Template
    block_template = Template(_tmpl)
    return str(block_template.render(**kwargs))

MAX_NUM_MBOARDS = 8
MAX_NUM_CHANNELS = MAX_NUM_MBOARDS*4

if __name__ == '__main__':
    for file in sys.argv[1:]:
        if file.endswith('source.block.yml'):
            sourk = 'source'
            direction = 'out'
        elif file.endswith('sink.block.yml'):
            sourk = 'sink'
            direction = 'in'
        else:
            raise Exception('is % a source or sink?'%file)
        params = ''.join([
            parse_tmpl(PARAMS_TMPL, n=n, sourk=sourk)
            for n in range(MAX_NUM_CHANNELS)
        ])
        params += SHOW_LO_CONTROLS_PARAM
        if sourk == 'sink':
            params += TSBTAG_PARAM
            lentag_arg = TSBTAG_ARG
        else:
            lentag_arg = ''
        open(file, 'w').write(
            parse_tmpl(
                MAIN_TMPL,
                lentag_arg=lentag_arg,
                max_nchan=MAX_NUM_CHANNELS,
                max_mboards=MAX_NUM_MBOARDS,
                params=params,
                sourk=sourk,
                direction=direction,
            )
        )
