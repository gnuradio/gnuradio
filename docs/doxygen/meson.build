doxygen = find_program('doxygen', required : false)

if doxygen.found()
  cdata = configuration_data()
  cdata.set('VERSION', meson.project_version())

  if find_program('dot', required : false).found()
    cdata.set('HAVE_DOT', 'YES')
  else
    cdata.set('HAVE_DOT', 'NO')
  endif

  cdata.set('PROJECT_NAME',meson.project_name())
  cdata.set('PROJECT_LOGO', join_paths(meson.current_source_dir()))
  cdata.set('TOP_SRCDIR', meson.project_source_root())
  cdata.set('TOP_BUILDDIR', meson.build_root())
  cdata.set('IMAGE_PATH', join_paths(meson.current_source_dir(), 'images'))
  cdata.set('ENABLE_HTML_DOCS', 'YES')
  cdata.set('ENABLE_LATEX_DOCS', 'NO')
  cdata.set('ENABLE_XML_DOCS', 'YES')
  cdata.set('CURRENT_SRC_DIR', join_paths(meson.current_source_dir()))

  doxyfile = configure_file(input: 'Doxyfile.in',
                            output: 'Doxyfile',
                            configuration: cdata,
                            install: false)

  doc_target = run_target('doxygen', command: [doxygen, doxyfile])

  # datadir = join_paths(get_option('datadir'), 'doc','gnuradio')

  # # TODO: Figure out how to install docs
  # html_target = custom_target('doc',
	# 		      build_always_stale : not meson.is_subproject(),
  #                             input: doxyfile,
  #                             output: 'html',
  #                             command: [doxygen, doxyfile],
  #                             install: false,
  #                             install_dir: datadir)

endif
