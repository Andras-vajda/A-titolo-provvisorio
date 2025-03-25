# Steiner Triple Systems Generator

A comprehensive implementation of Steiner Triple Systems (STS) generation using the elegant Heffter-Peltesohn method. This repository contains code examples that accompany the article "Design combinatori: un primo assaggio" which introduces combinatorial designs with a focus on Steiner Triple Systems.

## Repository Overview

This repository provides implementations in C and Python for generating cyclic Steiner Triple Systems using the Heffter-Peltesohn method. The code is designed to be didactic, clearly illustrating the mathematical concepts while remaining computationally efficient.

### What are Steiner Triple Systems?

A Steiner Triple System of order v, denoted STS(v), is a collection of 3-element subsets (called triples or blocks) of a v-element set such that every pair of distinct elements appears in exactly one triple. STS(v) exists if and only if v ? 1 or 3 (mod 6), v ? 7, with the exception of v = 9.

### Features

- Efficient generation of Steiner Triple Systems for any valid v from 7 to 99
- Look-up table (LUT) based implementation using precalculated difference triples
- Verification utilities to confirm correctness of generated systems
- CSV export functionality for generated systems
- Comprehensive examples in both C and Python
- Visual Studio solution for Windows development
- Support scripts in multiple languages (Python, Lua, Tcl, AWK)

## Directory Structure

- **[C/](C/README.md)**: C implementation of the STS generator
- **[Include/](Include/README.md)**: Header files for the C implementation
- **[Python/](Python/README.md)**: Python implementation of the STS generator
- **[Script/](Script/README.md)**: Utility scripts for generating look-up tables and testing
- **[VS_Solution/](VS_Solution/README.md)**: Visual Studio solution files for building and running on Windows

## Getting Started

### Prerequisites

- For C code:
  - C99 compliant compiler (e.g., gcc, Visual Studio)
- For Python code:
  - Python 3.6 or higher
  - NumPy library

### Quick Start with Python

```bash
cd Python
python STSC_LUT.py
```

Follow the prompts to generate a Steiner Triple System of your chosen order v.

### Quick Start with C (Visual Studio)

1. Open the solution file `VS_Solution/VS_Solution.sln` in Visual Studio
2. Build the solution
3. Run the `STSC_gen` project
4. Follow the prompts to generate a Steiner Triple System

### Quick Start with C (Command Line)

```bash
cd C
gcc -std=c99 STSC_LUT.c -o stsc_generator
./stsc_generator
```

## Theoretical Background

The implementation uses the Heffter-Peltesohn method, which is based on difference triples to efficiently generate cyclic STS. For a detailed explanation of the mathematical theory behind Steiner Triple Systems and this implementation, please refer to the article "Design combinatori: un primo assaggio" included in this repository.

Key parameters:
- v: Number of elements in the base set (must satisfy v ? 1 or 3 (mod 6), v ? 7, v ? 9)
- b: Number of triples in the STS, given by b = v(v-1)/6
- r: Number of triples containing a given element, given by r = (v-1)/2

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## References

- Colbourn, C.J., & Rosa, A. (1999). "Triple Systems." Oxford University Press. ISBN: 978-0198535768.
- Lindner, C.C., & Rodger, C.A. (2008). "Design Theory (2nd Edition)." Chapman & Hall/CRC. ISBN: 978-1420082968.
- Peltesohn, R. (1939). "Eine Loesung der beiden Heffterschen Differenzenprobleme." Compositio Mathematica, 6, 251-257.
