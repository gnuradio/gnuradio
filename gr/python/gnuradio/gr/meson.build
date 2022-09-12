
subdir('bindings')

srcs = [
    '__init__.py',
    'gateway_numpy.py',
    'gateway_cupy.py',
    'numpy_helpers.py'
    ]

if USE_CUDA
    srcs += 'gateway_cupy.py'
endif

foreach s: srcs
configure_file(copy: true,
    input: s,
    output: s
)
endforeach

py3_inst.install_sources(files(srcs), subdir : join_paths('gnuradio','gr'))
