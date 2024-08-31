# Install LIBUSB - WINDOWS ONLY

# Root to shared build of libusb
set(LIBUSB_BUILD_ROOT "${CMAKE_BINARY_DIR}/x64/Release/dll/")
# Grab all dlls
file(GLOB DLLS "${LIBUSB_BUILD_ROOT}*.dll")
message(STATUS "COPYING ${DLLS} DLLS into ${CMAKE_INSTALL_PREFIX}/libusb/bin")
# Grab all import libs
file(GLOB IMP_LIBS ${LIBUSB_BUILD_ROOT}*.lib)
message(STATUS "COPYING ${IMP_LIBS} import libraries into ${CMAKE_INSTALL_PREFIX}/libusb/lib")

# Grab headers
file(GLOB HEADERS "${CMAKE_BINARY_DIR}/libusb/*.h" )
message(STATUS "COPYING ${HEADERS} header files into ${CMAKE_INSTALL_PREFIX}/libusb/include")

file(COPY ${DLLS} DESTINATION ${CMAKE_INSTALL_PREFIX}/libusb/bin)
file(COPY ${IMP_LIBS} DESTINATION ${CMAKE_INSTALL_PREFIX}/libusb/lib)
file(COPY ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/libusb/include)
