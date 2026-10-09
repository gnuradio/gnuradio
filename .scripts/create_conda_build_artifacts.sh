#!/usr/bin/env bash

# INPUTS (environment variables that need to be set before calling this script):
#
# CI (azure/github_actions/UNSET)
# CI_RUN_ID (unique identifier for the CI job run)
# CONDA_BLD_PATH (path to the conda-bld directory)
# CONFIG (build matrix configuration string)
# CONFIG_SHORT (uniquely-shortened configuration string)
# FEEDSTOCK_NAME
# Optional:
# ARTIFACT_STAGING_DIR (use working directory if unset)
# BLD_ARTIFACT_PREFIX (prefix for the conda build artifact name, skip if unset)
# ENV_ARTIFACT_PREFIX (prefix for the conda build environments artifact name, skip if unset)
# WRK_ARTIFACT_PREFIX (prefix for the conda build work artifact name, skip if unset)

# OUTPUTS
#
# BLD_ARTIFACT_NAME
# BLD_ARTIFACT_PATH
# ENV_ARTIFACT_NAME
# ENV_ARTIFACT_PATH
# WRK_ARTIFACT_NAME
# WRK_ARTIFACT_PATH

source .scripts/logging_utils.sh

# DON'T do set -x, because it results in double echo-ing pipeline commands
# and that might end up inserting extraneous quotation marks in output variables
set -e

# mangle_homebrew hides zstd on GHA macos 15 runners, so use conda-forge tools
if [[ -d ${MINIFORGE_HOME} ]]; then
    . "${MINIFORGE_HOME}/etc/profile.d/conda.sh"
    conda activate
fi

# Check that the conda-build directory exists
if [ ! -d "$CONDA_BLD_PATH" ]; then
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
    ARTIFACT_UNIQUE_ID="${CI_RUN_ID}_${CONFIG_SHORT}"
fi
echo "ARTIFACT_UNIQUE_ID: $ARTIFACT_UNIQUE_ID"

# Set a descriptive ID for the archive(s), specialized for this particular job run
ARCHIVE_UNIQUE_ID="${CI_RUN_ID}_${CONFIG}"

pushd "${CONDA_BLD_PATH}"

# --use-compress-prog= lets us pass the options to GNU tar and libarchive tar
# -T0 uses all cores
# 12 gives reasonable compression while remaining fast
ZSTD="--use-compress-prog=zstd -T0 -12"

# Pattern matching can be quite overzealous, so rather than passing wildcards,
# we want to expand them into actual paths first.
#
# The split is:
# - artifacts: all packages and related repository files
# - environments: build, host and test environments
# - work directory: everything else (modulo excludes)

shopt -s nullglob
ARTIFACT_PATHS=(
    # all possible platform directories
    linux-*
    osx-*
    win-*

    # appending '*' here to have nullglob skip these directories and files
    # conveniently if they don't exist
    broken*
    noarch*
    channeldata.json*
    index.html*
)
ENVIRONMENT_PATHS=(
    # note: always include "./", so that only top-level paths match
    # conda-build
    ./*_*/*_env*
    ./*_*/*_prefix_moved_*

    # rattler-build
    ./bld/*_*/*_env*
    ./test/test_*/test_env
)

EXCLUDE_FROM_WORK=(
    .git

    # caches
    ./pkg_cache
    ./src_cache
    ./*_*/pip_cache

    # artifacts and environments that go into separate archives
    "${ARTIFACT_PATHS[@]}"
    "${ENVIRONMENT_PATHS[@]}"
)

# Make the build artifact archive
if [[ ! -z ${BLD_ARTIFACT_PREFIX} && -n ${ARTIFACT_PATHS} ]]; then
    echo "Creating build artifact archive ..."
    export BLD_ARTIFACT_NAME="${BLD_ARTIFACT_PREFIX}_${ARTIFACT_UNIQUE_ID}"
    export BLD_ARTIFACT_PATH="${ARTIFACT_STAGING_DIR}/${FEEDSTOCK_NAME}_${BLD_ARTIFACT_PREFIX}_${ARCHIVE_UNIQUE_ID}.tar.zst"

    # All our CI services have either GNU tar or bsdtar, and zstd.
    # Keep the command compatible with both!
    if ! tar -c -f "${BLD_ARTIFACT_PATH}" "${ZSTD}" "${ARTIFACT_PATHS[@]}" &&
        [[ -s ${BLD_ARTIFACT_PATH} ]]
    then
        # If tar failed but produced a (partial?) file, upload it as "broken".
        mv -v "${BLD_ARTIFACT_PATH}" "${BLD_ARTIFACT_PATH/%.tar.zst/-broken&}"
        BLD_ARTIFACT_NAME+=-broken
        BLD_ARTIFACT_PATH=${BLD_ARTIFACT_PATH/%.tar.zst/-broken&}
    fi

    if [[ -s ${BLD_ARTIFACT_PATH} ]]; then
        echo "Archive created:"
        ls -l -h "${BLD_ARTIFACT_PATH}"

        if [[ "$CI" == "azure" ]]; then
            echo "##vso[task.setVariable variable=BLD_ARTIFACT_NAME]$BLD_ARTIFACT_NAME"
            echo "##vso[task.setVariable variable=BLD_ARTIFACT_PATH]$BLD_ARTIFACT_PATH"
        elif [[ "$CI" == "github_actions" ]]; then
            echo "BLD_ARTIFACT_NAME=$BLD_ARTIFACT_NAME" >> $GITHUB_OUTPUT
            echo "BLD_ARTIFACT_PATH=$BLD_ARTIFACT_PATH" >> $GITHUB_OUTPUT
        fi
    else
        echo "No archive created"
    fi
else
    echo "Skipping build artifact archive"
fi
echo

# Make the workdir artifact archive
if [[ ! -z ${WRK_ARTIFACT_PREFIX} ]]; then
    echo "Creating work directory archive ..."
    export WRK_ARTIFACT_NAME="${WRK_ARTIFACT_PREFIX}_${ARTIFACT_UNIQUE_ID}"
    export WRK_ARTIFACT_PATH="${ARTIFACT_STAGING_DIR}/${FEEDSTOCK_NAME}_${WRK_ARTIFACT_PREFIX}_${ARCHIVE_UNIQUE_ID}.tar.zst"

    # All our CI services have either GNU tar or bsdtar, and zstd.
    # Keep the command compatible with both!
    if ! tar -c -f "${WRK_ARTIFACT_PATH}" "${ZSTD}" \
            "${EXCLUDE_FROM_WORK[@]/#/--exclude=}" . &&
        [[ -s ${WRK_ARTIFACT_PATH} ]]
    then
        # If tar failed but produced a (partial?) file, upload it as "broken".
        mv -v "${WRK_ARTIFACT_PATH}" "${WRK_ARTIFACT_PATH/%.tar.zst/-broken&}"
        WRK_ARTIFACT_NAME+=-broken
        WRK_ARTIFACT_PATH=${WRK_ARTIFACT_PATH/%.tar.zst/-broken&}
    fi

    if [[ -s ${WRK_ARTIFACT_PATH} ]]; then
        echo "Archive created:"
        ls -l -h "${WRK_ARTIFACT_PATH}"

        if [[ "$CI" == "azure" ]]; then
            echo "##vso[task.setVariable variable=WRK_ARTIFACT_NAME]$WRK_ARTIFACT_NAME"
            echo "##vso[task.setVariable variable=WRK_ARTIFACT_PATH]$WRK_ARTIFACT_PATH"
        elif [[ "$CI" == "github_actions" ]]; then
            echo "WRK_ARTIFACT_NAME=$WRK_ARTIFACT_NAME" >> $GITHUB_OUTPUT
            echo "WRK_ARTIFACT_PATH=$WRK_ARTIFACT_PATH" >> $GITHUB_OUTPUT
        fi
    else
        echo "No archive created"
    fi
else
    echo "Skipping work directory artifact archive"
fi
echo

# Make the environments artifact archive
if [[ ! -z ${ENV_ARTIFACT_PREFIX} && -n ${ENVIRONMENT_PATHS[@]} ]]; then
    echo "Creating build environment artifact archive ..."
    export ENV_ARTIFACT_NAME="${ENV_ARTIFACT_PREFIX}_${ARTIFACT_UNIQUE_ID}"
    export ENV_ARTIFACT_PATH="${ARTIFACT_STAGING_DIR}/${FEEDSTOCK_NAME}_${ENV_ARTIFACT_PREFIX}_${ARCHIVE_UNIQUE_ID}.tar.zst"

    if ! tar -c -f "${ENV_ARTIFACT_PATH}" "${ZSTD}" "${ENVIRONMENT_PATHS[@]}" &&
        [[ -s ${ENV_ARTIFACT_PATH} ]]
    then
        # If tar failed but produced a (partial?) file, upload it as "broken".
        mv -v "${ENV_ARTIFACT_PATH}" "${ENV_ARTIFACT_PATH/%.tar.zst/-broken&}"
        ENV_ARTIFACT_NAME+=-broken
        ENV_ARTIFACT_PATH=${ENV_ARTIFACT_PATH/%.tar.zst/-broken&}
    fi

    if [[ -s ${ENV_ARTIFACT_PATH} ]]; then
        echo "Archive created:"
        ls -l -h "${ENV_ARTIFACT_PATH}"

        if [[ "$CI" == "azure" ]]; then
            echo "##vso[task.setVariable variable=ENV_ARTIFACT_NAME]$ENV_ARTIFACT_NAME"
            echo "##vso[task.setVariable variable=ENV_ARTIFACT_PATH]$ENV_ARTIFACT_PATH"
        elif [[ "$CI" == "github_actions" ]]; then
            echo "ENV_ARTIFACT_NAME=$ENV_ARTIFACT_NAME" >> $GITHUB_OUTPUT
            echo "ENV_ARTIFACT_PATH=$ENV_ARTIFACT_PATH" >> $GITHUB_OUTPUT
        fi
    else
        echo "No archive created"
    fi
else
    echo "Skipping environment artifact archive"
fi
echo

popd
