#!/bin/sh

cd ..
svn-buildpackage \
    --svn-ignore-new \
    -S \
    -rfakeroot


