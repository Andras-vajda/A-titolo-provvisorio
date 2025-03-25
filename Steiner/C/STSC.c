/************************************************************************/
/* Libreria per la generazione di terne differenza con le formule
** parametriche di Rose Peltesohn per i problemi di Lothar Heffter. 
**
** Riferimenti:
** Rose Peltesohn, 1939.
** "Eine Loesung der beiden Heffterschen Differenzenprobleme".
** Compositio Mathematica, Vol. 6, pp. 251–257.
** URL: http://www.numdam.org/item/CM_1939__6__251_0/
**
** Lothar Heffter, 1899.
** "Uber Tripelsysteme".
** Mathematische Annalen, Vol. 52, Issue 1, pp. 1–28.
** DOI: 10.1007/BF01442634
**
** Utilizzata per la creazione di sistemi tripli di Steiner ciclici di 
** dimensione arbitraria tramite semplici trasformazioni aritmetiche
** e modulari.
**
** Compilare come eseguibile abilitando il simbolo LIB_TEST per un test 
** run automatico in modalita' standalone.
**
** Funzioni esportate:
**   size_t Genera_STSC(size_t v, Terna_t* STSC, Terna_t* TDiff, size_t td);
**   Terna_t* Genera_terne_differenza(size_t v, size_t* td);
**   bool Convalida_v(size_t v);
**   size_t Totale_terne_STS(size_t v);
**
** Licenza: MIT License
** Copyright (C) 2000-2025 M.A.W. 1968
*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "stsc.h"
#include "Peltesohn.h"

// Dimensionamento volutamente esorbitante per un esempio didattico
#define MAX_TERNE 10000

// String-ification per i messaggi d'errore
#define TO_STRING(x) #x

// Struttura per i parametri di libreria
struct {
    size_t   tot_terne;     // Totalizzatore terne differenza 
    uint32_t  s;            // Parametri s e k per le 
    uint32_t  k;            // formule di Peltesohn
    uint32_t limiti[3];     // Array dei vari limiti per le formule
} libparms;

// Array statico delle terne differenza
static Terna_t TD_array[MAX_TERNE];

// Indici dell'array per i limiti di iterazione sulle formule parametriche
enum {LOWER, MIDDLE, UPPER};

// Valori consentiti per k
const size_t K_VALUES[] = {1, 3, 7, 9, 13, 15};
const size_t MAX_K = 6;

/********************************************************************************/
// Funzioni interne
/********************************************************************************/

/********************************************************************************/
/*
** Gestisce i casi precalcolati: v = 7, 13, 15, 19, 27, 45 e 63
** Non esistono sistemi ciclici di Steiner per v = 9.
*/
/********************************************************************************/
static bool precalcolate(size_t v) {
    size_t i;

    // Trova l'indice di v nella LUT
    for (i = 0; i < MAX_LUT; i++) {
        if (LUT[i][0] == v) {
            break;
        }
    }

    if (i == MAX_LUT) {
        fprintf(stderr, ">> LIB_STS.precalcolate(): ERRORE\n"
                ">>    Incongruenza nella LUT interna (v=%zu).\n\n", v);
        return false; // v non trovato nella LUT
    }

    // Copia le terne precalcolate nell'array
    for (size_t j = 0; j < LUT[i][1]; j++) {
        TD_array[j] = TERNE_PELTESOHN[i][j];
    }
    libparms.tot_terne = LUT[i][1];

    return true;
}

/********************************************************************************/
/*
** Funzione core per la generazione delle terne differenza 
** tramite le formule parametriche della Peltesohn.
** Implementa pedissequamente quanto previsto dalle tabelle,
** a fine dichiaratamente illustrativo e didattico.
*/
/********************************************************************************/
static void genera_parametriche(void) {
    uint32_t r, s;
    // Assegnazione ridondante, per mera leggibilita'
    s = libparms.s;     
    switch (libparms.k) {
        case  1:
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s, 3 * s + 1, 6 * s + 1};
            for (r = 0; r < libparms.limiti[LOWER]; r++) {
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 1, 4 * s - r + 1,     4 * s + 2 * r + 2};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 2, 8 * s - r,         8 * s + 2 * r + 2};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 3, 6 * s - 2 * r - 1, 6 * s + r + 2};
            }
            // Casi extra per r = s-1 (MIDDLE)
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s - 2, 3 * s + 2,  6 * s};
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s - 1, 7 * s + 1, 10 * s};
            break;
        case  3:
            for (r = 0; r < libparms.limiti[MIDDLE]; r++) {
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 1, 8 * s - r + 1,     8 * s + 2 * r + 2};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 2, 4 * s - r,         4 * s + 2 * r + 2};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 3, 6 * s - 2 * r - 1, 6 * s + r + 2};
            }
            break;
        case  7:
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s + 1, 4 * s + 2, 7 * s + 3};
            for (r = 0; r < libparms.limiti[MIDDLE]; r++) {
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 1, 8 * s - r + 3,     8 * s + 2 * r + 4};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 2, 6 * s - 2 * r + 1, 6 * s + r + 3};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 3, 4 * s - r + 1,     4 * s + 2 * r + 4};
            }
            break;
        case  9:
            TD_array[libparms.tot_terne++] = (Terna_t){2, 8 * s + 3, 8 * s + 5};
            TD_array[libparms.tot_terne++] = (Terna_t){3, 8 * s + 1, 8 * s + 4};
            TD_array[libparms.tot_terne++] = (Terna_t){5, 8 * s + 2, 8 * s + 7};
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s - 1, 3 * s + 2, 6 * s + 1};
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s,     7 * s + 3, 8 * s + 6};
        
            // Casi extra per r = 0 e 1
            TD_array[libparms.tot_terne++] = (Terna_t){1, 4 * s + 3, 4 * s + 4};
            TD_array[libparms.tot_terne++] = (Terna_t){4, 4 * s + 2, 4 * s + 6};
            TD_array[libparms.tot_terne++] = (Terna_t){6, 6 * s - 1, 6 * s + 5};
            
            for (r = 2; r < libparms.limiti[LOWER]; r++) {
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 1, 4 * s - r + 3,     4 * s + 2 * r + 4};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 2, 8 * s - r + 2,     8 * s + 2 * r + 4};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 3, 6 * s - 2 * r + 1, 6 * s + r + 4};
            }
            // Casi extra per r = s-1 e s
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s - 2, 3 * s + 4, 6 * s + 2};
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s + 1, 3 * s + 3, 6 * s + 4};
            break;
        case 13:
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s + 2, 7 * s + 5, 8 * s + 6};
            for (r = 0; r < libparms.limiti[MIDDLE]; r++) {
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 2, 6 * s - 2 * r + 3, 6 * s +     r + 5};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 3, 8 * s -     r + 5, 8 * s + 2 * r + 8};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 1, 4 * s -     r + 3, 4 * s + 2 * r + 4};
            }
            // Caso extra per r = s
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s + 1, 3 * s + 3, 6 * s + 4};
            break;
        case 15:
            for (r = 0; r < libparms.limiti[MIDDLE]; r++) {
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 1, 4 * s - r + 3,     4 * s + 2 * r + 4};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 2, 8 * s - r + 6,     8 * s + 2 * r + 8};
                TD_array[libparms.tot_terne++] = (Terna_t){3 * r + 3, 6 * s - 2 * r + 3, 6 * s + r + 6};
            }
            // Casi extra per r = s
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s + 1, 3 * s + 3,  6 * s + 4};
            TD_array[libparms.tot_terne++] = (Terna_t){3 * s + 2, 7 * s + 6, 10 * s + 8};
            break;
        default:
            break;
    }
}

/************************************************************/
/*
** Convalida_v()
**
** Scopo:
**   - Controllo formale di congruenza per il parametro v
**     che definisce l'STS.
**
** Parametri:
**   - size_t v: dimensione del STS.
**
** Valore restituito:
**   - bool: true se il parametro risulta congruente.
*/
/************************************************************/
bool Convalida_v(size_t v) {
    return (v > 6) && (v != 9) && ((v % 6 == 1) || (v % 6 == 3));
}

/********************************************************************************/
/*
** Totale_terne_STS()
**
** Scopo:
**   - Calcola il numero atteso di terne nel Sistema Triplo di Steiner (STS).
**   - Utilizza la nota formula b = (v * (v - 1)) / 6
**
** Parametri:
**   - size_t v: dimensione del STS.
**
** Valore restituito:
**   - size_t: Numero di terne attese.
*/
/********************************************************************************/
size_t Totale_terne_STS(size_t v) {
    return (v * (v - 1U)) / 6U;
}

/********************************************************************************/
/*
** Genera_terne_differenza()
**
** Scopo:
**   - Funzione per la generazione di un array di terne differenza.
**     Con modifiche insignificanti, puo' produrre un array di Heffter,
**     oggetto combinatorio che negli ultimi anni ha riscosso moltissima
**     attenzione nella comunita' di ricerca. Per questo motivo viene
**     inclusa a scopo didattico nella libreria ed esportata come
**     funzione accessibile esternamente e separatamente.
**
** Parametri:
**   - Size_t  v....: La dimensione del STSC, della forma 6n+1 oppure 6n+3.
**   - size_t* td...: Puntatore al totalizzatore terne differenza.
**
** Valore restituito:
**   - Terna_t *....: Puntatore all'array statico per le terne differenza.
*/
/********************************************************************************/
Terna_t *Genera_terne_differenza(size_t v, size_t *td) {
    *td = 0;
    // Il contatore risulta duplicato internamente per eventuali 
    // future espansioni e modifiche.
    libparms.tot_terne = 0;

    // Il controllo di congruenza risulta ridondato per la massima
    // indipendenza e disaccoppiamento delle due funzioni principali
    if (!Convalida_v(v)) {
        fprintf(stderr, ">> LIB_STS.Genera_terne_differenza(): ERRORE\n"
                ">>    Controllo di congruenza fallito per v = %zu\n\n", v);
        return NULL;
    }
    
    // Calcolo dei parametri per le formule Peltesohn
    libparms.s = (uint32_t)((v - 1) / 18);
    libparms.k = v % 18;
    libparms.limiti[0] = libparms.s - 1;  // LOWER
    libparms.limiti[1] = libparms.s;      // MIDDLE
    libparms.limiti[2] = libparms.s + 1;  // UPPER

    // Dimensione richiesta
    size_t required_size = (size_t)(3 * libparms.s + 2);
    if (required_size > MAX_TERNE) {
        fprintf(stderr, ">> LIB_STS.Genera_terne_differenza(): ERRORE\n"
                ">>    Dimensione richiesta maggiore dell'hard limit compilato\n"
                ">>    \"MAX_TERNE\" (" TO_STRING(MAX_TERNE) ")\n\n");
        return NULL;
    }

    // Gestione casi singolari (precalcolati)
    if ((v < 20) || ((9 == libparms.k) && (libparms.s < 4))) {
        if (precalcolate(v)) {
#ifdef LIB_TEST
            fprintf(stderr, "** Uso terne precalcolate per "
                    "v = %2zd, s = %2zd, k = %2zd\n", 
                    v, libparms.s, libparms.k);
#endif        
            *td = libparms.tot_terne;
            return TD_array;
        } else {
            return NULL;
        }
    } else { // Calcolo standard con le formule Peltesohn
#ifdef LIB_TEST
        printf("** v = %zu, s = %2zd, k = %2zd\n", 
               v, libparms.s, libparms.k);
#endif
        genera_parametriche();
        *td = libparms.tot_terne;
        return TD_array;
    }
}

/********************************************************************************/
/*
** Genera_STSC()
**
** Scopo:
**   - Funzione core per la generazione del sistema triplo di Steiner ciclico 
**     per un dato valore di v. 
**
** Parametri:
**   - Size_t   v......: La dimensione del STSC, della forma 6n+1 oppure 6n+3.
**   - Terna_t* STSC...: Puntatore all'array di terne, allocato a carico
**                       del chiamante e dimensionato usando la funzione di 
**                       libreria apposita totale_terne_STS(v).
**
** Valore restituito:
**   - size_t: 0 in caso d'errore, oppure il numero di terne b del STSC(v).
*/
/********************************************************************************/
size_t Genera_STSC(size_t v, Terna_t *STSC) {
    size_t tc = 0U;         // Contatore locale terne
    size_t td = 0U;         // Contatore terne differenza
    Terna_t* TDiff;         // Array terne differenza  

    // Programmazione difensiva
    if (!Convalida_v(v)) {
        fprintf(stderr, ">> LIB_STS.Genera_STSC(): ERRORE\n"
                ">>    Controllo di congruenza fallito per v = %zu\n\n", v);
        return tc;
    }

    if (NULL == STSC) {
        fprintf(stderr, ">> LIB_STS.Genera_STSC(): ERRORE\n"
                ">>     L'array di terne del STS deve essere\n"
                ">>     allocato dal chiamante.\n\n");
        return tc;
    }

    // Genero dinamicamente le terne differenza
    TDiff = Genera_terne_differenza(v, &td);
    if (NULL == TDiff) {
        fprintf(stderr, ">> LIB_STS.Genera_STSC(): ERRORE\n"
                ">>    Errore nella generazione delle terne differenza.\n\n");
        return tc;
    }

    // Prima fase: terne di base e generazione parametrica
    for (uint32_t i = 0; i < td; ++i) {
        uint32_t b2;

        // Ricavo la terna base dalla corrispondente terna differenza
        // b0 = 0, b1 = a, b2 = a + b
        b2 = TDiff[i].a + TDiff[i].b;

        // Ricavo le terne del blocco principale, valido per ambedue i casi
        for (uint32_t j = 0; j < v; ++j) {
            // Salvo la terna generata
            STSC[tc++] = (Terna_t) {j, MOD(TDiff[i].a + j, v), MOD(b2 + j, v)};
        }
    }

    // Seconda fase: short orbit, solo se v = 6n + 3
    if (v % 6 == 3) {
        uint32_t n = (v - 3) / 6;
        uint32_t b1, b2;

        // Valori della terna Short Orbit (b0 = 0)
        b1 = 2 * n + 1;
        b2 = 4 * n + 2;

        for (uint32_t i = 0; i <= 2 * n; i++) {
            STSC[tc++] = (Terna_t) {i, MOD(b1 + i, v), MOD(b2 + i, v)};
        }
    }

    return tc;
}
/************************************************************/
/************************************************************/

#ifdef LIB_TEST
static void stampa_terne(size_t v) {
    printf("## %zu Terne differenza generate per v = %3zu (s = %2zu, k = %2zu)\n",
            libparms.tot_terne, v, libparms.s, libparms.k);
    for (size_t i = 0; i < libparms.tot_terne; ++i) {
        printf("  %4zu: (%2u, %2u, %2u)\n", i + 1,
               TD_array[i].a, 
               TD_array[i].b, 
               TD_array[i].c); 
    }
}

/*
** Self-test in batch, per valori ragionevoli di v, 
** copre anche tutti i casi delle terne precalcolate.
*/
int main() {
    size_t tot;
    
    for (size_t i = 0; i < 3; ++i)
    {
        size_t small_v[] = {7, 13, 15};
        size_t v = small_v[i];
        tot = 0;
        if (NULL != genera_terne_differenza(v, &tot)) {
            stampa_terne(v);
        }
    }
    
    for (size_t s = 1; s < 6; ++s) {
        for (size_t i = 0; i < MAX_K; ++i) {
            size_t v = 18 * s + K_VALUES[i];
            tot = 0;
            if (NULL != genera_terne_differenza(v, &tot)) {
                stampa_terne(v);
            }
        }
    }
    
    // Test con valori molto elevati
    tot = 0;
    size_t s = 3300;
    size_t k = 9;
    size_t v = 18 * s + k;
    if (NULL != genera_terne_differenza(v, &tot)) {
        printf("## %zu Terne differenza generate per v = %3zu (s = %2zu, k = %2zu)\n",
               libparms.tot_terne, v, libparms.s, libparms.k);
    }
    
    return EXIT_SUCCESS;
}
#endif
/** EOF: STSC.c **/