#!/bin/sh

cd ..
svn-buildpackage \
    --svn-ignore-new \
    --svn-dont-clean \
    --svn-dont-purge \
    --svn-reuse \
    -nc \
    -rfakeroot


