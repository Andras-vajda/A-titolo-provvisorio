#include <stdio.h>
#include <stdlib.h>

#define PART_SIZE 128

// Contatore globale delle partizioni generate
size_t cnt;

/****************************************************************************/
void DisplayPart(unsigned int c[], size_t s) {
    size_t i;

    printf("%5u: ", ++cnt);

    for (i = 1; i <= s; ++i) {
        printf("%3u", c[i]);
    }

    puts("");
}
/****************************************************************************/

void AccelDesc(unsigned int n) {
    size_t i, j, k, m;
    unsigned int part[PART_SIZE];

    cnt = 0L;

    for (i = 2; i <= n; ++i) {
        part[i] = 1;
    }

    i = 1;
    j = 1;
    part[1] = n;

    printf("\n** AccelDesc(%u)\n", n);

    DisplayPart(part, 1);

    while (j != 0) {
        if (part[j] == 2) {
            part[j] = 1;
            --j;
            ++i;
        } else {
            part[j] -= 1;
            m = part[j];
            k = i -j +1;

            while (k >= m) {
                part[++j] = m;
                k -= m;
            }

            if (k == 0) {
                i = j;
            } else {
                i = j +1;

                if (k > 1) {
                    part[++j] = k;
                }
            }
        }
        DisplayPart(part, i);
    }
}
/****************************************************************************/
// Zoghbi & Stojmenovic
void AccelAsc(unsigned int n) {
    size_t i, j, k, m;
    unsigned int part[PART_SIZE];

    cnt = 0L;

    i = 2;
    j = n -1;
    part[1] = 0;

    printf("\n** AccelAsc(%u)\n", n);

    while (i != 1) {
        --i;
        k = part[i] + 1;

        while (k<<1 <= j) {
            part[i] = k;
            j -= k;
            ++i;
        }

        m = i +1;

        while (k <= j) {
            part[i]  = k;
            part[m] = j;
            DisplayPart(part, m);
            ++k;
            --j;
        }

        j += k -1;
        part[i] = j +1;
        DisplayPart(part, i);
    }
}
/****************************************************************************/
/****************************************************************************/
int main(void) {
    size_t i;
    const unsigned test_vect[] = {5, 12, 32};

    for (i = 0; i < 3; ++i) {
        AccelDesc(test_vect[i]);
        AccelAsc(test_vect[i]);
    }

    return (EXIT_SUCCESS);
}
/****************************************************************************/
/* EOF: AccAsc.c                                                            */
/****************************************************************************/
