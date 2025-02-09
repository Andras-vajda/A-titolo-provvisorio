/*************************************************************************
**************************************************************************
**                                                                      **
**  PROGRAMMA......:  Stable_GI.h                                       **
**  AUTORE.........:  M.A.W. 1968                                       **
**  LICENZA........:  Public Domain                                     **
**  DATA...........:  31/mar/2020 19:36                                 **
**  VERSIONE.......:  1.0                                               **
**                                                                      **
**  Descrizione....:  Header per Stable_GI.c: algoritmo esteso di       **
**                    Gusfield & Irving per i matrimoni stabili.        **
**                                                                      **
**************************************************************************
*************************************************************************/
#ifndef __STABLE_EGS_H__
#define __STABLE_EGS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>  

/*
** Specifica il file dati da usare.
*/
#ifndef DATAFILE
 #define DATAFILE "GI_data1.h"
#endif
#include DATAFILE

#define BLANK (0xFF)
#define BUFF_SIZE (81)
/*#define MAX_ROTATIONS UCHAR_MAX*/
#define MAX_ROTATIONS (PROBLEM_SIZE*(PROBLEM_SIZE -1)/2)

#define H_SEP_STAR "************************************************************"
#define H_SEP_LINE "------------------------------------------------------------"

/*
** L'array delle soluzioni e tutti gli array corrrelati sono dimensionati e
** tipizzati come array di byte (unsigned char), quindi in grado di gestire
** problemi di dimensione massima pari a 255 x 255: del tutto esorbitante
** per un semplice esempio didattico di implementazione.
*/
typedef unsigned char       marriage_t;
typedef unsigned char       uint8_t;
typedef enum {FALSE, TRUE}  boole_t;

/* Nodo della lista dinamica delle preferenze (DLL) */
typedef struct node {
    struct node *next;
    struct node *prev;
    marriage_t   val;
} node_t;

/* Nodo header di lista, per ogni persona P */
typedef struct {
    node_t *first;
    node_t *last;
    uint8_t total;
} head_t;

/* Nodo della lista delle rotazioni Rho */
typedef struct rnode {
    marriage_t m, w;
} rnode_t;    

/************************************************************************/
/************************************************************************/

/* Dati condivisi a livello di modulo */
struct
{
    /* 
    ** Soluzioni estremali man-optimal M0 e woman-optimal Mz 
    */
    marriage_t  M_Optimal[PROBLEM_SIZE];
    marriage_t  F_Optimal[PROBLEM_SIZE];

    /* Soluzione corrente */
    marriage_t  Solution[PROBLEM_SIZE];

    /*
    ** Array delle preferenze: ad ogni persona corrisponde una smartlist.
    ** Le locazioni notevoli sono accessibili in O(1), ma anche
    ** la cancellazione è realizzata in O(1).
    */
    head_t M_pref[PROBLEM_SIZE];
    head_t F_pref[PROBLEM_SIZE];

    /*
    ** Array delle preferenze: reverse lookup per uomini e donne.
    ** Rendono possibile l'accesso diretto in O(1).
    */
    node_t *M_LUT[PROBLEM_SIZE][PROBLEM_SIZE];
    node_t *F_LUT[PROBLEM_SIZE][PROBLEM_SIZE];

    /*
    ** Matrice binaria delle coppie stabili.
    */
    boole_t StableCouples[PROBLEM_SIZE][PROBLEM_SIZE];

    /*
    ** Array delle rotazioni.
    */
    rnode_t Rho[MAX_ROTATIONS][PROBLEM_SIZE];

    /*
    ** Matrice di lookup delle rotazioni: Rotations[m][w] = idx.
    ** idx != BLANK se e solo se (m,w) appartiene alla rotazione idx in Rho[idx].
    */
    marriage_t Rotations[PROBLEM_SIZE][PROBLEM_SIZE];


    /* Stack */
    marriage_t Stack[2*PROBLEM_SIZE];
    boole_t    b_stack[PROBLEM_SIZE];
    size_t stk_ptr;

} SM;

/************************************************************************/
/************************************************************************/
boole_t IsInStack(const marriage_t d);
marriage_t Pop(void);
marriage_t Top(void);
void Delete(const marriage_t m, const marriage_t w);
void DisplayPrefs(void);
void DisplaySolution(marriage_t sol[PROBLEM_SIZE], const char *label);
void DisplayStableCouples(void);
void FindGSLists(void);
void FindRotations(void);
void InitData(void);
void Push(const marriage_t d);
/************************************************************************/
/************************************************************************/
#endif
