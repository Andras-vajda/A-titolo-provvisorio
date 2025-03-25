# C Implementation for Steiner Triple Systems

This directory contains the C implementation of the Steiner Triple Systems (STS) generator using the Heffter-Peltesohn method.

## Files

- **STSC_LUT.c**: Standalone implementation of the STS generator using precomputed difference triples
- **STSC_gen.c**: Core implementation for the STSC_gen executable, which serves as the main entry point for the STS generator
- **STSC.c**: Core library implementation of the STS generation functions

## STSC_LUT.c

This file provides a complete, standalone implementation of the Steiner Triple System generator using the Heffter-Peltesohn method with precomputed difference triples stored in lookup tables. It's designed to be educational and straightforward to understand.

### Features

- Supports all valid orders v where v ? 1 or 3 (mod 6), v ? 7, excluding v = 9
- Generates Steiner Triple Systems up to v = 99
- Uses the elegant Heffter-Peltesohn cyclic method
- Provides functions to verify the correctness of generated systems

### Compilation

```bash
gcc -std=c99 STSC_LUT.c -o stsc_lut
```

### Usage

```bash
./stsc_lut
```

The program will prompt you to input a value for v. It will then generate an STS of order v and display the results.

## STSC_gen.c

This file implements the core logic for the STSC_gen executable, which is a more feature-rich implementation of the STS generator.

### Features

- Supports interactive, batch, and test modes
- Can save generated systems to CSV files
- Supports command-line arguments for automation
- Uses the library approach for modularity

### Usage Modes

The executable supports several modes of operation:

- **Interactive Mode**: Default mode that prompts the user for input
- **Batch Mode**: Generates STS for a range of v values (e.g., `/batch 7 99`)
- **Test Mode**: Runs tests to verify correctness (`/test`)
- **Help Mode**: Displays usage instructions (`/?`)

## Building the Complete Application

For the complete application with all features, it's recommended to use the Visual Studio solution provided in the `VS_Solution` directory. The solution properly links all dependencies and includes additional features like batch processing and verification.

## Dependencies

- C99 compliant compiler
- Standard C libraries (stdio.h, stdlib.h, string.h)

## Notes on Implementation

The implementation follows the Heffter-Peltesohn method described in the main article. The key steps are:

1. Select precomputed difference triples for the given value of v
2. Generate base triples from these difference triples
3. Generate all v cyclic shifts of each base triple
4. For v ? 3 (mod 6), add a "short orbit" of additional triples
5. Verify that every pair of elements appears in exactly one triple

The computational complexity is O(v), making it extremely efficient compared to other methods, especially for large values of v.