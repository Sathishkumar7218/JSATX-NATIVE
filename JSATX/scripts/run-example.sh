#!/usr/bin/env bash
set -euo pipefail
# Build and run the Example from the jsatx-java module
script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "$script_dir/../java/jsatx-java"
# Ensure native library is built and installed
make -C ../../native install || true
mvn exec:java
