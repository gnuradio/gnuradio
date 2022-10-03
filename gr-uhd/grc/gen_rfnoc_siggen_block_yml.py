"""
Copyright 2021 Ettus Research, A National Instruments Brand.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import sys

MAX_NUM_CHANNELS = 16

MAIN_TMPL = """\
id: uhd_rfnoc_siggen
label: RFNoC SigGen Block
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
${siggen_params}
outputs:
- domain: rfnoc
  dtype: 'sc16'
  multiplicity: ${'$'}{num_chans}

templates:
  imports: |-
    from gnuradio import uhd
  make: |-
    uhd.rfnoc_siggen(
        self.rfnoc_graph,
        uhd.device_addr(${'$'}{block_args}),
        ${'$'}{device_select},
        ${'$'}{instance_index})
${init_params}  callbacks:
${callback_params}

documentation: |-
  RFNoC SigGen Block:
  A simple signal generator that does not require an input data steam to
  generate output.

  Channel count:
  Number of channels / streams to use with the RFNoC SigGen block. Note,
  this is defined by the RFNoC SigGen Block's FPGA build parameters and
  GNU Radio Companion is not aware of this value. An error will occur at
  runtime when connecting blocks if the number of channels is too large.

file_format: 1
"""

SIGGEN_PARAM = """- id: signal_type${n}
  label: 'Ch${n}: Signal Type'
  dtype: enum
  options: ['NOISE', 'CONSTANT', 'SINE_WAVE']
  default: 'NOISE'
  hide: ${'$'}{ 'part' if num_chans > ${n} else 'all'}
- id: sample_rate${n}
  label: 'Ch${n}: Sample Rate'
  dtype: float
  default: samp_rate
  hide: ${'$'}{'part' if num_chans > ${n} else 'all'}
- id: enable${n}
  label: 'Ch${n}: Enable'
  dtype: bool
  default: true
  hide: ${'$'}{'part' if num_chans > ${n} else 'all'}
- id: signal_frequency${n}
  label: 'Ch${n}: Frequency'
  dtype: float
  default: 0.0
  hide: ${'$'}{ 'none' if (signal_type${n} == 'SINE_WAVE') and num_chans > ${n} else 'all' }
- id: signal_amplitude${n}
  label: 'Ch${n}: Amplitude'
  dtype: float
  default: 1.0
  hide: ${'$'}{ 'none' if num_chans > ${n} else 'all' }
"""

INIT_PARAM = """    ${'%'} if signal_type${n} == 'NOISE' and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_amplitude(${'$'}{signal_amplitude${n}}, ${n})
    ${'%'} endif
    ${'%'} if signal_type${n} == 'SINE_WAVE' and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_sine_frequency(${'$'}{signal_frequency${n}}, ${'$'}{sample_rate${n}}, ${n})
    self.${'$'}{id}.set_amplitude(${'$'}{signal_amplitude${n}}, ${n})
    ${'%'} endif
    ${'%'} if signal_type${n} == 'CONSTANT' and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_constant(${'$'}{signal_amplitude${n}}, ${n})
    ${'%'} endif
    ${'%'} if context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_waveform('${'$'}{signal_type${n}}', ${n})
    self.${'$'}{id}.set_enable(${'$'}{enable${n}}, ${n})
    ${'%'} endif
"""

CALLBACKS_PARAM = """  - |
    ${'%'} if signal_type${n} == 'NOISE' and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_amplitude(${'$'}{signal_amplitude${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if signal_type${n} == 'SINE_WAVE' and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_sine_frequency(${'$'}{signal_frequency${n}}, ${'$'}{sample_rate${n}}, ${n})
    self.${'$'}{id}.set_amplitude(${'$'}{signal_amplitude${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if signal_type${n} == 'CONSTANT' and context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_constant(${'$'}{signal_amplitude${n}}, ${n})
    ${'%'} endif
  - |
    ${'%'} if context.get('num_chans')() > ${n}:
    self.${'$'}{id}.set_waveform('${'$'}{signal_type${n}}', ${n})
    self.${'$'}{id}.set_enable(${'$'}{enable${n}}, ${n})
    ${'%'} endif

"""


def parse_tmpl(_tmpl, **kwargs):
    """ Render _tmpl using the kwargs """
    from mako.template import Template

    block_template = Template(_tmpl)
    return str(block_template.render(**kwargs))


if __name__ == '__main__':
    file = sys.argv[1]
    siggen_params = ''.join([
        parse_tmpl(SIGGEN_PARAM, n=n)
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
            siggen_params=siggen_params,
            init_params=init_params,
            callback_params=callback_params
        )
    )
