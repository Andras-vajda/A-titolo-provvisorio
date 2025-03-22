```markdown
# Hamming Distance Algorithms

This repository contains example implementations of algorithms for generating and classifying subsets based on Hamming distance, as well as efficient k-subset generation algorithms.

## Overview

The Hamming distance between two sets of equal cardinality represents the number of elements that need to be replaced in one set to obtain the other. For k-subsets, the Hamming distance is always even, which leads to the concept of "semi-distance" (half the Hamming distance).

These algorithms provide efficient ways to:
- Generate all k-subsets of an n-set
- Classify subsets by their Hamming distance from a reference subset
- Generate subsets in lexicographic or Gray code order

## Directory Structure

- `src/` - Source code implementations
  - `C/` - C implementations
  - `Python/` - Python implementations
- `docs/` - Documentation and theoretical background
- `examples/` - Usage examples and test cases

## Implementation Details

### C Implementations

- `hamming_dl.c` - Optimized bitwise implementation for generating k-subsets by Hamming distance
- `knuth.c` - Implementation of Knuth's algorithms T and R for subset generation
- `ksubset_dl.c` - Simplified implementation for generating subsets at unit Hamming distance

### Python Implementation

- `hamming.py` - Pedagogical implementation using iterators and combinatorial functions

## Usage

### C Programs

```bash
# Compile Knuth's algorithms
gcc -o knuth knuth.c

# Run with parameters k and n (where k < n)
./knuth 3 10

# Compile and run Hamming distance generator
gcc -o hamming_dl hamming_dl.c
./hamming_dl 8 3

### Python Example
python hamming.py

### References
- Knuth, D. E. (2011). The Art of Computer Programming, Volume 4A: Combinatorial Algorithms, Part 1. Addison-Wesley Professional.
- Hamming, R. W. (1950). Error detecting and error correcting codes. The Bell System Technical Journal, 29(2), 147-160.