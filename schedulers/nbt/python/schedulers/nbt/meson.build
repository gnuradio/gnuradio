subdir('bindings')

srcs = ['__init__.py']
foreach s: srcs
configure_file(copy: true,
    input: s,
    output: s
)
endforeach

py3_inst.install_sources(files('__init__.py'), subdir : join_paths('gnuradio','schedulers','nbt'))