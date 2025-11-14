set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS TRUE)
include(InstallRequiredSystemLibraries)
get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
set(GNURadio_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(GNURadio_BINARY_DIR ${CMAKE_BINARY_DIR})

set(CPACK_PACKAGE_NAME "GNU Radio")
# Need to define CMAKE_PROJECT_VERSION_* variables. If they are not defined, 
# then CPACK_PACKAGE_VERSION_* will follow the CMAKE_PROJECT_VERSION_* defaults.
set(CMAKE_PROJECT_VERSION_MAJOR ${GR_VERSION_MAJOR})
set(CMAKE_PROJECT_VERSION_MINOR ${GR_VERSION_API})
set(CMAKE_PROJECT_VERSION_PATCH ${GR_VERSION_MINOR})
set(CMAKE_PROJECT_VERSION_TWEAK ${GR_VERSION_MAINT})
set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
# Note: CPACK does only provide 3 level version information, don't care about _MAINT or _TWEAK part

message(STATUS "DEBUG --- Version ${VERSION}, Dock Version ${DOCVER}, Lib Version ${LIBVER}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "GNU Radio")
set(CPACK_PACKAGE_VENDOR "https://www.gnuradio.org/")
set(CPACK_PACKAGE_DESCRIPTION
  "the Free and Open Software Radio Ecosystem")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(CPACK_PACKAGE_COPYRIGHT           "Copyright © 2009-${CURRENT_YEAR} Free Software Foundation, Inc.")

if(APPLE)
  set(CPACK_PACKAGE_ICON
    "${GNURadio_SOURCE_DIR}/release/resources/icons/gnuradio_logo_icon-square.ico")
  set(CPACK_BUNDLE_ICON "${CPACK_PACKAGE_ICON}")
  # set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
  # set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK ON)
  set(CPACK_PACKAGING_INSTALL_PREFIX "/GNU Radio")

endif()

if(WIN32)
  install(FILES release/win32/regenbin.bat DESTINATION ".")
  file(COPY_FILE COPYING ${CMAKE_BINARY_DIR}/COPYING.txt)
  set(COPYING_EXT .txt)
  set(GR_WIX_PRODUCT_ICON
    "${CMAKE_SOURCE_DIR}/release/resources/icons/gnuradio_logo_icon-square.ico"
  )
  set(GR_RELEASE_ICON_DIR "${CMAKE_SOURCE_DIR}/release/resources/icons")

  # settiing the CPACK_NSIS_DEFINES variable does not seem to work
  # (is included in CPackConfig.cmake but does not make it to project.nsi)
  set(CPACK_NSIS_DEFINES "FOOOOOOO" FORCE)

  # misusing CPACK_NSIS_BRANDING_TEXT_CODE to define the version info for the installer binary.
  #set(CPACK_NSIS_MANIFEST_DPI_AWARE_CODE
  #"${CPACK_NSIS_MANIFEST_DPI_AWARE_CODE}
  set(CPACK_NSIS_BRANDING_TEXT_CODE
    "${CPACK_NSIS_BRANDING_TEXT_CODE}
VIProductVersion ${GR_VERSION_MAJOR}.${GR_VERSION_API}.${GR_VERSION_MINOR}.${GR_VERSION_MAINT}-${GR_GIT_HASH}
VIFileVersion ${GR_VERSION_MAJOR}.${GR_VERSION_API}.${GR_VERSION_MINOR}.${GR_VERSION_MAINT}
VIAddVersionKey /LANG=0 \\\"ProductName\\\" \\\"${CPACK_PACKAGE_NAME}\\\"
VIAddVersionKey /LANG=0 \\\"ProductVersion\\\" \\\"${GR_VERSION_MAJOR}.${GR_VERSION_API}.${GR_VERSION_MINOR}.${GR_VERSION_MAINT}-${GR_GIT_HASH}\\\"
VIAddVersionKey /LANG=0 \\\"Comments\\\" \\\"${CPACK_PACKAGE_DESCRIPTION}\\\"
VIAddVersionKey /LANG=0 \\\"CompanyName\\\" \\\"${CPACK_PACKAGE_VENDOR}\\\"
VIAddVersionKey /LANG=0 \\\"LegalCopyright\\\" \\\"${CPACK_PACKAGE_COPYRIGHT}\\\"
VIAddVersionKey /LANG=0 \\\"FileDescription\\\" \\\"${CPACK_PACKAGE_NAME} Installer\\\"
VIAddVersionKey /LANG=0 \\\"FileVersion\\\" \\\"${GR_VERSION_MAJOR}.${GR_VERSION_API}.${GR_VERSION_MINOR}.${GR_VERSION_MAINT}\\\""
)

  # explicitly set version for installer GUI, and start menu folder
  set(CPACK_NSIS_DISPLAY_NAME 
    "${CPACK_PACKAGE_NAME}-${GR_VERSION_MAJOR}.${GR_VERSION_API}.${GR_VERSION_MINOR}.${GR_VERSION_MAINT}-${GR_GIT_HASH}"
  )
  # explicitly set version for installer file name, since we would like to have the git hash in the file name
  set(CPACK_PACKAGE_FILE_NAME 
    "${CPACK_PACKAGE_NAME}-${GR_VERSION_MAJOR}.${GR_VERSION_API}.${GR_VERSION_MINOR}.${GR_VERSION_MAINT}-${GR_GIT_HASH}-${CMAKE_SYSTEM_PROCESSOR}"
  )

#CPACK_PACKAGE_INSTALL_DIRECTORY -- Variable not defined
  #Variables starting with '$' but do not use {} are expanded by NSIS at build time.
  #Variables starting with '$' and using {} are expanded by CMAKE during generation
  #of CPackSourceConfig.cmake
  set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "${CPACK_NSIS_CREATE_ICONS_EXTRA}
     CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\GNU Radio Companion.lnk' '$WINDIR\\\\system32\\\\cmd.exe' '/C \\\"$INSTDIR\\\\bin\\\\gnuradio-companion.bat\\\"'
     CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\GNU Radio Command Prompt.lnk' '$WINDIR\\\\system32\\\\cmd.exe' '/C \\\"$INSTDIR\\\\bin\\\\gnuradio-prompt.bat\\\"'
     CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\GNU Radio Documentation.lnk' 'https://wiki.gnuradio.org/'"
  )
  set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "${CPACK_NSIS_DELETE_ICONS_EXTRA}
      Delete '$SMPROGRAMS\\\\$START_MENU\\\\GNU Radio Companion.lnk'
      Delete '$SMPROGRAMS\\\\$START_MENU\\\\GNU Radio Command Prompt.lnk'
      Delete '$SMPROGRAMS\\\\$START_MENU\\\\GNU Radio Documentation.lnk'"
  )

endif()

file(COPY_FILE COPYING ${CMAKE_BINARY_DIR}/COPYING${COPYING_EXT})

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/COPYING${COPYING_EXT}")
set(CPACK_PACKAGE_CONTACT "admin@gnuradio.org")

set(CPACK_WIX_UPGRADE_GUID  "4EE94E3E-1960-4446-B0F8-7F0A3D8CA001")

set(CPACK_WIX_EXTRA_SOURCES "${CMAKE_BINARY_DIR}/launcher.wxs")

configure_file("${CMAKE_CURRENT_LIST_DIR}/GNURadioCPackOptions.cmake.in"
  "${GNURadio_BINARY_DIR}/GNURadioCPackOptions.cmake" @ONLY)
set(CPACK_PROJECT_CONFIG_FILE
  "${GNURadio_BINARY_DIR}/GNURadioCPackOptions.cmake")

include(${PROJECT_SOURCE_DIR}/cmake/Modules/BundlePythonDeps.cmake)

include(CPack)

# Test, but CPACK defines after 'include(CPack)' should be ignored anyway.
set(CPACK_NSIS_DEFINES "${CPACK_NSIS_DEFINES} \\\"FOOOOOO2\\\"")

foreach(component ${_gr_enabled_components})
  GR_CONFIGURE_INSTALLER_COMPONENT(${component})
endforeach()
