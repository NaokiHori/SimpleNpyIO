#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "simple_npyio.h"


// two-dimensional array
#define NDIM 2

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
  // size in each dimension, outer first, to inner last
  const size_t shape[NDIM] = {ny, nx};
  // the above is a safer option so to say
  // you can use the following a bit flexible statement instead,
  //   AS LONG AS YOU ALLOCATE (and free) IT PROPERLY
  // "shape" should have memory of sizeof(size_t)*NDIM,
  //   but checking it internally is almost impossible
  // size_t *shape = NULL;
  // also be sure to free memory later
  /* size_t *shape = calloc(NDIM, sizeof(size_t)); */
  /* if(shape == NULL){ */
  /*   printf("memory allocation error (shape)\n"); */
  /*   exit(EXIT_FAILURE); */
  /* } */
  // datatype, see https://numpy.org/doc/stable/reference/arrays.dtypes.html
  const char dtype[] = {"'<i4'"};
  // memory ordering, usually false
  const bool is_fortran_order = false;
  /* open file, create/write header, write data, and close it */
  FILE *fp = fopen(fname, "w");
  if(fp == NULL){
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  size_t header_size = simple_npyio_w_header(NDIM, shape, dtype, is_fortran_order, fp);
  if(header_size == 0){
    printf("simple_npyio_w_header failed\n");
    exit(EXIT_FAILURE);
  }else{
    printf("header is successfully written (size: %zu)\n", header_size);
  }
  fwrite(data, sizeof(int), nx*ny, fp);
  fclose(fp);
  /* clean-up memories */
  free(data);
  /* free(shape); */
  return 0;
}

#undef NDIM

