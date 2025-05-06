# Repository: Petri

## Description

This repository contains various implementations of Petri Net simulators for 1-safe Petri Nets in multiple programming languages, supporting the theoretical and practical aspects described in the article "Intrappolati nelle Reti (di Petri)".

A 1-safe Petri Net is a specific type of Petri Net where each place can contain at most one token, making it particularly suitable for modeling systems with binary states. This constraint allows for efficient implementations using bitfield representations and boolean algebra operations.

## Contents

The repository includes implementations in the following languages:

- `petri_net_c99.c`: MISRA-C compliant implementation optimized for embedded systems
- `petri_net_python.py`: Python implementation with graphical visualization using matplotlib
- `petri_net_j.ijs`: J implementation showcasing array programming capabilities
- `petri_net_rust.rs`: Rust implementation with memory safety guarantees
- `petri_net_eiffel.e`: Eiffel implementation using Design by Contract
- `petri_net_winicon.icn`: WinIcon implementation using pattern matching
- `ada_petri_basic.adb` and `ada_petri_arinc.adb`: Ada implementations (basic and ARINC-653 compliant)

Each implementation models the same example Petri Net with 6 places and 5 transitions, as described in the accompanying paper.

## Example Petri Net

The implementations model a simple Petri Net with the following structure:

- 6 places: p1, p2, p3, p4, p5, p6
- 5 transitions: t1, t2, t3, t4, t5
- Initial marking: One token in p1
- Connectivity:
  - t1 consumes from p1 and produces in p2 and p3
  - t2 consumes from p2 and produces in p4
  - t3 consumes from p3 and produces in p5
  - t4 consumes from p4 and produces in p6
  - t5 consumes from p5 and produces in p6

This structure demonstrates both concurrency (after t1 fires, there are two parallel execution paths) and synchronization (both paths merge at p6).

## Features

Common features across all implementations:

- Efficient bitfield representation for 1-safe Petri Net markings
- Simulation of Petri Net execution with step-by-step tracing
- Analysis of reachability and other properties
- Verification of behavioral properties (boundedness, liveness, etc.)

Implementation-specific features:

- **C99**: MISRA-C compliant with extensive documentation and performance metrics
- **Python**: Interactive visualization using matplotlib with animation support
- **J**: Concise vector operations for matrix-based Petri Net representation
- **Rust**: Memory-safe implementation with ownership guarantees
- **Eiffel**: Design by Contract approach with formal verification support
- **WinIcon**: Pattern matching and string-oriented implementation
- **Ada**: Type-safe implementation with ARINC-653 compatible version

## Usage

Each implementation includes a `main` function or equivalent that demonstrates the use of the Petri Net simulator with the example network.

Example (C99):

```c
int main(void) {
    PetriNet net;
    
    /* Initialize the random number generator */
    srand((unsigned int)time(NULL));
    
    /* Initialize the Petri Net */
    PetriNet_Init(&net);
    
    /* Print the initial state */
    printf("Initial state of the 1-safe Petri Net:\n");
    PetriNet_PrintMarking(&net);
    PetriNet_PrintEnabledTransitions(&net);
    
    /* Run the simulation */
    PetriNet_Simulate(&net, 10);
    
    /* Reset the net to its initial state */
    PetriNet_Reset(&net);
    
    printf("\nThe net has been reset to its initial marking.\n");
    PetriNet_PrintMarking(&net);
    
    return EXIT_SUCCESS;
}
```

## Requirements

- **C99**: Standard C compiler (MSVC, Clang, Digital Mars etc.)
- **Python**: Python 3.x with matplotlib and numpy
- **J**: J interpreter (version 9.x recommended)
- **Rust**: Rust compiler (stable channel)
- **Eiffel**: EiffelStudio 19.x or higher
- **WinIcon**: WinIcon interpreter or compiler
- **Ada**: online compiler (Ada 2012 or newer)

## Performance Metrics

Each implementation includes detailed performance metrics in the initial comments, covering:

- Code quality metrics (cyclomatic complexity, LOC, etc.)
- Memory usage
- Execution time
- Compliance with relevant standards (if applicable)

## Citations

This repository accompanies the article "Reti di Petri: Teoria, Applicazioni, Esempi" which provides a comprehensive treatment of Petri Net theory with special focus on 1-safe nets and their connections to boolean rings.

Key references from the article:

- Murata, T. (1989). "Petri Nets: Properties, Analysis and Applications." Proceedings of the IEEE, 77(4):541-580.
- Peterson, J.L. (1981). "Petri Net Theory and the Modeling of Systems." Prentice-Hall.
- Reisig, W. (1985). "Petri Nets: An Introduction." Springer-Verlag.
- Silva, M., Teruel, E., Colom, J.M. (1998). "Linear Algebraic and Linear Programming Techniques for the Analysis of Place/Transition Net Systems." Lecture Notes in Computer Science, Vol. 1491, Springer.

## License

MIT License (see LICENSE file for details)

## Contact

Author: M.A.W. 1968

## Note for Article Publication

Due to space constraints in the associated academic article, only the complete source code for the C99, Python, and J implementations will be included. For other languages, only the initial sections containing performance metrics will be published.
