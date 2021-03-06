#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include "simple_npyio.h"


// example, 2d data (array) with complex double type
typedef double complex mytype;

static int reader(const char fname[], size_t *ndim, size_t **shape, char **dtype, bool *is_fortran_order, mytype **data){
  FILE *fp = fopen(fname, "r");
  if(fp == NULL){
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  // shape and dtype are now given by the library,
  //   we are responsible for their memory managements
  size_t header_size = simple_npyio_r_header(ndim, shape, dtype, is_fortran_order, fp);
  if(header_size == 0){
    printf("simple_npyio_r_header failed\n");
    exit(EXIT_FAILURE);
  }else{
    printf("header is successfully loaded  (size: %zu)\n", header_size);
  }
  size_t nitems = 1;
  for(size_t i = 0; i < *ndim; i++){
    nitems *= (*shape)[i];
  }
  *data = calloc(nitems, sizeof(mytype));
  if(*data == NULL){
    printf("memory allocation error (data)\n");
    exit(EXIT_FAILURE);
  }
  assert(nitems == fread(*data, sizeof(mytype), nitems, fp));
  fclose(fp);
  return 0;
}

static int writer(const char fname[], const size_t ndim, const size_t *shape, const char *dtype, const bool is_fortran_order, const mytype *data){
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
  size_t nitems = 1;
  for(size_t i = 0; i < ndim; i++){
    nitems *= shape[i];
  }
  fwrite(data, sizeof(mytype), nitems, fp);
  fclose(fp);
  return 0;
}

int main(void){
  // array dimension
  size_t ndim;
  // size in each dimension
  size_t *shape = NULL;
  // datatype, see https://numpy.org/doc/stable/reference/arrays.dtypes.html
  char *dtype = NULL;
  // memory ordering, usually false
  bool is_fortran_order;
  // data to be loaded
  mytype *data = NULL;
  // load
  reader("npyfiles/created-by-python-2d.npy", &ndim, &shape, &dtype, &is_fortran_order, &data);
  // dump
  writer("npyfiles/created-by-c-2d.npy",       ndim,  shape,  dtype,  is_fortran_order,  data);
  /* clean-up buffers */
  free(data);
  free(shape);
  free(dtype);
  return 0;
}

