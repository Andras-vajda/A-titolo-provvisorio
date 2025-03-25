@echo off
REM Script batch per eseguire tutti gli script gen_lut.* in sequenza su Windows

REM Imposta il percorso della cartella dei CSV
set CSV_FOLDER=..\data

REM Controlla se la cartella esiste
if not exist %CSV_FOLDER% (
    echo Errore: La cartella %CSV_FOLDER% non esiste.
    exit /b 1
)

REM Esegue lo script Python
if exist gen_lut.py (
    echo Eseguendo Python...
    python gen_lut.py
)

REM Esegue lo script Tcl
if exist gen_lut.tcl (
    echo Eseguendo Tcl...
    tclsh gen_lut.tcl
)

REM Esegue lo script Lua
if exist gen_lut.lua (
    echo Eseguendo Lua...
    lua gen_lut.lua
)

REM Esegue lo script AWK (MAWK)
if exist gen_lut.awk (
    echo Eseguendo AWK (MAWK)...
    mawk -f gen_lut.awk *.csv > STSC_LUT.h
)

REM Controllo finale
if exist STSC_LUT.h (
    echo STSC_LUT.h generato con successo!
) else (
    echo Errore: STSC_LUT.h non trovato.
)

echo Operazione completata.
pause
