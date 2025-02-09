# 🔥 Implementazione in C dell'Algoritmo di Gusfield-Irving

Questa cartella contiene la mia originale **implementazione in C** dell'algoritmo di **Gusfield e Irving** (Dan Gusfield and Robert W. Irving, 1989, "The stable marriage problem: structure and algorithms.", MIT Press, ISBN 978-0262071185) per la generazione esaustiva delle soluzioni ai problema dei **matrimoni stabili**.

## 📖 Descrizione dell'Algoritmo

L'algoritmo di **Gusfield e Irving** estende il classico algoritmo di Gale-Shapley per il **Stable Marriage Problem (SMP)**, fornendo un metodo esaustivo per la generazione di tutte le soluzioni stabili.

Le istanze del problema Stable Marriage di cui parliamo sono dell'ordine di grandezza di \(10^3\)–\(10^5\) proponenti (cliniche, college, posti pubblici in generale a livello di nazioni o grandi regioni). Sono questi i numeri che hanno motivato le ricerche di Gusfield, Irving e loro collaboratori negli anni Ottanta. Per questo motivo, le prestazioni dell'algoritmo in spazio e in tempo sono fondamentali, ed è essenziale comprendere almeno a grandi linee la struttura algebrica dello spazio delle soluzioni e le idee alla base dell'algoritmo.

## 🛠️ Implementazione

La mia implementazione si basa scrupolosamente sulla monografia e segue rigorosamente la trattazione teorica dell'algoritmo, con particolare attenzione a:

- **Strutture dati efficienti**, per rappresentare liste di preferenze e insiemi di coppie stabili.
- **Gestione ottimizzata della memoria**, evitando ridondanze e sprechi.
- **Codice altamente leggibile**, con commenti chiari che spiegano ogni passaggio critico.
- **Output strutturato**, che consente di analizzare rapidamente i risultati ottenuti.

L'algoritmo che qui analizziamo è in grado di generare tutte le soluzioni stabili dell'insieme \(TSM\) in \(O(n^{2}+n\cdot\left|TSM\right|)\).

## 📂 Struttura del Codice

- **marriage.c** – Implementazione principale dell'algoritmo di Gusfield-Irving.
- **marriage.h** – File di intestazione con dichiarazioni delle funzioni e strutture dati.

## ✅ Validazione Accademica

L'implementazione è stata sottoposta anche agli autori originali, e in particolare il professor **Robert W. Irving** ha dato il suo **autorevolissimo imprimatur**, oltre a rilasciare una **lunga intervista** riportata nell'articolo associato.

## ⚡ Performance e Ottimizzazioni

Questa implementazione ha principalmente lo scopo di offrire la massima **leggibilità e didatticità** del codice. Si basa su una piattaforma coerente di decisioni progettuali e ingegneristiche tipiche di un *proof of concept*, ed è stata volutamente ottimizzata solo in minima parte, come peraltro ampiamente spiegato nell'articolo, che rimane il primo e più importante riferimento documentale per il codice qui presentato.

## 📖 Riferimenti

La monografia a cui facciamo riferimento è totalmente incentrata sugli aspetti algoritmici e sulla struttura algebrica dello spazio delle soluzioni: gli autori, come d'abitudine in simili testi teorici, non forniscono neppure un minimale esempio di codice in alcun linguaggio, né nel testo né in un sito web di riferimento. Anche i riferimenti alle strutture dati da utilizzare concretamente in una implementazione rimangono solo **cenni impliciti**, affidati all'intuizione e all'esperienza del lettore.

Il risultato di numerose ricerche conferma che **non sembra esservi pubblicamente reperibile sul web alcuna implementazione di riferimento o di studio** dell'algoritmo di Gusfield e Irving. In una recente comunicazione privata, il professor **David Manlove** ([Università di Glasgow](https://www.dcs.gla.ac.uk/~davidm/)), peraltro autore di *Matching under Preferences*, ha reso noto che un suo ex studente di dottorato, **Augustine Kwanashie**, ha implementato l'algoritmo esaustivo nella sua [tesi di dottorato](https://theses.gla.ac.uk/6706/1/2015kwanashiephd.pdf), sebbene i sorgenti non siano stati resi pubblici. Tale lavoro è poi confluito in un framework accademico della stessa università, un toolkit web che include oltre 40 algoritmi di accoppiamento con preferenze: [MATWA](https://matwa.optimalmatching.com/) (vedi la sezione *Hospitals Residents*).

Esempi di codice per l'algoritmo classico di **Gale-Shapley** abbondano invece sul web: a maggior ragione, essendo il **Gusfield-Irving** un algoritmo sofisticato e articolato, l'esperienza didattica conferma che un semplice esempio di codice aumenta notevolmente la comprensione dell'intera dinamica. Il codice qui presentato permette di colmare questa lacuna fornendo un'implementazione dettagliata e commentata, che compendia le spiegazioni teoriche della monografia, riprese ampliate e sempificate nell'articolo.

Riferimenti fondamentali (per la bibliografia completa, si consulti l'articolo):

- D. Gusfield, R. W. Irving, *The Stable Marriage Problem: Structure and Algorithms*, MIT Press, 1989.
- R. W. Irving, "Stable marriage and indifference", *Discrete Applied Mathematics*, 1985.

## 📜 Licenza

🆓 **Questa implementazione è rilasciata sotto licenza MIT.**

🚀 **Buono studio e buon coding!**


