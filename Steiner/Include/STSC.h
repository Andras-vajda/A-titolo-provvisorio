#ifndef _LIB_STS_H_
 #define _LIB_STS_H_
#include <stdint.h>

#define MOD(a, b) (((a) % (b) + (b)) % (b))

// Struttura per una terna
typedef struct {
    uint32_t a, b, c;
} Terna_t;

/********************************************************************************/
/*
** Genera_STS_ciclico()
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
size_t Genera_STSC(size_t v, Terna_t* STSC);

/********************************************************************************/
/*
** Genera_terne_differenza()
**
** Scopo:
**   - Funzione per la generazione di un array di terne differenza.
**     Con modifiche insignificanti, puo' produrre un array di Heffter,
**     oggetto combinatorio che negli ultimi anni ha riscosso moltissima
**     attenzione nella comunita' di ricerca. Per questo motivo, ed a scopo
**     esplicitamente didattico, viene inclusa  nella libreria ed esportata
**     come funzione accessibile esternamente e separatamente.
**
**     In un normale contesto produttivo, la funzione rimarrebbe static
**     e restituirebbe direttamente le terne base, saltando il passaggio
**     non necessario delle terne differenza.
**
** Parametri:
**   - Size_t  v....: La dimensione del STSC, della forma 6n+1 oppure 6n+3.
**   - size_t* td...: Puntatore al totalizzatore terne differenza.
**
** Valore restituito:
**   - Terna_t *....: Puntatore all'array statico per le terne differenza.
*/
/********************************************************************************/
Terna_t* Genera_terne_differenza(size_t v, size_t* td);

/************************************************************/
/*
** Convalida_v()
**
** Scopo:
**   - Controllo formale di congruenza per il parametro v
**     che definisce l'STS. Accessibile anche esternamente
**     per mera comodita' del programmatore.
**
** Parametri:
**   - size_t v: dimensione del STS.
**
** Valore restituito:
**   - bool: true se il parametro risulta congruente.
*/
/************************************************************/
bool Convalida_v(size_t v);

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
size_t Totale_terne_STS(size_t v);
#endif