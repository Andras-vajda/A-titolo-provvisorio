# Script Directory for Steiner Triple Systems

This directory contains utility scripts for generating lookup tables and testing the Steiner Triple Systems (STS) generator.

## Files

- **Gen_LUT.py**: Python script for generating the C header file containing lookup tables
- **gen_lut.lua**: Lua implementation of the lookup table generator
- **Gen_LUT.tcl**: Tcl implementation of the lookup table generator
- **gen_lut.awk**: AWK implementation of the lookup table generator
- **test_script.bat**: Batch script for running all LUT generators sequentially on Windows

## Gen_LUT.py

This Python script generates a C header file containing the lookup tables of difference triples used by the Heffter-Peltesohn method.

### Features

- Parses CSV files containing precomputed difference triples
- Organizes the data into C-compatible arrays
- Generates a well-formatted C header file with all necessary constants and data structures

### Usage

```bash
python Gen_LUT.py
```

This will generate a `STSC_LUT.h` file that can be included in C applications.

### Input Format

The script expects CSV files in the `./data` directory with the following format:
- First line: Header with comma-separated values, where the 4th and 5th values are v and td (number of difference triples)
- Subsequent lines: Comma-separated triples representing difference triples

## gen_lut.lua

This is a Lua implementation of the lookup table generator, providing the same functionality as the Python version.

### Usage

```bash
lua gen_lut.lua
```

## Gen_LUT.tcl

This is a Tcl implementation of the lookup table generator, providing the same functionality as the Python version.

### Usage

```bash
tclsh Gen_LUT.tcl
```

## gen_lut.awk

This is an AWK implementation of the lookup table generator, providing similar functionality to the other versions.

### Usage

```bash
mawk -f gen_lut.awk *.csv > STSC_LUT.h
```

## test_script.bat

This Windows batch script runs all the LUT generator scripts sequentially, allowing for comparison of their outputs.

### Features

- Tests all implemented generators (Python, Lua, Tcl, AWK)
- Verifies that the output file is created successfully
- Provides feedback on each step of the process

### Usage

```bash
test_script.bat
```

## Purpose of Multiple Implementations

The availability of multiple implementations in different languages serves several purposes:

1. **Educational Value**: Shows how the same algorithm can be implemented in different programming paradigms
2. **Portability**: Ensures the lookup tables can be generated on various platforms with different available tools
3. **Verification**: Multiple implementations can cross-validate each other, increasing confidence in the correctness of the lookup tables
4. **Performance Comparison**: Allows for comparing the efficiency of different languages for this specific task

## Generated Output

All scripts generate a similarly structured `STSC_LUT.h` file with:

1. A `MAX_LUT` constant defining the number of entries in the lookup table
2. A `LUT` array mapping v values to the number of difference triples
3. A `Terna_t` struct definition for representing triples
4. A `TERNE_PELTESOHN` array containing the precomputed difference triples

This header file is then used by the C implementation to efficiently generate Steiner Triple Systems.