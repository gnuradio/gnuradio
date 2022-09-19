srcs = files([
    'kernel_math_pybind.cc',
    'random_pybind.cc'
 ] )

cpp_args = []
deps = [python3_dep, pybind11_dep, gr_kernel_math_lib_dep]

kernel_math_pybind = py3.extension_module('kernel_math_python',
    srcs,
    dependencies : deps,
    include_directories: ['../../../../include', incdir_numpy],
    link_language : 'cpp',
    install : true,
    cpp_args : cpp_args,
    install_dir : join_paths(py3_install_dir,'gnuradio','kernel','math')
)

kernel_math_pybind_dep = declare_dependency(
					   link_with : kernel_math_pybind)
