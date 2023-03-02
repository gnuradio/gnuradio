"""
Copyright 2023 Ettus Research, A National Instruments Brand.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import sys

MAX_NUM_CHANNELS = 4

MAIN_TMPL = """\
id: uhd_rfnoc_replay
label: RFNoC Replay Block
category: '[Core]/UHD/RFNoC/Blocks'

parameters:
- id: num_in_ports
  label: In Port Count
  dtype: int
  options: [ ${", ".join([str(n) for n in range(1, max_num_chans+1)])} ]
  default: 1
  hide: ${'$'}{ 'part' }
- id: num_out_ports
  label: Out Port Count
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
- domain: message
  id: command
  optional: true
${input_params}

outputs:
- domain: message
  id: debug_msgs
  optional: true
${output_params}

templates:
  imports: |-
    from gnuradio import uhd
  make: |-
    uhd.rfnoc_replay(
        self.rfnoc_graph,
        uhd.device_addr(${'$'}{block_args}),
        ${'$'}{device_select},
        ${'$'}{instance_index})
${init_params}  callbacks:
${callback_params}

documentation: |-
  This RFNoC block can record signals to memory from any of the input ports and play
  back those signals from any of the output ports. The memory throughout the block
  is shared among all ports, so it is the responsibility of the user to manage. The offset
  of memory and size of memory used is controlled by 'Record Offset' (record_offset) and
  'Record Size' (record_size) for Record and 'Play Offset' (play_offset) and 'Play Size'
  (play_size) for playback.

  For both record and playback, the offset and the size must be aligned to the memory's
  word size. For playback, the size must also be aligned to the size of an item. An item
  is a single unit of data as defined by the data type of a port. It is the user's
  responsibility to manage the types of the individual record and the playback ports.

  One key configuration of playback is the packet size. Larger packet sizes provide for
  better throughput while smaller packet sizes provide for lower latency.

  To send commands such as record or play to the replay block, the user must send a
  dictionary command. This can be accomplished using the Replay Msg Push Button.
  The only required key is 'command'. Other optional keys are 'port', 'offset', 'size',
  and 'time'. 'Port' will default to 0, 'time' will default to 0.0. 'offset' and 'size'
  will default to using the block properties.

  There is also an optional debug output port that can be used to print output from
  commands such as getting port record fullness or replay block size

file_format: 1
"""

COEFFS_PARAM = """- id: record_offset${n}
  label: 'Ch${n}: Record Offset'
  dtype: int
  default: 0
  hide: ${'$'}{ 'none' if num_in_ports > ${n} else 'all' }
- id: record_size${n}
  label: 'Ch${n}: Record Size'
  dtype: int
  default: 80000
  hide: ${'$'}{ 'none' if num_in_ports > ${n} else 'all' }
- id: record_type${n}
  label: 'Ch${n}: Record Type'
  dtype: enum
  options: ['s16', 'sc16', 'u8']
  default: 's16'
  hide: ${'$'}{ 'part' if num_in_ports > ${n} else 'all' }
- id: play_offset${n}
  label: 'Ch${n}: Play Offset'
  dtype: int
  default: 0
  hide: ${'$'}{ 'none' if num_out_ports > ${n} else 'all' }
- id: play_size${n}
  label: 'Ch${n}: Play Size'
  dtype: int
  default: 80000
  hide: ${'$'}{ 'none' if num_out_ports > ${n} else 'all' }
- id: play_type${n}
  label: 'Ch${n}: Play Type'
  dtype: enum
  options: ['s16', 'sc16', 'u8']
  default: 's16'
  hide: ${'$'}{ 'part' if num_out_ports > ${n} else 'all' }
"""

INPUT_PARAM = """- domain: rfnoc
  dtype: '${'$'}{record_type${n}}'
  label: 'in${n}'
  hide: ${'$'}{ num_in_ports <= ${n} }
"""

OUTPUT_PARAM = """- domain: rfnoc
  dtype: '${'$'}{play_type${n}}'
  label: 'out${n}'
  hide: ${'$'}{ num_out_ports <= ${n} }
"""

INIT_PARAM = """    ${'%'} if context.get('num_in_ports')() > ${n}:
    self.${'$'}{id}.set_record_type("${'$'}{record_type${n}}", ${n})
    self.${'$'}{id}.set_property('record_offset',${'$'}{record_offset${n}}, ${n}, typename='uint64_t')
    self.${'$'}{id}.set_property('record_size',${'$'}{record_size${n}}, ${n}, typename='uint64_t')
    ${'%'} endif
    ${'%'} if context.get('num_out_ports')() > ${n}:
    self.${'$'}{id}.set_play_type("${'$'}{play_type${n}}", ${n})
    self.${'$'}{id}.set_property('play_offset',${'$'}{play_offset${n}}, ${n}, typename='uint64_t')
    self.${'$'}{id}.set_property('play_size',${'$'}{play_size${n}}, ${n}, typename='uint64_t')
    ${'%'} endif
"""

CALLBACKS_PARAM = """  - |
    ${'%'} if context.get('num_in_ports')() > ${n}:
    self.${'$'}{id}.set_property('record_offset',${'$'}{record_offset${n}}, ${n}, typename='uint64_t')
    ${'%'} endif
  - |
    ${'%'} if context.get('num_in_ports')() > ${n}:
    self.${'$'}{id}.set_property('record_size',${'$'}{record_size${n}}, ${n}, typename='uint64_t')
    ${'%'} endif
  - |
    ${'%'} if context.get('num_in_ports')() > ${n}:
    self.${'$'}{id}.set_record_type("${'$'}{record_type${n}}", ${n})
    ${'%'} endif
  - |
    ${'%'} if context.get('num_out_ports')() > ${n}:
    self.${'$'}{id}.set_property('play_offset',${'$'}{play_offset${n}}, ${n}, typename='uint64_t')
    ${'%'} endif
  - |
    ${'%'} if context.get('num_out_ports')() > ${n}:
    self.${'$'}{id}.set_property('play_size',${'$'}{play_size${n}}, ${n}, typename='uint64_t')
    ${'%'} endif
  - |
    ${'%'} if context.get('num_out_ports')() > ${n}:
    self.${'$'}{id}.set_play_type("${'$'}{play_type${n}}", ${n})
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
    input_params = ''.join([
        parse_tmpl(INPUT_PARAM, n=n)
        for n in range(MAX_NUM_CHANNELS)
    ])
    output_params = ''.join([
        parse_tmpl(OUTPUT_PARAM, n=n)
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
            input_params=input_params,
            output_params=output_params,
            init_params=init_params,
            callback_params=callback_params
        )
    )
