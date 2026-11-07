#!/usr/bin/env bash
# Copyright 2026 Marcus Müller
# shellcheck source=.github/ci-tools/common.bash
source "$(dirname "$(realpath "$0")")/common.bash"
builddir="$2"
if type -p sccache > /dev/null ; then
  stats="$(sccache -s)"
  errors="$(printf '%s' "${stats}" | sed -n 's/\(.*\) errors[[:space:]]*\([0-9]*\)$/\1:\2/p')"
  misses="$(printf '%s' "${stats}" | sed -n 's/Cache misses[[:space:]]*\([0-9]*\)$/\1/p')"
  hits="$(printf '%s' "${stats}" | sed -n 's/Cache hits[[:space:]]*\([0-9]*\)$/\1/p')"
  requests="$(printf '%s' "${stats}" | sed -n 's/Compile requests[[:space:]]*\([0-9]*\)$/\1/p')"
  echo "sccache: ${misses} misses, ${hits} hits ${requests} compile requests"
  printf '::group::sccache stats\n'
  printf '%s\n' "${stats}"
  while IFS=: read -r error_kind count; do
    if [[ count -gt 0 ]] ; then
      gh_message "${error_kind}" "${count} errors"
    fi
  done <<< "${errors}"
  if [[ "${misses}" -gt 0 && -r "${SCCACHE_ERROR_LOG}" ]]; then
    printf '::group::sccache misses (N=%d)\n' "${misses}"
    sed -n  "s/.*sccache::compiler::compiler.* \[\(.*\)\]: *Cache miss.*$/\1/p" "${SCCACHE_ERROR_LOG}"
    printf '\n::endgroup::\n'
  fi
  printf '\n::endgroup::\n'
else
  echo 'skipping sccache stats: sccache not found'
fi

printf '::group::build directory stats\n'
kiB_to_MiB () {
  python3 -c "print(f'{${1} / 1024:3.2f}')"
}
(
  shopt -s dotglob globstar
  kB_build="$( du -sk "${builddir}" | sed 's/\([^[:space:]]*\).*/\1/')"
  kB_o_files="$( (du -ck "${builddir}"/**/*.o 2> /dev/null || echo 0) | tail -1 | sed 's/\([^[:space:]]*\).*/\1/')"
  kB_so_files="$( (du -ck "${builddir}"/**/*.so 2> /dev/null || echo 0) | tail -1 | sed 's/\([^[:space:]]*\).*/\1/')"
  kB_a_files="$( (du -ck "${builddir}"/**/*.a 2> /dev/null || echo 0) | tail -1 | sed 's/\([^[:space:]]*\).*/\1/')"
  echo "Build directory size: $( kiB_to_MiB "${kB_build}" ) MiB"
  echo "Build directory shared library (.so) files cumulative size: $( kiB_to_MiB "${kB_so_files}" ) MiB"
  echo "Build directory object (.o) files cumulative size: $( kiB_to_MiB "${kB_o_files}" ) MiB"
  echo "Build directory static library (.a) files cumulative size: $( kiB_to_MiB "${kB_a_files}" ) MiB"
)
printf '::endgroup::\n'
