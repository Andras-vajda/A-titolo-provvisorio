# Problema di Frobenius - Implementazioni Didattiche

Questo repository contiene molteplici implementazioni del **Problema di Frobenius** (noto anche come il "problema del resto" o "problema delle monete") in diversi linguaggi di programmazione, ottimizzate per Windows e Visual Studio.

## Il Problema di Frobenius

Il problema di Frobenius consiste nel determinare il più grande intero che **non può** essere espresso come combinazione lineare non-negativa di un dato insieme di interi positivi.

Date `n` monete con valori `a₁, a₂, ..., aₙ` (dove il MCD è 1), il "numero di Frobenius" è il più grande importo che non può essere pagato utilizzando quelle monete.

Un esempio famoso è il "Problema McNugget": se le confezioni di McNuggets sono disponibili in formati da 6, 9 e 20 pezzi, qual è il più grande numero di McNuggets che non può essere acquistato esattamente? (La risposta è 43).

## Implementazioni

Il repository contiene le seguenti implementazioni:

1. **C** (`Frobenius.c`): Implementazione minimalista in C con tipi interi standard.
2. **Rust** (`Frobenius_num.rs`): Implementazione in Rust con supporto multithreading e utilizzo di `num-bigint` per numeri di grandi dimensioni.
3. **Python** (`Frobenius_solver.py`): Implementazione in Python utilizzando NumPy e SymPy per approcci sia numerici che simbolici.
4. **Haskell** (`Frobenius.hs`): Implementazione funzionale in Haskell con tecniche avanzate di memoizzazione e ottimizzazione.

## Requisiti di Sistema

Questo progetto è ottimizzato per l'ambiente Windows con Visual Studio:

### Requisiti generali

- Windows 10/11
- Visual Studio 2019/2022
- CMake 3.10 o superiore

### Per la versione C

- Compilatore C standard (Visual Studio, GCC o Clang)

### Per la versione Rust

- Rust e Cargo (installabili da [rustup.rs](https://rustup.rs/))

### Per la versione Python

- Python 3.7+
- NumPy e SymPy (`pip install numpy sympy`)

### Per la versione Haskell

- Glasgow Haskell Compiler (GHC) 8.10+
- Haskell Platform o Stack

## Installazione degli Strumenti di Sviluppo

### Visual Studio

1. Scaricare Visual Studio 2019 o 2022 da [visualstudio.microsoft.com](https://visualstudio.microsoft.com/it/downloads/)
2. Durante l'installazione, selezionare i seguenti componenti:
   - Sviluppo di applicazioni desktop C++
   - Supporto per CMake
   - Windows 10 SDK

### CMake

1. Scaricare CMake da [cmake.org/download](https://cmake.org/download/)
2. Durante l'installazione, selezionare l'opzione "Add CMake to the system PATH"

### Rust

1. Visitare [rustup.rs](https://rustup.rs/)
2. Scaricare e eseguire `rustup-init.exe`
3. Seguire le istruzioni a schermo per un'installazione standard
4. Dopo l'installazione, verificare con `cargo --version` da un nuovo prompt dei comandi

### Python

1. Scaricare Python da [python.org](https://www.python.org/downloads/windows/)

2. Durante l'installazione, abilitare l'opzione "Add Python to PATH"

3. Dopo l'installazione, aprire un nuovo prompt dei comandi e installare le dipendenze:

   ```
   pip install numpy sympy
   ```

### Haskell

1. Scaricare Haskell Platform da [haskell.org/platform](https://www.haskell.org/platform/windows.html)
2. Seguire le istruzioni di installazione standard
3. In alternativa, è possibile utilizzare [ghcup](https://www.haskell.org/ghcup/) per una gestione più flessibile
4. Verificare l'installazione con `ghc --version`

## Compilazione della versione C

La versione C utilizza solo tipi interi standard della libreria standard C e non richiede librerie esterne. Può essere compilata facilmente con qualsiasi compilatore C moderno:

### Con Visual Studio

```
cl /W4 /O2 /Fo"build\\" /Fe"bin\frobenius.exe" Frobenius.c
```

### Con Clang

```
clang -Wall -Wextra -O2 -o bin/frobenius Frobenius.c
```

## Compilazione

### Metodo 1: Visual Studio (consigliato)

1. Eseguire `Generate_VS_Solution.bat` per creare una soluzione Visual Studio
2. Aprire `build\Frobenius.sln` in Visual Studio
3. Compilare la soluzione normalmente

### Metodo 2: Compilazione diretta

Utilizzare lo script batch per compilare direttamente:

```
build_vs.bat
```

Questo script deve essere eseguito da una "Developer Command Prompt for VS", accessibile dal menu Start di Windows.

### Metodo alternativo: Clang

Per compilare utilizzando Clang/LLVM:

```
build_clang.bat
```

Assicurarsi che Clang sia installato e presente nel PATH.

### Compilazione Rust

Per la versione Rust, utilizzare Cargo:

```
cargo build --release
```

Gli eseguibili generati vengono copiati automaticamente nella cartella `bin`.

### Compilazione Haskell

Per la versione Haskell:

```
ghc -O2 -o bin\frobenius_hs.exe Frobenius.hs
```

## Esecuzione

```
bin\frobenius.exe    # Versione C
bin\frobenius_num.exe    # Versione Rust
python Frobenius_solver.py    # Versione Python
bin\frobenius_hs.exe     # Versione Haskell
```

## Esempi di Utilizzo

### Versione C

```
bin\frobenius.exe
```

Esegue alcuni test predefiniti, mostrando il risultato per casi come [3,5], [3,5,7] e il problema McNugget [6,9,20].

### Versione Rust

```
bin\frobenius_num.exe --verbose 6 9 20
```

Calcola il numero di Frobenius per le monete di valore 6, 9 e 20 (risultato: 43) con output dettagliato.

### Versione Python

```
python Frobenius_solver.py -v
```

Esegue il solver Python in modalità verbose, mostrando esempi predefiniti.

### Versione Haskell

```
bin\frobenius_hs.exe
```

Esegue la versione Haskell con esempi e test integrati.

## Note sulle Prestazioni

Le implementazioni offrono diverse caratteristiche di prestazione:

- La versione **C**, puramente didattica, offre il miglior equilibrio tra semplicità, velocità e flessibilità.
- La versione **Python** è la più leggibile e intuitiva, con supporto per approcci sia simbolici che numerici.
- La versione **Rust** è la più veloce per problemi di grandi dimensioni, grazie alla parallelizzazione.
- La versione **Haskell** offre un'elegante e completa implementazione funzionale, mantenendo ottime prestazioni.

## Algoritmi Implementati

Tutte le versioni implementano diversi algoritmi:

1. **Formula chiusa** per n=2: `g(a,b) = a*b - a - b`
2. **Round-Robin** basato su classi di resto
3. **Programmazione Dinamica** con crivello ottimizzato
4. **Approccio simbolico** basato sulla teoria dei semigruppi numerici

Il solver seleziona automaticamente l'algoritmo più efficiente in base alle caratteristiche dell'input.

## Dettagli Implementativi

La complessità algoritmica e la struttura del codice sono documentate nei rispettivi file sorgente. Ciascun file contiene un'analisi dettagliata della complessità del codice, incluse metriche come:

- Function Points
- Complessità Ciclomatica (McCabe)
- Metriche di Halstead
- Analisi di manutenibilità

## Soluzione di Problemi

### Errori di compilazione C

- Assicurarsi di utilizzare la "Developer Command Prompt for VS" per la compilazione manuale

### Errori di compilazione Rust

- Aggiornare Rust con `rustup update`
- Verificare le dipendenze con `cargo check`
- Per problemi di threading, provare a compilare con `cargo build --release --no-default-features`

### Errori Python

- Verificare l'installazione delle dipendenze con `pip list | findstr numpy` e `pip list | findstr sympy`
- Per problemi di compatibilità, aggiornare le librerie con `pip install --upgrade numpy sympy`

### Errori Haskell

- Verificare l'installazione con `ghc --version`
- Per problemi di dipendenze, utilizzare `cabal install --lib containers` per installare le librerie necessarie

## Licenza

Questo software è disponibile con licenza MIT.

## Autore

M.A.W. 1968