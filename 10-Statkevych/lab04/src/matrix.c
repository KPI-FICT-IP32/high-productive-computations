#ifndef MATRIX_C
#define MATRIX_C

#include <stdlib.h>
#include <stdio.h>

struct matrix 
{
	int size;
	int width;
	int height;
	int start_row;
	float * data;	
};

typedef struct matrix matrix;

float get_value(matrix * m, int i, int j) {
	return m->data[i * m->size + j];
}

float * get_row_ptr(matrix * m, int start_row) {
	return m->data + (start_row * m->size);
}

matrix * get_rows(int world_size, int index, matrix * m) {
	matrix * out = malloc(sizeof(matrix));
	int per_row = m->size / world_size;
	int start_row = per_row * index;
	out->size = m->size;
	out->width = m->width;
	out->height = per_row;
	out->start_row = per_row * index;
	out->data = get_row_ptr(m, start_row);
	return out;
}

float get_value_shifted(matrix * m, int i, int j) {
	return m->data[(i - m->start_row) * m->size + j];
}

void set_value_shifted(matrix * m, int i, int j, float value) {
	if((i - m->start_row) * m->size + j > m->width * m->height) {
		printf("Catched, %f\n", value);
	}
	if((i - m->start_row) * m->size + j < 0) {
		printf("Catched less then 0 i=%d start=%d, %f\n", i, m->start_row, value);
	}
	m->data[(i - m->start_row) * m->size + j] = value;
}

matrix * read_matrix_from_file(char * file_name) {
	FILE * matrix_file = fopen(file_name, "r");
	matrix * m = malloc(sizeof(matrix));
	int matrix_size = 0;

	fscanf(matrix_file, "%d\n", &matrix_size);

	float * data = malloc(sizeof(float) * matrix_size * matrix_size);

	char * current_str = malloc(sizeof( char) * 256 * 256);

	for (int i = 0; i < matrix_size; i++) 
	{
		fgets(current_str, 256 * 256, matrix_file);
		int j = 0, n;
		float value;

		while (sscanf(current_str, "%f%n", &value, &n) == 1)
	    {
			*(data + (i * matrix_size + j++)) = value;
			current_str += n;
		}
	}

	fclose(matrix_file);

	m->size = matrix_size;
	m->width = matrix_size;
	m->height = matrix_size;
	m->data = data;
	m->start_row = 0;
	return m;
}

void print_matrix(matrix * m) {
	int sz = m->size;
	int height = m->height;

	for(int i = 0; i < height; i++) {
		printf("\n");
		for(int j = 0; j < sz; j++) {
			printf("(%f)\t", get_value(m, i, j));
		}
	}
	printf("\n");
}

#endif