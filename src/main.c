#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "snpyio.h"

// type of array
typedef int mytype;

static void print_array (
    const char title[],
    const size_t nx,
    const size_t ny,
    const int * const data
) {
  printf("%s\n", title);
  for (size_t j = 0; j < ny; j++) {
    for (size_t i = 0; i < nx; i++) {
      char eol = nx - 1 == i ? '\n' : ' ';
      printf("%3d%c", data[j * nx + i], eol);
    }
  }
}

static int example_writer (
    const char fname[]
) {
  // create dataset to be dumped
  //  0  1  2  3  4
  //  5  6  7  8  9
  // 10 11 12 13 14
  // size of array, sparse first, contiguous last
  const size_t nx = 5;
  const size_t ny = 3;
  mytype * data = NULL;
  data = calloc(nx * ny, sizeof(mytype));
  if (NULL == data) {
    printf("memory allocation error (data)\n");
    exit(EXIT_FAILURE);
  }
  for (size_t j = 0; j < ny; j++) {
    for (size_t i = 0; i < nx; i++) {
      data[j * nx + i] = (int)(j * nx + i);
    }
  }
  print_array("data (dumped)", nx, ny, data);
  // array dimension
  const size_t ndim = 2;
  // size in each dimension
  const size_t shape[] = {ny, nx};
  // datatype, see https://numpy.org/doc/stable/reference/arrays.dtypes.html
  const char dtype[] = {"'<i4'"};
  // memory ordering, usually false
  const bool is_fortran_order = false;
  // open file, create/write header, write data, and close it
  FILE * fp = fopen(fname, "w");
  if (NULL == fp) {
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  size_t header_size = 0;
  const int retval = snpyio_w_header(
      ndim,
      shape,
      dtype,
      is_fortran_order,
      fp,
      &header_size
  );
  if (0 != retval) {
    printf("snpyio_w_header failed\n");
    exit(EXIT_FAILURE);
  }
  printf("header is successfully dumped (size: %zu)\n", header_size);
  if (nx * ny != fwrite(data, sizeof(mytype), nx * ny, fp)) {
    printf("fwrite failed\n");
    exit(EXIT_FAILURE);
  }
  fclose(fp);
  free(data);
  return 0;
}

static int example_reader (
    const char fname[]
) {
  // variables which are loaded from npy file
  // array dimension
  size_t ndim = 0;
  // size in each dimension
  size_t * shape = NULL;
  // datatype, see https://numpy.org/doc/stable/reference/arrays.dtypes.html
  char * dtype = NULL;
  // memory ordering, usually false
  bool is_fortran_order = false;
  // data to be loaded
  mytype * data = NULL;
  // open file, read header, read data, and close it
  FILE * fp = fopen(fname, "r");
  if (NULL == fp) {
    printf("file open error: %s\n", fname);
    exit(EXIT_FAILURE);
  }
  size_t header_size = 0;
  const int retval = snpyio_r_header(
      &ndim,
      &shape,
      &dtype,
      &is_fortran_order,
      fp,
      &header_size
  );
  if (0 != retval) {
    printf("snpyio_r_header failed\n");
    exit(EXIT_FAILURE);
  } else {
    printf("header is successfully loaded (size: %zu)\n", header_size);
  }
  data = calloc(shape[0] * shape[1], sizeof(mytype));
  if (NULL == data) {
    printf("memory allocation error (data)\n");
    exit(EXIT_FAILURE);
  }
  if (shape[0] * shape[1] != fread(data, sizeof(mytype), shape[0] * shape[1], fp)) {
    printf("fread failed\n");
    exit(EXIT_FAILURE);
  }
  fclose(fp);
  print_array("data (loaded)", shape[1], shape[0], data);
  // clean-up memories
  free(shape);
  free(dtype);
  free(data);
  return 0;
}

int main (
    void
) {
  const char fname[] = {"example.npy"};
  example_writer(fname);
  example_reader(fname);
  return 0;
}

