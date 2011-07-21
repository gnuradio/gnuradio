# http://tim.klingt.org/code/projects/supernova/repository/revisions/d336dd6f400e381bcfd720e96139656de0c53b6a/entry/cmake_modules/FindFFTW3f.cmake
# Find single-precision (float) version of FFTW3

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(FFTW3F "fftw3f >= 3.0")
IF(NOT FFTW3F_FOUND)

FIND_PATH(
    FFTW3F_INCLUDE_DIRS
    NAMES fftw3.h
    HINTS $ENV{FFTW3_DIR}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    FFTW3F_LIBRARIES
    NAMES fftw3f libfftw3f
    HINTS $ENV{FFTW3_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
)

SET(FFTW3F_FOUND "NO")

IF( FFTW3F_INCLUDE_DIRS AND FFTW3F_LIBRARIES )
    SET(FFTW3F_FOUND "YES")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFTW3F DEFAULT_MSG FFTW3F_LIBRARIES FFTW3F_INCLUDE_DIRS)
ENDIF(NOT FFTW3F_FOUND)
