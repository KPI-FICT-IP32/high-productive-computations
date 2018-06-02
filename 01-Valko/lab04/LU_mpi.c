#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

#define ln() putchar('\n')
#define GENERIC_TAG (0)

float *gen_mx (size_t dim);
float *gen_mx_rect (size_t dim, size_t dim2);
float *gen_row(size_t dim);
float *gen_row_ref (size_t dim, size_t ref);
void print_mx (float *M, size_t dim, size_t sep);
void forw_elim(float **origin, float *master_row, size_t dim);
void U_print (float *M, int dim);
void L_print (float *M, int dim);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    const int root_p = 0;
    int mx_size = 0, p, id;
    if (argc < 2) {
      printf("Matrix size missing in the arguments\n");
      return EXIT_FAILURE;
    }
    mx_size = atol(argv[1]);
    float *A = gen_mx(mx_size);

    int mynode, totalnodes;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &totalnodes);
    MPI_Comm_rank(MPI_COMM_WORLD, &mynode);

    int i, j, k, index;
    double scaling;

    int numrows = mx_size / totalnodes;
    float *A_local = gen_mx_rect(mx_size, numrows);
    int *myrows = malloc(sizeof(int) * numrows);

    for (i=0; i < numrows; i++)
    {
        index = mynode + totalnodes * i;
        myrows[i] = index;
    }

    float *tmp = malloc(sizeof(float) * mx_size);
    float *x = malloc(sizeof(float) * mx_size);
    double start = MPI_Wtime();

    int cnt = 0;
    for (i=0; i < mx_size - 1; i++)
    {
        if(i == myrows[cnt])
        {
            MPI_Bcast(&A_local[cnt], mx_size + 1, MPI_FLOAT, mynode, MPI_COMM_WORLD);
            for(j = 0;j < mx_size + 1; j++) tmp[j] = A_local[cnt * mx_size+j];
            cnt++;
        }
        else
        {
            MPI_Bcast(tmp, mx_size + 1, MPI_FLOAT, i % totalnodes, MPI_COMM_WORLD);
        }
        for(j = cnt; j < numrows; j++)
        {
            scaling = A_local[j*mx_size +i]/tmp[i];
            for(k = i;k < mx_size + 1; k++)
            A_local[j * mx_size + k] = A_local[j * mx_size + k] - scaling * tmp[k];
        }
    }

    cnt = 0;
    for (i = 0; i < mx_size; i++)
    {
        if(i == myrows[cnt])
        {
            x[i] = A_local[cnt * mx_size + mx_size];
            cnt++;
        } else
            x[i] = 0;
    }

    cnt = numrows - 1;
    for (i = mx_size - 1; i > 0; i--)
    {
        if(cnt>=0)
        {
            if(i == myrows[cnt])
            {
                x[i] = x[i] / A_local[cnt * mx_size + i];
                MPI_Bcast(x + i, 1, MPI_DOUBLE, mynode, MPI_COMM_WORLD);
                cnt--;
            } else
                MPI_Bcast(x + i, 1, MPI_DOUBLE, i % totalnodes, MPI_COMM_WORLD);
        }
            else
                MPI_Bcast(x + i, 1,MPI_DOUBLE, i % totalnodes, MPI_COMM_WORLD);
            for(j = 0; j <= cnt; j++)
                x[myrows[j]] = x[myrows[j]] - A_local[j * mx_size + i] * x[i];
    }

    if(mynode==0)
    {
        x[0] = x[0] / A_local[cnt * mx_size];
        MPI_Bcast(x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    } else
        MPI_Bcast(x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (mynode == 0)
    {
        for (i = 0; i < mx_size; i++)
        {
            #ifdef ALU
            printf("x: %f s\n", x[i]);
            #endif
        }
        printf("mpi: %f s\n", end - start);
    }

    free(A);
    free(A_local);
    free(tmp);
    free(x);
    free(myrows);

    MPI_Finalize();
    return EXIT_SUCCESS;
}

/*
 * gen_mx - generate contiguous matrix
 *
 * @dim dim x dim matrix
 * @return matrix
 */
float *gen_mx (size_t dim)
{
    int i, j, tot = dim * dim;
    float *M = malloc(sizeof(float) * tot);
    for (i = 0; i < tot; i++) {
        M[i] = rand() % 101 - 50;
    }

    return M;
}

float *gen_mx_rect (size_t dim, size_t dim2)
{
    int i, j, tot = dim * dim2;
    float *M = malloc(sizeof(float) * tot);
    for (i = 0; i < tot; i++) {
        M[i] = rand() % 101 - 50;
    }

   return M;
}

/*
 * mx_print - dumb matrix print function
 *
 * @M matrix/row
 * @dim matrix/row dimension
 * @sep where put separator
 */
void print_mx (float *M, size_t dim, size_t sep)
{
    int i, j;
    for (i = 0; i < dim; i++) {
    //      printf("% *.*f\t", 4, 2, M[i]);
        if ((i + 1) % sep == 0) {
            ln();
        }
    }
}

/*
 * forw_elim - forward Gauss elimination
 *
 * @origin row pointer by reference
 * @master_row row in which lays diagonal
 */
void forw_elim(float **origin, float *master_row, size_t dim)
{
    if (**origin == 0)
        return;

    float k = **origin / master_row[0];

    int i;
    for (i = 1; i < dim; i++) {
        (*origin)[i] = (*origin)[i] - k * master_row[i];
    }
    **origin = k;
}

/*
 * U_print - dumb U matrix print function
 */
void U_print (float *M, int dim)
{
    int i, j;
    float z = 0;
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            if (j >= i) {
                printf("% *.*f\t", 4, 2, M[i * dim + j]);
            } else {
                printf("% *.*f\t", 4, 2, z);
            }
        }
        ln();
    }
}

/*
 * L_print - dumb L matrix print function
 */
void L_print (float *M, int dim)
{
    int i, j;
    float z = 0, u = 1;
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            if (j > i) {
                printf("% *.*f\t", 4, 2, z);
            } else if (i == j) {
                printf("% *.*f\t", 4, 2, u);
            } else {
                printf("% *.*f\t", 4, 2, M[i * dim + j]);
            }
        }
        ln();
    }
}
