# Problema di Frobenius - Implementazioni Multiple

Questo repository contiene molteplici implementazioni del **Problema di Frobenius** (noto anche come il "problema del resto" o "problema delle monete") in diversi linguaggi di programmazione, ottimizzate per Windows e Visual Studio.

## Il Problema di Frobenius

Il problema di Frobenius consiste nel determinare il più grande intero che **non può** essere espresso come combinazione lineare non-negativa di un dato insieme di interi positivi.

Date `n` monete con valori `a₁, a₂, ..., aₙ` (dove il MCD è 1), il "numero di Frobenius" è il più grande importo che non può essere pagato utilizzando quelle monete.

Un esempio famoso è il "Problema McNugget": se le confezioni di McNuggets sono disponibili in formati da 6, 9 e 20 pezzi, qual è il più grande numero di McNuggets che non può essere acquistato esattamente? (La risposta è 43).

## Implementazioni

Il repository contiene le seguenti implementazioni:

1. **C con LibTomMath** (`Frobenius_ltm.c`): Implementazione in C utilizzando la libreria LibTomMath per la gestione di numeri di precisione arbitraria.
2. **Rust** (`Frobenius_num.rs`): Implementazione in Rust con supporto multithreading e utilizzo di `num-bigint` per numeri di grandi dimensioni.
3. **Python** (`Frobenius_solver.py`): Implementazione in Python utilizzando NumPy e SymPy per approcci sia numerici che simbolici.
4. **LaTeX** (`Frobenius.tex`): Documento LaTeX che descrive il problema, la sua storia e l'analisi degli algoritmi.

## Requisiti di Sistema

Questo progetto è ottimizzato per l'ambiente Windows con Visual Studio:

### Requisiti generali

- Windows 10/11
- Visual Studio 2019/2022
- CMake 3.10 o superiore

### Per la versione C

- LibTomMath (vedere le istruzioni di installazione più avanti)

### Per la versione Rust

- Rust e Cargo (installabili da [rustup.rs](https://rustup.rs/))

### Per la versione Python

- Python 3.7+
- NumPy e SymPy (`pip install numpy sympy`)

## Configurazione di LibTomMath

Per compilare la versione C, è necessario configurare LibTomMath:

1. Scaricare LibTomMath da [GitHub](https://github.com/libtom/libtommath)
2. Compilare la libreria con Visual Studio
3. Creare le seguenti cartelle nella radice del progetto:
   - `include` - Mettere qui i file header `.h` di LibTomMath
   - `lib` - Mettere qui `tommath.lib` compilata

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

### Compilazione Rust

Per la versione Rust, utilizzare Cargo:

```
cargo build --release
```

Gli eseguibili generati vengono copiati automaticamente nella cartella `bin`.

## Esecuzione

```
bin\frobenius_ltm.exe    # Versione C
bin\frobenius_num.exe    # Versione Rust
python Frobenius_solver.py    # Versione Python
```

## Note sulle Prestazioni

Le implementazioni offrono diverse caratteristiche di prestazione:

- La versione **C** offre il miglior equilibrio tra velocità e flessibilità
- La versione **Rust** è la più veloce per problemi di grandi dimensioni grazie alla parallelizzazione
- La versione **Python** è la più flessibile, con supporto per approcci sia simbolici che numerici

## Dettagli Implementativi

La complessità algoritmica e la struttura del codice sono documentate nei rispettivi file sorgente. Ciascun file contiene un'analisi dettagliata della complessità del codice, incluse metriche come:

- Function Points
- Complessità Ciclomatica (McCabe)
- Metriche di Halstead
- Analisi di manutenibilità

## Licenza

Questo software è disponibile con licenza MIT.

## Autore

M.A.W. 1968