analog_deps += [gnuradio_gr_dep, gr_kernel_math_lib_dep, gr_kernel_analog_lib_dep, gnuradio_blocklib_filter_dep, 
    volk_dep, fmt_dep, pmtf_dep]

# analog_sources += 'kernel/agc.cc'
block_cpp_args = ['-DHAVE_CPU', '-DHAVE_HIER']
# if IMPLEMENT_CUDA
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_analog_cu = library('gnuradio-blocklib-analog-cu', 
#         analog_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_analog_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_analog_cu,
#                         dependencies : cuda_dep)

#     analog_deps += [gnuradio_blocklib_analog_cu_dep, cuda_dep]

# endif

incdir = include_directories(['../include/gnuradio/analog','../include'])
gnuradio_blocklib_analog_lib = library('gnuradio-blocklib-analog', 
    analog_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : analog_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_analog_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_analog_lib,
                       dependencies : analog_deps)

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'analog')
cmake.configure_package_config_file(
  name : 'gnuradio-analog',
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
             name : 'libgnuradio-analog',
             filebase : 'gnuradio-analog',
             description : 'GNU Radio Analog Module')
