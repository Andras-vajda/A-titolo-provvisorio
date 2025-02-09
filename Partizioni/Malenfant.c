#include <stdio.h>
#include <stdlib.h>

#define MAX_PENTA 11
#define MAX_PART  51

const size_t GP[] = {0, 1, 2, 5, 7, 12, 15, 22, 26, 35, 40};

int P[MAX_PART];
int Coef[MAX_PENTA];

void EnumPartitions2(int m)
/*
**  Algorithm 3.6
**  compute the partition numbers P[i] for i <= m
*/
{
    int i;

    P[0] = 1;
    P[1] = 1;

    for(i = 2; i <= m; ++i)
    {
        int j, sum, omega, omega2, sign;

        sign  = 1;
        omega = 1;
        sum   = 0;
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

int main()
{
    size_t i;
    size_t M;
    
    EnumPartitions2(50);
    
    printf("n |p(n)   |M |gp(M)|Dq(n,M)     |");
    for (i = 1; i < MAX_PENTA; ++i)
    {   
        printf("%3d", GP[i]);
    }    
    puts("\n-------------------------------------------------------------------------");
    
    for (i = 1, M = 1; i < 51; ++i)
    {
        size_t j;
        size_t Dq = 1;
        if (GP[M+1] == i)
        {                          
            ++M;                   
        }        

        for (j = 1; j <= M; ++j)
        {
            Coef[j] = 1 + i / GP[j];
            Dq *= Coef[j];
        }
        printf("%2d|%7d|%2d|%5d|%12d|", i, P[i], M, GP[M], Dq);
        for (j = 1; j <= M; ++j)
        {
            printf("%3d", Coef[j]);
        }
        puts("");
    };
    
    return EXIT_SUCCESS;
}



