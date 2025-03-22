```markdown
# Hamming Distance Examples

This document provides practical examples of using the Hamming distance algorithms included in this repository.

## 1. Generating Subsets at Unit Distance

The simplest example is generating all subsets that differ from a reference subset by exactly one element.

### Example with `ksubset_dl.c`:

```bash
./ksubset_dl 3 8
```

This generates all 3-subsets from an 8-element universe that have Hamming semi-distance 1 from the base subset {1, 2, 3}.

Output:
```
Base subset:
{ 1 2 3 }
{ 1 2 4 }
{ 1 2 5 }
{ 1 2 6 }
{ 1 2 7 }
{ 1 2 8 }
{ 1 3 4 }
{ 1 3 5 }
{ 1 3 6 }
{ 1 3 7 }
{ 1 3 8 }
{ 2 3 4 }
{ 2 3 5 }
{ 2 3 6 }
{ 2 3 7 }
{ 2 3 8 }
```

## 2. Complete Hamming Distance Classification

Using `hamming_dl.c` to generate all subsets classified by their Hamming distance:

```bash
./hamming_dl 6 3
```

This generates all 3-subsets from a 6-element universe, classified by their Hamming semi-distance from {1, 2, 3}.

Output extract:
```
** Base set: (000111) {a, b, c}

=== d = 1 ===
   1 [1]: (001011) {a, c, d}
   2 [1]: (001101) {b, c, d}
   3 [1]: (010011) {a, b, d}
   4 [1]: (001110) {a, b, e}
   5 [1]: (010101) {a, c, e}
   6 [1]: (011001) {b, c, e}
   7 [1]: (001111) {a, b, f}
   8 [1]: (010110) {a, c, f}
   9 [1]: (011010) {b, c, f}

=== d = 2 ===
  10 [2]: (011100) {d, e, f}
  11 [2]: (010110) {a, e, f}
  12 [2]: (011010) {b, e, f}
  13 [2]: (011001) {c, e, f}
  14 [2]: (100101) {a, d, f}
  15 [2]: (101001) {b, d, f}
...
```

## 3. Using Different Generation Orders

Using `knuth.c` to generate subsets in different orders:

```bash
./knuth 3 6
```

This generates all 3-subsets of a 6-element set in both lexicographic order and revdoor (Gray code) order.

Lexicographic order output (excerpt):
```
T_ksubsetLex(3, 6) for lexicographic order:
    1 {0, 1, 2}
    2 {0, 1, 3}
    3 {0, 1, 4}
    4 {0, 1, 5}
    5 {0, 2, 3}
...
```

Revdoor order output (excerpt):
```
R_ksubsetRD(3, 6) for revdoor (Gray) order:
    1 {0, 1, 2}
    2 {0, 1, 3}
    3 {0, 2, 3}
    4 {0, 2, 4}
    5 {0, 1, 4}
...
```

## 4. Python Interactive Example

The Python implementation provides an interactive interface:

```bash
python hamming.py
```

Sample interaction:
```
*** Step 1: Generation 4-subset of a 10-set with distance limit d=4 ***
4.01: [ 7, 8, 9,10]
4.02: [ 6, 8, 9,10]
4.03: [ 6, 7, 9,10]
...

*** Step 2: User interaction ***
Enter a value for k (2 < k < 7, or 0 to end): 3
Enter a value for n (minimum 6, max 12, or 0 to end): 8
Do you want the extended printing of all generated subsets? (y/n): y

Base subset:  1  2  3

** d =  0: 1 subsets
** d =  1: 15 subsets
** d =  2: 35 subsets
** d =  3: 7 subsets

0.01: [ 1, 2, 3]
1.01: [ 1, 2, 4]
1.02: [ 1, 2, 5]
...
```
```
