#!/bin/bash
runname="$1"
dir="$2"
archive="$3"

bail_with_message() {
  printf '::notice title="%s: %s"::%s\n' "${runname}" "$1" "$2"
  exit 0
}
error_out_with_message() {
  printf '::notice title="%s: %s"::%s\n' "${runname}" "$1" "$2"
  exit 13
}
# exit early on lack of mksquashfs
type mksquashfs > /dev/null || bail_with_message 'skipping installation collection' 'mksquashfs not found'

# check whether installations found
[[ -d "${dir}" ]] || error_out_with_message 'failing installation collection' "installation directory '${dir}' not a directory"

# count files in installation dir
shopt -s nullglob
installations=( "${dir}"/* "${dir}"/.* )
installed_files=$(( $(echo ${#installations[@]}) - 2))
[[ ${installed_files} -gt 0 ]] || error_out_with_message 'failing installation collection' "expected at least 1 file, got ${installed_files}"

mksquashfs "${dir}" "${archive}" -comp zstd
