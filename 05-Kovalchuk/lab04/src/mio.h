#ifndef MIO_H

struct Matrix {
    int size;
    int width;
    int height;
    double *data;
};
typedef struct Matrix Matrix;

Matrix *read_matrix(char *filename);
double get(Matrix *m, int r, int c);
void set(Matrix *m, int r, int c, double val);
void print_matrix(Matrix *m);


struct Vector {
    int start_point;
    double data[1];
};
typedef struct Vector Vector;

Vector *alloc_vector(Matrix *m);
double *read_vector(char *filename);

#define MIO_H
#endif
