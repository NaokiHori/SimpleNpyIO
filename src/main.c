#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "simple_npyio.h"


// type of array
typedef int mytype;

static int example_writer(const char fname[]){
  /* create dataset to be written */
  //  0  1  2  3  4
  //  5  6  7  8  9
  // 10 11 12 13 14
  mytype *data = NULL;
  const int nx = 5;
  const int ny = 3;
  data = calloc(nx*ny, sizeof(mytype));
  if(data == NULL){
    printf("memory allocation error (data)\n");
    exit(EXIT_FAILURE);
  }
  for(int j = 0; j < ny; j++){
    for(int i = 0; i < nx; i++){
      data[j * nx + i] = j * nx + i;
    }
  }
  // output data which will be written just after this
  printf("data (written):\n");
  for(int j = 0; j < ny; j++){
    for(int i = 0; i < nx; i++){
      char eol = i == nx - 1 ? '\n' : ' ';
      printf("%3d%c", data[j * nx + i], eol);
    }
  }
  /* prepare arguments which are needed by library */
  // number of dimension of dataset
  const size_t ndim = 2;
  // size in each dimension, outer first, to inner last
  size_t *shape = calloc(ndim, sizeof(size_t));
  if(shape == NULL){
    printf("memory allocation error (shape)\n");
    exit(EXIT_FAILURE);
  }
  shape[0] = ny;
  shape[1] = nx;
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
  size_t header_size = simple_npyio_w_header(ndim, shape, dtype, is_fortran_order, fp);
  if(header_size == 0){
    printf("simple_npyio_w_header failed\n");
    exit(EXIT_FAILURE);
  }else{
    printf("header is successfully written (size: %zu)\n", header_size);
  }
  fwrite(data, sizeof(mytype), nx*ny, fp);
  fclose(fp);
  /* clean-up memories */
  free(data);
  free(shape);
  return 0;
}

static int example_reader(const char fname[]){
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
  mytype *data = NULL;
  /* open file, read header, read data, and close it */
  FILE *fp = fopen(fname, "r");
  if(fp == NULL){
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  size_t header_size = simple_npyio_r_header(&ndim, &shape, &dtype, &is_fortran_order, fp);
  if(header_size == 0){
    printf("simple_npyio_r_header failed\n");
    exit(EXIT_FAILURE);
  }else{
    printf("header is successfully loaded  (size: %zu)\n", header_size);
  }
  data = calloc(shape[0]*shape[1], sizeof(mytype));
  if(data == NULL){
    printf("memory allocation error (data)\n");
    exit(EXIT_FAILURE);
  }
  fread(data, sizeof(mytype), shape[0]*shape[1], fp);
  fclose(fp);
  int ny = (int)(shape[0]);
  int nx = (int)(shape[1]);
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
  free(dtype);
  free(data);
  return 0;
}

int main(void){
  const char fname[] = {"example.npy"};
  example_writer(fname);
  example_reader(fname);
  return 0;
}

