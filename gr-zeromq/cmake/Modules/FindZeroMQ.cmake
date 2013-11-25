# - Try to find ZeroMQ headers and libraries
# - THANKS CUBIT FOR THIS FIND MODULE
#
# Usage of this module as follows:
#
#     find_package(ZeroMQ)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  ZeroMQ_ROOT_DIR  Set this variable to the root installation of
#                            ZeroMQ if the module has problems finding
#                            the proper installation path.
#
# Variables defined by this module:
#
#  ZEROMQ_FOUND              System has ZeroMQ libs/headers
#  ZeroMQ_LIBRARIES          The ZeroMQ libraries
#  ZeroMQ_INCLUDE_DIRS       The location of ZeroMQ headers

find_path(ZeroMQ_ROOT_DIR
    NAMES include/zmq.h
)

find_library(ZeroMQ_LIBRARIES
    NAMES zmq libzmq
    HINTS ${ZeroMQ_ROOT_DIR}/lib
)

find_path(ZeroMQ_INCLUDE_DIRS
    NAMES zmq.h
    HINTS ${ZeroMQ_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZeroMQ DEFAULT_MSG
    ZeroMQ_LIBRARIES
    ZeroMQ_INCLUDE_DIRS
)

mark_as_advanced(
    ZeroMQ_ROOT_DIR
    ZeroMQ_LIBRARIES
    ZeroMQ_INCLUDE_DIRS
)
