#!/bin/sh

cd ..
svn-buildpackage \
    --svn-ignore-new \
    -uc \
    -us \
    -rfakeroot

cd ../build-area
dpkg-scanpackages . /dev/null | gzip -9c > Packages.gz

