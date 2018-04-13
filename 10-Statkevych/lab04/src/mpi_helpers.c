#ifndef MPI_HELPERS_C
#define MPI_HELPERS_C

#include <mpi.h>
#include <math.h>
#include "matrix.c"
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

matrix * get_matrix_parts(char * in_file, int rank, int world_size) {
	MPI_Aint disp[4];
	MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
	MPI_Datatype mpi_matrix_meta;
	int blocks[] = {sizeof(int),sizeof(int),sizeof(int),sizeof(int)};
	disp[0] = 0;
	disp[1] = sizeof(int);
	disp[2] = 2 * sizeof(int);
	disp[3] = 3 * sizeof(int);

	void * matrix_meta_bytes = malloc(sizeof(matrix) - sizeof(float));
	matrix * matrix_meta;

	MPI_Type_create_struct(4, blocks, disp, types, &mpi_matrix_meta);
	MPI_Type_commit(&mpi_matrix_meta);

	if(rank == 0) {
		matrix * m = read_matrix_from_file(in_file);
		matrix * to_return;
		
		print_matrix(m);

		for (int i = 0; i < world_size; i++) {
			matrix * matrix_meta = get_rows(world_size, i, m);
			if (i == 0) {
				to_return = matrix_meta;
			} else {
				MPI_Send(matrix_meta, 1, mpi_matrix_meta, i, 1, MPI_COMM_WORLD);
				MPI_Send(matrix_meta->data, matrix_meta->width * matrix_meta->height, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
			}
		}

		free(m->data);
		free(m);
		return to_return;
	} else {

		MPI_Recv(matrix_meta_bytes, 1, mpi_matrix_meta, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		matrix_meta = (matrix *) matrix_meta_bytes;
		matrix_meta->data = malloc(sizeof(float) * matrix_meta->width * matrix_meta->height);

		MPI_Recv(matrix_meta->data, matrix_meta->width * matrix_meta->height, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		return matrix_meta;
	}
}

int calculate_determinant(matrix * m, int rank, int world_size) {
	float * current_u_vector = malloc(m->size * sizeof(float));
	int start_index = -1;
	int np = m->size / world_size;

	for (int i = 0; i < m->size - 1; i++) {
		int root = i / np;
		if (rank == root) {
			for (int uj = i; uj < m->size; uj++) {
				current_u_vector[uj] = get_value_shifted(m, i, uj);
			}
			start_index = i;
			for (int dest = rank + 1; dest < world_size; dest++) {
	    		MPI_Send(&start_index, 1, MPI_INT, dest, 2, MPI_COMM_WORLD);
	    		MPI_Send(current_u_vector, m->size, MPI_INT, dest, 2, MPI_COMM_WORLD);
			}
		} else if (rank > root) {
	    	MPI_Recv(&start_index, 1, MPI_INT, root, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    		MPI_Recv(current_u_vector, m->size, MPI_FLOAT, root, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		} else {
			break;
		}

		for (int j = MAX(rank * np, start_index + 1); j < (rank + 1) * np; ++j)
		{
			float divisor = get_value_shifted(m, j, start_index);
			float ratio = divisor / current_u_vector[start_index];

			for(int k = start_index; k < m->size; k++) {
				// for each col
				float prev_value = get_value_shifted(m, j, k);
				prev_value -= ratio * current_u_vector[k];
				set_value_shifted(m, j, k, prev_value);
			}
		}
	}
	float prod = 1, total = 0;

	for(int i = rank * np; i<(rank + 1) * np; i++) {
		prod *= get_value_shifted(m, i, i);
	}
	MPI_Reduce(&prod, &total, 1, MPI_FLOAT, MPI_PROD, 0,
           MPI_COMM_WORLD);

	if(rank == 0) {
		printf("Determinant %f\n", total);
	}
	free(current_u_vector);
	print_matrix(m);
}

#endif
