#include <stdlib.h>
#include <stdio.h>
#include "mio.h"

#define _BUFSIZE 65536


Matrix *read_matrix(char *filename) {
    FILE *f = fopen(filename, "r");
    Matrix *m = malloc(sizeof(Matrix));

    fscanf(f, "%d\n", &(m->size));
    m->width = m->height = m->size;
    m->data = malloc(sizeof(double) * m->width * m->height);

    char *buf = malloc(sizeof(char) * _BUFSIZE);
    char *buf_ptr;

    for (int r = 0; r < m->height; r++) {
        buf_ptr = buf;
        fgets(buf, _BUFSIZE, f);
        int c = 0;
        int n;
        double val;

        while (sscanf(buf_ptr, "%lf%n", &val, &n) == 1) {
            set(m, r, c++, val);
            buf_ptr += n;
        }
    }
    fclose(f);

    free(buf);
    buf = buf_ptr = NULL;

    return m;
}


double get(Matrix *m, int r, int c) {
    return m->data[r * m->height + c];
}

void set(Matrix *m, int r, int c, double val) {
    m->data[r * m->height + c] = val;
}

void print_matrix(Matrix *m) {
    printf("Matrix[size=%d; width=%d' height=%d]\n",
           m->size, m->width, m->height);
    for (int r = 0; r < m->height; r++) {
        for (int c = 0; c < m->width; c++) {
            printf("%lf\t", get(m, r, c));
        }
        printf("\n");
    }
}


Vector *alloc_vector(Matrix *m) {
    Vector *vec = malloc(sizeof(Vector) + (m->size - 1) * sizeof(double));
    return vec;
}

double *read_vector(char * filename) {
    FILE *f = fopen(filename, "r");
    int vector_size = 0;

    fscanf(f, "%d\n", &vector_size);

    double *data = malloc(sizeof(double) * vector_size);

    char *buf = malloc(sizeof(char) * _BUFSIZE);
    char *buf_ptr = buf;

    fgets(buf, _BUFSIZE, f);
    int j = 0, n;
    double value;

    while (sscanf(buf_ptr, "%lf%n", &value, &n) == 1) {
        *(data +  j++) = value;
        buf_ptr += n;
    }

    fclose(f);
    free(buf);
    buf = buf_ptr = NULL;

    return data;
}
