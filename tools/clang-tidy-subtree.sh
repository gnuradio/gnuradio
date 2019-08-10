#!/usr/bin/zsh

files=$(git ls-files "$@")
cpp_files=$(echo ${files}|grep -E '\.(cpp|hpp|c|cc|h)$')

clang-tidy -fix-errors -header-filter=.\* -p build/
