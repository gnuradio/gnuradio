# - Config file for the libad9361-iio package
# It defines the following variables
#  AD9361_INCLUDE_DIRS - include directories for FooBar
#  AD9361_LIBRARIES    - libraries to link against
#  LIBAD9361_FOUND     - system has libad9361 installed

find_library(AD9361_LIBRARIES ad9361)
find_path(AD9361_INCLUDE_DIRS ad9361.h)
if(AD9361_LIBRARIES)
    set(LIBAD9361_FOUND TRUE)
endif(AD9361_LIBRARIES)
