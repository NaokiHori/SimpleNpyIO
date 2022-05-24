#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "simple_npyio.h"


int example_reader(const char fname[]){
  /* variables which are loaded from npy file */
  // array dimension
  size_t ndim;
  // size in each dimension
  size_t *shape = NULL;
  // datatype, see https://numpy.org/doc/stable/reference/arrays.dtypes.html
  char *dtype = NULL;
  // memory ordering, usually false
  bool is_fortran_order;
  // data to be loaded
  int *data = NULL;
  /* open file, read header, read data, and close it */
  FILE *fp = fopen(fname, "r");
  if(fp == NULL){
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  simple_npyio_r_header(&ndim, &shape, &dtype, &is_fortran_order, fp);
  data = calloc(shape[0]*shape[1], sizeof(int));
  if(data == NULL){
    printf("memory allocation error (data)\n");
    exit(EXIT_FAILURE);
  }
  fread(data, sizeof(int), shape[0]*shape[1], fp);
  fclose(fp);
  int ny = shape[0];
  int nx = shape[1];
  // output from C to compare with python output
  printf("data (loaded):\n");
  for(int j = 0; j < ny; j++){
    for(int i = 0; i < nx; i++){
      char eol = i == nx - 1 ? '\n' : ' ';
      printf("%3d%c", data[j * nx + i], eol);
    }
  }
  /* clean-up memories */
  free(shape);
  free(data);
  return 0;
}

