# Python Implementation for Steiner Triple Systems

This directory contains the Python implementation of the Steiner Triple Systems (STS) generator using the Heffter-Peltesohn method.

## Files

- **STSC_LUT.py**: Main implementation of the STS generator using precomputed difference triples
- **STSC_test.py**: Utilities for verifying generated Steiner Triple Systems
- **Peltesohn.py**: Contains the lookup table of precomputed difference triples

## STSC_LUT.py

This file provides a complete implementation of the Steiner Triple System generator using the Heffter-Peltesohn method with precomputed difference triples. It's designed to be educational and straightforward to understand.

### Features

- Supports all valid orders v where v ≡ 1 or 3 (mod 6), v ≥ 7, excluding v = 9
- Generates Steiner Triple Systems up to v = 99
- Uses the elegant Heffter-Peltesohn cyclic method
- Interactive interface for exploring different STS orders
- CSV export functionality

### Usage

```bash
python STSC_LUT.py
```

The program will prompt you to input a value for v. It will then generate an STS of order v and offer options to display the results and save to a CSV file.

## STSC_test.py

This file provides utilities for verifying the correctness of generated Steiner Triple Systems.

### Features

- Generates and verifies the incidence matrix (IM) of an STS
- Generates and verifies the Cartesian coverage matrix (CIM) to ensure all pairs are covered
- Standalone validation of STS stored in CSV files
- CSV export functionality

### Usage as a Module

```python
from STSC_test import genera_e_verifica_IM, genera_e_verifica_CIM, salva_terne_csv

# Generate an STS
triples = [...] # Your STS triples
v = 7 # Order of your STS

# Verify the STS using incidence matrix
IM, valid_IM = genera_e_verifica_IM(triples, v)
if valid_IM:
    print("STS is valid according to incidence matrix check")

# Verify the STS using Cartesian coverage matrix
CIM, valid_CIM = genera_e_verifica_CIM(triples, v)
if valid_CIM:
    print("STS is valid according to Cartesian coverage check")

# Save the STS to a CSV file
salva_terne_csv(v, triples)
```

### Usage as a Standalone Validator

```bash
python STSC_test.py STS_7.csv
```

## Peltesohn.py

This file contains the lookup table of precomputed difference triples used by the Heffter-Peltesohn method.

### Content

The file defines a Python dictionary `TERNE_PELTESOHN` that maps v values to lists of difference triples. These difference triples are the foundation of the STS generation process.

Example:
```python
TERNE_PELTESOHN = {
     7: [  (1,2,3)],
    13: [  (1,3,4),  (2,5,6)],
    15: [  (1,3,4),  (2,6,7)],
    # ... more entries
}
```

## Dependencies

- Python 3.6 or higher
- NumPy (for matrix operations)
- CSV module (included in standard library)

## Implementation Notes

The implementation follows the Heffter-Peltesohn method described in the main article. The key steps are:

1. Select precomputed difference triples for the given value of v
2. Generate base triples from these difference triples
3. Generate all v cyclic shifts of each base triple
4. For v ≡ 3 (mod 6), add a "short orbit" of additional triples
5. Verify that every pair of elements appears in exactly one triple

The use of NumPy for matrix operations significantly speeds up the verification process, making it feasible to verify even large systems in reasonable time.
