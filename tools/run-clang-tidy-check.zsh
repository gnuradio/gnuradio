#!/usr/bin/zsh
if [[ $# -lt 1 ]]; then
  printf 'Syntax:\n   %s CHECKS [output file|-] [build directory] [parallelism]\n' "${0}" >&2
  printf 'Where:\n   CHECKS          enabled clang-tidy checks\n'  >&2
  printf '   output file     Write clang-tidy output here. Defaults to $CHECKS.txt.\n'  >&2
  printf '   build directory Where compile_commands.json resides. Defaults to build.\n'  >&2
  printf '   parallelism     How many clang-tidy to run in parallel. Defaults 32.\n'  >&2
  exit 1
fi
checks="${1}"
if [[ -z "${checks}" ]] ; then
  printf 'Need to specify checks' >&2
  exit 12
fi
outfile="${2:-${1}.txt}"
if [[ "${outfile}" = "-" ]] ; then
  tee=cat
else
  tee="tee ${outfile}"
fi
builddir="${3:-build}"
parallelism=${4:-32}

jq -r '.[] | .file' < "${builddir}/compile_commands.json" \
  | grep -v '/python/.*/bindings' \
  | parallel -m -j "${parallelism}" -- \
    clang-tidy --quiet -p "${builddir}" --checks "-\*,${1}" \
  | ${=tee}
