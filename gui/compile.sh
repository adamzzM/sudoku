

#!/bin/bash

clang++ -std=c++17 -arch arm64 \
main.cpp popup.cpp board.cpp \
-I/opt/homebrew/include \
-L/opt/homebrew/lib \
-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network \
-o SudokuApp.out

