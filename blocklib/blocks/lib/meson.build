blocks_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep]

block_cpp_args = ['-DHAVE_CPU']
link_args = []
# if IMPLEMENT_CUDA
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_blocks_cu = library('gnuradio-blocklib-blocks-cu', 
#         blocks_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_blocks_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_blocks_cu,
#                         dependencies : cuda_dep)

#     blocks_deps += [gnuradio_blocklib_blocks_cu_dep, cuda_dep, cusp_dep]
# endif 

incdir = include_directories(['../include/gnuradio/blocks','../include'])
gnuradio_blocklib_blocks_lib = library('gnuradio-blocklib-blocks', 
    blocks_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : blocks_deps,
    link_args : link_args,
    cpp_args : block_cpp_args)

gnuradio_blocklib_blocks_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_blocks_lib,
                       dependencies : blocks_deps)

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'blocks')
cmake.configure_package_config_file(
  name : 'gnuradio-blocks',
  input : join_paths(meson.source_root(),'cmake','Modules','gnuradioConfigModule.cmake.in'),
  install_dir : get_option('prefix') / get_option('libdir') / 'cmake' / 'gnuradio',
  configuration : cmake_conf
)

pkg = import('pkgconfig')
libs = []     # the library/libraries users need to link against
h = ['.'] # subdirectories of ${prefix}/${includedir} to add to header path
pkg.generate(libraries : libs,
             subdirs : h,
             version : meson.project_version(),
             name : 'libgnuradio-blocks',
             filebase : 'gnuradio-blocks',
             description : 'GNU Radio General Purpose Blocks')
