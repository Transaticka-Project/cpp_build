# cpp_build
A quick way to build SDL2 projects (unity builds only)

on windows, build with:
>g++ build.cpp -std=c++14 -o build.exe
or
> clang++ build.cpp -std=c++14 -o build.exe

on an operating system however, build with:
> g++14 build.cpp -o build
or
> clang++14 build.cpp -o build


example of how to use the program from the terminal:
> build.exe my_program.cpp

optional parameters are "opt" and "web"

NOTE: you need to install SDL2 on the machine through a package manager if you're not on windows
