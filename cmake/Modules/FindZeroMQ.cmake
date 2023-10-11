find_package(PkgConfig)
pkg_check_modules(PC_ZEROMQ "libzmq")

find_path(
    ZEROMQ_INCLUDE_DIRS
    NAMES zmq.hpp
    HINTS ${PC_ZEROMQ_INCLUDE_DIR} ${CMAKE_INSTALL_PREFIX}/include
    PATHS /usr/local/include /usr/include)

find_library(
    ZEROMQ_LIBRARIES
    NAMES zmq libzmq ${ZEROMQ_LIBRARY_NAME}
    HINTS ${PC_ZEROMQ_LIBDIR} ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/lib64
    PATHS ${ZEROMQ_INCLUDE_DIRS}/../lib /usr/local/lib /usr/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZeroMQ DEFAULT_MSG ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIRS)
mark_as_advanced(ZEROMQ_LIBRARIES ZEROMQ_INCLUDE_DIRS)

if(ZEROMQ_FOUND AND NOT TARGET ZEROMQ::ZEROMQ)
    add_library(ZeroMQ::ZeroMQ INTERFACE IMPORTED)
    set_target_properties(
        ZeroMQ::ZeroMQ PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ZEROMQ_INCLUDE_DIRS}"
                                  INTERFACE_LINK_LIBRARIES "${ZEROMQ_LIBRARIES}")

    # Check for context_t.shutdown() function, not present in some
    # older versions of cppzmq.
    set(CMAKE_REQUIRED_INCLUDES ${ZEROMQ_INCLUDE_DIRS})
    check_cxx_source_compiles("
#include <zmq.hpp>
using _ = decltype(zmq::context_t().shutdown());
int main() { return 0; }
"
      CPPZMQ_HAS_SHUTDOWN
    )
    set(CMAKE_REQUIRED_INCLUDES "")
endif()
