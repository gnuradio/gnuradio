#!/usr/bin/env bash

# -*- mode: jinja-shell -*-

source .scripts/logging_utils.sh

set -xe

MINIFORGE_HOME="${MINIFORGE_HOME:-${HOME}/miniforge3}"
MINIFORGE_HOME="${MINIFORGE_HOME%/}" # remove trailing slash
export CONDA_BLD_PATH="${CONDA_BLD_PATH:-${MINIFORGE_HOME}/conda-bld}"

( startgroup "Provisioning base env with micromamba" ) 2> /dev/null
MICROMAMBA_VERSION="1.5.10-0"
if [[ "$(uname -m)" == "arm64" ]]; then
  osx_arch="osx-arm64"
else
  osx_arch="osx-64"
fi
MICROMAMBA_URL="https://github.com/mamba-org/micromamba-releases/releases/download/${MICROMAMBA_VERSION}/micromamba-${osx_arch}"
MAMBA_ROOT_PREFIX="${MINIFORGE_HOME}-micromamba-$(date +%s)"
echo "Downloading micromamba ${MICROMAMBA_VERSION}"
micromamba_exe="$(mktemp -d)/micromamba"
curl -L -o "${micromamba_exe}" "${MICROMAMBA_URL}"
chmod +x "${micromamba_exe}"
echo "Creating environment"
"${micromamba_exe}" create --yes --root-prefix "${MAMBA_ROOT_PREFIX}" --prefix "${MINIFORGE_HOME}" \
  --channel conda-forge \
  pip python=3.12 conda-build conda-forge-ci-setup=4 "conda-build>=24.1"
echo "Moving pkgs cache from ${MAMBA_ROOT_PREFIX} to ${MINIFORGE_HOME}"
mv "${MAMBA_ROOT_PREFIX}/pkgs" "${MINIFORGE_HOME}"
echo "Cleaning up micromamba"
rm -rf "${MAMBA_ROOT_PREFIX}" "${micromamba_exe}" || true
( endgroup "Provisioning base env with micromamba" ) 2> /dev/null

( startgroup "Configuring conda" ) 2> /dev/null
echo "Activating environment"
source "${MINIFORGE_HOME}/etc/profile.d/conda.sh"
conda activate base
export CONDA_SOLVER="libmamba"
export CONDA_LIBMAMBA_SOLVER_NO_CHANNELS_FROM_INSTALLED=1





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

    ( startgroup "Inspecting artifacts" ) 2> /dev/null

    # inspect_artifacts was only added in conda-forge-ci-setup 4.9.4
    command -v inspect_artifacts >/dev/null 2>&1 && inspect_artifacts --recipe-dir ./.conda/recipe -m ./.ci_support/${CONFIG}.yaml || echo "inspect_artifacts needs conda-forge-ci-setup >=4.9.4"

    ( endgroup "Inspecting artifacts" ) 2> /dev/null

    ( startgroup "Uploading packages" ) 2> /dev/null

    if [[ "${UPLOAD_PACKAGES}" != "False" ]] && [[ "${IS_PR_BUILD}" == "False" ]]; then
      upload_package  ./ ./.conda/recipe ./.ci_support/${CONFIG}.yaml
    fi

    ( endgroup "Uploading packages" ) 2> /dev/null
fi