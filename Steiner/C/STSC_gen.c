/********************************************************************************/
/*
** Progetto: STSC_gen.exe
**
** Scopo: generazione di STS ciclici (STSC) con metodo Heffter-Peltesohn
**        tramite la libreria STSC.lib.
**
** Nome del modulo: STSC_gen.c
**   - Implementa il core del programma.
**   - Contiene la funzione `main()`, che gestisce il flusso principale.
**   - Implementa la logica di generazione degli STS ciclici.
**
** Funzioni definite:
**   - bool genera_STS_ciclico(void);
**   - int main(int argc, char *argv[]);
**
** Licenza: MIT License
** Copyright (C) 2000-2025 M.A.W. 1968
*/
/********************************************************************************/

#include "STSC_gen.h"

SysParams sysparms = {false, false, false, 0U, 0U, 0U};

/********************************************************************************/
/*
** genera_STS_ciclico()
**
** Scopo:
**   - Funzione core per la generazione del sistema triplo di Steiner ciclico 
**     per un dato valore di v. 
**   - Richiama la funzione di libreria genera_terne() per creare al volo le
**     terne differenza dalle formule della Peltesohn.  
**   - Se attivata l'opzione, salva l'output su file.
**
** Parametri:
**   - Nessuno (usa `sysparms.v` per il valore di v).
**
** Valore restituito:
**   - bool: `true` se la generazione ha successo, `false` altrimenti.
*/
/********************************************************************************/
bool genera_STS_ciclico(void) {
    int di_v;           // Maschera cifre di v e valori terne
    int di_b;           // Maschera cifre del totalizzatore terne 
    Terna_t* STSC;      // Array terne del STS(v) 
    char buff[20];      // Buffer di appoggio per maschere   

    // Inizializza i parametri di visualizzazione e gestione 
    sysparms.total = 0;
    sysparms.b = Totale_terne_STS(sysparms.v);
    sprintf(buff, "%zu", sysparms.b);
    di_b = strlen(buff);
    sprintf(buff, "%zu", sysparms.v);
    di_v = strlen(buff);

    // Visualizza le terne differenza, a scopo illustrativo
    if (sysparms.diff_flg) {
        size_t td = 0;      // Contatore locale terne differenza 
        int di_td;          // Maschera cifre di td
        size_t p_diff = 1;  // Totalizzatore terne differenza
        Terna_t* TDiff;     // Array delle terne differenza

        // Genero dinamicamente le terne differenza tramite la libreria
        TDiff = Genera_terne_differenza(sysparms.v, &td);
        if (NULL == TDiff) {
            // Messaggio d'errore gia' emesso dalla libreria.
            return false;
        }

        sprintf(buff, "%zu", td);
        di_td = strlen(buff);

        puts("** Terne differenza e di base:");
        for (size_t i = 0; i < td; ++i) {
            printf("%0*zu [%*u, %*u, %*u] -> (0, %*u, %*zu)\n",
                di_td, p_diff,
                di_v, TDiff[i].a, di_v, TDiff[i].b,
                di_v, TDiff[i].c, di_v, TDiff[i].a,
                di_v, MOD(TDiff[i].a + TDiff[i].b, sysparms.v));
            p_diff += 1;
        }
    }

    // Allocazione buffer per STS(v)
    STSC = (Terna_t*)malloc(sysparms.b * sizeof(Terna_t));
    if (NULL == STSC) {
        fprintf(stderr, "## Errore: Allocazione memoria fallita per %zu terne.\n", sysparms.b);
        return false;
    }

    // Generazione del STS(v)
    sysparms.total = Genera_STSC(sysparms.v, STSC);
    if (0 == sysparms.total) {
        fprintf(stderr, "## Errore: generazione delle terne STS non riuscita per v=%zu.\n", sysparms.v);
        free(STSC);
        return false;
    }

    // Visualizza le terne del STS(v)
    if (sysparms.print_flg) {
        printf("\n** Terne del STS(%zu) ciclico:\n", sysparms.v);
        for (size_t i = 0; i < sysparms.total; ++i) {
            printf("%0*zu (%*u, %*u, %*u)\n", 
               di_b, i+1, di_v, STSC[i].a, 
               di_v, STSC[i].b, di_v, STSC[i].c);
        }
    }

    // Salvataggio su file CSV
    if (sysparms.save_flg) {
        FILE *fp;
        char fname[FNAME_MAX];      // Nome file csv
        // Genera il nome del file in base al valore di v
        snprintf(fname, sizeof(fname), FILENAME, sysparms.v);
        printf("** Salvataggio STS(%zu) su file %s.\n", sysparms.v, fname);

        // Apre il file per la scrittura
        fp = fopen(fname, "w+");
        if (!fp) {
            fprintf(stderr, "## Errore: impossibile aprire il file %s in scrittura.\n", fname);
            return false;
        }
        fprintf(fp, "a,b,c,%zu,%zu\n", sysparms.v, sysparms.b);
        for (size_t i = 0; i < sysparms.b; ++i) {
            fprintf(fp, "%u,%u,%u\n", STSC[i].a, STSC[i].b, STSC[i].c);
        }
        fclose(fp);
    }

    printf("** Generazione STSC completata con successo per v=%zu. Numero di terne: %zu.\n", sysparms.v, sysparms.total);
    free(STSC);
    return true;
}

/********************************************************************************/
/********************************************************************************/
int main(int argc, char *argv[]) {
    size_t min = 0U, max = 0U;
    bool retval = false;

    switch (parse_args(argc, argv, &min, &max)) {
        case MODE_TEST:
            retval = run_test_mode();
            break;

        case MODE_INTERACTIVE:
            retval = run_interactive_mode();
            break;

        case MODE_BATCH:
            retval = run_batch_mode(min, max);
            break;

        case MODE_HELP:    
        default:
            // Stampa concise istruzioni d'uso
            usage(argv[0]);
            retval = false;
            break;
    }
    return retval ? EXIT_SUCCESS : EXIT_FAILURE;
}
/** EOF: STSC_gen.c **/