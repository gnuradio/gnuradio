#!/bin/bash
runname="$1"
dir="$2"
archive="$3"

bail_with_message() {
  printf '::notice title="%s: %s"::%s\n' "${runname}" "$1" "$2"
  exit 0
}
# exit early on lack of mksquashfs
type mksquashfs > /dev/null || bail_with_message 'skipping coredump collection' 'mksquashfs not found'

# check whether coredumps found
[[ -d "${dir}" ]] || bail_with_message 'skipping coredump collection' "coredump directory '${dir}' not a directory"

# count files in coredump dir
shopt -s nullglob
coredumps=( "${dir}"/* "${dir}"/.* )
[[ ${#coredumps[@]} -eq 2 ]] && bail_with_message 'no coredumps' 'coredump directory empty'

mksquashfs "${dir}" "${archive}" -comp zstd
