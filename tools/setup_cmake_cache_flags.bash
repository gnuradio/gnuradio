#!/bin/bash
runname="$1"

bail_with_message() {
  printf '::notice title="%s: %s"::%s\n' "${runname}" "$1" "$2"
  exit 0
}

type -p sccache > /dev/null || bail_with_message 'skipping sccache setup' 'sccache not found'
[[ -z "${SCCACHE_S3_BUCKET}" ]] && bail_with_message 'skipping sccache setup' 'S3 bucket not defined (empty/missing SCCACHE_S3_BUCKET env var)'

SCCACHE_BIN="$(type -p sccache)"

add_output() {
  printf '%s=%s\n' "$1" "$2" >> "${GITHUB_OUTPUT}"
}

add_output C_LAUNCHER   "-DCMAKE_C_COMPILER_LAUNCHER=${SCCACHE_BIN}"
add_output CXX_LAUNCHER "-DCMAKE_CXX_COMPILER_LAUNCHER=${SCCACHE_BIN}"

mkdir -p ~/.config/sccache

>> ~/.config/sccache/config cat << EOF
[cache.s3]
endpoint = "${SCCACHE_S3_ENDPOINT}"
bucket = "${SCCACHE_S3_BUCKET}"
use_ssl = true
server_side_encryption = false
no_credentials = false
region = "${SCCACHE_S3_REGION}"
EOF

add_output SCCACHE_CONF "$(realpath ~/.config/sccache/config)"

bail_with_message 'enabling sccache' "Using ${SCCACHE_BIN} ($(${SCCACHE_BIN} --version)) as sccache binary."

