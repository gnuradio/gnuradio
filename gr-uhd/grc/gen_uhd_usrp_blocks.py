"""
Copyright 2010-2011,2014,2018 Free Software Foundation, Inc.

This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""

import sys

MAIN_TMPL = """\
id: uhd_usrp_${sourk}
label: 'UHD: USRP ${sourk.title()}'
flags: [python, cpp, throttle]

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
    hide: ${'$'}{ 'none' if dev_args else 'part'}
-   id: sync
    label: Sync
    dtype: enum
    options: [sync, pc_clock, pc_clock_next_pps, gps_time, none]
    option_labels: [Unknown PPS, PC Clock, PC Clock on Next PPS, GPS Time on Next PPS, No Sync]
    hide: ${'$'}{ 'none' if sync else 'part'}
-   id: start_time
    label: Start Time (seconds)
    dtype: real
    default: -1.0
    options: [-1.0]
    option_labels: [Default]
    hide: ${'$'}{ 'none' if start_time >= 0.0 else 'part' }
-   id: clock_rate
    label: Clock Rate (Hz)
    dtype: real
    default: 0e0
    options: [0e0, 200e6, 184.32e6, 153.6e6, 125.0e6, 122.88e6, 120e6, 30.72e6]
    option_labels: [Default, 200 MHz, 184.32 MHz, 153.6 MHz, 125 MHz, 122.88 MHz, 120 MHz, 30.72 MHz]
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
            ${'%'} if clock_rate():
            ",".join((${'$'}{dev_addr}, ${'$'}{dev_args}, "master_clock_rate=${'$'}{clock_rate}")),
            ${'%'} else:
            ",".join((${'$'}{dev_addr}, ${'$'}{dev_args})),
            ${'%'} endif
            uhd.stream_args(
                cpu_format="${'$'}{type}",
                ${'%'} if otw:
                otw_format="${'$'}{otw}",
                ${'%'} endif
                ${'%'} if stream_args:
                args=${'$'}{stream_args},
                ${'%'} endif
                ${'%'} if eval(stream_chans):
                channels=${'$'}{stream_chans},
                ${'%'} else:
                channels=list(range(0,${'$'}{nchan})),
                ${'%'} endif
            ),
            % if sourk == 'sink':
            ${'%'} if len_tag_name:
            ${'$'}{len_tag_name},
            ${'%'} endif
            % endif
        )
        % for m in range(max_mboards):
        ${'%'} if context.get('num_mboards')() > ${m}:
        ########################################################################
        ${'%'} if context.get('clock_source${m}')():
        self.${'$'}{id}.set_clock_source(${'$'}{${'clock_source' + str(m)}}, ${m})
        ${'%'} endif
        ########################################################################
        ${'%'} if context.get('time_source${m}')():
        self.${'$'}{id}.set_time_source(${'$'}{${'time_source' + str(m)}}, ${m})
        ${'%'} endif
        ########################################################################
        ${'%'} if context.get('sd_spec${m}')():
        self.${'$'}{id}.set_subdev_spec(${'$'}{${'sd_spec' + str(m)}}, ${m})
        ${'%'} endif
        ########################################################################
        ${'%'} endif
        % endfor  # for m in range(max_mboards)
        self.${'$'}{id}.set_samp_rate(${'$'}{samp_rate})
        ${'%'} if sync == 'sync':
        self.${'$'}{id}.set_time_unknown_pps(uhd.time_spec(0))
        ${'%'} elif sync == 'pc_clock':
        self.${'$'}{id}.set_time_now(uhd.time_spec(time.time()), uhd.ALL_MBOARDS)
        ${'%'} elif sync == 'pc_clock_next_pps':
        _last_pps_time = self.${'$'}{id}.get_time_last_pps().get_real_secs()
        # Poll get_time_last_pps() every 50 ms until a change is seen
        while(self.${'$'}{id}.get_time_last_pps().get_real_secs() == _last_pps_time):
            time.sleep(0.05)
        # Set the time to PC time on next PPS
        self.${'$'}{id}.set_time_next_pps(uhd.time_spec(int(time.time()) + 1.0))
        # Sleep 1 second to ensure next PPS has come
        time.sleep(1)
        ${'%'} elif sync == 'gps_time':
        # Set the time to GPS time on next PPS
        # get_mboard_sensor("gps_time") returns just after the PPS edge,
        # thus add one second and set the time on the next PPS
        self.${'$'}{id}.set_time_next_pps(uhd.time_spec(self.${'$'}{id}.get_mboard_sensor("gps_time").to_int() + 1.0))
        # Sleep 1 second to ensure next PPS has come
        time.sleep(1)
        ${'%'} else:
        # No synchronization enforced.
        ${'%'} endif

        % for n in range(max_nchan):
        ${'%'} if context.get('nchan')() > ${n}:
        self.${'$'}{id}.set_center_freq(${'$'}{${'center_freq' + str(n)}}, ${n})
        ${'%'} if context.get('ant${n}')():
        self.${'$'}{id}.set_antenna(${'$'}{${'ant' + str(n)}}, ${n})
        ${'%'} endif
        ${'%'} if context.get('bw${n}')():
        self.${'$'}{id}.set_bandwidth(${'$'}{${'bw' + str(n)}}, ${n})
        ${'%'} endif
        % if sourk == 'source':
        ${'%'} if context.get('rx_agc${n}')() == 'Enabled':
        self.${'$'}{id}.set_rx_agc(True, ${n})
        ${'%'} elif context.get('rx_agc${n}')() == 'Disabled':
        self.${'$'}{id}.set_rx_agc(False, ${n})
        ${'%'} endif
        ${'%'} if context.get('rx_agc${n}')() != 'Enabled':
        ${'%'} if context.get('gain_type' + '${n}')() == 'normalized':
        self.${'$'}{id}.set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} elif context.get('gain_type' + '${n}')() == 'power':
        self.${'$'}{id}.set_power_reference(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} else:
        self.${'$'}{id}.set_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} endif
        ${'%'} endif  # if rx_agc${n} != 'Enabled'
        ${'%'} if context.get('dc_offs_enb${n}')() in ('auto', 'disabled'):
        self.${'$'}{id}.set_auto_dc_offset(${'$'}{True if ${'dc_offs_enb' + str(n)} == 'auto' else False}, ${n})
        ${'%'} elif context.get('dc_offs_enb${n}')() == 'manual':
        self.${'$'}{id}.set_dc_offset(${'$'}{${'dc_offs' + str(n)}}, ${n})
        ${'%'} endif
        ${'%'} if context.get('iq_imbal_enb${n}')() in ('auto', 'disabled'):
        self.${'$'}{id}.set_auto_iq_balance(${'$'}{True if ${'iq_imbal_enb' + str(n)} == 'auto' else False}, ${n})
        ${'%'} elif context.get('iq_imbal_enb${n}')() == 'manual':
        self.${'$'}{id}.set_iq_balance(${'$'}{${'iq_imbal' + str(n)}}, ${n})
        ${'%'} endif
        % else:
        ${'%'} if context.get('gain_type' + '${n}')() == 'normalized':
        self.${'$'}{id}.set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} elif context.get('gain_type' + '${n}')() == 'power':
        self.${'$'}{id}.set_power_reference(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} else:
        self.${'$'}{id}.set_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} endif
        % endif  # if sourk == 'source'

        ${'%'} if context.get('show_lo_controls')():
        self.${'$'}{id}.set_lo_source(${'$'}{${'lo_source' + str(n)}}, uhd.ALL_LOS, ${n})
        self.${'$'}{id}.set_lo_export_enabled(${'$'}{${'lo_export' + str(n)}}, uhd.ALL_LOS, ${n})
        ${'%'} endif
        ${'%'} endif  # nchan > n
        % endfor  # for n in range(max_nchan)
        ${'%'} if start_time() >= 0.0:
        self.${'$'}{id}.set_start_time(uhd.time_spec(${'$'}{start_time}))
        ${'%'} endif
    callbacks:
    -   set_samp_rate(${'$'}{samp_rate})
    % for n in range(max_nchan):
    -   set_center_freq(${'$'}{${'center_freq' + str(n)}}, ${n})
    % if sourk == 'source':
    -   ${'$'}{'set_rx_agc(True, ${n})' if context.get('rx_agc${n}')() == 'Enabled' else ''}
    -   ${'$'}{'set_rx_agc(False, ${n})' if context.get('rx_agc${n}')() == 'Disabled' else ''}
    -   |
        ${'%'} if context.get('rx_agc${n}')() != 'Enabled':
        ${'%'} if context.get('gain_type' + '${n}')() == 'normalized':
        self.${'$'}{id}.set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} elif context.get('gain_type' + '${n}')() == 'power':
        self.${'$'}{id}.set_power_reference(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} else:
        self.${'$'}{id}.set_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} endif
        ${'%'} endif
    % else:
    -   |
        ${'%'} if context.get('gain_type' + '${n}')() == 'normalized':
        self.${'$'}{id}.set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} elif context.get('gain_type' + '${n}')() == 'power':
        self.${'$'}{id}.set_power_reference(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} else:
        self.${'$'}{id}.set_gain(${'$'}{${'gain' + str(n)}}, ${n})
        ${'%'} endif
    % endif
    -   ${'$'}{'set_lo_source(' + lo_source${n} + ', uhd.ALL_LOS, ${n})' if show_lo_controls else ''}
    -   ${'$'}{'set_lo_export_enabled(' + lo_export${n} + ', uhd.ALL_LOS, ${n})' if show_lo_controls else ''}
    -   set_antenna(${'$'}{${'ant' + str(n)}}, ${n})
    -   set_bandwidth(${'$'}{${'bw' + str(n)}}, ${n})
    % endfor

cpp_templates:
    includes: [ '#include <gnuradio/uhd/usrp_${sourk}.h>' ]
    declarations: 'gr::uhd::usrp_${sourk}::sptr ${'$'}{id};'
    make: |
      this->${'$'}{id} = gr::uhd::usrp_${sourk}::make(
         ::uhd::device_addr_t("${'$'}{",".join((str(dev_addr).strip('"\\''), str(dev_args).strip('"\\''))) if len(str(dev_args).strip('"\\'')) > 0 else dev_addr.strip('"\\'')}"),
         ::uhd::stream_args_t("${'$'}{type}", "${'$'}{otw}"));
      % for m in range(max_mboards):
      ${'%'} if context.get('num_mboards')() > ${m}:
      ${'%'} if context.get('sd_spec${m}')():
      this->${'$'}{id}->set_subdev_spec(${'$'}{${'sd_spec' + str(m)}}, ${m});
      ${'%'} endif
      ${'%'} if context.get('time_source${m}')():
      this->${'$'}{id}->set_time_source(${'$'}{${'time_source' + str(m)}}, ${m});
      ${'%'} endif
      ${'%'} if context.get('clock_source${m}')():
      this->${'$'}{id}->set_clock_source("${'$'}{${'clock_source' + str(m)}.strip('\\'')}", ${m});
      ${'%'} endif
      ${'%'} endif
      % endfor
      this->${'$'}{id}->set_samp_rate(${'$'}{samp_rate});
      ${'%'} if sync == 'sync':
      this->${'$'}{id}->set_time_unknown_pps(::uhd::time_spec_t());
      ${'%'} elif sync == 'pc_clock':
      this->${'$'}{id}->set_time_now(::uhd::time_spec_t(time(NULL)), ::uhd::usrp::multi_usrp::ALL_MBOARDS);
      ${'%'} else:
      // No synchronization enforced.
      ${'%'} endif

      % for n in range(max_nchan):
      ${'%'} if context.get('nchan')() > ${n}:
      this->${'$'}{id}->set_center_freq(${'$'}{${'center_freq' + str(n)}}, ${n});
      % if sourk == 'source':
      ${'%'} if context.get('rx_agc${n}')() == 'Enabled':
      this->${'$'}{id}->set_rx_agc(True, ${n});
      ${'%'} elif context.get('rx_agc${n}')() == 'Disabled':
      this->${'$'}{id}->set_rx_agc(False, ${n});
      ${'%'} endif
      ${'%'} if context.get('rx_agc${n}')() != 'Enabled':
      ${'%'} if context.get('gain_type' + '${n}')() == 'normalized':
      this->${'$'}{id}->set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n});
      ${'%'} elif context.get('gain_type' + '${n}')() == 'power':
      this->${'$'}{id}->set_power_reference(${'$'}{${'gain' + str(n)}}, ${n});
      ${'%'} else:
      this->${'$'}{id}->set_gain(${'$'}{${'gain' + str(n)}}, ${n});
      ${'%'} endif
      ${'%'} endif
      % else:
      ${'%'} if context.get('gain_type' + '${n}')() == 'normalized':
      this->${'$'}{id}->set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n});
      ${'%'} elif context.get('gain_type' + '${n}')() == 'power':
      this->${'$'}{id}->set_power_reference(${'$'}{${'gain' + str(n)}}, ${n});
      ${'%'} else:
      this->${'$'}{id}->set_gain(${'$'}{${'gain' + str(n)}}, ${n});
      ${'%'} endif
      % endif
      ${'%'} if context.get('ant${n}')():
      this->${'$'}{id}->set_antenna(${'$'}{${'ant' + str(n)}}, ${n});
      ${'%'} endif
      ${'%'} if context.get('bw${n}')():
      this->${'$'}{id}->set_bandwidth(${'$'}{${'bw' + str(n)}}, ${n});
      ${'%'} endif
      ${'%'} if context.get('show_lo_controls')():
      this->${'$'}{id}->set_lo_source(${'$'}{${'lo_source' + str(n)}}, ::uhd::usrp::multi_usrp::ALL_LOS, ${n});
      this->${'$'}{id}->set_lo_export_enabled(${'$'}{${'lo_export' + str(n)}}, ::uhd::usrp::multi_usrp::ALL_LOS, ${n});
      ${'%'} endif
      ${'%'} endif
      % endfor
      ${'%'} if clock_rate():
      this->${'$'}{id}->set_clock_rate(${'$'}{clock_rate}, ::uhd::usrp::multi_usrp::ALL_MBOARDS);
      ${'%'} endif
      ${'%'} if start_time() >= 0.0:
      this->${'$'}{id}->set_start_time(::uhd::time_spec_t(${'$'}{float(start_time)}));
      ${'%'} endif
    link: ['gnuradio-uhd uhd']
    callbacks:
    - set_samp_rate(${'$'}{samp_rate})
    % for n in range(max_nchan):
    -   set_center_freq(${'$'}{${'center_freq' + str(n)}}, ${n})
    % if sourk == 'source':
    -   ${'$'}{'set_rx_agc(True, ${n})' if context.get('rx_agc${n}')() == 'Enabled' else ''}
    -   ${'$'}{'set_rx_agc(False, ${n})' if context.get('rx_agc${n}')() == 'Disabled' else ''}
    -   |
        ${'%'} if context.get('rx_agc${n}')() != 'Enabled':
        ${'%'} if context.get('gain_type' + '${n}')() == 'normalized':
        this->${'$'}{id}->set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n});
        ${'%'} elif context.get('gain_type' + '${n}')() == 'power':
        this->${'$'}{id}->set_power_reference(${'$'}{${'gain' + str(n)}}, ${n});
        ${'%'} else:
        this->${'$'}{id}->set_gain(${'$'}{${'gain' + str(n)}}, ${n});
        ${'%'} endif
        ${'%'} endif
    % else:
    -   |
        ${'%'} if context.get('gain_type' + '${n}')() == 'normalized':
        this->${'$'}{id}->set_normalized_gain(${'$'}{${'gain' + str(n)}}, ${n});
        ${'%'} elif context.get('gain_type' + '${n}')() == 'power':
        this->${'$'}{id}->set_power_reference(${'$'}{${'gain' + str(n)}}, ${n});
        ${'%'} else:
        this->${'$'}{id}->set_gain(${'$'}{${'gain' + str(n)}}, ${n});
        ${'%'} endif
    % endif
    -   ${'$'}{'set_lo_source(' + lo_source${n} + ', ::uhd::usrp::multi_usrp::ALL_LOS, ${n})' if show_lo_controls else ''}
    -   ${'$'}{'set_lo_export_enabled(' + lo_export${n} + ', ::uhd::usrp::multi_usrp::ALL_LOS, ${n})' if show_lo_controls else ''}
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
    dtype: raw
    default: '0'
    hide: ${'$'}{ 'none' if (nchan > ${n}) else 'all' }
% if sourk == 'source':
-   id: rx_agc${n}
    label: 'Ch${n}: AGC'
    category: RF Options
    dtype: string
    default: 'Default'
    options: ['Default', 'Disabled', 'Enabled']
    option_labels: [Default, Disabled, Enabled]
    hide: ${'$'}{ 'none' if (nchan > ${n}) else 'all' }
% endif
-   id: gain${n}
    label: 'Ch${n}: Gain Value'
    category: RF Options
    dtype: float
    default: '0'
% if sourk == 'source':
    hide: ${'$'}{ 'none' if nchan > ${n} and rx_agc${n} != 'Enabled' else 'all' }
% else:
    hide: ${'$'}{ 'none' if nchan > ${n} else 'all' }
% endif
-   id: gain_type${n}
    label: 'Ch${n}: Gain Type'
    category: RF Options
    dtype: enum
    options: [default, normalized, power]
    option_labels: [Absolute (dB), Normalized, Absolute Power (dBm)]
% if sourk == 'source':
    hide: ${'$'}{ 'all' if nchan <= ${n} or rx_agc${n} == 'Enabled' else ('part' if (eval('gain_type' + str(${n})) == 'default') else 'none')}
% else:
    hide: ${'$'}{ 'all' if nchan <= ${n} else ('part' if (eval('gain_type' + str(${n})) == 'default') else 'none')}
% endif
-   id: ant${n}
    label: 'Ch${n}: Antenna'
    category: RF Options
    dtype: string
% if sourk == 'source':
    options: ['"TX/RX"', '"RX2"', '"RX1"']
    option_labels: [TX/RX, RX2, RX1]
    default: '"RX2"'
% else:
    options: ['"TX/RX"']
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
    dtype: enum
    options: [default, auto, disabled, manual]
    option_labels: [Default, Automatic, Disabled, Manual]
    hide: ${'$'}{ 'all' if not nchan > ${n} else 'part'}
-   id: dc_offs${n}
    label: 'Ch${n}: DC Offset Correction Value'
    category: FE Corrections
    dtype: complex
    default: 0+0j
    hide: ${'$'}{ 'all' if not dc_offs_enb${n} == 'manual' else 'part'}
-   id: iq_imbal_enb${n}
    label: 'Ch${n}: Enable IQ Imbalance Correction'
    category: FE Corrections
    dtype: enum
    options: [default, auto, disabled, manual]
    option_labels: [Default, Automatic, Disabled, Manual]
    hide: ${'$'}{ 'all' if not nchan > ${n} else 'part'}
-   id: iq_imbal${n}
    label: 'Ch${n}: IQ imbalance Correction Value'
    category: FE Corrections
    dtype: complex
    default: 0+0j
    hide: ${'$'}{ 'all' if not iq_imbal_enb${n} == 'manual' else 'part'}
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
    default: '""'
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
