# dynoXOR

![dynoXOR logo](./assets/dynoxor.jpg)

A simple XOR encryption and decryption tool written in C++.
Encrypt or decrypt files using a user-supplied or randomly generated XOR key.

## Features

- XOR encryption and decryption of arbitrary files
- Support for user-supplied or randomly generated XOR keys
- Processing files in configurable chunk sizes for efficiency
- Optional backup of input files before overwriting
- Logging of used keys with filenames for auditing
- Cross-platform support for configuration directory paths

## Prerequisites

- C++20 compatible compiler (e.g., clang++, g++)
- [CLI11](https://github.com/CLIUtils/CLI11) library (included as header in `include/`)

## Build Instructions

Make sure you have a C++20 comptabile compiler installed on your system:

#### Windows (using MSYS2 / MinGW or Visual Studio Command Prompt)

- Using MSYS2 with clang++:

`clang++ -std=c++20 -Iinclude -o dynoXOR.exe src/main.cpp src/functions.cpp`

- Using Visual Studio Developer Command Prompt:

`cl /std:c++20 /I include src\main.cpp src\functions.cpp /Fe:dynoXOR.exe`

*Note: if you use Visual Studio IDE, create a project and add source and header files accordingly.*

#### macOS

- Using the built-in clang++ (Xcode Command Line Tools required):

`clang++ -std=c++20 -Iinclude -o dynoXOR src/main.cpp src/functions.cpp`

#### Linux

- Using g++ (GCC):

`g++ -std=c++20 -Iinclude -o dynoXOR src/main.cpp src/functions.cpp`

- Or use clang++ if preferred:

`clang++ -std=c++20 -Iinclude -o dynoXOR src/main.cpp src/functions.cpp`

*Ensure the include directory is specified correctly with -Iinclude so the compiler finds your headers (e.g., constants.hpp, functions.hpp, CLI11.hpp).*

## Usage

Basic usage examples:

*Encrypt a file with a provided key:*

`./dynoXOR -f input.txt -k mysecretsuperlongandrandomkey -o output.enc`

*Generate a random key and encrypt:*

`./dynoXOR -f input.txt --generate -o output.enc`

*For full command line options and flags, run:*

`./dynoXOR --help`

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

