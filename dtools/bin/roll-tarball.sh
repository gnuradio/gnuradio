#!/usr/bin/zsh
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

setopt ERR_EXIT

project=gnuradio
version=$1
tempdir=$(mktemp -d)

#use gpg2 if available, gpg else
gpg=$(which gpg2 2> /dev/null || which gpg)
#use parallel pigz if available, else gzip
gz=$(which pigz 2> /dev/null || which gzip)

echo "Releasing version ${version}"

echo "Making temporary clean worktree…"
git worktree add "${tempdir}/${project}" "v$1"
pushd "${tempdir}/${project}"
gitroot=$(pwd)
gitversion=$(git describe)
signingkey=$(git config user.signingkey)
echo "…made ${gitroot}"

echo "Will use the following key for signing…"
gpg2 --list-keys "${signingkey}" || echo "Can't get info about key ${signingkey}.  Did you forget to do 'git config --local user.signingkey=0xDEADBEEF'?'"
echo "… end of key info."

echo "making sure submodules are up-to-date…"
git submodule update --init
echo "…sure."

archivedir=${gitroot}/archives
plaindir=${project}-${version}
plaindirpath=${tempdir}/${plaindir}

mkdir "${plaindirpath}"
mkdir -p "${archivedir}"

pwd
echo "$Checking out git revision ${gitversion} into folder ${plaindirpath}/…"
pushd "${plaindirpath}"
git --git-dir="${gitroot}/.git" checkout --recurse-submodules -f ${gitversion}
echo "…checked out."
popd # back from plaindirpath
popd # back from tempdir/project

pushd "${tempdir}"
echo "Gathering files…"
echo "…gathered. Auf nach Mordor!"
echo "creating tar…"
tar c --exclude-vcs  -f "${plaindir}.tar" "${plaindir}"
echo "…created."

echo  "compressing:"
echo  "gzip…"
${gz} --keep --best "${plaindir}.tar"
#--threads=0: guess number of CPU cores
echo "xz…"
xz --keep -9 --threads=0 "${plaindir}.tar"
echo "zstd…"
zstd --threads=0 -18 "${plaindir}.tar"
echo "…compressed."

popd #tempdir->original pwd

pushd ${archivedir}
echo "Copying over files from ${tempdir} to ${archivedir}"
cp ${tempdir}/${plaindir}.tar.* ./
echo "…copied."

echo "signing…"
for archive in ${plaindir}.* ; do
    echo "…${archive}…"
    ${gpg} --default-key=${signingkey} --personal-digest-preferences=SHA512 --armor --detach-sign "${archive}"
done
echo "…signed."

echo "calculating MD5 sums…"
md5sums=$(md5sum ${plaindir}.tar*)
echo "…calculated. Those are":
echo ${md5sums}

echo "downloading previous files' md5sums…"
md5sums_old=$(curl --cert-status https://www.gnuradio.org/releases/gnuradio/md5sums)
echo "…loaded down down down down down (whoohoohooo)."

echo "appending new md5sums…"
echo "${md5sums}${md5sums_old}" > md5sums
echo "…appended."
popd

echo "files can be found in ${archivedir}. Moving them over to cwd $(pwd)…"
mkdir -p "archives"
mv "${archivedir}/"* "./archives/"
echo "… I'm moved."

echo "Clean up temporary files…"
git worktree prune
rm -r ${tempdir}
echo "…all clean."
