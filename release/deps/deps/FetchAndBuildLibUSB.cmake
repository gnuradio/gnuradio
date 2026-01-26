# Module responsible for fetching, building, and installing LibUSB for
# consumption by subproject
set(LIBUSB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/libusb")
set(LIBUSB_URL "https://github.com/libusb/libusb/archive/refs/tags/v1.0.26.tar.gz")
set(LIBUSB_HASH "a09bff99c74e03e582aa30759cada218ea8fa03580517e52d463c59c0b25e240")
if(WIN32)
    set(LIBUSB_CONF_ARGS "")
    set(LIBUSB_BUILD_ARGS cd msvc && msbuild /property:Configuration=Release libusb_2019.sln)
    set(LIBUSB_INSTALL_COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BINARY_DIR=${CMAKE_BINARY_DIR} -P ${CMAKE_CURRENT_LIST_DIR}/installLibUSB.cmake)
else()
    set(LIBUSB_CONF_ARGS /configure --prefix=${LIBUSB_INSTALL_DIR} --disable-udev --disable-dependency-tracking)
    set(LIBUSB_BUILD_ARGS make -j4)
    set(LIBUSB_INSTALL_COMMAND make install)
endif()

ExternalProject_Add(
    gnu-radio-libusb
    URL ${LIBUSB_URL}
    URL_HASH SHA256=${LIBUSB_HASH}
    PREFIX ${CMAKE_BINARY_DIR}/deps
    INSTALL_DIR ${LIBUSB_INSTALL_DIR}
    CONFIGURE_COMMAND "${LIBUSB_CONF_ARGS}"
    BUILD_COMMAND ${LIBUSB_BUILD_ARGS}
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND ${LIBUSB_INSTALL_COMMAND}
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_INSTALL TRUE
)

set(LIBUSB_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/libusb/include)
set(LIBUSB_LIBRARIES ${CMAKE_INSTALL_PREFIX}/libusb/lib/libusb-1.0.lib)
