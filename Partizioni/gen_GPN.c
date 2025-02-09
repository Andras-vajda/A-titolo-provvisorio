#include <stdio.h>

#define MAX_GPN 103

int main(void)
{
	int j;
	unsigned long int gpn1, gpn2;
	
	printf("#define MAX_GPN %d\n\nunsigned long int GPN[MAX_GPN] = {\n", 2 * (MAX_GPN -1));
	for (j = 1; j < MAX_GPN; ++j)
	{
		gpn1 = j*(3*j-1)/2;
		gpn2 = j*(3*j+1)/2;
		printf("%5lu, %5lu%s", 
		       gpn1, gpn2,
		       j == MAX_GPN -1 ? "};\n" : ", ");
		       
		if (0 == (j % 6))
		{
			puts("");
		}
	}
	
	return(0);
}