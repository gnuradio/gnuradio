setlocal EnableDelayedExpansion
@echo on

:: meson options
(
  echo [binaries]
  echo cmake = '%BUILD_PREFIX%/Library/bin/cmake'
  echo pkgconfig = '%BUILD_PREFIX%/Library/bin/pkg-config'
  echo python = '%PYTHON%'

  echo [built-in options]
  echo backend = 'ninja'
  echo buildtype = 'release'
  echo prefix = '%LIBRARY_PREFIX%'
  echo wrap_mode = 'nodownload'
) >meson-conda.ini
if errorlevel 1 exit 1

set ^"MESON_OPTIONS=^
  --cross-file meson-conda.ini ^
  -D enable_gr_audio=false ^
 ^"
if errorlevel 1 exit 1

if NOT "%cuda_compiler_version%" == "None" (
  set ^"MESON_OPTIONS=!MESON_OPTIONS!^
    -D enable_cuda=true ^
   ^"
)

:: configure build using meson
meson setup buildconda !MESON_OPTIONS!
if errorlevel 1 exit 1

:: print results of build configuration
meson configure buildconda
if errorlevel 1 exit 1

:: build
meson compile -v -C buildconda -j %CPU_COUNT%
if errorlevel 1 exit 1

:: test
meson test -C buildconda --print-errorlogs --timeout-multiplier 10 --num-processes %CPU_COUNT%
if errorlevel 1 exit 1

:: install
meson install -C buildconda --no-rebuild
if errorlevel 1 exit 1
