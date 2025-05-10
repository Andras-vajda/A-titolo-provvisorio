/**
 * @file TLSC.c
 * @brief Traffic Light System Controller basato su rete di Petri 1-safe
 * @author M.A.W. 1968
 * @date May 10, 2025
 * @version 1.0
 *
 * @details Implementazione di un sistema di controllo semaforico failsafe
 * secondo il modello a due sottosistemi sincronizzati (veicolare/pedonale).
 * Il semaforo pedonale diviene verde quando il veicolare passa al rosso,
 * implementando un ciclo temporizzato fisso.
 *
 * @metrics
 * - SLOC: 128
 * - Complessità ciclomatica: 14
 * - NPath: 16
 *
 * @copyright MIT License - M.A.W. 1968
 * 
 * @note Il codice integrale è disponibile nel repository Github dell'autore,
 * il cui link e QR code sono riportati in penultima pagina dell'articolo.
 */
 
#include <stdint.h>
#include <stdio.h>

/* 
** Valori booleani come enum, da modificare secondo le 
** estensioni booleane del cross-compilatore in uso. 
*/
typedef enum {
    TLS_FALSE = 0, 
    TLS_TRUE = 1
} TLS_Boolean;

/*
** Definizione degli stati del semaforo come enum con valori binari.
** Quasi tutti i cross-compiler moderni supportano la notazione binaria
** per le costanti, analogamente ai comuni assembler.
*/
typedef enum {
    SV_GREEN  = 0b00001,  /* Semaforo veicolare verde */
    SV_YELLOW = 0b00010,  /* Semaforo veicolare giallo */
    SV_RED    = 0b00100,  /* Semaforo veicolare rosso */
    SP_RED    = 0b01000,  /* Semaforo pedonale rosso */
    SP_GREEN  = 0b10000   /* Semaforo pedonale verde */
} TLS_State;

/* Struttura dati per rappresentare il sistema di controllo semaforico */
typedef struct {
    uint8_t state;          /* Stato corrente del sistema (rappresentazione a bit degli stati) */
    uint32_t timer_v;       /* Timer per la durata dei segnali veicolari */
    uint32_t timer_p;       /* Timer per la durata dei segnali pedonali */
} TrafficLightSystem;

/* Inizializza il sistema semaforico nello stato iniziale sicuro */
void TLS_Init(TrafficLightSystem* tls) {
    if (tls == NULL) return;
    
    /* Stato iniziale: veicolare verde, pedonale rosso (configurazione sicura predefinita) */
    tls->state = SV_GREEN | SP_RED;
    tls->timer_v = 300;     /* Timer iniziale per verde veicolare (30 sec) */
    tls->timer_p = 0;
}

/* Verifica se una transizione e' abilitata */
TLS_Boolean TLS_IsTransitionEnabled(const TrafficLightSystem* tls, uint8_t transition) {
    if (tls == NULL) return TLS_FALSE;
    
    /* Utilizziamo una costante per ogni pattern di stato */
    static const uint8_t PATTERNS[4] = {
        SV_GREEN | SP_RED,                /* Pattern per t1 */
        SV_YELLOW | SP_RED,               /* Pattern per t2 */
        SP_GREEN | SV_RED,                /* Pattern per t3 */
        SV_RED | SP_RED                   /* Pattern per t4 */
    };
    
    /* Utilizziamo una maschera per ogni pattern di stato */
    static const uint8_t MASKS[4] = {
        SV_GREEN | SV_YELLOW | SV_RED | SP_RED | SP_GREEN,  /* Maschera per t1 */
        SV_YELLOW | SV_GREEN | SV_RED | SP_RED | SP_GREEN,  /* Maschera per t2 */
        SP_GREEN | SP_RED | SV_RED,                         /* Maschera per t3 */
        SV_RED | SV_GREEN | SV_YELLOW | SP_RED | SP_GREEN   /* Maschera per t4 */
    };
    
    /* Transizioni valide solo da 1 a 4 */
    if (transition < 1 || transition > 4) 
        return TLS_FALSE;
        
    /* Confronto atomico: verifica se, applicando la maschera, otteniamo il pattern atteso */
    return ((tls->state & MASKS[transition-1]) == PATTERNS[transition-1]) ? TLS_TRUE : TLS_FALSE;
}

/* Esegue una transizione se abilitata */
TLS_Boolean TLS_FireTransition(TrafficLightSystem* tls, uint8_t transition) {
    if (tls == NULL) return TLS_FALSE;
    
    if (TLS_IsTransitionEnabled(tls, transition) == TLS_FALSE) {
        return TLS_FALSE; /* Transizione non abilitata */
    }
    
    switch (transition) {
        case 1: /* t1: Verde veicolare -> Giallo veicolare */
            tls->state &= ~SV_GREEN;   /* Rimuovi verde veicolare */
            tls->state |= SV_YELLOW;   /* Aggiungi giallo veicolare */
            tls->timer_v = 30;         /* Timer per giallo (3 sec a 10 tick/sec) */
            break;
            
        case 2: /* t2: Giallo veicolare -> Rosso veicolare + Verde pedonale */
            tls->state &= ~SV_YELLOW;  /* Rimuovi giallo veicolare */
            tls->state |= SV_RED;      /* Aggiungi rosso veicolare */
            tls->state &= ~SP_RED;     /* Rimuovi rosso pedonale */
            tls->state |= SP_GREEN;    /* Aggiungi verde pedonale */
            tls->timer_p = 150;        /* Timer per attraversamento (15 sec) */
            break;
            
        case 3: /* t3: Verde pedonale -> Rosso pedonale */
            tls->state &= ~SP_GREEN;   /* Rimuovi verde pedonale */
            tls->state |= SP_RED;      /* Aggiungi rosso pedonale */
            tls->timer_v = 20;         /* Timer sicurezza (2 sec) prima del verde veicolare */
            break;
            
        case 4: /* t4: Rosso veicolare -> Verde veicolare */
            tls->state &= ~SV_RED;     /* Rimuovi rosso veicolare */
            tls->state |= SV_GREEN;    /* Aggiungi verde veicolare */
            tls->timer_v = 300;        /* Timer per verde veicolare (30 sec) */
            break;
            
        default:
            return TLS_FALSE; /* Transizione non valida */
    }
    
    return TLS_TRUE; /* Transizione eseguita con successo */
}

/* Aggiorna il sistema semaforico (da chiamare periodicamente, es. ogni 100ms) */
void TLS_Update(TrafficLightSystem* tls) {
    if (tls == NULL) return;
    
    /* Decrementa i timer se attivi */
    if (tls->timer_v > 0) {
        tls->timer_v--;
        
        /* Verifica scadenza timer veicolare */
        if (tls->timer_v == 0) {
            /* Timer scaduto, verifica transizioni automatiche basate sullo stato corrente */
            if (tls->state & SV_YELLOW) {
                TLS_FireTransition(tls, 2); /* Da giallo a rosso veicolare/verde pedonale */
            } 
            else if ((tls->state & SV_RED) && (tls->state & SP_RED)) {
                TLS_FireTransition(tls, 4); /* Da rosso veicolare a verde veicolare */
            }
            else if (tls->state & SV_GREEN) {
                /* Scaduto il tempo del verde veicolare, passa a giallo */
                TLS_FireTransition(tls, 1); /* Da verde veicolare a giallo */
            }
        }
    }
    
    if (tls->timer_p > 0) {
        tls->timer_p--;
        
        /* Verifica scadenza timer pedonale */
        if (tls->timer_p == 0 && (tls->state & SP_GREEN)) {
            TLS_FireTransition(tls, 3); /* Da verde pedonale a rosso pedonale */
        }
    }
}

/* Funzione di debug per visualizzare lo stato corrente */
void TLS_PrintState(const TrafficLightSystem* tls) {
    if (tls == NULL) return;
    
    printf("Stato Semaforo: \n");
    printf("  Veicolare: %s%s%s\n", 
           (tls->state & SV_RED) ? "[ROSSO] " : "",
           (tls->state & SV_YELLOW) ? "[GIALLO] " : "",
           (tls->state & SV_GREEN) ? "[VERDE] " : "");
    
    printf("  Pedonale: %s%s\n", 
           (tls->state & SP_RED) ? "[ROSSO] " : "",
           (tls->state & SP_GREEN) ? "[VERDE] " : "");
    
    printf("  Timer veicolare: %u.%u s\n", tls->timer_v/10, tls->timer_v%10);
    printf("  Timer pedonale: %u.%u s\n\n", tls->timer_p/10, tls->timer_p%10);
}

/* Esempio di utilizzo */
#ifdef TEST_TRAFFIC_LIGHT
int main(void) {
    TrafficLightSystem tls;
    int simulation_steps = 100;
    int step;
    
    TLS_Init(&tls);
    printf("Sistema semaforico inizializzato.\n");
    TLS_PrintState(&tls);
    
    /* Simulazione del ciclo semaforico */
    for (step = 0; step < simulation_steps; step++) {
        TLS_Update(&tls);
        
        /* Stampa lo stato ogni secondo (10 passi) */
        if (step % 10 == 0) {
            printf("Tempo: %d.0 s\n", step/10);
            TLS_PrintState(&tls);
        }
    }
    
    return 0;
}
#endif /* TEST_TRAFFIC_LIGHT */