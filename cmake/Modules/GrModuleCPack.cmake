set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS TRUE)
include(InstallRequiredSystemLibraries)

configure_file("${CMAKE_CURRENT_LIST_DIR}/GNuRadioCPackOptions.cmake.in"
  "${GNURadio_BINARY_DIR}/GNURadioCPackOptions.cmake" @ONLY)
set(CPACK_PROJECT_CONFIG_FILE
  "${GNURadio_BINARY_DIR}/GNURadioCPackOptions.cmake")

include(${PROJECT_SOURCE_DIR}/cmake/Modules/BundlePythonDeps.cmake)

include(CPack)

foreach(component ${_gr_enabled_components})
  GR_CONFIGURE_INSTALLER_COMPONENT(${component})
endforeach()
