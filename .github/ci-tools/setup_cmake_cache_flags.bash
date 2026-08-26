#!/usr/bin/env bash
# Copyright 2026 Marcus Müller
source "$(dirname "$(realpath "$0")")/common.bash"

type -p sccache > /dev/null || bail_with_message 'sccache' 'skipping sccache setup: sccache not found'
# [[ -z "${SCCACHE_S3_BUCKET}" ]] && bail_with_message 'skipping sccache setup' 'S3 bucket not defined (empty/missing SCCACHE_S3_BUCKET env var)'

use_default=false
printf '::group::S3 setup\n'
{
  if [[ -z "${SCCACHE_S3_BUCKET}" ]] ; then
    echo "using default S3 bucket"
    use_default=true
    export SCCACHE_S3_BUCKET="sccache"
    add_env SCCACHE_S3_BUCKET "${SCCACHE_S3_BUCKET}"
  fi
  if [[ -z "${SCCACHE_S3_ENDPOINT}" ]] ; then
    echo "using default S3 endpoint"
    use_default=true
    export SCCACHE_S3_ENDPOINT="s3.us-west-002.backblazeb2.com"
    add_env SCCACHE_S3_ENDPOINT "${SCCACHE_S3_ENDPOINT}"
  fi
  if [[ -z "${SCCACHE_S3_REGION}" ]] ; then
    echo "using default S3 region"
    use_default=true
    export SCCACHE_S3_REGION="auto"
    add_env SCCACHE_S3_REGION "${SCCACHE_S3_REGION}"
  fi
  if [[ -z "${AWS_ACCESS_KEY_ID}" ]] ; then
    # set up default bucket here
    echo "AWS key ID not set. Using default read-only key ID & key"
    use_default=true
    export AWS_ACCESS_KEY_ID="0021090e73dcc120000000014"
    add_env AWS_ACCESS_KEY_ID "${AWS_ACCESS_KEY_ID}"
    # Can't have a key without a key id.
    export AWS_SECRET_ACCESS_KEY="K002mqu3/bPthZfamA/ZGzVg1FxgR38"
  fi
  add_env AWS_SECRET_ACCESS_KEY "${AWS_SECRET_ACCESS_KEY}"
  # lets mask a few things in output. I bet people are scraping github action
  # output for leaked credentials, and while there's nothing secret on that, I
  # don't want to pay for some stupid scraper downloading the whole cache.
  add_mask "${AWS_SECRET_ACCESS_KEY}"
  add_mask "${AWS_ACCESS_KEY_ID}"
}
printf '::endgroup::'
if [[ "${use_default}" = "true" ]] ; then
  gh_message "sccache bucket config" "using default cache bucket, read-only!"
fi;

SCCACHE_BIN="$(type -p sccache)"
SCCACHE_CONF_DIR="${HOME}/.config/sccache"
SCCACHE_CONF="${SCCACHE_CONF_DIR}/config"
add_output C_LAUNCHER   "-DCMAKE_C_COMPILER_LAUNCHER=${SCCACHE_BIN}"
add_output CXX_LAUNCHER "-DCMAKE_CXX_COMPILER_LAUNCHER=${SCCACHE_BIN}"

if [[ ! -e "${SCCACHE_CONF}" ]]; then
  echo "Creating sccache config in ${SCCACHE_CONF_DIR}"
  mkdir -p "${SCCACHE_CONF_DIR}"
>> "${SCCACHE_CONF}" cat << EOF
[cache.s3]
endpoint = "${SCCACHE_S3_ENDPOINT}"
bucket = "${SCCACHE_S3_BUCKET}"
use_ssl = true
server_side_encryption = false
no_credentials = false
region = "${SCCACHE_S3_REGION}"
EOF
fi

add_output SCCACHE_CONF "${SCCACHE_CONF}"
export SCCACHE_ERROR_LOG="/tmp/local_sccache.log"
add_env SCCACHE_ERROR_LOG "${SCCACHE_ERROR_LOG}"
### Debug output (disabled if all of the following 4 lines commented)
# These could be set here, but they do spam the compile output slightly.
# add_env SCCACHE_LOG debug
# Instead, we just set the environment locally with debug logging on
# SCCACHE_LOG=debug sccache --start-server

echo "Using ${SCCACHE_BIN} ($(${SCCACHE_BIN} --version)) as sccache binary."
