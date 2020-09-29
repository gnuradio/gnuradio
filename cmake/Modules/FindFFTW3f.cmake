# http://tim.klingt.org/code/projects/supernova/repository/revisions/d336dd6f400e381bcfd720e96139656de0c53b6a/entry/cmake_modules/FindFFTW3f.cmake
# Modified to use pkg config and use standard var names

# Find single-precision (float) version of FFTW3

if(NOT PKG_CONFIG_FOUND)
    INCLUDE(FindPkgConfig)
endif()
PKG_CHECK_MODULES(PC_FFTW3f "fftw3f >= 3.0")

FIND_PATH(
    FFTW3f_INCLUDE_DIRS
    NAMES fftw3.h
    HINTS $ENV{FFTW3_DIR}/include
        ${PC_FFTW3f_INCLUDE_DIR}
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    FFTW3f_LIBRARIES
    NAMES fftw3f libfftw3f
    HINTS $ENV{FFTW3_DIR}/lib
        ${PC_FFTW3f_LIBDIR}
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib64
)

FIND_LIBRARY(
    FFTW3f_THREADS_LIBRARIES
    NAMES fftw3f_threads libfftw3f_threads
    HINTS $ENV{FFTW3_DIR}/lib
        ${PC_FFTW3f_LIBDIR}
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib64
)



INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW3f DEFAULT_MSG FFTW3f_LIBRARIES FFTW3f_INCLUDE_DIRS)
MARK_AS_ADVANCED(FFTW3f_LIBRARIES FFTW3f_INCLUDE_DIRS FFTW3f_THREADS_LIBRARIES)

if (FFTW3f_FOUND AND NOT TARGET fftw3f::fftw3f)
  add_library(fftw3f::fftw3f INTERFACE IMPORTED)
  set_target_properties(fftw3f::fftw3f PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${FFTW3f_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES "${FFTW3f_LIBRARIES}"
    )
  if (FFTW3f_THREADS_LIBRARIES)
    set_property(TARGET fftw3f::fftw3f APPEND PROPERTY INTERFACE_LINK_LIBRARIES
      "${FFTW3f_THREADS_LIBRARIES}"
      )
    set_target_properties(fftw3f::fftw3f PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS "FFTW3F_THREADS"
      )
  endif()
endif()
