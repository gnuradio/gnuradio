set(PA_URL https://github.com/PortAudio/portaudio/archive/refs/tags/v19.7.0.tar.gz)
set(PA_HASH 5af29ba58bbdbb7bbcefaaecc77ec8fc413f0db6f4c4e286c40c3e1b83174fa0)
set(PA_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/portaudio)


ExternalProject_Add(
    gnu-radio-portaudio
    URL ${PA_URL}
    URL_HASH SHA256=${PA_HASH}
    PREFIX ${CMAKE_BINARY_DIR}/deps
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PA_INSTALL_DIR}
)
