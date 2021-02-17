#!/usr/bin/zsh
zmodload zsh/zutil
defaultsrc="../"
defaultrun="/usr/share/clang/run-clang-tidy.py"
defaulthash="gr-utils/bindtool/scripts/binding-hash-hotfixer.zsh"
zparseopts -D -E -F - -runclangtidy:=run h=help -src:=src -hash-updater:=hasher -branch-name:=branch
parsingsuccess=$?
if [[ ! -z "${help}" ]]; then
    echo "USAGE:"
    echo "$0 [-h] [--runclangtidy /path/to/run-clang-tidy.py] [PASS-ON]"
    echo "-h:             print help"
    echo "--runclangtidy: specifies path to run-clang-tidy.py"
    echo "--src:          Path to GNU Radio source root (default: ${defaultsrc})"
    echo "--hash-updater: Path to the hash hotfixer (default: --src/${defaulthash})"
    echo "--branch-name:  Name of branch to work on; specify string 'NONE' to disable (default: clang-tidy-{random uuid})"
    echo "PASS-ON:        Optional, remainder of options, which is passed on to run-clang-tidy.py (default: ${defaultrun})"
    echo "\nThis tool must be run from a build directory of a CMake run with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    echo "\n\nThis runs clang-tidy on the whole codebase, applying all the checks specified in the .clang-tidy file."
    echo "Afterwards, the changed files are clang-formatted, and all PyBind binding headers are updated"
    exit 0
fi

if [[ -z "$run[-1]" ]]; then
    run="${defaultrun}"
else
    run="${run[-1]}"
fi
if [[ -z "$src[-1]" ]]; then
    src="$defaultsrc"
else
    src="${src[-1]}"
fi
if [[ ! -x "$run" ]]; then
    echo "'$run' needs to be executable"
    exit -1
fi
if [[ (! ( -d "${src}" && -x "${src}")) || (! -r "${src}/.clang-tidy") ]]; then
    echo "'${src}' needs to be a searchable directory containing the .clang-tidy file" >&2
    exit -2
fi

branching=1
if [[ -z "${branch[-1]}" ]]; then
    branch="clang-tidy-$(uuidgen)"
elif [[ "${branch}" == "NONE" ]]; then
    branching=0
else
    branch="${branch[-1]}"
fi

gitcmd=(git -C "${src}" --no-pager)

changes=$($gitcmd diff-index --name-only HEAD)
if [[ ! $? == 0 ]]; then
    echo "git diff-index failed with error code; giving up" >&2
    exit -3
fi
if [[ ! -z "${changes}" ]]; then
    echo "unclean work tree; commit or stash before proceeding" >&2
    exit -4
fi

if [[ $branching != 0 ]]; then
    $gitcmd checkout -b ${branch}
    if [[ ! $? == 0 ]]; then
        echo "git checkout -b ${branch} failed with error code; giving up" >&2
        exit -5
    fi
fi

if [[ -z "$hasher[-1]" ]]; then
    hasher="${src}/$defaulthash"
else
    hasher=$hasher[-1]
fi

#setopt -e
echo "Running clang-tidy..."
$run -checks=file '-header-filter=*' -fix "${src}" || echo "WARNING: run-clang-tidy had non-zero return code" >&2
echo "clang-tidy done."

echo "Committing... (signoff implied, this operation contributes no code)"
$gitcmd commit -sam "clang-tidy: run-clang-tidy with default checks on codebase" || echo "WARNING: git commit had non-zero return code" >&2
echo "Committing done"

changed_files=$($gitcmd diff-index --name-only -z 'HEAD~1')
for change in ${changed_files} ; do
    clang-format "${change}"
done

formatted_files=$($gitcmd diff name-only -z 'HEAD')

if [[ ${#formatted_files} -ne 0 ]]; then
    git commit -sam "Clang-formatted files changed during clang-tidy" || echo "WARNING: git commit clang-formatting had non-zero return code" >&2
fi
