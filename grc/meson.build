
########################################################################
# Create and install the grc and grc-docs conf file
########################################################################
blocksdir = join_paths(prefix, GRC_BLOCKS_DIR)
if (prefix == '/usr')
  blocksdir = blocksdir + ':' + join_paths('/usr/local/',GRC_BLOcKS_DIR)
endif

GRC_XTERM_EXE = find_program('xterminal-emulator','gnome-terminal','konsole','xterm', required : false).full_path()

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
  install_dir         : join_paths(py3_inst.get_install_dir(),'gnuradio','grc'),
  strip_directory     : false,
)

install_subdir(
  'converter',
  install_dir         : join_paths(py3_inst.get_install_dir(),'gnuradio','grc'),
  strip_directory     : false,
)

install_subdir(
  'gui',
  install_dir         : join_paths(py3_inst.get_install_dir(),'gnuradio','grc'),
  strip_directory     : false,
)

# Target for pure python
py3_inst.install_sources([
    '__init__.py',
    '__main__.py',
    'compiler.py',
    'main.py',
], 
subdir : join_paths('gnuradio','grc'))
