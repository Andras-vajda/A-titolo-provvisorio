@echo off
setlocal
rem Eseguire dal Developer Command Prompt x64 di Visual Studio.
pushd "%~dp0"
cl /nologo /std:c11 /TC /W4 /O2 sbm3.c sbm3_cli.c /Fe:sbm3.exe
if errorlevel 1 goto failed
cl /nologo /std:c11 /TC /W4 /O2 /DSBM3_TEST_HOOKS sbm3.c sbm3_test.c /Fe:sbm3_test.exe
if errorlevel 1 goto failed
sbm3_test.exe
if errorlevel 1 goto failed
popd
exit /b 0
:failed
popd
exit /b 1
