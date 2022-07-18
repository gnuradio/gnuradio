# math_sources += [
#     'fast_atan2f.cc'
# ]

math_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep]

link_args = []
block_cpp_args = ['-DHAVE_CPU', '-DHAVE_NUMPY']
if USE_CUDA
    block_cpp_args += '-DHAVE_CUDA'

    # gnuradio_blocklib_math_cu = library('gnuradio-blocklib-math-cu', 
    #     math_cu_sources, 
    #     include_directories : incdir, 
    #     install : true, 
    #     dependencies : [cuda_dep])

    # gnuradio_blocklib_math_cu_dep = declare_dependency(include_directories : incdir,
    #                     link_with : gnuradio_blocklib_math_cu,
    #                     dependencies : cuda_dep)

    # math_deps += [gnuradio_blocklib_math_cu_dep, cuda_dep, cusp_dep]
    math_deps += [cuda_dep, cusp_dep]

endif

incdir = include_directories(['../include/gnuradio/math','../include'])
gnuradio_blocklib_math_lib = library('gnuradio-blocklib-math', 
    math_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    link_args : link_args,
    dependencies : math_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_math_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_math_lib,
                       dependencies : math_deps)

# TODO - export this as a subproject of gnuradio

conf = configuration_data()
conf.set('prefix', prefix)
conf.set('exec_prefix', '${prefix}')
conf.set('libdir', join_paths('${prefix}',get_option('libdir')))
conf.set('includedir', join_paths('${prefix}',get_option('includedir')))
conf.set('LIBVER', '0.0.0')

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'math')
cmake.configure_package_config_file(
  name : 'gnuradio-math',
  input : join_paths(meson.source_root(),'cmake','Modules','gnuradioConfigModule.cmake.in'),
  install_dir : get_option('prefix') / 'lib' / 'cmake' / 'gnuradio',
  configuration : cmake_conf
)

pkg = import('pkgconfig')
libs = []     # the library/libraries users need to link against
h = ['.'] # subdirectories of ${prefix}/${includedir} to add to header path
pkg.generate(libraries : libs,
             subdirs : h,
             version : meson.project_version(),
             name : 'libgnuradio-math',
             filebase : 'gnuradio-math',
             install_dir : get_option('prefix') / 'lib' / 'pkgconfig',
             description : 'GNU Radio Math Blocks')
