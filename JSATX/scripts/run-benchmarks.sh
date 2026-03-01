#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
cd "$ROOT"

echo "Building native for local platform..."
make -C native PLATFORM=linux-x86_64
make -C native install PLATFORM=linux-x86_64

echo
echo "Building Java package..."
cd java/jsatx-java
mvn -DskipTests package

echo
echo "Running Java benchmark..."
java -cp target/classes com.jsatx.examples.Benchmark

echo
echo "Running Python benchmark..."
cd "$ROOT"  # ensure relative paths inside python script resolve
python3 "$ROOT/python_benchmark.py"
