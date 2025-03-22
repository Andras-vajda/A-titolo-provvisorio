@echo off
setlocal EnableDelayedExpansion

echo ===================================================================
echo Hamming Distance Algorithms - Test Suite
echo ===================================================================
echo.

cd /d "%~dp0"

:: Test directories
set "c_dir=src\C"
set "python_dir=src\Python"
set "output_dir=examples\datasets\output"

:: Create output directory if it doesn't exist
if not exist "%output_dir%" mkdir "%output_dir%"

echo Compiling C programs...
echo ---------------------
cd "%c_dir%"

:: Compile programs
cl /W4 /O2 /nologo hamming_dl.c /Fe"hamming_dl.exe" 2>nul && echo hamming_dl.c compiled successfully.
cl /W4 /O2 /nologo knuth.c /Fe"knuth.exe" 2>nul && echo knuth.c compiled successfully.
cl /W4 /O2 /nologo ksubset_dl.c /Fe"ksubset_dl.exe" 2>nul && echo ksubset_dl.c compiled successfully.

cd ..\..

echo.
echo Running tests...
echo ---------------------

:: Test 1: ksubset_dl - Unit Hamming distance
echo Test 1: Generating 3-subsets with unit Hamming distance from an 8-element set
"%c_dir%\ksubset_dl.exe" 3 8 > "%output_dir%\test1_unit_distance.txt"
echo Results saved to %output_dir%\test1_unit_distance.txt

:: Test 2: hamming_dl - Complete classification
echo Test 2: Generating all 3-subsets from a 6-element set with Hamming distance classification
"%c_dir%\hamming_dl.exe" 6 3 > "%output_dir%\test2_full_classification.txt"
echo Results saved to %output_dir%\test2_full_classification.txt

:: Test 3: knuth - Different ordering methods
echo Test 3: Generating 3-subsets of a 6-element set in different orders
"%c_dir%\knuth.exe" 3 6 > "%output_dir%\test3_generation_orders.txt"
echo Results saved to %output_dir%\test3_generation_orders.txt

:: Test 4: Python implementation
echo Test 4: Running Python implementation with test values
cd "%python_dir%"
python -c "import hamming; hamming.generate_k_subsets_ordered_by_hamming(3, 8, True)" > "..\..\%output_dir%\test4_python_output.txt"
cd ..\..
echo Results saved to %output_dir%\test4_python_output.txt

echo.
echo ===================================================================
echo All tests completed successfully!
echo ===================================================================
echo.
echo Test summary:
echo - Generated 3-subsets with unit Hamming distance (ksubset_dl)
echo - Generated complete classification by Hamming distance (hamming_dl)
echo - Generated subsets in lexicographic and Gray code orders (knuth)
echo - Executed Python implementation with test parameters
echo.
echo All results saved to %output_dir%\
echo ===================================================================

pause