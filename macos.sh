#!/bin/bash

clang++ -std=c++11 -Wall -pedantic \
  *.cpp \
  */*.cpp \
  -I"./include" \
  -L"./macos" \
  -o Fantoccini.app/Contents/MacOS/fantoccini \
  -framework OpenGL \
  -framework CoreVideo \
  -framework IOKit \
  -framework Cocoa \
  -framework Carbon \
  -lglfw3 \
  -lglew \
  -lfbxsdk \
  -lphysfs \
&& ./Fantoccini.app/Contents/MacOS/fantoccini
