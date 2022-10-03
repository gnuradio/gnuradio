"""
Copyright 2022 Ettus Research, A National Instruments Brand.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import sys

MAX_NUM_CHANNELS = 16

MAIN_TMPL = """\
id: uhd_rfnoc_fir_filter
label: RFNoC FIR Filter Block
category: '[Core]/UHD/RFNoC/Blocks'

parameters:
- id: num_chans
  label: Channel count
  dtype: int
  options: [ ${", ".join([str(n) for n in range(1, max_num_chans+1)])} ]
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
  multiplicity: ${'$'}{num_chans}

outputs:
- domain: rfnoc
  dtype: 'sc16'
  multiplicity: ${'$'}{num_chans}

templates:
  imports: |-
    from gnuradio import uhd
  make: |-
    uhd.rfnoc_fir_filter(
        self.rfnoc_graph,
        uhd.device_addr(${'$'}{block_args}),
        ${'$'}{device_select},
        ${'$'}{instance_index})
${init_params}  callbacks:
${callback_params}

documentation: |-
  RFNoC FIR Filter Block:
  Applies a FIR filter with user defined coefficients to the input sample
  stream.

  Channel count:
  Number of channels / streams to use with the RFNoC FIR filter block.
  Note, this is defined by the RFNoC FIR Filter Block's FPGA build
  parameters and GNU Radio Companion is not aware of this value. An error
  will occur at runtime when connecting blocks if the number of channels is
  too large.

  Coefficient Type:
  Choice between floating point or integer coefficients.
  Floating point coefficients must be within range [-1.0, 1.0].
  Integer coefficients must be within range [-32768, 32767].

  Coefficients:
  Array of coefficients. Number of elements in the array implicitly sets
  the filter length, and must be less than or equal to the maximum number
  of coefficients supported by the block.

file_format: 1
"""

COEFFS_PARAM = """- id: coeffs_type${n}
  label: 'Ch${n}: Coeffs Type'
  dtype: enum
  options: ['float', 'integer']
  default: 'float'
  hide: ${'$'}{ 'part' if num_chans > ${n} else 'all' }
- id: coeffs_float${n}
  label: 'Ch${n}: Coefficients'
  dtype: float_vector
  default: [1.0]
  hide: ${'$'}{ 'none' if coeffs_type${n} == 'float' and num_chans > ${n} else 'all' }
- id: coeffs_int${n}
  label: 'Ch${n}: Coefficients'
  dtype: int_vector
  default: [32767]
  hide: ${'$'}{ 'none' if coeffs_type${n} == 'integer' and num_chans > ${n} else 'all' }
"""

INIT_PARAM = """    ${'%'} if coeffs_type${n} == "float" and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_coefficients(${'$'}{coeffs_float${n}}, ${n})
    ${'%'} endif
    ${'%'} if coeffs_type${n} == "integer" and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_coefficients(${'$'}{coeffs_int${n}}, ${n})
    ${'%'} endif
"""

CALLBACKS_PARAM = """  - |
    ${'%'} if coeffs_type${n} == "float" and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_coefficients(${'$'}{coeffs_float${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if coeffs_type${n} == "integer" and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_coefficients(${'$'}{coeffs_int${n}}, ${n})
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
