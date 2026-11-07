#!/bin/bash
# Copyright 2026 Marcus Müller
runname="$1"
gh_message() {
  printf '::notice title="%s: %s"::%s\n' "${runname}" "$1" "$2"
}
bail_with_message() {
  gh_message "$1" "$2"
  exit 0
}
fail_with_message() {
  gh_message "$1" "$2"
  exit 2
}
add_output() {
  printf '%s=%s\n' "$1" "$2" >> "${GITHUB_OUTPUT}"
}
add_env() {
  printf '%s=%s\n' "$1" "$2" >> "${GITHUB_ENV}"
}
add_mask() {
  printf '::add-mask::%s\n' "$1"
}
