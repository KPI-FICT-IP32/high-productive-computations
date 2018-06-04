#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "mio.h"
#include "lu.h"

#define MPI_TAG_HEADER 0x1
#define MPI_TAG_DATA 0x2


MPI_Datatype register_matrix_description() {
    MPI_Aint offsets[3] = {0, sizeof(int), 2*sizeof(int)};
    MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_INT};
    int sizes[3] = {sizeof(int), sizeof(int), sizeof(int)};

    MPI_Datatype MPI_Matrix_description;
    MPI_Type_create_struct(3, sizes, offsets, types, &MPI_Matrix_description);
	MPI_Type_commit(&MPI_Matrix_description);

    return MPI_Matrix_description;
}


Matrix *get_matrix(char * filename, int rank, int world_size) {
    MPI_Datatype MPI_Matrix_description = register_matrix_description();

	void *matrix_header = malloc(sizeof(Matrix));
	Matrix *m;

	if (rank == 0) {
		m = read_matrix(filename);
		for (int ps = 1; ps < world_size; ps++) {
            // send matrix header to target process
            MPI_Send(m, 1, MPI_Matrix_description,
                     ps, MPI_TAG_HEADER, MPI_COMM_WORLD);
            // send matrix data to target process
            MPI_Send(m->data, m->width * m->height, MPI_DOUBLE, 
                     ps, MPI_TAG_DATA, MPI_COMM_WORLD);
		}
	} else {
        int source_ps = 0;
		MPI_Recv(matrix_header, 1, MPI_Matrix_description,
                 source_ps, MPI_TAG_HEADER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		m = (Matrix *)matrix_header;
		m->data = malloc(sizeof(double) * m->width * m->height);
		MPI_Recv(m->data, m->width * m->height, MPI_DOUBLE,
                 source_ps, MPI_TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
    return m;
}


int main(int argc, char **argv) {
    // Initialize MPI env
    MPI_Init(NULL, NULL);

    // Get world info
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double t1, t2; 
    t1 = MPI_Wtime(); 

    // read in
    Matrix *m = get_matrix("matrix.in", world_rank, world_size);

    lu(m, world_rank, world_size);

    if (world_rank == 0) {
        double *b = read_vector("b.in");
        double *y = calc_y(m, b);
        double *x = calc_x(m, y);

        printf("Result: ");
        for(int i = 0; i< m->size; i++) {
            printf("%lf\t", x[i]);
        }
        printf("\n");
    }
    t2 = MPI_Wtime();

    if (world_rank == 0) {
        printf("Processes %d; Elapsed time is %lf\n", world_size, t2 - t1); 
    }

    MPI_Finalize();
    return 0;
}
