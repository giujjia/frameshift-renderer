#!/bin/bash

# compila a biblioteca nativa c++
cd native
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ../..

# compila a interface java
cd java
mkdir -p build
javac -d build src/jni/OpenGLBridge.java src/model/*.java src/ui/MainWindow.java src/main/Main.java

# executa a aplicacao integrada
java -cp build -Djava.library.path=../native/build main.Main
