#!/bin/bash
# Copyright 2024 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Bash on Mac OS is atrociously old, but this should work nevertheless
# fail on any unhandled error
set -e

PY="$1"
SRCDIR="$2"
MODNAME="$3"
BLOCKNAME="$4"
NEWNAME="$5"
WORKDIR="$6"

MODTOOL="$1 $2/gr_modtool"

echo "using '$1'\n$MODTOOL" >&2

cd "${WORKDIR}"

echo "Check that we fail if make a new module in batch mode w/o name"
$MODTOOL newmod --batch && exit 1

echo "Make new module"
$MODTOOL newmod --batch ${MODNAME}

# Change into directory
cd gr-${MODNAME} 

echo "Check we fail on incomplete add commands"
$MODTOOL add --batch && exit 2
$MODTOOL add --batch ${BLOCKNAME} && exit 3
$MODTOOL add --batch --lang python ${BLOCKNAME} && exit 4
$MODTOOL add --batch --block-type sink ${BLOCKNAME} && exit 5

echo "Make new block"
$MODTOOL add --batch --lang python --block-type sink --copyright "GNU Radio QA" ${BLOCKNAME}

echo "Rename block"
$MODTOOL rename --batch && exit 6
$MODTOOL rename --batch ${BLOCKNAME} ${NEWNAME}
echo "... and back"
$MODTOOL rename --batch ${NEWNAME} ${BLOCKNAME}

# Can't test XML update, no XML

