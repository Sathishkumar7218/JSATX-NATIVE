#!/usr/bin/env bash
set -euo pipefail

JAR_PATH=${1:-java/jsatx-java/target/jsatx-java-1.0-SNAPSHOT.jar}

if [ ! -f "$JAR_PATH" ]; then
  echo "ERROR: JAR not found: $JAR_PATH"
  exit 2
fi

echo "Inspecting $JAR_PATH for platform-native directories..."

# List entries
tmpfile=$(mktemp)
jar tf "$JAR_PATH" > "$tmpfile"

check_dir() {
  dir=$1
  if grep -q "^$dir" "$tmpfile"; then
    echo "  OK: found $dir"
  else
    echo "  MISSING: $dir"
    missing=1
  fi
}

missing=0
check_dir "natives/linux-x86_64/"
check_dir "natives/macos-x86_64/"
check_dir "natives/windows-x86_64/"

rm -f "$tmpfile"

if [ "$missing" -ne 0 ]; then
  echo "ERROR: Fat JAR is missing one or more platform native directories"
  exit 1
fi

echo "Fat JAR verification passed."
exit 0
