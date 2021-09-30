#!/usr/bin/env bash

source .scripts/logging_utils.sh

set -xe

MINIFORGE_HOME=${MINIFORGE_HOME:-${HOME}/miniforge3}

( startgroup "Installing a fresh version of Miniforge" ) 2> /dev/null

MINIFORGE_URL="https://github.com/conda-forge/miniforge/releases/latest/download"
MINIFORGE_FILE="Miniforge3-MacOSX-$(uname -m).sh"
curl -L -O "${MINIFORGE_URL}/${MINIFORGE_FILE}"
rm -rf ${MINIFORGE_HOME}
bash $MINIFORGE_FILE -b -p ${MINIFORGE_HOME}

( endgroup "Installing a fresh version of Miniforge" ) 2> /dev/null

( startgroup "Configuring conda" ) 2> /dev/null

GET_BOA=boa
BUILD_CMD=mambabuild

source ${MINIFORGE_HOME}/etc/profile.d/conda.sh
conda activate base

echo -e "\n\nInstalling conda-forge-ci-setup=3 and conda-build."
conda install -n base --quiet --yes "conda-forge-ci-setup=3" conda-build pip ${GET_BOA:-}



echo -e "\n\nSetting up the condarc and mangling the compiler."
setup_conda_rc ./ ./.packaging/conda_recipe ./.ci_support/${CONFIG}.yaml

if [[ "${CI:-}" != "" ]]; then
  mangle_compiler ./ ./.packaging/conda_recipe .ci_support/${CONFIG}.yaml
fi

if [[ "${CI:-}" != "" ]]; then
  echo -e "\n\nMangling homebrew in the CI to avoid conflicts."
  /usr/bin/sudo mangle_homebrew
  /usr/bin/sudo -k
else
  echo -e "\n\nNot mangling homebrew as we are not running in CI"
fi

echo -e "\n\nRunning the build setup script."
source run_conda_forge_build_setup



( endgroup "Configuring conda" ) 2> /dev/null


echo -e "\n\nMaking the build clobber file"
make_build_number ./ ./.packaging/conda_recipe ./.ci_support/${CONFIG}.yaml

conda $BUILD_CMD ./.packaging/conda_recipe -m ./.ci_support/${CONFIG}.yaml --suppress-variables --clobber-file ./.ci_support/clobber_${CONFIG}.yaml ${EXTRA_CB_OPTIONS:-}

( startgroup "Uploading packages" ) 2> /dev/null

if [[ "${UPLOAD_PACKAGES}" != "False" ]] && [[ "${IS_PR_BUILD}" == "False" ]]; then
  upload_package  ./ ./.packaging/conda_recipe ./.ci_support/${CONFIG}.yaml
fi

( endgroup "Uploading packages" ) 2> /dev/null