#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "snpyio.h"

static int reader(
    const char fname[],
    const size_t size_of_element,
    size_t * ndim,
    size_t ** shape,
    char ** dtype,
    bool * is_fortran_order,
    void ** data
){
  FILE * fp = fopen(fname, "r");
  if(NULL == fp){
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  // shape and dtype are now given by the library,
  //   we are responsible for their memory managements
  size_t header_size = snpyio_r_header(ndim, shape, dtype, is_fortran_order, fp);
  if(0 == header_size){
    printf("snpyio_r_header failed\n");
    exit(EXIT_FAILURE);
  }
  size_t nitems = 1;
  for(size_t i = 0; i < *ndim; i++){
    nitems *= (*shape)[i];
  }
  *data = calloc(nitems, size_of_element);
  if(NULL == *data){
    printf("memory allocation error (data)\n");
    exit(EXIT_FAILURE);
  }
  if(nitems != fread(*data, size_of_element, nitems, fp)){
    printf("fread failed\n");
    exit(EXIT_FAILURE);
  }
  fclose(fp);
  return 0;
}

static int writer(
    const char fname[],
    const size_t size_of_element,
    const size_t ndim,
    const size_t * shape,
    const char * dtype,
    const bool is_fortran_order,
    const void * data
){
  FILE * fp = fopen(fname, "w");
  if(NULL == fp){
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  size_t header_size = snpyio_w_header(ndim, shape, dtype, is_fortran_order, fp);
  if(0 == header_size){
    printf("snpyio_w_header failed\n");
    exit(EXIT_FAILURE);
  }
  size_t nitems = 1;
  for(size_t i = 0; i < ndim; i++){
    nitems *= shape[i];
  }
  if(nitems != fwrite(data, size_of_element, nitems, fp)){
    printf("fwrite failed\n");
    exit(EXIT_FAILURE);
  }
  fclose(fp);
  return 0;
}

int main(
    int argc,
    char * argv[]
){
  if(4 != argc){
    return 1;
  }
  const char * pfname = argv[1];
  const char * cfname = argv[2];
  const size_t size_of_element = (size_t)strtol(argv[3], NULL, 10);
  // array dimension
  size_t ndim = 0;
  // size in each dimension
  size_t * shape = NULL;
  // datatype, see https://numpy.org/doc/stable/reference/arrays.dtypes.html
  char * dtype = NULL;
  // memory ordering, usually false
  bool is_fortran_order = false;
  // data to be loaded
  void * data = NULL;
  // load
  reader(pfname, size_of_element, &ndim, &shape, &dtype, &is_fortran_order, &data);
  // dump
  writer(cfname, size_of_element,  ndim,  shape,  dtype,  is_fortran_order,  data);
  // clean-up buffers
  free(data);
  free(shape);
  free(dtype);
  return 0;
}

