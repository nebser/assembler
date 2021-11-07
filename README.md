# ARM-like Assembler
## Description
A very primitive assembler example implemented in c++, much similar to the ARM assembler, only simplified.
## Requirements:

* g++ with c++11 support (g++-4.8 or higher)

## Compilation:
Via g++:
```
g++ cpp/*.cpp -I ./h -std=c++11 -o assembler.out
```
Or via makefile just run:
```
make
```

## Execution:
Executable expects two to three arguments in the provided order:
1. Name of the file that is going to be assembed (required)
2. Name of the output object file (required)
3. Presumed address of the first instruction in the created object file (optional, default iz zero)

Examples:
```
./assembler.out input/hello_world.txt output/hello_world.obj 0x10
./assembler.out input/max.txt output/max.obj
```
