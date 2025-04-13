# Manuale d'uso delle Implementazioni del Problema di Frobenius

Questo documento fornisce una guida dettagliata all'utilizzo delle diverse implementazioni del Problema di Frobenius presenti in questo repository.

## Sommario

1. [Introduzione al Problema di Frobenius](#introduzione-al-problema-di-frobenius)
2. [Implementazione C](#implementazione-c)
3. [Implementazione Rust](#implementazione-rust)
4. [Implementazione Python](#implementazione-python)
5. [Implementazione Haskell](#implementazione-haskell)
6. [Strumenti di Benchmarking](#strumenti-di-benchmarking)
7. [Risoluzione dei Problemi Comuni](#risoluzione-dei-problemi-comuni)
8. [Esempi Avanzati](#esempi-avanzati)

## Introduzione al Problema di Frobenius

Il problema di Frobenius consiste nel determinare il più grande intero che **non può** essere espresso come combinazione lineare non-negativa di un dato insieme di interi positivi.

In altre parole, date le monete di valori `a₁, a₂, ..., aₙ` (dove il MCD è 1), il "numero di Frobenius" è il più grande importo che non può essere pagato utilizzando queste monete.

### Esempio Classico

Con monete di valore 3 e 5, possiamo formare:

- 3 = 1×3
- 5 = 1×5
- 6 = 2×3
- 8 = 1×3 + 1×5
- 9 = 3×3
- ...

Ma i numeri 1, 2, 4 e 7 non possono essere formati. Quindi, 7 è il numero di Frobenius.

## Implementazione C

L'implementazione C offre un'interfaccia minima ma efficiente per il calcolo del numero di Frobenius.

### Compilazione

```bash
# Con il compilatore GCC
gcc -Wall -Wextra -O2 -o bin/frobenius Frobenius.c

# Con Visual Studio
cl /nologo /W4 /O2 /Fo"build\\" /Fe"bin\frobenius.exe" Frobenius.c
```

### Utilizzo di Base

L'implementazione base non accetta parametri ed esegue una serie di test predefiniti:

```bash
bin/frobenius
```

### Utilizzo Avanzato (Frobenius_extended.c)

La versione estesa offre un'interfaccia a riga di comando più completa:

```bash
# Calcolo per monete specifiche
bin/frobenius_extended 3 5

# Modalità verbose
bin/frobenius_extended -v 3 5 7

# Eseguire test predefiniti
bin/frobenius_extended -t

# Eseguire benchmark
bin/frobenius_extended -b

# Benchmark con monete specifiche
bin/frobenius_extended -b 6 9 20
```

#### Opzioni Disponibili

- `-h, --help`: Mostra il messaggio di aiuto
- `-v, --verbose`: Mostra informazioni dettagliate durante l'esecuzione
- `-t, --test`: Esegue test predefiniti
- `-b, --benchmark`: Esegue benchmark

#### Limitazioni

- L'implementazione C è limitata a interi a 32 bit, quindi non può gestire monete o risultati estremamente grandi
- La complessità computazionale aumenta significativamente con il numero di monete

## Implementazione Rust

L'implementazione Rust offre maggiore flessibilità e supporto per numeri di grandi dimensioni grazie a `num-bigint`.

### Compilazione

```bash
# Usando Cargo (consigliato)
cargo build --release

# Copia dell'eseguibile
cp target/release/frobenius_num bin/
```

### Utilizzo

```bash
# Calcolo base
bin/frobenius_num 3 5

# Output dettagliato
bin/frobenius_num --verbose 6 9 20

# Utilizzo di multithreading (fino a 8 thread)
bin/frobenius_num --threads=4 31 41 59 26 53

# Benchmark degli algoritmi
bin/frobenius_num --benchmark 11 13 15 17 19
```

#### Opzioni Disponibili

- `-v, --verbose`: Abilita output dettagliato
- `-b, --benchmark`: Esegue benchmark di confronto tra i vari algoritmi
- `--threads=N`: Imposta il numero di thread (1-8, default: auto)
- `-h, --help`: Mostra il messaggio di aiuto

#### Vantaggi

- Supporto per numeri di dimensione arbitraria
- Parallelizzazione automatica per problemi grandi
- Selezione automatica dell'algoritmo migliore
- Prestazioni eccellenti grazie a ottimizzazioni a basso livello

## Implementazione Python

L'implementazione Python utilizza NumPy e SymPy per approcci sia numerici che simbolici.

### Requisiti

```bash
pip install numpy sympy
```

### Utilizzo

```bash
# Esecuzione standard
python Frobenius_solver.py

# Modalità verbose
python Frobenius_solver.py -v

# Esecuzione dei test
python Frobenius_solver.py -t

# Benchmark
python Frobenius_solver.py -b

# Disabilitare il calcolo parallelo
python Frobenius_solver.py --no-parallel
```

#### Opzioni Disponibili

- `-v, --verbose`: Abilita output dettagliato
- `-b, --benchmark`: Esegue benchmark di confronto
- `-t, --test`: Esegue test di verifica
- `-p, --parallel`: Abilita calcolo parallelo (default: attivo)
- `--no-parallel`: Disabilita calcolo parallelo

#### Vantaggi

- Grande flessibilità grazie agli approcci simbolici e numerici
- Facile da estendere e modificare
- Ideale per prototipazione e sperimentazione

## Implementazione Haskell

L'implementazione Haskell offre un'elegante soluzione funzionale con ottimizzazioni avanzate.

### Compilazione

```bash
ghc -O2 -o bin/frobenius_hs Frobenius.hs
```

### Utilizzo

```bash
bin/frobenius_hs
```

L'implementazione attuale esegue una serie di test e benchmark predefiniti.

#### Vantaggi

- Eleganza dell'approccio funzionale
- Memoizzazione avanzata per ottime prestazioni
- Algoritmi specifici per casi particolari

## Strumenti di Benchmarking

Il repository include strumenti di benchmarking avanzati per confrontare le diverse implementazioni.

### Benchmark Semplice

```bash
run_benchmarks.bat
```

### Benchmark Avanzato con Visualizzazione

```bash
advanced_benchmark.bat
```

Questo script esegue benchmark approfonditi e genera un rapporto HTML interattivo con grafici comparativi.

## Risoluzione dei Problemi Comuni

### Errore "Le monete devono essere coprime (MCD=1)"

Il problema di Frobenius richiede che le monete siano coprime (cioè, il loro massimo comun divisore deve essere 1). Se ricevi questo errore, modifica i valori delle monete in modo che siano coprimi.

### Performance Lente

- Per problemi con molte monete, l'implementazione Rust con multithreading è la più veloce
- Per problemi con poche monete ma valori grandi, l'implementazione C può essere più efficiente
- Considera di ridurre il numero di monete attraverso ottimizzazioni matematiche

### Limiti di Memoria

- L'implementazione C ha un consumo di memoria ridotto ma è limitata nei valori gestibili
- L'implementazione Rust ha un ottimo bilanciamento tra uso di memoria e dimensione dei problemi gestibili
- L'implementazione Python può richiedere molta memoria per problemi grandi

## Esempi Avanzati

### Utilizzo della Libreria in Altri Progetti

#### C

```c
#include "Frobenius.h"

int main() {
    FrobUint32_t coins[] = {3, 5, 7};
    FrobInt32_t result = compute_frobenius(coins, 3);
    printf("Risultato: %d\n", result);
    return 0;
}
```

#### Rust

```rust
use frobenius_num::FrobeniusSolver;

fn main() {
    let solver = FrobeniusSolver::new(false);
    let coins = vec![3, 5, 7];
    match solver.solve(coins) {
        Ok(result) => println!("Risultato: {}", result),
        Err(e) => println!("Errore: {}", e),
    }
}
```

#### Python

```python
from Frobenius_solver import FrobeniusSolver

solver = FrobeniusSolver(verbose=False)
result = solver.solve([3, 5, 7])
print(f"Risultato: {result}")
```

#### Haskell

```haskell
import Frobenius

main = do
    let result = frobenius [3, 5, 7]
    putStrLn $ "Risultato: " ++ show result
```

### Risoluzione del Problema McNugget

Il "Problema McNugget" è un esempio classico del problema di Frobenius: se i McNuggets sono disponibili in confezioni da 6, 9 e 20 pezzi, qual è il più grande numero di McNuggets che non può essere acquistato esattamente?

```bash
# Con C
bin/frobenius_extended 6 9 20

# Con Rust
bin/frobenius_num 6 9 20

# Con Python
python Frobenius_solver.py -v
# (Esempio predefinito nel programma)
```

Il risultato è 43: è impossibile acquistare esattamente 43 McNuggets, ma tutti i numeri maggiori di 43 sono possibili.

### Analisi di Prestazioni per Monete Prime

Le monete con valori primi presentano spesso casi interessanti per il problema di Frobenius:

```bash
bin/frobenius_num --benchmark 101 103 107 109
```

Questo esempio confronta diversi algoritmi su un set di monete prime relativamente grandi, mostrando i vantaggi dell'approccio multithreading nell'implementazione Rust.