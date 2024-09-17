set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS TRUE)
include(InstallRequiredSystemLibraries)
get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
set(GNURadio_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(GNURadio_BINARY_DIR ${CMAKE_BINARY_DIR})

set(CPACK_PACKAGE_NAME "GNU Radio")
set(CPACK_PACKAGE_VERSION_MAJOR ${GNURadio_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${GNURadio_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${GNURadio_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${GNURadio_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "GNU Radio")
set(CPACK_PACKAGE_VENDOR "https://www.gnuradio.org/")
set(CPACK_PACKAGE_DESCRIPTION
  "the Free and Open Software Radio Ecosystem")

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

foreach(component ${_gr_enabled_components})
  GR_CONFIGURE_INSTALLER_COMPONENT(${component})
endforeach()
