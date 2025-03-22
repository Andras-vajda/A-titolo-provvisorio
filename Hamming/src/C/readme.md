```markdown
# C Implementations

This directory contains C implementations of algorithms for k-subset generation and Hamming distance classification.

## Files

1. **hamming_dl.c** - Optimized bitwise implementation for generating k-subsets by Hamming distance
2. **knuth.c** - Implementation of Knuth's algorithms T and R for subset generation
3. **ksubset_dl.c** - Simplified implementation for generating subsets at unit Hamming distance

## Compilation

All files can be compiled with standard C compilers. For Visual Studio Community Edition:

```bash
cl hamming_dl.c
cl knuth.c
cl ksubset_dl.c
```

With GCC:

```bash
gcc -o hamming_dl hamming_dl.c
gcc -o knuth knuth.c
gcc -o ksubset_dl ksubset_dl.c
```

## Usage Details

### hamming_dl.c

Generates k-subsets of an n-set, classified by Hamming distance from a reference subset.

```bash
./hamming_dl n k
```

- `n`: Size of the universe
- `k`: Size of subsets to generate
- Requirements: 2 < k and 2k ? n ? 32

### knuth.c

Implements Knuth's algorithms for generating k-subsets in different orders.

```bash
./knuth k n
```

- `k`: Size of subsets to generate
- `n`: Size of the universe
- Requirements: 2 < k < n

### ksubset_dl.c

Generates all k-subsets at Hamming semi-distance 1 from a reference subset.

```bash
./ksubset_dl k n
```

- `k`: Size of subsets to generate
- `n`: Size of the universe
- Requirements: 2 < k and 2k ? n ? 64
```
