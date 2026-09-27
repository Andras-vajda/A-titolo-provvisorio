# SBM-3: enumeratore C11

Generazione dei quadrati base e, su richiesta, di tutte le matrici quadrate
0–1 con tre uni in ogni riga e in ogni colonna. Il limite è **hardcoded:
3 ≤ n ≤ 11**. Le colonne rimangono etichettate; il quoziente riguarda
esclusivamente le permutazioni delle righe.

Il programma visita tutti i multinsiemi candidati, senza pruning. La versione
ottimizzata riutilizza i controlli dei prefissi già esaminati; una seconda
versione, selezionabile con `--reference`, ricalcola il vettore dei gradi.
Entrambe percorrono lo stesso odometro, un candidato alla volta.

## Compilazione e collaudo

### Visual Studio / Windows

Dal **Developer Command Prompt x64**, nella cartella dei sorgenti:

```bat
build_msvc.cmd
python verify.py sbm3.exe
sbm3.exe 6
```

Lo script usa `/std:c11 /TC /W4 /O2`, produce `sbm3.exe` e
`sbm3_test.exe`, quindi esegue i test C. Microsoft documenta il supporto
C11 da Visual Studio 2019 16.8, con Windows SDK e UCRT aggiornati:
[requisiti ufficiali](https://learn.microsoft.com/en-us/cpp/overview/install-c17-support?view=msvc-170).
I test Python richiedono Python 3.9 o successivo, senza pacchetti esterni.

Lo script MSVC è fornito, ma il collaudo qui registrato è stato eseguito
con GCC su Linux. La compatibilità con una specifica versione
Borland/Embarcadero rimane da verificare sul relativo compilatore: non si
presuppone che un compilatore C++11 implementi anche C11.

### GCC / Linux

```sh
make check
./sbm3 6
python3 verify.py ./sbm3 --slow
```

L'ultima istruzione aggiunge la scansione completa a n=8; non è richiesta
dal collaudo ordinario. In alternativa al Makefile:

```sh
cc -std=c11 -O3 -Wall -Wextra -Wpedantic sbm3.c sbm3_cli.c -o sbm3
```

## Uso

```text
sbm3 [n] [--reference] [--limit N]
     [--emit-bases | --emit-matrices] [--simple]
```

Senza argomenti viene contato il caso n=6. I risultati sono interi esatti;
soltanto la misura del tempo usa il floating point.

| Comando | Effetto |
|---|---|
| `sbm3 5` | Conta basi, matrici e sottoclasse senza ripetizioni. |
| `sbm3 7 --reference` | Usa il controllo di riferimento dei margini. |
| `sbm3 5 --emit-bases` | Emette ciascuna base valida, con il relativo peso. |
| `sbm3 5 --emit-matrices` | Emette tutte le 2040 matrici ordinate, una sola volta ciascuna. |
| `sbm3 5 --emit-matrices --simple` | Emette le 1440 matrici senza righe ripetute. |
| `sbm3 11 --limit 1000000` | Esamina soltanto il primo milione di candidati, dichiarando `PARTIAL`. |

Su Unix anteporre `./`; su Windows si può usare il suffisso `.exe`.
`--limit 0` significa scansione completa. Il budget conta i **candidati**,
non le basi valide o le matrici emesse. Anche un prefisso può non contenere
alcuna base valida. Una scansione parziale riuscita termina con codice zero;
la sua incompletezza è dichiarata dal campo `status=PARTIAL` e dal messaggio
finale, e non va interpretata come una valutazione di f(n).

Con emissione, i dati vanno su `stdout` e il riepilogo su `stderr`, così da
poter usare, per esempio, `sbm3 5 --emit-matrices > matrici5.txt`.
Il formato è una riga per risultato:

```text
B peso riga1 ... rigan
M riga1 ... rigan
```

Ogni riga della matrice è una stringa di n bit, con la colonna 1 a sinistra.
Per una base, i pattern sono nell'ordine canonico del catalogo. Per espandere
l'orbita, il driver ordina le maschere e ne genera le permutazioni distinte
in ordine lessicografico; questo ordine interno non cambia le etichette
delle colonne. `--simple` filtra l'emissione e richiede uno dei due comandi
di emissione: non cambia l'odometro né il numero dei candidati visitati.
I contatori delle basi semplici e delle relative matrici sono sempre calcolati.

## Struttura del codice e contratto

| File | Funzione |
|---|---|
| `sbm3.h`, `sbm3.c` | Stato, catalogo, odometro, validatori e conteggio esatto. |
| `sbm3_cli.c` | Argomenti, emissione delle basi o delle matrici, riepilogo. |
| `sbm3_test.c` | Test C, inclusi cache, pause e guardie di overflow. |
| `verify.py` | Stanley esatto, DP indipendente, confronto delle matrici e dei design. |
| `benchmark.py` | Confronto riproducibile dei due validatori. |
| `COLLAUDO.md` | Risultati effettivi e limiti della verifica. |
| `benchmark.json`, `verification_n8.txt` | Misure e risultato completo a n=8. |

Il nucleo non usa heap, VLA, ricorsione, intrinseci o variabili globali
mutabili. Il chiamante possiede uno `sbm3_state` di dimensione fissa.
Il catalogo contiene al massimo 165 maschere `uint32_t`; contatori e pesi
sono `uint64_t`. `_Static_assert` rende esplicite le dipendenze dal limite 11.
Queste scelte seguono l'impostazione prudente del progetto C11; non costituiscono
una certificazione di conformità MISRA.

`sbm3_init` restituisce `SBM3_PAUSED` quando lo stato è pronto.
`sbm3_scan(&state, budget, visitor, context)` visita al massimo `budget`
candidati; zero significa tutti i rimanenti. `visitor=NULL` abilita il solo
conteggio. Il callback riceve una base valida e il suo peso; il puntatore è
valido soltanto durante la chiamata. Può copiarne il contenuto e restituire
`false` per fermarsi dopo quel candidato, senza modificarne l'enumeratore.
Il risultato è `SBM3_PAUSED`, oppure `SBM3_DONE` se era l'ultimo candidato.
Una nuova chiamata a `sbm3_scan` riparte dal candidato successivo.

Tra chiamate, l'indice canonico è il primo candidato non ancora visitato,
oppure l'ultimo quando `finished` è vero; i contatori rappresentano il
prefisso già esaminato. Una copia della struttura produce una scansione
indipendente. Il driver non salva checkpoint su disco.
Su overflow, la guardia precede ogni accumulo e notifica del candidato
corrente. Le guardie e i controlli dei test restano attivi con `NDEBUG`.
Il codice non costituisce un raffinamento verificato meccanicamente della
specifica Z: il contratto ne conserva l'invariante osservabile e i test
verificano l'implementazione.

## Perché l'ottimizzazione preserva il controllo

Per ciascuna colonna j, il grado parziale è memorizzato in due piani di bit:

```text
grado(j) = bit_j(low) + 2 * bit_j(high)
```

Una maschera di riga incrementa simultaneamente tre gradi. Se
`low & high & mask` è non nullo, almeno un grado passerebbe da 3 a 4 e il
candidato è invalido. Altrimenti le istruzioni

```c
high ^= low & mask;
low ^= mask;
```

realizzano esattamente le transizioni binarie 0→1, 1→2 e 2→3, lasciando
inalterate le colonne non selezionate. L'ordine è essenziale: il riporto usa
il vecchio `low`. Dopo n righe, `low & high == all_columns` esprime che
tutti i gradi valgono tre.

L'odometro modifica un suffisso: `dirty` indica la sua prima posizione,
indicizzata da zero. I gradi dei prefissi di lunghezza al più `dirty`
rimangono corretti e possono essere riutilizzati. Se il precedente controllo
aveva già incontrato un grado quattro prima della posizione modificata,
quel medesimo prefisso invalida anche il candidato attuale. Negli altri
casi vengono riesaminate le righe a partire da `dirty`.

In entrambi i casi si incrementa `visited` di uno e si esegue un solo
successore dell'odometro: nessun intervallo di candidati viene saltato.
La cache riduce il lavoro del predicato, non il dominio enumerato.
Ogni base valida pesa n! diviso il prodotto dei fattoriali delle lunghezze
dei suoi run. Tale prodotto divide n!; i fattoriali tabulati arrivano a 11!.

## Il caso dei design semplici

Quando ogni molteplicità è 0 o 1, il denominatore è 1 e il peso è **n!**.
Le basi sono i design semplici **1-(n,3,3)** sui punti etichettati [n]:
ogni blocco ha tre punti e ogni punto appartiene a tre blocchi. La somma
delle incidenze impone esattamente n blocchi. I blocchi non sono ordinati;
le matrici emesse assegnano loro tutti i possibili ordini di riga.
Non si richiede una molteplicità costante per le coppie di punti.

Per n=5 si ottengono 12 basi semplici e 12·5! = 1440 matrici;
il conteggio totale, ammettendo ripetizioni, è invece 22 basi e 2040 matrici.

## Limite aritmetico e costo combinatorio

I contatori esatti sono sufficienti perché f(11) = 7 673 688 777 463 632 000
è minore di `UINT64_MAX`, mentre f(12) lo supera. Anche il numero dei
candidati N(n) = binom(binom(n,3)+n−1,n) entra in `uint64_t`:

```text
N(11) = 85 695 033 024 571 425
11 · N(11) = 942 645 363 270 285 675 < UINT64_MAX
```

L'ultima disuguaglianza copre anche il massimo prodotto intermedio nella
ricorrenza usata per calcolare N(n). Il limite 11 garantisce la capacità
aritmetica, non tempi pratici per la scansione esaustiva. Non è stata
eseguita una scansione completa per n=9, 10 o 11. Il caso massimo realmente
percorso in questa consegna è n=8, con 3 872 894 697 candidati.

## Licenza

Codice distribuito con licenza MIT, in `LICENSE.txt`.
Copyright (c) 1990–2026 M.A.W. 1968.
