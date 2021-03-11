#!/bin/bash

DATESTR=$(date +"%a, %d %b %Y %T %z")
GITBRANCH_CLEAN=$(echo "$GITBRANCH" | sed "s/-//")
# Clone gnuradio repo
cd /build
cp -r $GITHUB_WORKSPACE /build/
cd /build/gnuradio
GITREV="$(git rev-list --count HEAD)"

# Scrape the version number from CMakeLists.txt
VERSION_MAJOR="$(cat CMakeLists.txt | grep "SET(VERSION_MAJOR" | tr -s ' ' | cut -d' ' -f2 | cut -d')' -f1)"
VERSION_API="$(cat CMakeLists.txt | grep "SET(VERSION_API" | tr -s ' ' | cut -d' ' -f2 | cut -d')' -f1)"
VERSION_ABI="$(cat CMakeLists.txt | grep "SET(VERSION_ABI" | tr -s ' ' | cut -d' ' -f2 | cut -d')' -f1)"
VERSION_PATCH="$(cat CMakeLists.txt | grep "SET(VERSION_PATCH" | tr -s ' ' | cut -d' ' -f2 | cut -d')' -f1)"

# Remove '-' from the patch
#VERSION_PATCH="${VERSION_PATCH/-/}"
VERSION_PATCH=$(echo "$VERSION_PATCH" | sed "s/-//")
VERSION_STRING=$VERSION_MAJOR"."$VERSION_API"."$VERSION_ABI"."$VERSION_PATCH
echo "Creating build for GNU Radio "$VERSION_STRING

GIT_COMMIT="$(git log --pretty=oneline | head -n 1)"
echo $GIT_COMMIT

cp -r .packaging/debian ../

# Tar.gz it
rm -rf .git
rm -rf .packaging

cd ..
cp -r debian gnuradio/
tar -cf gnuradio_$VERSION_STRING~$GITBRANCH_CLEAN~$GITREV~$DISTRIBUTION.orig.tar gnuradio
gzip gnuradio_$VERSION_STRING~$GITBRANCH_CLEAN~$GITREV~$DISTRIBUTION.orig.tar

# Update changelog 
# gnuradio (3.9.0.0~368-6~bionic) bionic; urgency=medium
cd gnuradio/debian
# Update the changelog
# Increment the Debian Revision
cp changelog changelog.prev
echo "gnuradio ($VERSION_STRING~$GITBRANCH_CLEAN~$GITREV~$DISTRIBUTION-$REV) $DISTRIBUTION; urgency=medium\n\n  * $GITBRANCH at $GIT_COMMIT\n\n -- $NAME $EMAIL  $DATESTR\n\n$(cat changelog)" > changelog

debuild -S -d
