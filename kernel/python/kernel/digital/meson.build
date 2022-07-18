
subdir('bindings')

srcs = [
    '__init__.py',
    ]

foreach s: srcs
configure_file(copy: true,
    input: s,
    output: s
)
endforeach

py3_inst.install_sources(files(srcs), subdir : join_paths('gnuradio', 'kernel', 'digital'))
