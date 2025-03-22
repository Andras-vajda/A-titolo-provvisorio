@echo off
setlocal EnableDelayedExpansion

echo Compiling C implementations...
echo ===========================

cd /d "%~dp0\src\C"

echo.
echo Compiling hamming_dl.c...
cl /W4 /O2 /nologo hamming_dl.c /Fe"hamming_dl.exe"
if %ERRORLEVEL% EQU 0 (
    echo - Success: hamming_dl.exe created
) else (
    echo - Error compiling hamming_dl.c
)

echo.
echo Compiling knuth.c...
cl /W4 /O2 /nologo knuth.c /Fe"knuth.exe"
if %ERRORLEVEL% EQU 0 (
    echo - Success: knuth.exe created
) else (
    echo - Error compiling knuth.c
)

echo.
echo Compiling ksubset_dl.c...
cl /W4 /O2 /nologo ksubset_dl.c /Fe"ksubset_dl.exe"
if %ERRORLEVEL% EQU 0 (
    echo - Success: ksubset_dl.exe created
) else (
    echo - Error compiling ksubset_dl.c
)

echo.
echo Compilation complete.
cd ..\..

pause