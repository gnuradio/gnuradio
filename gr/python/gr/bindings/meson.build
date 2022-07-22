runtime_pybind_sources = files([
    'block_work_io_pybind.cc',
    'port_pybind.cc',
    'tag_pybind.cc',
    'block_pybind.cc',
    'node_pybind.cc',
    'sync_block_pybind.cc',
    'edge_pybind.cc',
    'graph_pybind.cc',
    'hier_block_pybind.cc',
    'flowgraph_pybind.cc',
    'gr_pybind.cc',
    'scheduler_pybind.cc',
    'buffer_pybind.cc',
    'buffer_cpu_vmcirc_pybind.cc',
    'buffer_net_zmq_pybind.cc',
    'constants_pybind.cc',
    'python_block_pybind.cc',
    'pyblock_detail_pybind.cc',
    'runtime_pybind.cc',
    'runtime_proxy_pybind.cc',
    'graph_utils_pybind.cc',
    'rpc_client_interface_pybind.cc',
 ] )

cpp_args = []
runtime_pybind_deps = [gnuradio_gr_dep, python3_dep, pybind11_dep, pmtf_dep]
if USE_CUDA
    runtime_pybind_sources += [
        'buffer_cuda_pybind.cc', 
        'buffer_cuda_pinned.cc',
        'buffer_cuda_sm_pybind.cc']
    runtime_pybind_deps += cuda_dep
    cpp_args = '-DHAVE_CUDA'
endif
gnuradio_runtime_pybind = py3_inst.extension_module('gr_python',
    runtime_pybind_sources, 
    dependencies : runtime_pybind_deps,
    link_language : 'cpp',
    install : true,
    cpp_args : cpp_args,
    install_dir : join_paths(py3_inst.get_install_dir(),'gnuradio','gr')
)

gnuradio_runtime_pybind_dep = declare_dependency(
					   link_with : gnuradio_runtime_pybind)
