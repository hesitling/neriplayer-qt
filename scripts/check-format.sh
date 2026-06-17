#!/usr/bin/env bash
# Check C++ formatting with clang-format. Exits non-zero on violation.
set -euo pipefail

base_ref="${1:-HEAD~1}"

files=$(git diff --name-only --diff-filter=ACMR "$base_ref" -- '*.h' '*.cpp' || true)
if [ -z "$files" ]; then
  echo "No changed C++ files."
  exit 0
fi

echo "Checking formatting:"
echo "$files" | sed 's/^/  /'

diff=$(echo "$files" | xargs clang-format --dry-run --Werror --style=file 2>&1 || true)
if [ -n "$diff" ]; then
  echo ""
  echo "::error::Format violations found. Run 'just format' locally."
  echo "$diff"
  exit 1
fi

echo "All files formatted correctly."
