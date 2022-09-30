#!/usr/bin/env bash

set -ex

if [[ $target_platform == osx* ]] ; then
    CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
fi

cat <<EOT >> meson-conda.ini
[binaries]
cmake = '$BUILD_PREFIX/bin/cmake'
pkgconfig = '$BUILD_PREFIX/bin/pkg-config'
python = '$PYTHON'

[built-in options]
buildtype = 'release'
libdir = 'lib'
prefix = '$PREFIX'
wrap_mode = 'nodownload'
EOT
cat meson-conda.ini

meson_config_args=(
    --cross-file meson-conda.ini
)

if [[ $target_platform == osx* ]] ; then
    meson_config_args+=(
        -D enable_gr_audio=false
    )
fi

if [[ ${cuda_compiler_version} != "None" ]]; then
    meson_config_args+=(
        -D enable_cuda=true
    )
fi

# MESON_ARGS is set by conda compiler activation script
meson setup buildconda \
    ${MESON_ARGS} \
    "${meson_config_args[@]}" \
    || (cat buildconda/meson-logs/meson-log.txt; false)
meson compile -v -C buildconda -j ${CPU_COUNT}

meson test -C buildconda --print-errorlogs --timeout-multiplier 10 --num-processes ${CPU_COUNT}
meson install -C buildconda --no-rebuild
