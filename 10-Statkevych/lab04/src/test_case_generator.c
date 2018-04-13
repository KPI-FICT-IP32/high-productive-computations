#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	int N = 20;
	int rand_range = 1000;
	
	if (argc > 1) {
		N = atoi(argv[1]);
        }
	if (argc > 2) {
  		rand_range = atoi(argv[2]);
  	}

	srand(time(NULL));   // should only be called once

	FILE * outfile = fopen("test_matrix.txt", "w+");
	printf("Matrix generation %d * %d started.\n", N, N);
	

	fprintf(outfile, "%d\n", N);

	for (int i = 0; i < N * N; i++) {
		int value = rand() % 1000;

		if (i != 0 && i % N == 0) {
			fprintf(outfile, "\n");
			printf("\n");
		} 
		fprintf(outfile, "%d ", value);
		printf("%d\t", value);
	}
	fclose(outfile);
	return 0;
}
