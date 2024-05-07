#!/usr/bin/env bash

# -*- mode: jinja-shell -*-

source .scripts/logging_utils.sh

set -xe

MINIFORGE_HOME=${MINIFORGE_HOME:-${HOME}/miniforge3}

( startgroup "Installing a fresh version of Miniforge" ) 2> /dev/null

MINIFORGE_URL="https://github.com/conda-forge/miniforge/releases/latest/download"
MINIFORGE_FILE="Mambaforge-MacOSX-$(uname -m).sh"
curl -L -O "${MINIFORGE_URL}/${MINIFORGE_FILE}"
rm -rf ${MINIFORGE_HOME}
bash $MINIFORGE_FILE -b -p ${MINIFORGE_HOME}

( endgroup "Installing a fresh version of Miniforge" ) 2> /dev/null

( startgroup "Configuring conda" ) 2> /dev/null

source ${MINIFORGE_HOME}/etc/profile.d/conda.sh
conda activate base
export CONDA_SOLVER="libmamba"
export CONDA_LIBMAMBA_SOLVER_NO_CHANNELS_FROM_INSTALLED=1

mamba install --update-specs --quiet --yes --channel conda-forge --strict-channel-priority \
    pip mamba conda-build conda-forge-ci-setup=4 "conda-build>=24.1"
mamba update --update-specs --yes --quiet --channel conda-forge --strict-channel-priority \
    pip mamba conda-build conda-forge-ci-setup=4 "conda-build>=24.1"



echo -e "\n\nSetting up the condarc and mangling the compiler."
setup_conda_rc ./ ./.conda/recipe ./.ci_support/${CONFIG}.yaml

if [[ "${CI:-}" != "" ]]; then
  mangle_compiler ./ ./.conda/recipe .ci_support/${CONFIG}.yaml
fi

if [[ "${CI:-}" != "" ]]; then
  echo -e "\n\nMangling homebrew in the CI to avoid conflicts."
  /usr/bin/sudo mangle_homebrew
  /usr/bin/sudo -k
else
  echo -e "\n\nNot mangling homebrew as we are not running in CI"
fi

if [[ "${sha:-}" == "" ]]; then
  sha=$(git rev-parse HEAD)
fi

echo -e "\n\nRunning the build setup script."
source run_conda_forge_build_setup



( endgroup "Configuring conda" ) 2> /dev/null

echo -e "\n\nMaking the build clobber file"
make_build_number ./ ./.conda/recipe ./.ci_support/${CONFIG}.yaml

if [[ -f LICENSE.txt ]]; then
  cp LICENSE.txt ".conda/recipe/recipe-scripts-license.txt"
fi

if [[ "${BUILD_WITH_CONDA_DEBUG:-0}" == 1 ]]; then
    if [[ "x${BUILD_OUTPUT_ID:-}" != "x" ]]; then
        EXTRA_CB_OPTIONS="${EXTRA_CB_OPTIONS:-} --output-id ${BUILD_OUTPUT_ID}"
    fi
    conda debug ./.conda/recipe -m ./.ci_support/${CONFIG}.yaml \
        ${EXTRA_CB_OPTIONS:-} \
        --clobber-file ./.ci_support/clobber_${CONFIG}.yaml

    # Drop into an interactive shell
    /bin/bash
else

    if [[ "${HOST_PLATFORM}" != "${BUILD_PLATFORM}" ]]; then
        EXTRA_CB_OPTIONS="${EXTRA_CB_OPTIONS:-} --no-test"
    fi

    conda-build ./.conda/recipe -m ./.ci_support/${CONFIG}.yaml \
        --suppress-variables ${EXTRA_CB_OPTIONS:-} \
        --clobber-file ./.ci_support/clobber_${CONFIG}.yaml \
        --extra-meta flow_run_id="$flow_run_id" remote_url="$remote_url" sha="$sha"

    ( startgroup "Uploading packages" ) 2> /dev/null

    if [[ "${UPLOAD_PACKAGES}" != "False" ]] && [[ "${IS_PR_BUILD}" == "False" ]]; then
      upload_package  ./ ./.conda/recipe ./.ci_support/${CONFIG}.yaml
    fi

    ( endgroup "Uploading packages" ) 2> /dev/null
fi