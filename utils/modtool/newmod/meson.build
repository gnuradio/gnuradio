
###############################################################################
# If this is an OOT module, the following top level project things are needed
###############################################################################
project('ns-newmod', 'cpp', 
  version : '0.0.0',
  license : 'GPLv3',
  default_options : ['cpp_std=c++17'])

python3_dep = dependency('python3', required : get_option('enable_python'))
python3_embed_dep = dependency('python3-embed', required : get_option('enable_python'))
py3_inst = import('python').find_installation('python3')
pybind11_dep = dependency('pybind11', required : get_option('enable_python'))

prefix = get_option('prefix')
# Escape spaces
prefix_parts = prefix.split(' ')
prefix = '\ '.join(prefix_parts)

cuda_available = add_languages('cuda', required : false)
IMPLEMENT_CUDA = cuda_available and get_option('enable_cuda')
IMPLEMENT_CPU = true

###############################################################################
# IF Out of Tree Module
###############################################################################
share_dir = join_paths(prefix,'share')
SCRIPTS_DIR = join_paths(share_dir, 'utils','blockbuilder','scripts' )
###############################################################################
# Else In-Tree module
###############################################################################
# share_dir = meson.project_source_root()

###############################################################################
# Dependencies
###############################################################################
volk_dep = dependency('volk', version : '>=2.2')
fmt_dep = dependency('fmt', method: 'cmake', modules: ['fmt::fmt'])
pmtf_dep = dependency('pmtf', version : '>= 0.0.2')
gnuradio_gr_dep = dependency('gnuradio-runtime')

run_command('python3', join_paths(SCRIPTS_DIR,'gen_meson.py'), 
  join_paths(meson.project_source_root(),'blocklib'), check: true)
  
subdir('blocklib/newmod')

