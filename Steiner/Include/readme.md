# Include Directory for Steiner Triple Systems

This directory contains the header files for the C implementation of the Steiner Triple Systems (STS) generator.

## Files

- **STSC.h**: Main header file defining the core functions and structures for the STS library
- **STSC_LUT.h**: Header file containing the lookup tables of precomputed difference triples
- **Peltesohn.h**: Simplified version of the lookup tables for specific cases
- **STSC_gen.h**: Header file for the STSC_gen executable

## STSC.h

This header file defines the core functions and data structures for the STS library implementation.

### Key Definitions

- `Terna_t`: Structure representing a triple (block) in the STS
- `Genera_STSC()`: Function to generate an STS of order v
- `Genera_terne_differenza()`: Function to generate difference triples based on the Peltesohn formulas
- `Convalida_v()`: Function to validate that a given v value satisfies the conditions for an STS
- `Totale_terne_STS()`: Function to calculate the expected number of triples in an STS of order v

### Usage Example

```c
#include "STSC.h"
#include <stdio.h>

int main() {
    size_t v = 7;
    size_t b = Totale_terne_STS(v);
    Terna_t *sts = (Terna_t*)malloc(b * sizeof(Terna_t));
    
    if (sts == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    size_t terne_generate = Genera_STSC(v, sts);
    
    if (terne_generate == b) {
        printf("STS(%zu) generated successfully with %zu triples\n", v, b);
        
        // Use the generated STS
        for (size_t i = 0; i < b; i++) {
            printf("%zu: (%u, %u, %u)\n", i + 1, sts[i].a, sts[i].b, sts[i].c);
        }
    }
    
    free(sts);
    return 0;
}
```

## STSC_LUT.h

This header file contains the lookup tables of precomputed difference triples for the Heffter-Peltesohn method.

### Key Definitions

- `MAX_LUT`: Maximum number of entries in the lookup table
- `MAX_DIFF`: Maximum number of difference triples for any v
- `LUT`: Array mapping v values to the number of difference triples
- `TERNE_PELTESOHN`: Array of precomputed difference triples for various v values

### Purpose

The lookup tables enable efficient generation of STS by providing precomputed difference triples for v values from 7 to 99. This eliminates the need to calculate these triples at runtime, which can be computationally intensive for large v values.

## Peltesohn.h

This is a simplified version of the lookup tables, focusing on specific cases that are particularly useful for testing and demonstration.

### Key Definitions

- `MAX_LUT`: Number of specific cases included
- `MAX_DIFF`: Maximum number of difference triples for these cases
- `LUT`: Array with specific v values and their number of difference triples
- `TERNE_PELTESOHN`: Array of difference triples for the specific cases

### Purpose

This header provides a more manageable subset of the lookup tables for testing and demonstration purposes, focusing on key values that illustrate the properties of the Heffter-Peltesohn method.

## STSC_gen.h

This header file defines structures and functions for the STSC_gen executable.

### Key Definitions

- `SysParams`: Structure containing system parameters for the generator
- Function declarations for different modes of operation (interactive, batch, test)
- Constants and macros for file naming and other operational aspects

### Purpose

This header supports the more feature-rich STSC_gen executable, which provides interactive, batch, and test modes for generating and validating Steiner Triple Systems.