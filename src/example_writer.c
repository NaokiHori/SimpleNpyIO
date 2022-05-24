#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "simple_npyio.h"


int example_writer(const char fname[]){
  /* create dataset to be written */
  //  0  1  2  3  4
  //  5  6  7  8  9
  // 10 11 12 13 14
  int *data = NULL;
  const int nx = 5;
  const int ny = 3;
  data = calloc(nx*ny, sizeof(int));
  if(data == NULL){
    printf("memory allocation error (data)\n");
    exit(EXIT_FAILURE);
  }
  for(int j = 0; j < ny; j++){
    for(int i = 0; i < nx; i++){
      data[j * nx + i] = j * nx + i;
    }
  }
  // output from C to compare with python output
  printf("data (written):\n");
  for(int j = 0; j < ny; j++){
    for(int i = 0; i < nx; i++){
      char eol = i == nx - 1 ? '\n' : ' ';
      printf("%3d%c", data[j * nx + i], eol);
    }
  }
  /* variables which are needed to create npy file */
  // two-dimensional array
  const size_t ndim = 2;
  // size in each dimension, assigned later
  size_t *shape = NULL;
  // datatype, see https://numpy.org/doc/stable/reference/arrays.dtypes.html
  const char dtype[] = {"'<i4'"};
  // memory ordering, usually false
  const bool is_fortran_order = false;
  /* shape depends on your data */
  shape = calloc(ndim, sizeof(size_t));
  if(shape == NULL){
    printf("memory allocation error (shape)\n");
    exit(EXIT_FAILURE);
  }
  // size in each dimension, assigned later
  shape[0] = ny; // outer first
  shape[1] = nx; // inner last
  /* open file, create/write header, write data, and close it */
  FILE *fp = fopen(fname, "w");
  if(fp == NULL){
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  size_t header_size = simple_npyio_w_header(ndim, shape, dtype, is_fortran_order, fp);
  if(header_size == 0){
    printf("simple_npyio_w_header failed\n");
    exit(EXIT_FAILURE);
  }else{
    printf("header is successfully written (size: %zu)\n", header_size);
  }
  fwrite(data, sizeof(int), nx*ny, fp);
  fclose(fp);
  /* clean-up memories */
  free(shape);
  free(data);
  return 0;
}

