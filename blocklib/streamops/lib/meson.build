sources = [
]

streamops_sources += sources
streamops_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep]

block_cpp_args = ['-DHAVE_CPU']
link_args = []
if IMPLEMENT_CUDA
    block_cpp_args += '-DHAVE_CUDA'

    gnuradio_blocklib_streamops_cu = library('gnuradio-blocklib-streamops-cu', 
        streamops_cu_sources, 
        include_directories : incdir, 
        install : true, 
        dependencies : [cuda_dep])

    gnuradio_blocklib_streamops_cu_dep = declare_dependency(include_directories : incdir,
                        link_with : gnuradio_blocklib_streamops_cu,
                        dependencies : cuda_dep)

    streamops_deps += [cuda_dep, cusp_dep, gnuradio_blocklib_streamops_cu_dep]

endif

incdir = include_directories(['../include/gnuradio/streamops','../include'])
gnuradio_blocklib_streamops_lib = library('gnuradio-blocklib-streamops', 
    streamops_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    link_args : link_args,
    dependencies : streamops_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_streamops_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_streamops_lib,
                       dependencies : streamops_deps)

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'streamops')
cmake.configure_package_config_file(
  name : 'gnuradio-streamops',
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
             name : 'libgnuradio-streamops',
             filebase : 'gnuradio-streamops',
             description : 'GNU Radio StreamOps Module')