#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include "lu.h"


#define MPI_ROOT_PS 0
#define MPI_DATA_TAG 0x3


MPI_Datatype register_vector(int size) {
    MPI_Datatype MPI_Vector;
    MPI_Aint extent;
    MPI_Type_extent(MPI_INT, &extent);
    MPI_Aint offsets[] = {0, extent};
    int lengths[] = {1, size};

    MPI_Datatype oldtypes[] = {MPI_INT, MPI_DOUBLE};
    MPI_Type_create_struct(2, lengths, offsets, oldtypes, &MPI_Vector);
    MPI_Type_commit(&MPI_Vector);

    return MPI_Vector;
}


void lu(Matrix * m, int rank, int world_size) {
    MPI_Datatype MPI_Vector = register_vector(m->size);
    Vector *buf = alloc_vector(m);
    for (int i = 0; i < m->size - 1; i++) {
        if (i % world_size == rank) {
            for (int j = i; j < m->size; j++){
                buf->data[j] = get(m, j, i) / get(m, i, i);
            }
            buf->start_point = i;
        } 
        MPI_Bcast(buf, 1, MPI_Vector, i % world_size, MPI_COMM_WORLD);
        for (int j = i + 1; j < m->size; j++) {
            for(int k = i + 1; k < m->size; k++) {
                if (k % world_size != rank) { continue; }
                double row_value = get(m, i, k);
                double value = get(m, j, k);
                value -= buf->data[j] * row_value;
                set(m, j, k, value);
            }
        }
        if (i % world_size == rank || world_size == 1) {
            for(int j = i + 1; j < m->size; j++) {
                set(m, j, i, buf->data[j]);
            }
        }
    }

    int matrix_size = m->width * m->height;
    if (rank == MPI_ROOT_PS) {
        for (int ps = 1; ps < world_size; ps++) {
            double *data = (double *) malloc(sizeof(double) * matrix_size);
            MPI_Recv(data, matrix_size, MPI_DOUBLE, ps, MPI_DATA_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i<m->height; i++) {
                for(int j = ps; j<m->width; j+=world_size) {
                    set(m, i, j, data[i * m->size + j]);
                }
            }
        }
        // print_matrix(m);
    } else {
        MPI_Send(m->data, matrix_size, MPI_DOUBLE, MPI_ROOT_PS,
                 MPI_DATA_TAG, MPI_COMM_WORLD);
    }
}

double *calc_y(Matrix *m, double *b) {
    // Use backward substitution to calculate Y
    double *y = malloc(sizeof(double) * m->size);
    memcpy(y, b, sizeof(double) * m->size);
    for (int i = 0; i<m->size; i++) {
        for(int j = 0; j < i; j++) {
            y[i] -= get(m, i, j) * y[j];
        }
    }
    return y;
}

double * calc_x(Matrix *m, double *y) {
    // Use backward substitution to calculate X
    double *x = malloc(sizeof(double) * m->size);
    memcpy(x, y, sizeof(double) * m->size);
    for (int i = m->size-1; i>=0; i--) {
        for(int j = m->size-1; j > i; j--) {
            x[i] -= get(m, i, j) * x[j];
        }
        x[i] = x[i] / get(m, i, i);
    }
    return x;
}
