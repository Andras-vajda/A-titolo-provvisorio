/********************************************************************************/
/*
** Progetto: STSC_gen.exe
**
** Scopo: generazione di STS ciclici (STSC) con metodo Heffter-Peltesohn
**        tramite la libreria STSC.lib.
**
** Nome del modulo: STSC_gen.h
** Scopo: 
**   - Contiene le definizioni globali, le costanti e i prototipi delle funzioni.
**   - Include i tipi di dato e le strutture dati usate nel progetto.
**
** Licenza: MIT License
** Copyright (C) 2000-2025 M.A.W. 1968
*/
/********************************************************************************/

#ifndef _STSC_gen_h_
 #define _STSC_gen_h_

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "stsc.h"

// Template per il filename di output
#define FILENAME  "STSC_%04zu.csv"
#define FNAME_MAX 64

/* 
** Limiti per v. Il limite superiore e' del tutto arbitrario.
** La libreria è precompilata per un massimo di 10000 terne differenza,
** un valore decisamente esorbitante (non solo per un test).
*/
#define MIN_V 7
#define MAX_V 999

// Definizione delle modalita' operative
typedef enum {
    MODE_NONE,        // Nessuna modalita' valida
    MODE_TEST,        // Test
    MODE_INTERACTIVE, // Interattivo
    MODE_BATCH,       // Batch
    MODE_HELP         // Help  
} Mode;    

typedef struct {
    bool    print_flg;  // Gestisce la stampa a video
    bool    save_flg;   // Gestisce il salvataggio in un file CDF
    bool    diff_flg;   // Gestisce la stampa delle terne differenza
    size_t  v;          // Dimensione del STS, se 1 si esce dal programma
    size_t  b;          // Parametro b del STS(v): totale blocchi (terne)
    size_t  total;      // Totalizzatore terne generate
} SysParams;

extern SysParams sysparms;  // Variabile globale per la configurazione

// Funzione di generazione (STSC_gen.c)
bool genera_STS_ciclico(void);

// Funzioni UI (STSC_UI.c)
bool get_flag(const char *prompt, const bool deflt);
bool prompt_user(void);
void usage(char *fn);

// Funzioni modalita' di esecuzione (STSC_mode.c)
bool run_interactive_mode(void);
bool run_batch_mode(size_t min, size_t max);
bool run_test_mode(void);
Mode parse_args(int argc, char *argv[], size_t *min, size_t *max);

#endif
