# Collaudo SBM-3 C11

Data: 26 settembre 2026. Piattaforma: Linux x86_64, GCC 13.3.0,
AMD EPYC 9V74. Tutte le scansioni riportate usano un solo thread.
I tempi sono misure di questa macchina, non previsioni per altri sistemi.

## Scansioni complete eseguite

| n | Candidati visitati | Basi valide | Matrici f(n) | Basi semplici | Matrici con righe distinte |
|---:|---:|---:|---:|---:|---:|
| 3 | 1 | 1 | 1 | 0 | 0 |
| 4 | 35 | 1 | 24 | 1 | 24 |
| 5 | 2 002 | 22 | 2 040 | 12 | 1 440 |
| 6 | 177 100 | 550 | 297 200 | 330 | 237 600 |
| 7 | 22 481 940 | 16 700 | 68 938 800 | 11 205 | 56 473 200 |
| 8 | 3 872 894 697 | 703 297 | 24 046 189 440 | 505 505 | 20 381 961 600 |

Per n=3,…,7 sono state eseguite entrambe le versioni; per n=8 la sola
versione ottimizzata. I risultati f(n) coincidono con la formula di Stanley
valutata in aritmetica razionale esatta. A n=8 sono occorsi **12,564803
secondi CPU**; il resoconto integrale è in `verification_n8.txt`.

Non sono state eseguite scansioni complete a n=9, 10 o 11. Per questi ordini
sono stati invece controllati catalogo, validatori su campioni deterministici,
basi cicliche valide e transizione terminale dell'odometro. I test artificiali
dello stato non sono presentati come una scansione esaustiva.

## Controlli indipendenti e casi di confine

- Confronto candidato per candidato dei due validatori per n=3,…,6;
  confronto dei conteggi completi anche per n=7.
- Accordo fra Stanley esatto e una DP sugli istogrammi dei gradi delle
  colonne per n=0,…,12. La DP costruisce righe ordinate e non usa né la
  formula di Stanley né i pesi orbitali. Confermata anche la soglia
  `f(11) <= UINT64_MAX < f(12)`.
- Per n=3,…,5, uguaglianza degli insiemi di matrici emesse e di quelli
  ottenuti con il prodotto cartesiano di tutte le righe ammesse; assenza di
  duplicati e corrispondenza con la somma dei pesi delle basi.
- Per n=3,…,6, uguaglianza delle basi semplici con i risultati di una
  generazione indipendente mediante combinazioni ordinarie di blocchi.
- Per ogni n=3,…,11, 500 indici canonici pseudocasuali, con seme fisso e
  fino a 64 successori ciascuno: confronto fra cache e ricalcolo diretto.
- Basi cicliche valide fino a n=11; controllo del peso massimo 11!.
- Pause mediante budget e callback, ripresa senza perdita o duplicazione,
  copie indipendenti dello stato, ultimo candidato contato una sola volta.
- Guardie di overflow dei totali verificate mediante fault injection:
  nessun accumulo parziale e nessuna notifica del candidato respinto.
- Input malformati, interi decimali eccedenti `uint64_t`, n fuori dominio,
  budget di un candidato e prefissi a n=11.

I controlli C usano `CHECK` e quelli Python un controllo esplicito con
eccezione; non dipendono da `assert` eliminabili in modalità ottimizzata.

## Compilatore, sanitizzatori e analisi statica

Compilazione senza diagnostiche, sia dell'eseguibile sia dei test, con:

```sh
cc -std=c11 -O3 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
   -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wvla -Werror \
   sbm3.c sbm3_cli.c -o sbm3
cc -std=c11 -O3 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
   -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wvla -Werror \
   -DSBM3_TEST_HOOKS sbm3.c sbm3_test.c -o sbm3_test
./sbm3_test
python3 verify.py ./sbm3
```

AddressSanitizer e UndefinedBehaviorSanitizer: suite C superata anche con
`NDEBUG` definito.

```sh
cc -std=c11 -O1 -g -DNDEBUG -DSBM3_TEST_HOOKS \
   -fsanitize=address,undefined -fno-omit-frame-pointer \
   sbm3.c sbm3_test.c -o sbm3_test_sanitize
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ./sbm3_test_sanitize
```

Il rilevamento delle perdite è disattivato perché LeakSanitizer non può
ispezionare i processi in questo ambiente; ASan e UBSan sono rimasti attivi.
Il nucleo non alloca memoria dinamicamente. Anche l'analisi statica GCC
`-fanalyzer` su nucleo e driver è terminata senza diagnostiche.
I simboli di accesso ai dettagli interni dei test esistono soltanto nelle
compilazioni con `SBM3_TEST_HOOKS`.

## Misura dell'ottimizzazione

Cinque scansioni complete per ciascun validatore, n=7, senza emissione:

| Validatore | Secondi CPU delle cinque prove | Mediana |
|---|---|---:|
| Cache dei prefissi e piani di bit | 0,076849; 0,075134; 0,075014; 0,076549; 0,074914 | 0,075134 s |
| Vettore dei gradi ricalcolato | 0,500866; 0,526529; 0,502585; 0,488129; 0,498047 | 0,500866 s |

Rapporto fra le mediane: **6,6663**. Entrambi i validatori hanno visitato
gli stessi 22 481 940 candidati. Il confronto misura la riduzione del
lavoro per candidato: non è ottenuto saltando combinazioni.
Le misure originali sono conservate in `benchmark.json`.

Per ripetere il confronto sulla propria macchina:

```sh
python3 benchmark.py ./sbm3 --n 7 --runs 5 > benchmark-locale.json
```

## Portabilità e portata dei risultati

È fornito un driver di compilazione MSVC, ma Visual Studio e i compilatori
Borland/Embarcadero non sono stati eseguiti nell'ambiente di collaudo.
Non si dichiara una certificazione MISRA né una prova meccanizzata del
raffinamento Z→C. I test forniscono evidenza sull'implementazione; le ragioni
matematiche del peso e del dominio enumerato rimangono quelle dell'articolo.
