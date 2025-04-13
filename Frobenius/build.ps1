# ================================================================
# Script PowerShell per la compilazione del progetto Frobenius
# ================================================================

Write-Host "============================================================"
Write-Host "Compilazione del progetto Frobenius con PowerShell"
Write-Host "============================================================"

# Verifica esistenza delle directory necessarie
if (-not (Test-Path "bin")) { New-Item -Path "bin" -ItemType Directory | Out-Null }
if (-not (Test-Path "build")) { New-Item -Path "build" -ItemType Directory | Out-Null }

# Verifica requisiti per la versione C
$libTomMathAvailable = $true
if (-not (Test-Path "include")) {
    Write-Host "AVVISO: La directory 'include' non esiste. LibTomMath necessaria." -ForegroundColor Yellow
    $libTomMathAvailable = $false
}
if (-not (Test-Path "lib")) {
    Write-Host "AVVISO: La directory 'lib' non esiste. LibTomMath necessaria." -ForegroundColor Yellow
    $libTomMathAvailable = $false
}

# Verifica che Visual Studio sia disponibile
$vsCl = Get-Command cl -ErrorAction SilentlyContinue
if ($null -eq $vsCl) {
    Write-Host "AVVISO: Compilatore Visual Studio (cl) non trovato." -ForegroundColor Yellow
    Write-Host "Eseguire questo script da 'Developer PowerShell for VS'." -ForegroundColor Yellow
}
else {
    Write-Host "Compilazione della versione C..."
    if ($libTomMathAvailable) {
        & cl /nologo /W4 /wd4996 /O2 /Zi /MD /EHsc /I"include" /Fo"build\\" /Fe"bin\frobenius_ltm.exe" Frobenius_ltm.c /link /LIBPATH:"lib" tommath.lib
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERRORE: Compilazione della versione C fallita." -ForegroundColor Red
        }
        else {
            Write-Host "Compilazione C completata con successo." -ForegroundColor Green
        }
    }
    else {
        Write-Host "Compilazione C saltata a causa di LibTomMath mancante." -ForegroundColor Yellow
    }
}

# Verifica Rust
$cargo = Get-Command cargo -ErrorAction SilentlyContinue
if ($null -eq $cargo) {
    Write-Host "AVVISO: Cargo (Rust) non trovato. Installare Rust da https://rustup.rs/" -ForegroundColor Yellow
}
else {
    Write-Host "`nCompilazione della versione Rust..."
    & cargo build --release
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERRORE: Compilazione della versione Rust fallita." -ForegroundColor Red
    }
    else {
        Copy-Item "target\release\frobenius_num.exe" "bin\" -Force
        Write-Host "Eseguibile Rust copiato in bin\frobenius_num.exe" -ForegroundColor Green
    }
}

# Verifica Python
$python = Get-Command python -ErrorAction SilentlyContinue
if ($null -eq $python) {
    Write-Host "`nAVVISO: Python non trovato. Installare Python da https://www.python.org/" -ForegroundColor Yellow
}
else {
    Write-Host "`nVerificando dipendenze Python..."
    try {
        & python -c "import numpy, sympy" 2>$null
        Write-Host "Dipendenze Python verificate." -ForegroundColor Green
    }
    catch {
        Write-Host "AVVISO: Dipendenze Python mancanti. Eseguire:" -ForegroundColor Yellow
        Write-Host "pip install numpy sympy" -ForegroundColor Yellow
    }
}

# Verifica Haskell
$ghc = Get-Command ghc -ErrorAction SilentlyContinue
if ($null -eq $ghc) {
    Write-Host "`nAVVISO: GHC (Haskell) non trovato. Installare Haskell Platform da https://www.haskell.org/platform/" -ForegroundColor Yellow
}
else {
    Write-Host "`nCompilazione della versione Haskell..."
    & ghc -O2 -o bin\frobenius_hs.exe Frobenius.hs
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERRORE: Compilazione della versione Haskell fallita." -ForegroundColor Red
    }
    else {
        Write-Host "Eseguibile Haskell creato in bin\frobenius_hs.exe" -ForegroundColor Green
    }
}

Write-Host "`n============================================================"
Write-Host "Compilazione completata. Gli eseguibili sono nella cartella 'bin'."
Write-Host "============================================================"