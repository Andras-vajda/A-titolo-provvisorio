#include <stdio.h>
#include "gpn_table.h"

#define MAX_INTEGER 57

unsigned GP[] = {1,1,2,2,3,4,5,7,8,10,12,14,18,20,25,29,34,40,45,
                 53,60,69,80,89,103,114,131,147,165,186,207,232,
                 258,286,319,352,392,432,477,525,578,636,699,765,
                 839,916,1002,1093,1192,1298,1413,1536,1671,1810,
                 1965,2126,2304};

unsigned long int P[MAX_INTEGER];

void EnumPartitions2(int m)
/*
**  Algorithm 3.6
**  compute the partition numbers P[i] for i <= m
*/
{
    int i;

    P[0] = 1;
    P[1] = 1;

    for (i = 2; i <= m; ++i)
    {
        int j, omega, omega2, sign;
        unsigned long int sum;

        sign  = 1;
        omega = 1;
        sum   = 0L;
        j     = 1;

        while (omega <= i)
        {
            omega2 = omega + j;

            sum += sign * P[i - omega];

            if (omega2 <= i)
            {
                sum += sign * P[i - omega2];
            }

            omega += 3 * j + 1;
            ++j;
            sign = -sign;
        }
        P[i] = sum;
    }
}

int main(void)
{
    int i, j;
    int M = 0;

    EnumPartitions2(MAX_INTEGER);

    printf("n  M  q(M) GP(n) P(n)   D(n,M)   ");
    for (i = 0; i < 12; ++i)
    {
        printf("%2d ", GPN[i]);
    }

    puts("\n------------------------------------"
         "------------------------------------");

    for (j = 1; j < MAX_INTEGER; ++j)
    {
        unsigned long int r, res = 1L;
        unsigned long prod[16];
        int k = 0;

        if (j == GPN[M +1])
        {
            ++M;
        }

        printf("%2d %2d %2d %5u %7lu ", j, M+1, GPN[M], GP[j], P[j]);
        for (i = 0; i <= M; ++i)
        {
            r = (unsigned long)(j / GPN[i]) + 1L;
            prod[k++] = r;
            res *= r;
        }

        printf("%10lu ", res);

        for (i = 0; i < k; ++i)
        {
            printf("%2d ", prod[i]);
        }
        puts("");
    }
}
