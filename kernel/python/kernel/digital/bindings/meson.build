srcs = files([
    'kernel_digital_pybind.cc',
    'constellation_pybind.cc'
 ] )

cpp_args = []
deps = [python3_dep, pybind11_dep, gr_kernel_lib_dep]

kernel_digital_pybind = py3_inst.extension_module('kernel_digital_python',
    srcs, 
    dependencies : deps,
    include_directories: ['../../../../include/'],
    link_language : 'cpp',
    install : true,
    cpp_args : cpp_args,
    install_dir : join_paths(py3_inst.get_install_dir(),'gnuradio','kernel','digital')
)

kernel_digital_pybind_dep = declare_dependency(
					   link_with : kernel_digital_pybind)
