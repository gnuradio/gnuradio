"""
Copyright 2023 Ettus Research, A National Instruments Brand.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import sys

MAX_NUM_CHANNELS = 4

MAIN_TMPL = """\
id: uhd_rfnoc_rx_radio
label: RFNoC RX Radio
flags: [python, throttle]
category: '[Core]/UHD/RFNoC/Blocks'

parameters:
- id: num_chans
  label: Channel count
  dtype: int
  default: 1
  hide: ${'$'}{ 'part' }
- id: block_args
  label: Block Args
  dtype: string
  default: ""
  hide: ${'$'}{ 'part' if not block_args else 'none'}
- id: device_select
  label: Device Select
  dtype: int
  default: -1
  hide: ${'$'}{ 'part' if device_select == -1 else 'none'}
- id: instance_index
  label: Instance Select
  dtype: int
  default: -1
  hide: ${'$'}{ 'part' if instance_index == -1 else 'none'}
- id: rate
  label: Sample Rate (Hz)
  dtype: real
${coeffs_params}

outputs:
- domain: rfnoc
  dtype: 'sc16'
  vlen: 1
  multiplicity: ${'$'}{num_chans}

inputs:
- domain: message
  id: command
  optional: true

templates:
  imports: |-
    from gnuradio import uhd
  make: |-
    uhd.rfnoc_rx_radio(
        self.rfnoc_graph,
        uhd.device_addr(${'$'}{block_args}),
        ${'$'}{device_select},
        ${'$'}{instance_index})
    self.${'$'}{id}.set_rate(${'$'}{rate})
${init_params}  callbacks:
  - set_rate(${'$'}{rate})
${callback_params}

file_format: 1
"""

COEFFS_PARAM = """- id: antenna${n}
  label: 'Ch${n}: Antenna Select'
  dtype: string
  options: [TX/RX, RX2, RX1]
  option_labels: [TX/RX, RX2, RX1]
  default: RX2
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
- id: frequency${n}
  label: 'Ch${n}: Center Frequency (Hz)'
  dtype: real
  default: 1e9
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
- id: gain${n}
  label: 'Ch${n}: Gain'
  dtype: real
  default: 0
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
- id: bandwidth${n}
  label: 'Ch${n}: Bandwidth (Hz)'
  dtype: real
  default: 0
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
- id: dc_offset${n}
  label: 'Ch${n}: DC Offset Correction'
  dtype: bool
  options: ['True', 'False']
  default: 'False'
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
- id: iq_balance${n}
  label: 'Ch${n}: IQ Balance'
  dtype: bool
  options: ['True', 'False']
  default: 'False'
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
- id: enable_timestamps${n}
  label: 'Ch${n}: Enable Timestamps'
  dtype: bool
  options: ['True', 'False']
  default: 'True'
  hide: ${'$'}{ 'all' if ${n} >= num_chans else 'part' if enable_timestamps${n} else 'none' }
"""

INIT_PARAM = """    ${'%'} if context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_antenna(${'$'}{antenna${n}}, ${n})
    self.${'$'}{id}.set_gain(${'$'}{gain${n}}, ${n})
    self.${'$'}{id}.set_bandwidth(${'$'}{bandwidth${n}}, ${n})
    self.${'$'}{id}.set_frequency(${'$'}{frequency${n}}, ${n})
    self.${'$'}{id}.set_dc_offset(${'$'}{dc_offset${n}}, ${n})
    self.${'$'}{id}.set_iq_balance(${'$'}{iq_balance${n}}, ${n})
    self.${'$'}{id}.enable_rx_timestamps(${'$'}{enable_timestamps${n}}, ${n})
    ${'%'} endif
"""

CALLBACKS_PARAM = """  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    set_antenna(${'$'}{antenna${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    set_gain(${'$'}{gain${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    set_bandwidth(${'$'}{bandwidth${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    set_frequency(${'$'}{frequency${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    set_dc_offset(${'$'}{dc_offset${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    set_iq_balance(${'$'}{iq_balance${n}}, ${n})
    ${'%'} endif
"""


def parse_tmpl(_tmpl, **kwargs):
    """ Render _tmpl using the kwargs """
    from mako.template import Template

    block_template = Template(_tmpl)
    return str(block_template.render(**kwargs))


if __name__ == '__main__':
    file = sys.argv[1]
    coeffs_params = ''.join([
        parse_tmpl(COEFFS_PARAM, n=n)
        for n in range(MAX_NUM_CHANNELS)
    ])
    init_params = ''.join([
        parse_tmpl(INIT_PARAM, n=n)
        for n in range(MAX_NUM_CHANNELS)
    ])
    callback_params = ''.join([
        parse_tmpl(CALLBACKS_PARAM, n=n)
        for n in range(MAX_NUM_CHANNELS)
    ])
    open(file, 'w').write(
        parse_tmpl(
            MAIN_TMPL,
            max_num_chans=MAX_NUM_CHANNELS,
            coeffs_params=coeffs_params,
            init_params=init_params,
            callback_params=callback_params
        )
    )
