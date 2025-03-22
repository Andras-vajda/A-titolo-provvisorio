```markdown
# Documentation

This directory contains the theoretical background and documentation for the Hamming distance algorithms.

## Contents

- **Hamming.pdf** - Full academic paper describing the theory behind the algorithms
- **images/** - Directory containing diagrams and visualizations

## Key Concepts

### Hamming Distance

The Hamming distance between two strings of equal length is the number of positions at which the corresponding symbols differ. When applied to sets of equal cardinality, it represents the number of elements that need to be replaced in one set to obtain the other.

### Semi-Distance

For k-subsets, the Hamming distance is always even (proven in the paper), which leads to the concept of "semi-distance" defined as:

d(A, B) = H(A, B)/2

where H(A, B) is the Hamming distance between sets A and B.

### Visualization

The images directory contains visualizations of these partitions:

- **hamming_6_3.png** - Partition of 3-subsets of a 6-element set by Hamming distance
- **isomorfismo.png** - Illustration of the isomorphism between subsets and binary vectors

## References

Full references are available in the paper, with key works including:

- Knuth, D. E. (2011). The Art of Computer Programming, Volume 4A: Combinatorial Algorithms, Part 1. Addison-Wesley Professional.
- Hamming, R. W. (1950). Error detecting and error correcting codes. The Bell System Technical Journal, 29(2), 147-160.
```

