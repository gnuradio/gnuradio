
########################################################################
# Create and install the grc and grc-docs conf file
########################################################################
blocksdir = join_paths(prefix, GRC_BLOCKS_DIR)
if (prefix == '/usr')
  blocksdir = blocksdir + ':' + join_paths('/usr/local/',GRC_BLOcKS_DIR)
endif

grc_xterm_program = find_program('xterminal-emulator','gnome-terminal','konsole','xterm', required : false)
GRC_XTERM_EXE = ''
if grc_xterm_program.found()
  GRC_XTERM_EXE = grc_xterm_program.full_path()
endif

cdata = configuration_data()
cdata.set('GRC_XTERM_EXE', GRC_XTERM_EXE)
cdata.set('blocksdir', blocksdir)
grc_conf_file = configure_file(input: 'grc.yml.in',
                            output: 'grc.yml',
                            configuration: cdata,
                            install: true,
                            install_dir: GR_PREFSDIR)

subdir('blocks')
subdir('scripts')

install_subdir(
  'core',
  install_dir         : join_paths(py3_install_dir,'gnuradio','grc'),
  strip_directory     : false,
)

install_subdir(
  'converter',
  install_dir         : join_paths(py3_install_dir,'gnuradio','grc'),
  strip_directory     : false,
)

install_subdir(
  'gui',

  install_dir         : join_paths(py3_install_dir,'gnuradio','grc'),
  strip_directory     : false,
)

# Target for pure python
py3.install_sources([
    '__init__.py',
    '__main__.py',
    'compiler.py',
    'main.py',
], 
subdir : join_paths('gnuradio','grc'),
pure : py3_purelib)

#install_data([
#    '__init__.py',
#    '__main__.py',
#    'compiler.py',
#    'main.py',
#], 
#install_dir : join_paths(py3_install_dir,'gnuradio','grc'))
