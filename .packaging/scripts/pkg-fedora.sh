#!/bin/bash

cp $GITHUB_WORKSPACE/.packaging/fedora/gnuradio.spec ./
cp -r $GITHUB_WORKSPACE /build/

mkdir -p rpmbuild
mkdir -p rpmbuild/BUILD
mkdir -p rpmbuild/BUILDROOT
mkdir -p rpmbuild/RPMS
mkdir -p rpmbuild/SOURCES
mkdir -p rpmbuild/SRPMS

DATESTR=$(date +"%a, %d %b %Y %T %z")
GITBRANCH_CLEAN=${GITBRANCH/-/}

cd gnuradio
GITREV="$(git rev-list --count HEAD)"

# Scrape the version number from CMakeLists.txt
VERSION_MAJOR="$(cat CMakeLists.txt | grep "SET(VERSION_MAJOR" | tr -s ' ' | cut -d' ' -f2 | cut -d')' -f1)"
VERSION_API="$(cat CMakeLists.txt | grep "SET(VERSION_API" | tr -s ' ' | cut -d' ' -f2 | cut -d')' -f1)"
VERSION_ABI="$(cat CMakeLists.txt | grep "SET(VERSION_ABI" | tr -s ' ' | cut -d' ' -f2 | cut -d')' -f1)"
VERSION_PATCH="$(cat CMakeLists.txt | grep "SET(VERSION_PATCH" | tr -s ' ' | cut -d' ' -f2 | cut -d')' -f1)"

# Remove '-' from the patch
VERSION_PATCH=${VERSION_PATCH/-/}

VERSION_STRING=$VERSION_MAJOR"."$VERSION_API"."$VERSION_ABI"."$VERSION_PATCH
echo "Creating build for GNU Radio "$VERSION_STRING

GIT_COMMIT="$(git log --pretty=oneline | head -n 1)"
echo $GIT_COMMIT

# Tar.gz it
cd ..
cp -r gnuradio "gnuradio-$VERSION_STRING"
cd "gnuradio-$VERSION_STRING"
rm -rf .git
cd ..
tar cfJ rpmbuild/SOURCES/gnuradio_$VERSION_STRING~$GITBRANCH_CLEAN~$GITREV~$DISTRIBUTION.tar.xz gnuradio-$VERSION_STRING

ls rpmbuild/SOURCES
ls 

sed -i 's/\%{VERSION}/'$VERSION_STRING'/g' gnuradio.spec
sed -i 's/\%{RELEASE}/'$REV'/g' gnuradio.spec

SOURCE="gnuradio_$VERSION_STRING~$GITBRANCH~$GITREV~fedora.tar.xz"
sed -i 's/\%{SOURCE}/'$SOURCE'/g' gnuradio.spec

# build the source packages
rpmbuild \
	  --define "_topdir %(pwd)" \
	  --define "_builddir %{_topdir}/rpmbuild/BUILD" \
	  --define "_buildrootdir %{_topdir}/rpmbuild/BUILDROOT" \
	  --define "_rpmdir %{_topdir}/rpmbuild/RPMS" \
	  --define "_srcrpmdir %{_topdir}/rpmbuild/SRPMS" \
	  --define "_specdir %{_topdir}" \
	  --define "_sourcedir %{_topdir}/rpmbuild/SOURCES" \
	  -bs gnuradio.spec
