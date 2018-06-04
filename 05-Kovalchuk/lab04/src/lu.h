#ifndef LU_H

#include "mio.h"
void lu(Matrix *m, int rank, int partitions);
double *calc_y(Matrix *m, double *b);
double *calc_x(Matrix *m, double *y);

#define LU_H
#endif
