set(VOLK_URL https://www.libvolk.org/releases/volk-3.1.2.tar.gz)
set(VOLK_HASH eded90e8a3958ee39376f17c1f9f8d4d6ad73d960b3dd98cee3f7ff9db529205)
set(VOLK_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/volk)
include(ExternalProject)
ExternalProject_Add(
    gnu-radio-volk
    URL ${VOLK_URL}
    URL_HASH SHA256=${VOLK_HASH}
    PREFIX ${CMAKE_BINARY_DIR}/deps
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX=${VOLK_INSTALL_DIR}
)
