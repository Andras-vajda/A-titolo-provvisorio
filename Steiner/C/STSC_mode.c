/********************************************************************************/
/*
** Progetto: STSC_gen.exe
**
** Scopo: generazione di STS ciclici (STSC) con metodo Heffter-Peltesohn
**        tramite la libreria STSC.lib.
**
** Nome del modulo: STSC_mode.c
** Scopo:
**   - Gestisce le modalita' operative del programma.
**   - Implementa la logica per le modalita':
**     * Test (`run_test_mode()`)
**     * Interattiva (`run_interactive_mode()`)
**     * Batch (`run_batch_mode()`)
**   - Analizza gli argomenti della riga di comando (`parse_args()`).
**
** Funzioni definite:
**   - bool run_interactive_mode(void);
**   - bool run_batch_mode(size_t min, size_t max);
**   - bool run_test_mode(void);
**   - Mode parse_args(int argc, char *argv[], size_t *min, size_t *max);
**
** Licenza: MIT License
** Copyright (C) 2000-2025 M.A.W. 1968
*/
/********************************************************************************/

#include "STSC_gen.h"

/********************************************************************************/
/*
** run_interactive_mode()
**
** Scopo:
**   - Gestisce la modalita' interattiva ciclica.
**   - L'utente inserisce manualmente il valore di `v`, oppure un valore 1
**     per terminare.
**
** Parametri:
**   - Nessuno.
**
** Valore restituito:
**   - bool: `true` se la modalita' interattiva termina correttamente.
*/
/********************************************************************************/
bool run_interactive_mode(void) {
    printf("**** Modalita' interattiva. Digita 1 per uscire. ****\n");

    while (prompt_user()) {
        // Genera l'STS per il valore fornito
        if (genera_STS_ciclico()) {
            printf("** Numero terne previste..............: %zu\n"
                   "** Terne generate.....................: %zu\n"
                   "** STS(%zu) generato con successo.\n",
                   sysparms.b, sysparms.total, sysparms.v);
        } else {
            fprintf(stderr, "## Errore: generazione di STS(%zu) fallita.\n", sysparms.v);
            return false;
        }
    }
    printf("**** Modalita' interattiva terminata. ****\n");
    return true;
}

/********************************************************************************/
/*
** run_batch_mode()
**
** Scopo:
**   - Gestisce la modalita' batch, generando STS per un intervallo di `v`
**     passato come parametro da command line.
**   - Viene generato un singolo file CSV per ogni valore valido.
**
** Parametri:
**   - min (size_t) : Valore minimo di `v`.
**   - max (size_t) : Valore massimo di `v`.
**
** Valore restituito:
**   - bool: `true` se tutte le generazioni hanno successo.
*/
/********************************************************************************/
bool run_batch_mode(size_t min, size_t max) {
    size_t k1, k2, r;

    r = ((min - 1U) % 6U) / 2U;
    k1 = r + (min - 1U - 2U * r) / 3U;
    r = ((max - 1U) % 6U) / 2U;
    k2 = r + (max - 1U - 2U * r) / 3U;
    r = 1U;

    printf("**** Esecuzione in modalita' batch per l'intervallo [%zu, %zu]...\n", min, max);

    sysparms.save_flg  = true;
    sysparms.print_flg = false;
    sysparms.diff_flg  = true;
    
    for (size_t k = k1; k <= k2; k++) {
        // Genera direttamente i soli valori validi di v con una formula modulare
        // v = 6*floor(k/2)+1+2*(k&1)
        sysparms.v = 6U * (k / 2U) + 1U + 2U * (k & 1);

        if (sysparms.v == 9U) {
            continue;
        }

        printf("** Step %zu: generazione STS(%zu)\n", r++, sysparms.v);
        if (genera_STS_ciclico()) {
            printf("** Numero terne previste..............: %zu\n"
                "** Terne generate.....................: %zu\n"
                "** STS(%zu) generato con successo.\n",
                sysparms.b, sysparms.total, sysparms.v);
        }
        else {
            fprintf(stderr, "## Errore: generazione di STS(%zu) fallita.\n", sysparms.v);
            return false;
        }
    }

    printf("**** Modalita' batch completata con successo.\n");
    return true;
}

/********************************************************************************/
/*
** run_test_mode()
**
** Scopo:
**   - Esegue una batterie di test che genera un insieme predefinito di STS,
**     da confrontare con i file di riferimento generati tramite spreadsheet
**     contenuti nella cartella \REFDATA.   
**
** Parametri:
**   - Nessuno.
**
** Valore restituito:
**   - bool: `true` se tutti i test hanno successo.
*/
/********************************************************************************/
bool run_test_mode(void) {
    size_t test_values[] = { 7U, 13U, 15U, 19U, 27U, 45U, 63U, 
                            73U, 75U, 79U, 81U, 85U, 87U};
    size_t num_tests = sizeof(test_values) / sizeof(test_values[0]);

/*
** Effettua un test per tutti i piccoli valori precalcolati e per
** l'intero set dei valori validi per k (1, 3, 7, 9, 13, 15) con s = 4,
** ossia il valor minimo che garantisce l'esistenza di formule valide
** per ognuno dei sei casi parametrici in k.
*/
    printf("**** Modalita' di test: generazione di STS noti. ****\n");

    for (size_t i = 0U; i < num_tests; i++) {
        sysparms.v = test_values[i];
        sysparms.save_flg  = true;
        sysparms.print_flg = false;
        sysparms.diff_flg  = false;

        printf("Generazione STS(%zu)...\n", sysparms.v);
        if (genera_STS_ciclico()) {
            printf("** Numero terne previste..............: %zu\n"
                "** Terne generate.....................: %zu\n"
                "** STS(%zu) generato con successo.\n",
                sysparms.b, sysparms.total, sysparms.v);
        }
        else {
            fprintf(stderr, "## Errore: generazione di STS(%zu) fallita.\n", sysparms.v);
            return false;
        }
    }

    printf("**** Modalita' di test completata. ****\n");
    return true;
}

/********************************************************************************/
/*
** parse_args()
**
** Scopo:
**   - Analizza gli argomenti della riga di comando e imposta la modalita' operativa.
**
** Parametri:
**   - argc (int)        : Numero di argomenti.
**   - argv (char **)    : Array di stringhe con gli argomenti.
**   - min (size_t *)    : Puntatore al valore minimo per modalita' batch.
**   - max (size_t *)    : Puntatore al valore massimo per modalita' batch.
**
** Valore restituito:
**   - Mode: Enum che identifica la modalita' selezionata.
*/
/********************************************************************************/
Mode parse_args(int argc, char *argv[], size_t *min, size_t *max) {
    if (argc < 2) {
        return MODE_INTERACTIVE;
    }

    if (strcmp(argv[1], "/help") == 0) {
        return MODE_HELP;
    }

    if (strcmp(argv[1], "/test") == 0) {
        return MODE_TEST;
    }

    if (strcmp(argv[1], "/interactive") == 0) {
        return MODE_INTERACTIVE;
    }

    if (strcmp(argv[1], "/batch") == 0) {
        if (argc != 4) {
            fprintf(stderr, "## Errore: /batch richiede due argomenti: <min> <max>.\n");
            return MODE_NONE;
        }

        char *endptr;
        *min = strtoul(argv[2], &endptr, 10);
        if (*endptr != '\0') {
            fprintf(stderr, "## Errore: il valore minimo '%s' non è un numero valido.\n", argv[2]);
            return MODE_NONE;
        }

        *max = strtoul(argv[3], &endptr, 10);
        if (*endptr != '\0') {
            fprintf(stderr, "## Errore: il valore massimo '%s' non è un numero valido.\n", argv[3]);
            return MODE_NONE;
        }

        if (*min > *max) {
            fprintf(stderr, "## Errore: il valore minimo (%zu) non può essere maggiore del massimo (%zu).\n", *min, *max);
            return MODE_NONE;
        }

        return MODE_BATCH;
    }

    fprintf(stderr, "## Errore: modalita' '%s' non riconosciuta.\n", argv[1]);
    return MODE_NONE;
}
/** EOF: STSC_mode.c **/