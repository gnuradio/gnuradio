#!/usr/bin/env bash
# Copyright 2026 Marcus Müller
# This script is mostly for documentation.
#
# Prerequisites:
# - a Backblaze.com account, of which you hold a master or application key empowered to create buckets
# - uv (the python manager), to run the b2 utility (I don't plan on installing that on every machine)
# - gh (the github CLI client) https://github.com/cli/cli
#
#############
# CONSTANTS #
# the B2 program; you might want to want to invoke something else
B2="${B2:=uvx b2}"
# The gh program
GH="${GH:=gh}"
# The profile under which you're using the master key
B2_PROFILE="GNU Radio Master"
# The name of the bucket you want to create
BUCKET_NAME="gnuradio3-sccache"
# Some info about the bucket. Currently not used by anything.
BUCKET_INFO="{\"description\": \"Cache for Docker CI action\", \"creating_user\": \"$(whoami)\"}"
# Filename prefix
B2_FNAME_PREFIX=""
### Life times:
# Cancel unfinished uploads 2 days after start
B2_CANCEL_UPLOAD=2
# Fully delete hidden (i.e., overwritten/manually hidden) files after days:
B2_HIDDEN_TO_DELETED=1
# automatically delete uploaded files after days:
B2_UPLOAD_TO_DELETED=30
# Suffixes etc
RO_SUFFIX="ro"
RW_SUFFIX="rw"
#############
# AUTHORIZATION
# if you haven't yet, you'll have to authenticate with your master/app key
# ${B2} account authorize --profile "${B2_PROFILE}" "${KEYID}" "${KEY}"
######

(
  $B2 bucket create \
  --profile "${B2_PROFILE}" \
  --bucket-info "${BUCKET_INFO}" \
  --lifecycle-rule \
  "{\"daysFromHidingToDeleting\": ${B2_HIDDEN_TO_DELETED}, \"daysFromStartingToCancelingUnfinishedLargeFiles\": ${B2_CANCEL_UPLOAD}, \"daysFromUploadingToHiding\": ${B2_UPLOAD_TO_DELETED}, \"fileNamePrefix\": \"${B2_FNAME_PREFIX}\" }" \
  "${BUCKET_NAME}" \
  allPrivate \
  | tee -a "${BUCKET_NAME}.id" \
  && echo "created bucket ${BUCKET_NAME}"
) && (
  $B2 key create \
  --profile "${B2_PROFILE}" \
  --bucket "${BUCKET_NAME}" \
  "${BUCKET_NAME}-${RO_SUFFIX}" \
  "readBuckets,listFiles,readFiles,shareFiles,readBucketEncryption,readBucketLogging,readBucketReplications,readBucketNotifications,readBucketLifecycleRules" \
  | tee -a "${BUCKET_NAME}-${RO_SUFFIX}.key" \
  && echo "created RO key for ${BUCKET_NAME}"
) && (
  $B2 key create \
  --profile "${B2_PROFILE}" \
  --bucket "${BUCKET_NAME}" \
  "${BUCKET_NAME}-${RW_SUFFIX}" \
  "listAllBucketNames,listBuckets,readBuckets,listFiles,readFiles,shareFiles,writeFiles,deleteFiles,readBucketEncryption,readBucketLogging,readBucketReplications,readBucketNotifications,writeBucketNotifications" \
  | tee -a "${BUCKET_NAME}-${RW_SUFFIX}.key" \
  && echo "created RW key for ${BUCKET_NAME}"
) && (
  $GH variable set \
  "SCCACHE_S3_KEY_ID" \
  --body "$(cut -f 1 -d " " "${BUCKET_NAME}-${RW_SUFFIX}.key")" \
  && echo "set variable: key ID to $(cut -f 1 -d " " "${BUCKET_NAME}-${RW_SUFFIX}.key")"
) && (
  $GH secret set \
  "SCCACHE_S3_KEY" \
  -a actions \
  --body "$(cut -f 2 -d " " "${BUCKET_NAME}-${RW_SUFFIX}.key")" \
  && echo "set secret: key"
)

echo "Don't forget to update setup_cmake_cache_flags.bash with the key from ${BUCKET_NAME}-${RO_SUFFIX}.key and the correct endpoint from above"
