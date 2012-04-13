#!/bin/bash

cd $1
files=`ls`
for file in $files
do
    sed 's/libvector/volk/g' < $file > tempfile
    sed 's/LIBVECTOR/VOLK/g' < tempfile > $file
done
for file in $files
do
    echo $file > tempfile
    newfile=`sed 's/libvector/volk/g' < tempfile`
    if (test "$file" != "$newfile"); then
	mv $file $newfile
	echo "changed $file to $newfile"
    fi
done
