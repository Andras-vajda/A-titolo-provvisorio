/* Implementazione dell'algoritmo H di Knuth: "partition n into m parts". */

#include <stdio.h>
#include <stdlib.h>

void GenParts(unsigned n, unsigned m, unsigned u)
{
    unsigned j, s, x;
    unsigned a[16];

    /* [Initialize.] */
H1:
    a[1] = n -m +1;
    a[m+1] = m -1;
    for (j = 2; j < m +1; ++j) {
        a[j] = 1;
    }

    /* [Visit.] */
H2:
    if (a[1] > u) {
        goto H20;
    }

    printf("%d ", a[1]);
    for (j = 2; j < m +1; ++j) {
        printf("+ %d ", a[j]);
    }
    puts("");

H20:
    if (a[2] > (a[1] - 2)) {
        goto H4;
    }

    /* [Tweak a[1] and a[2].] */
H3:
    a[1] -= 1;
    a[2] += 1;
    goto H2;

    /* [Find j.] */
H4:
    j = 3;
    s = a[1] + a[2] -1;
    while (a[j] > (a[1] -2)) {
        s += a[j];
        ++j;
    }


    /* [Increase a.] */
H5:
    if (j > m) {
        return;
    } else {
        x = a[j] +1;
        a[j] = x;
        --j;
    }

    /* [Tweak a[1]...a[j].] */
H6:
    while (j > 1) {
        a[j] = x;
        s -= x;
        --j;
    }
    a[1] = s;
    goto H2;
}

#ifdef TEST
int main()
{
    GenParts(11, 4, 8);
}
#endif
/* EOF: Hpart.c */
