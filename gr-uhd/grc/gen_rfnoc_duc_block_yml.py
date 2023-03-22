"""
Copyright 2023 Ettus Research, A National Instruments Brand.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import sys

MAX_NUM_CHANNELS = 4

MAIN_TMPL = """\
id: uhd_rfnoc_duc
label: RFNoC Digital Upconverter Block
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
${coeffs_params}

inputs:
- domain: rfnoc
  dtype: 'sc16'
  vlen: 1
  multiplicity: ${'$'}{num_chans}

outputs:
- domain: rfnoc
  dtype: 'sc16'
  vlen: 1
  multiplicity: ${'$'}{num_chans}

templates:
  imports: |-
    from gnuradio import uhd
  make: |-
    uhd.rfnoc_duc(
        self.rfnoc_graph,
        uhd.device_addr(${'$'}{block_args}),
        ${'$'}{device_select},
        ${'$'}{instance_index})
${init_params}  callbacks:
${callback_params}

file_format: 1
"""

COEFFS_PARAM = """- id: freq${n}
  label: 'Ch${n}: Frequency Shift (Hz)'
  dtype: real
  default: 0
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
- id: input_rate${n}
  label: 'Ch${n}: Input Rate (Hz)'
  dtype: real
  default: 0
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
"""

INIT_PARAM = """    ${'%'} if context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_freq(${'$'}{freq${n}}, ${n})
    self.${'$'}{id}.set_input_rate(${'$'}{input_rate${n}}, ${n})
    ${'%'} endif
"""

CALLBACKS_PARAM = """  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    set_freq(${'$'}{freq${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    set_input_rate(${'$'}{input_rate${n}}, ${n})
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
