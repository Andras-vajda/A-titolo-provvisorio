# Visual Studio Solution for Steiner Triple Systems

This directory contains a Visual Studio solution for building and running the Steiner Triple Systems (STS) generator on Windows.

## Solution Structure

The solution (`VS_Solution.sln`) contains the following projects:

- **STSC_lib**: Static library project implementing the core STS generation functionality
- **STSC_gen**: Console application for generating STS with various options
- **STSC_LUT**: Python project with the Python implementation of the STS generator

## STSC_lib Project

This project builds a static library (`STSC.lib`) that implements the core functionality for generating Steiner Triple Systems.

### Files

- STSC.c: Implementation of the STS generation functions
- STSC.h: Header file defining the library interface
- Peltesohn.h: Header file containing lookup tables of difference triples

### Build Output

When built, this project produces `STSC.lib` in the `/lib` directory of the repository.

## STSC_gen Project

This project builds a console application (`STSC_gen.exe`) that provides a user-friendly interface for generating STS.

### Files

- STSC_gen.c: Core implementation of the application
- STSC_mode.c: Implementation of different operation modes (interactive, batch, test)
- STSC_UI.c: User interface functions
- STSC_gen.h: Header file defining application structures and functions

### Features

- Interactive mode for generating single STS instances
- Batch mode for generating multiple STS over a range of v values
- Test mode for verifying the correctness of the implementation
- CSV export functionality

### Usage

The application supports the following command-line arguments:

```
STSC_gen                  # Interactive mode
STSC_gen /?               # Help mode
STSC_gen /test            # Test mode
STSC_gen /batch min max   # Batch mode for v in [min, max]
```

Example batch mode output can be found in `x64/Release/batch.txt`.

## STSC_LUT Project

This is a Python project containing the Python implementation of the STS generator.

### Files

- STSC_LUT.py: Main Python implementation of the STS generator

### Purpose

This project is included in the solution to provide access to the Python implementation from within Visual Studio, enabling easy comparison between the C and Python implementations.

## Building the Solution

1. Open `VS_Solution.sln` in Visual Studio (2019 or later recommended)
2. Select the desired configuration (Debug/Release) and platform (x86/x64)
3. Build the solution (F7 or Build ? Build Solution)

The build outputs will be placed in the appropriate configuration and platform directory (e.g., `x64/Release/`).

## Running the Application

After building, you can run the application directly from Visual Studio (F5 or Debug ? Start Debugging) or from the command line by navigating to the output directory and running `STSC_gen.exe`.

### Example Test Script

The `x64/Release/test.bat` file provides a simple batch script for testing the application:

```batch
echo off
STSC_gen 
STSC_gen /batch 7 99
STSC_gen /test
```

## Dependencies

- Visual Studio 2019 or later
- .NET Desktop development workload
- Python development workload (for the Python project)

## Notes

- The solution is configured to place the library output in the repository's `/lib` directory to make it easily accessible to the console application.
- The Python project requires Python to be installed and configured properly in Visual Studio.
- The x64 configuration is recommended for better performance, especially when generating STS for large v values.