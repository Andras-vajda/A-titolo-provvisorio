/********************************************************************************/
/*
** Progetto: STSC_gen.exe
**
** Scopo: generazione di STS ciclici (STSC) con metodo Heffter-Peltesohn
**        tramite la libreria STSC.lib.
**
** Nome del modulo: STSC_UI.c
** Scopo:
**   - Implementa l'interfaccia utente.
**   - Gestisce l'input da tastiera e la validazione dei dati.
**   - Fornisce funzioni per richiedere conferme (`get_flag()`) e input numerici (`Prompt_user()`).
**
** Funzioni definite:
**   - bool get_flag(const char *prompt, const bool deflt);
**   - bool Prompt_user(void);
**   - void usage(char *fn);
**
** Licenza: MIT License
** Copyright (C) 2000-2025 M.A.W. 1968
*/
/********************************************************************************/

#include "STSC_gen.h"

/********************************************************************************/
/*
** get_flag()
**
** Scopo:
**   - Richiede all'utente un input booleano (`S/N`).
**
** Parametri:
**   - prompt (const char *) : Messaggio di richiesta input.
**   - deflt (bool)          : Valore predefinito se non viene fornito input.
**
** Valore restituito:
**   - bool: `true` per 'S', `false` per 'N'.
*/
/********************************************************************************/
bool get_flag(const char* prompt, const bool deflt) {
    char input[100];
    bool retval;

    while (true) {
        printf("%s", prompt);
        if (!fgets(input, sizeof(input), stdin)) {
            fprintf(stderr, "## Errore di input. Riprova.\n");
            continue;
        }

        // Rimuove il newline dall'input
        input[strcspn(input, "\n")] = '\0';

        // Gestione caso di invio vuoto
        if (strlen(input) == 0) {
            retval = deflt;
            break;
        }
        else {
            char scelta = tolower(input[0]);
            if (scelta == 's') {
                retval = true;
                break;
            }
            else if (scelta == 'n') {
                retval = false;
                break;
            }
            else {
                fprintf(stderr, "## Input non valido. Usa solo S/s o N/n. Riprova.\n");
            }
        }
    }
    return retval;
}

/********************************************************************************/
/*
** prompt_user()
**
** Scopo:
**   - Richiede un valore di `v` all'utente e lo convalida.
**
** Parametri:
**   - Nessuno.
**
** Valore restituito:
**   - bool: `true` se il valore è valido, `false` in caso di errore.
*/
/********************************************************************************/
bool prompt_user(void) {
    char input[100];
    while (true) {
        // Prompt per il valore di v
        printf("Inserisci un valore di v (1 per uscire, v = 6n+1 o 6n+3): ");
        if (!fgets(input, sizeof(input), stdin)) {
            fprintf(stderr, "## Errore di input. Riprova.\n");
            continue;
        }

        // Rimuove il newline dall'input
        input[strcspn(input, "\n")] = '\0';

        // Enter -> default
        if (strlen(input) == 0) {
            return false; // Uscita senza eseguire ulteriori calcoli
        }

        size_t v_temp = 0;
        if (sscanf(input, "%zu", &v_temp) != 1) {
            fprintf(stderr, "## Input non valido. Riprova.\n");
            continue;
        }

        // v = 1 per uscire
        if (v_temp == 1) {
            return false; // Uscita senza eseguire ulteriori calcoli
        }

        // Caso singolare: v = 9
        if (v_temp == 9) {
            fprintf(stderr, "## ## Errore: non esiste un STS ciclico di dimensione v = 9. Riprova.\n");
            continue;
        }

        // Controllo congruenza formale per v
        if (v_temp > 6 && (v_temp % 6 == 1 || v_temp % 6 == 3)) {
            sysparms.v = v_temp;
        }
        else {
            fprintf(stderr, "## ## Errore: v deve essere del tipo 6n+1 o 6n+3 e maggiore di 6. Riprova.\n");
            continue;
        }

        // Prompt per `print_flg`
        sysparms.print_flg = get_flag("Vuoi stampare a video il STS(v) (S/s o N/n)?......................: ", true);

        if (sysparms.print_flg) {
            sysparms.diff_flg = get_flag("Vuoi stampare a video le terne differenza (S/s o N/n)?............: ", true);
        }

        // Prompt per `save_flg`
        sysparms.save_flg = get_flag("Vuoi salvare il STS(v) su file (S/s o N/n)?.......................: ", false);

        // Tutti i parametri convalidati
        printf("**** Parametri configurati:\n**  v = %zu\n"
            "**  print_flg = %s\n**  diff_flg  = %s\n"
            "**  save_flg  = % s\n\n",
            sysparms.v, sysparms.print_flg ? "SI" : "NO",
            sysparms.diff_flg ? "SI" : "NO", sysparms.save_flg ? "SI" : "NO");

        return true;
    }
}

void usage(char *fn) {
 
    fprintf(stderr, "Uso: %s [/help | /test | /interactive | /batch <min> <max>]\n"
        "  /help              : Mostra la presente schermata.\n"
        "  /test              : Esegue i test automatici.\n"
        "  /interactive       : Genera STS interattivamente.\n"
        "  /batch <min> <max> : Genera STS per l'intervallo specificato,\n"
        "                       usando solo valori validi per v della forma\n"
        "                       6n+1 oppure 6n+3, compresi tra %u e %u.\n\n", fn, MIN_V, MAX_V);
}
/** EOF: STSC_UI.c **/