#!/bin/bash

# Provide a unified interface for the different logging
# utilities CI providers offer. If unavailable, provide
# a compatible fallback (e.g. bare `echo xxxxxx`).

function startgroup {
    # Start a foldable group of log lines
    # Pass a single argument, quoted
    case ${CI:-} in
        azure )
            echo "##[group]$1";;
        travis )
            echo "$1"
            echo -en 'travis_fold:start:'"${1// /}"'\\r';;
        github_actions )
            echo "::group::$1";;
        * )
            echo "$1";;
    esac
} 2> /dev/null

function endgroup {
    # End a foldable group of log lines
    # Pass a single argument, quoted

    case ${CI:-} in
        azure )
            echo "##[endgroup]";;
        travis )
            echo -en 'travis_fold:end:'"${1// /}"'\\r';;
        github_actions )
            echo "::endgroup::";;
    esac
} 2> /dev/null
