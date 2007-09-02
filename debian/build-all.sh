#!/bin/sh

cd ..
svn-buildpackage \
    --svn-ignore-new \
    -uc \
    -us \
    -rfakeroot


