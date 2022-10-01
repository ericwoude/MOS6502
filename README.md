[![Main Status](https://img.shields.io/github/workflow/status/ericwoude/MOS6502/main?style=social)](https://github.com/ericwoude/MOS6502/actions/workflows/main.yml)
<!-- [![Latest Release](https://img.shields.io/github/v/release/ericwoude/MOS6502?style=social)](https://github.com/ericwoude/MOS6502/releases) -->
[![License](https://img.shields.io/github/license/ericwoude/MOS6502?style=social)](https://github.com/ericwoude/MOS6502/blob/main/LICENSE)

# MOS Technology 6502 emulator
WIP

- All instructions are implemented
- Still fixing bugs and refactoring code

## Usage
1. Configure the project and generate the native build system, and call the build system to compile and link the project:
```bash
$ make build
```
The compiled executables will be placed in build/bin.

2. Test the project
```bash
$ make test
```