#!/bin/bash
# Copyright 2024 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Bash on Mac OS is atrociously old, but this should work nevertheless
# fail on any unhandled error
set -e

# Check we can find gr_modtool; else, exit
command -v gr_modtool || exit 0

tmpdir="$(mktemp -d)"
cd "${tmpdir}"

function cleanup(){
  cd ..
  rm -rf -- "${tmpdir}"
  exit $1
}
# No interference from user config
export GR_PREFS_PATH="${tmpdir}/doesnotexist"

echo Check that we fail if make a new module in batch mode w/o name
gr_modtool newmod --batch && cleanup 1

echo Make new module
modn="testmod"
gr_modtool newmod --batch ${modn}

# Change into directory
cd gr-${modn} 

echo Check we fail on incomplete add commands
blockn="tunnelpy"
gr_modtool add --batch && cleanup 2
gr_modtool add --batch ${blockn} && cleanup 3
gr_modtool add --batch --lang python ${blockn} && cleanup 4
gr_modtool add --batch --block-type sink ${blockn} && cleanup 5

echo Make new block
gr_modtool add --batch --lang python --block-type sink --copyright "GNU Radio QA" ${blockn}

echo Rename block
newname="domythesis"
gr_modtool rename --batch && cleanup 6
gr_modtool rename --batch ${blockn} ${newname}
echo ... and back
gr_modtool rename --batch ${newname} ${blockn}

# Can't test XML update, no XML


# Finally, everything having happened:
cleanup 0
