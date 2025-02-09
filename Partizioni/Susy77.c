#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PARTS 16
#define FMT_PERMS "\n   %02d: "

/*
** Totalizzatori per le soluzioni:
** Partizioni, Partizioni in parti distinte, Permutazioni delle
** parti su a-b-c.
*/
unsigned Cnt_parts, Cnt_unique, Cnt_perms;

/*
** Tabella precalcolata delle permutazioni di tre elementi
*/
unsigned perms[6][3] = {{0,1,2},{0,2,1},{1,0,2},
                        {1,2,0},{2,0,1},{2,1,0}};

/*
** Tabella precalcolata delle permutazioni di un multiset
** con occorrenze {1,2}
*/
unsigned mperms[3][3] = {{0,1,2},{1,0,2},{1,2,0}};

/************************************************************************/
/*
** Stampa la partizione corrente e le relative permutazioni su a,b,c.
*/
/************************************************************************/
void Visit(unsigned a[], const unsigned m) {
    size_t i, j;

    Cnt_parts++;
    printf("%02d: %d ", Cnt_parts, a[0]);

    for (i = 1; i < m; ++i) {
        printf("+ %d ", a[i]);
    }

    /*
    ** Il caso di partizione in tre elementi identici non e'
    ** consentito dai vincoli del problema. Si esaminano i
    ** restanti tre casi.
    */
    if (a[0] == a[1]) {
        /* Partizione di tipo j+j+k, j>k */
        for (i = 0; i < 3; ++i) {
            Cnt_perms++;
            printf(FMT_PERMS, Cnt_perms);

            for (j = 0; j < 3; ++j) {
                printf("%c=%d ", 'a'+j, a[2-mperms[i][j]]);
            }
        }
    } else if (a[1] == a[2]) {
        /* Partizione di tipo j+k+k, j>k */
        for (i = 0; i < 3; ++i) {
            Cnt_perms++;
            printf(FMT_PERMS, Cnt_perms);

            for (j = 0; j < 3; ++j) {
                printf("%c=%d ", 'a'+j, a[mperms[i][j]]);
            }
        }
    } else {
        /* Partizione in tre elementi distinti */
        Cnt_unique++;

        for (i = 0; i < 6; ++i) {
            Cnt_perms++;
            printf(FMT_PERMS, Cnt_perms);

            for (j = 0; j < 3; ++j) {
                printf("%c=%d ", 'a'+j, a[perms[i][j]]);
            }
        }
    }

    puts("");
}
/************************************************************************/
/*
** Genera tutte le partizioni di n in esattamente m parti, con valore
** massimo non superiore ad u.
**
** Basato sull'algoritmo H dal volume 4 del TAOCP di Knuth.
*/
/************************************************************************/
void GenParts(const unsigned n, const unsigned m, const unsigned u) {
    size_t j, s, x;
    unsigned a[MAX_PARTS];

H1: /* [Initialize.] */
    a[0] = n -m +1;
    a[m] = m -1;

    for (j = 1; j < m; ++j) {
        a[j] = 1;
    }

H2: /* [Visit.] */

    if (a[0] > u) {
        goto H20;
    }

    Visit(a, m);
H20:

    if (a[1] > (a[0] - 2)) {
        goto H4;
    }

H3: /* [Tweak a[1] and a[2].] */
    a[0] -= 1;
    a[1] += 1;
    goto H2;
H4: /* [Find j.] */
    j = 2;
    s = a[0] + a[1] -1;

    while (a[j] > (a[0] -2)) {
        s += a[j];
        ++j;
    }

H5: /* [Increase a.] */

    if (j > m-1) {
        return;
    } else {
        x = a[j] +1;
        a[j] = x;
        --j;
    }

H6: /* [Tweak a[1]...a[j].] */

    while (j) {
        a[j] = x;
        s -= x;
        --j;
    }

    a[0] = s;
    goto H2;
}
/************************************************************************/
/*
** Risolve il 964° "Quesito con la Susi", pubblicato sulla rivista
** "La settimana enigmistica" N° 4577 del 12 dicembre 2019 come 4513°
** concorso settimanale.
** Calcola inoltre tutte le possibili attribuzioni di valori ai simboli
** a, b e c usati nella vignetta per caratterizzare le singole cifre dei
** numeri incogniti da sommare.
*/
/************************************************************************/
int main(void) {
    const unsigned mul = 111;
    unsigned s, p;
    bool unique[10];
    char val[10];

    Cnt_parts = 0;
    Cnt_perms = 0;
    Cnt_unique = 0;

    for (s = 11; s < 28; ++s) {
        size_t i;
        bool valid = true;

        p = mul * s;
        itoa(p, val, 10);

        for (i = 0; i < 10; ++i) {
            unique[i] = false;
        }

        for (i = 0; i < 4; ++i) {
            if (unique[val[i] - '0']) {
                valid = false;
                break;
            } else {
                unique[val[i] - '0'] = true;
            }
        }

        if (valid) {
            printf(">> Totale: %d = %d x %d\n\n"
                   "> Elenco delle partizioni di %d in 3 parti:\n",
                   p, mul, s, s);
            GenParts(s, 3, 9);
            printf("****************************************\n>> "
                   "Esistono %d diverse partizioni del numero %d che "
                   "risolvono il problema,\n>> a cui corrispondono"
                   " un totale di %d distinte attribuzioni di valori "
                   "ai\n>> simboli a, b e c. Di queste partizioni, "
                   "%d sono semplici ossia constano\n di parti tutte "
                   "distinte.",
                   Cnt_parts, s, Cnt_perms, Cnt_unique);
            break;
        }
    }

    return (0);
}
/*** EOF: susy77.c ***/