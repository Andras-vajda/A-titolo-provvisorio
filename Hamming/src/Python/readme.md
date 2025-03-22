```markdown
# Python Implementation

This directory contains a Python implementation of algorithms for k-subset generation based on Hamming distance.

## Files

- **hamming.py** - A pedagogical implementation using Python's itertools and focusing on clarity rather than performance

## Requirements

- Python 3.6 or newer

## Usage

Simply run the Python script:

```bash
python hamming.py
```

The script provides an interactive interface that allows you to:

1. Generate 4-subsets of a 10-set with distance limit d=4 (as a demonstration)
2. Enter custom values for k and n to generate k-subsets of an n-set
3. Choose whether to display all generated subsets or just summary statistics

## Implementation Details

The Python implementation emphasizes clarity and educational value over performance. It uses:

- Python's `itertools.combinations` for efficient generation of combinations
- Generator functions (`yield`) for memory-efficient processing
- A user-friendly interactive interface

This implementation is particularly useful for:
- Understanding the basic concepts of Hamming distance
- Visualizing the partition of subsets by distance
- Educational purposes where clarity is more important than performance

For high-performance applications with large sets, consider using the C implementations.
```
