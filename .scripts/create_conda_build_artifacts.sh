#!/usr/bin/env bash

# INPUTS (environment variables that need to be set before calling this script):
#
# CI (azure/github_actions/UNSET)
# CI_RUN_ID (unique identifier for the CI job run)
# FEEDSTOCK_NAME
# CONFIG (build matrix configuration string)
# SHORT_CONFIG (uniquely-shortened configuration string)
# CONDA_BLD_DIR (path to the conda-bld directory)
# ARTIFACT_STAGING_DIR (use working directory if unset)
# BLD_ARTIFACT_PREFIX (prefix for the conda build artifact name, skip if unset)
# ENV_ARTIFACT_PREFIX (prefix for the conda build environments artifact name, skip if unset)

# OUTPUTS
#
# BLD_ARTIFACT_NAME
# BLD_ARTIFACT_PATH
# ENV_ARTIFACT_NAME
# ENV_ARTIFACT_PATH

source .scripts/logging_utils.sh

# DON'T do set -x, because it results in double echo-ing pipeline commands
# and that might end up inserting extraneous quotation marks in output variables
set -e

# Check that the conda-build directory exists
if [ ! -d "$CONDA_BLD_DIR" ]; then
    echo "conda-build directory does not exist"
    exit 1
fi

# Set staging dir to the working dir, in Windows style if applicable
if [[ -z "${ARTIFACT_STAGING_DIR}" ]]; then
    if pwd -W; then
        ARTIFACT_STAGING_DIR=$(pwd -W)
    else
        ARTIFACT_STAGING_DIR=$PWD
    fi
fi
echo "ARTIFACT_STAGING_DIR: $ARTIFACT_STAGING_DIR"

FEEDSTOCK_ROOT=$(cd "$(dirname "$0")/.."; pwd;)
if [ -z ${FEEDSTOCK_NAME} ]; then
    export FEEDSTOCK_NAME=$(basename ${FEEDSTOCK_ROOT})
fi

# Set a unique ID for the artifact(s), specialized for this particular job run
ARTIFACT_UNIQUE_ID="${CI_RUN_ID}_${CONFIG}"
if [[ ${#ARTIFACT_UNIQUE_ID} -gt 80 ]]; then
    ARTIFACT_UNIQUE_ID="${CI_RUN_ID}_${SHORT_CONFIG}"
fi
echo "ARTIFACT_UNIQUE_ID: $ARTIFACT_UNIQUE_ID"

# Set a descriptive ID for the archive(s), specialized for this particular job run
ARCHIVE_UNIQUE_ID="${CI_RUN_ID}_${CONFIG}"

# Make the build artifact zip
if [[ ! -z "$BLD_ARTIFACT_PREFIX" ]]; then
    export BLD_ARTIFACT_NAME="${BLD_ARTIFACT_PREFIX}_${ARTIFACT_UNIQUE_ID}"
    export BLD_ARTIFACT_PATH="${ARTIFACT_STAGING_DIR}/${FEEDSTOCK_NAME}_${BLD_ARTIFACT_PREFIX}_${ARCHIVE_UNIQUE_ID}.zip"

    ( startgroup "Archive conda build directory" ) 2> /dev/null

    # Try 7z and fall back to zip if it fails (for cross-platform use)
    if ! 7z a "$BLD_ARTIFACT_PATH" "$CONDA_BLD_DIR" '-xr!.git/' '-xr!_*_env*/' '-xr!*_cache/' -bb; then
        pushd "$CONDA_BLD_DIR"
        zip -r -y -T "$BLD_ARTIFACT_PATH" . -x '*.git/*' '*_*_env*/*' '*_cache/*'
        popd
    fi

    ( endgroup "Archive conda build directory" ) 2> /dev/null

    echo "BLD_ARTIFACT_NAME: $BLD_ARTIFACT_NAME"
    echo "BLD_ARTIFACT_PATH: $BLD_ARTIFACT_PATH"

    if [[ "$CI" == "azure" ]]; then
        echo "##vso[task.setVariable variable=BLD_ARTIFACT_NAME]$BLD_ARTIFACT_NAME"
        echo "##vso[task.setVariable variable=BLD_ARTIFACT_PATH]$BLD_ARTIFACT_PATH"
    elif [[ "$CI" == "github_actions" ]]; then
        echo "::set-output name=BLD_ARTIFACT_NAME::$BLD_ARTIFACT_NAME"
        echo "::set-output name=BLD_ARTIFACT_PATH::$BLD_ARTIFACT_PATH"
    fi
fi

# Make the environments artifact zip
if [[ ! -z "$ENV_ARTIFACT_PREFIX" ]]; then
    export ENV_ARTIFACT_NAME="${ENV_ARTIFACT_PREFIX}_${ARTIFACT_UNIQUE_ID}"
    export ENV_ARTIFACT_PATH="${ARTIFACT_STAGING_DIR}/${FEEDSTOCK_NAME}_${ENV_ARTIFACT_PREFIX}_${ARCHIVE_UNIQUE_ID}.zip"

    ( startgroup "Archive conda build environments" ) 2> /dev/null

    # Try 7z and fall back to zip if it fails (for cross-platform use)
    if ! 7z a "$ENV_ARTIFACT_PATH" -r "$CONDA_BLD_DIR"/'_*_env*/' -bb; then
        pushd "$CONDA_BLD_DIR"
        zip -r -y -T "$ENV_ARTIFACT_PATH" . -i '*_*_env*/*'
        popd
    fi

    ( endgroup "Archive conda build environments" ) 2> /dev/null

    echo "ENV_ARTIFACT_NAME: $ENV_ARTIFACT_NAME"
    echo "ENV_ARTIFACT_PATH: $ENV_ARTIFACT_PATH"

    if [[ "$CI" == "azure" ]]; then
        echo "##vso[task.setVariable variable=ENV_ARTIFACT_NAME]$ENV_ARTIFACT_NAME"
        echo "##vso[task.setVariable variable=ENV_ARTIFACT_PATH]$ENV_ARTIFACT_PATH"
    elif [[ "$CI" == "github_actions" ]]; then
        echo "::set-output name=ENV_ARTIFACT_NAME::$ENV_ARTIFACT_NAME"
        echo "::set-output name=ENV_ARTIFACT_PATH::$ENV_ARTIFACT_PATH"
    fi
fi