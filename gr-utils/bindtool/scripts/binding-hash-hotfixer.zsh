#!/usr/bin/zsh
verbose=false
if [[ "$1" == "-v" || "$1" == "--verbose" ]]; then
    verbose=true
    shift
fi

if [[ "$#" -lt 1 ]]; then
    echo "too few arguments: $0 [-v|--verbose] FILE_1 [… FILE_N]" >&2
    exit -127
fi

echo "WARNING: This script updates header hashes without checking whether the \
underlying functionality changed. If you added or removed any python-bound \
function in the header to this binding file THIS IS NOT THE TOOL YOU SHOULD BE \
USING. Use bindtool instead to generate correct binding code." >&2

errcount=0

patchfile() {
    header_fname=$(grep -o -e 'BINDTOOL_HEADER_FILE(\(.*\))' "${1}" | sed 's/BINDTOOL_HEADER_FILE(\(.*\))/\1/')
    if [[ -z "$header_fname" ]]; then
        echo "WARNING: skipping '$1': no BINDTOOL_HEADER_FILE defined" >&2
        return -1
    fi

    fullp=$(realpath "$1")
    folder=$(dirname "${fullp}")
    if [[ "${folder}" =~ "gnuradio-runtime" ]]; then
        includedir="${folder}/../../../../include/gnuradio/"
    else
        mod=$(basename $(dirname "${folder}"))
        includedir="${folder}/../../../include/gnuradio/${mod}/"
    fi
    headerp=$(realpath "${includedir}${header_fname}")
    $verbose && echo "Include directory: ${includedir}"
    $verbose && echo "Full File Path: ${headerp}"
    if [[ -r "${headerp}" ]]; then
        hhash=$(md5sum "${headerp}" | cut '-d ' -f1)
        $verbose && echo "Calculated Hash: ${hhash}"
        sed -i "s/BINDTOOL_HEADER_FILE_HASH(.*)/BINDTOOL_HEADER_FILE_HASH(${hhash})/" "$1" && $verbose && echo "success"
    else
        echo "ERROR: '$1': Header file '${headerp}' not found" >&2
        errcount=$(($errcount + 1))
    fi
}

for binding in $@; do
    patchfile $binding
done

exit $((0 - $errcount))
