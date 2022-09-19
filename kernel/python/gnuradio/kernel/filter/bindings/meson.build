srcs = files([
    'kernel_filter_pybind.cc',
    'firdes_pybind.cc',
    'pm_remez_pybind.cc'
 ] )

cpp_args = []
deps = [python3_dep, pybind11_dep, gr_kernel_lib_dep]

kernel_filter_pybind = py3.extension_module('kernel_filter_python',
    srcs,
    dependencies : deps,
    include_directories: ['../../../../../include/', incdir_numpy],
    link_language : 'cpp',
    install : true,
    cpp_args : cpp_args,
    install_dir : join_paths(py3.get_install_dir(),'gnuradio','kernel','filter')
)

kernel_filter_pybind_dep = declare_dependency(
					   link_with : kernel_filter_pybind)
