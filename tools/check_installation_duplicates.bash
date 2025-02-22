#!/bin/bash
# Checks whether any files got installed twice
# Argument: path of install_manifest.txt from a cmake build
infile="$1" 
sort "${infile}" > /tmp/sorted \
  && sort -u "${infile}" > /tmp/sorted_unique \
  && differences="$(comm -3 /tmp/sorted /tmp/sorted_unique)" \
  || exit 12

if [[ -n "${differences}" ]]; then
  printf '%s\n' "${differences}" | while read -r line ; do
    printf '::error file=%s"::duplicately installed file "%s"\n' "${line}" "${line}"
  done
exit 42
fi
