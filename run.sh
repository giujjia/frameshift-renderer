#!/bin/bash
set -e

# Vai para a pasta onde o script está localizado
cd "$(dirname "$0")"

echo "============================================================"
echo "Building native C++ library"
echo "============================================================"

mkdir -p native/build
cd native/build

cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j"$(nproc)"

cd ../..

echo "============================================================"
echo "Compiling Java sources"
echo "============================================================"

cd java
mkdir -p build

# Compila todos os .java do projeto, incluindo model, service, jni e main
find src -name "*.java" > sources.txt
javac -encoding UTF-8 -d build @sources.txt

echo "============================================================"
echo "Running ShiftGL - Frameshift Mutation Visualizer"
echo "============================================================"

java \
  -Djava.library.path=../native/build \
  -cp build \
  main.MainWindow