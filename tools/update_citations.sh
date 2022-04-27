#!/usr/bin/env bash

# Find all contributors according to git and update `.zenodo.json` accordingly.

script_name=$0
script_full_path=$(dirname "$0")
python_script=$"$script_full_path/run_citations_update.py"

contributors_list="$(git log --pretty="%an <%ae>" | sort | uniq)"

# Run a Python script to make things easier.
python3 $python_script "$contributors_list"
