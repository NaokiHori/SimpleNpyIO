#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "simple_npyio.h"


// check 1 byte is 8 bits
#if CHAR_BIT != 8
#error "CHAR_BIT is not 8"
#endif

// all npy files should start from this magic string
#define MAGIC_STRING "\x93NUMPY"

// end-of-string
#define NUL '\x00'

// memory allocation
#define ALLOCATE(ptr, size){                                        \
  if(((ptr) = calloc(1, (size))) == NULL){                          \
    fprintf(stderr, "[ERROR] line %d in %s: ", __LINE__, __FILE__); \
    fprintf(stderr, "memory allocation of %s\n", #ptr);             \
    exit(EXIT_FAILURE);                                             \
  }                                                                 \
}

// memory deallocation
#define DEALLOCATE(ptr){\
  free((ptr));          \
  (ptr) = NULL;         \
}

#if defined(LOGGING_SIMPLE_NPYIO)
#define LOG_PREFIX "[SIMPLE_NPYIO writer]"
#endif

// https://github.com/NaokiHori/CSnippets/blob/master/src/check_endian.c
static bool is_big_endian(void){
  const uint16_t val = 1 << 8;
  return (bool)(((uint8_t *)(&val))[0]);
}

// https://github.com/NaokiHori/CSnippets/blob/master/src/convert_endian.c
static int convert_endian(void *val, const size_t size){
  size_t n_bytes = size/sizeof(uint8_t);
  uint8_t *buf = calloc(n_bytes, sizeof(uint8_t));
  for(size_t i = 0; i < n_bytes; i++){
    buf[i] = ((uint8_t *)val)[n_bytes-i-1];
  }
  memcpy(val, buf, size);
  free(buf);
  return 0;
}

static int create_descr_value(char **value, const char dtype[]){
  /*
   * Create a value of a dictionary key: "descr",
   * containing user-specified dtype with fore-aft single quotations (2) + NUL (1),
   * e.g., '<f8' or 'float64'
   */
  // "+3" is for two quotations and NUL
  size_t n_chars = strlen(dtype)+3;
  ALLOCATE(*value, sizeof(char)*n_chars);
  // first character is a single quotation
  (*value)[        0] = '\'';
  // last-1 character is a single quotation
  (*value)[n_chars-2] = '\'';
  // last character is NUL
  (*value)[n_chars-1] = NUL;
  // copy dtype in between single quotations
  memcpy((*value)+1, dtype, sizeof(char)*strlen(dtype));
  return 0;
}

static int create_fortran_order_value(char **value, const bool is_fortran_order){
  /*
   * Create a value of a dictionary key: "fortran_order",
   * which is True or False with last NUL
   */
  if(is_fortran_order){
    const char string[] = {"True"};
    // "True" + NUL
    size_t n_chars = strlen(string)+1;
    ALLOCATE(*value, sizeof(char)*n_chars);
    memcpy(*value, string, sizeof(char)*strlen(string));
    (*value)[n_chars-1] = NUL;
  }else{
    const char string[] = {"False"};
    // "False" + NUL
    size_t n_chars = strlen(string)+1;
    ALLOCATE(*value, sizeof(char)*n_chars);
    memcpy(*value, string, sizeof(char)*strlen(string));
    (*value)[n_chars-1] = NUL;
  }
  return 0;
}

static int create_shape_value(char **value, const size_t ndim, const size_t *shape){
  /*
   * Create a value of a dictionary key: "shape"
   * Examples:
   * 0D array: ndim = 0, *dims = NULL  -> ()
   * 1D array: ndim = 1, *dims = {5}   -> (5,)
   * 2D array: ndim = 2, *dims = {5,2} -> (5,2,)
   * NOTE: from left to right,
   *       from outer (less contiguous) to inner (contiguous)
   */
  // count number of digits (e.g., 5: 1 digit, 15: 2 digits) of shape in each direction
  size_t *n_digits = NULL;
  ALLOCATE(n_digits, sizeof(size_t)*ndim);
  for(size_t i = 0; i < ndim; i++){
    // sanitise input, size of array in each dimention should be positive
    if(shape[i] <= 0){
      printf("shape in %zu-th dimention is non-positive: %zu\n", i, shape[i]);
      printf("modify to a positive number\n");
      exit(EXIT_FAILURE);
    }
    size_t num = shape[i];
    n_digits[i] = 1;
    while(num /= 10){
      n_digits[i]++;
    }
  }
  // now we know number of characters (memory size to be allocated)
  size_t n_chars = 3; // at least "(", ")", and "NUL" exist
  for(size_t i = 0; i < ndim; i++){
    // number of digits in the corresponding direction
    // with comma (+1)
    n_chars += n_digits[i]+1;
  }
  // allocate memory and assign values
  ALLOCATE(*value, sizeof(char)*n_chars);
  for(size_t i = 0, offset = 1; i < ndim; i++){
    // assign size of the array in each direction to "value"
    //   after converting the integer to characters, e.g., 128 -> "128"
    char *buf = NULL;
    // + "," and "NUL"
    ALLOCATE(buf, sizeof(char)*(n_digits[i]+2));
    sprintf(buf, "%zu,", shape[i]); // including ","
    memcpy((*value)+offset, buf, sizeof(char)*strlen(buf));
    offset += strlen(buf);
    DEALLOCATE(buf);
  }
  // first character is a parenthesis
  (*value)[        0] = '(';
  // last-1 character is a parenthesis
  (*value)[n_chars-2] = ')';
  // last character is NUL
  (*value)[n_chars-1] = NUL;
  // clean-up buffer
  DEALLOCATE(n_digits);
  return 0;
}

static int create_dict(char **dict, size_t *n_dict, const size_t ndim, const size_t *shape, const char dtype[], const bool is_fortran_order){
  /*
   * "dict" contains information which is necessary to recover the original array,
   *   1. datatype, 2. memory ordering, and 3. shape of the data
   * It is a python-like dictionary, whose structure is a pair of key and value:
   * --- -------------- -----------------------------------------------------------------
   *  1.  descr         Datatype, e.g., '<f8', 'float64'
   *  2.  fortran_order Memory order, True or False (usually False)
   *  3.  shape         A tuple having number of elements in each direction, e.g., (5,2,)
   * See corresponding function for how they are created
   * Also the number of elements of the dict is returned (to be consistent with "create_padding")
   */
  // keys, which are completely fixed
  const char descr_key[] = {"'descr'"};
  const char fortran_order_key[] = {"'fortran_order'"};
  const char shape_key[] = {"'shape'"};
  // values, which depend on inputs
  char *descr_value = NULL;
  char *fortran_order_value = NULL;
  char *shape_value = NULL;
  // 1. create dictionary values
  create_descr_value(&descr_value, dtype);
  create_fortran_order_value(&fortran_order_value, is_fortran_order);
  create_shape_value(&shape_value, ndim, shape);
  // 2. assign all elements (strings) which compose dict
  const size_t n_elements_dict = 13;
  char **elements = NULL;
  ALLOCATE(elements, sizeof(char *)*n_elements_dict);
  // initial wave bracket
  elements[ 0] = "{";
  // 'descr':descr_value
  elements[ 1] = (char *)descr_key;
  elements[ 2] = ":";
  elements[ 3] = (char *)descr_value;
  elements[ 4] = ",";
  // 'fortran_order':fortran_order_value
  elements[ 5] = (char *)fortran_order_key;
  elements[ 6] = ":";
  elements[ 7] = (char *)fortran_order_value;
  elements[ 8] = ",";
  // 'shape':shape_value
  elements[ 9] = (char *)shape_key;
  elements[10] = ":";
  elements[11] = (char *)shape_value;
  // final wave bracket
  elements[12] = "}";
  // 3. check total number of characters of
  //   "{'descr':VALUE,'fortran_order':VALUE,'shape':VALUE}"
  //   to allocate dict
  // NOTE: n_chars_dict is the number of characters of dict
  //   INCLUDING the last NUL, while n_dict = strlen(dict),
  //   EXCLUDING the last NUL.
  //   Thus n_dict = n_chars_dict - 1
  size_t n_chars_dict = 0;
  for(size_t i = 0; i < n_elements_dict; i++){
    // check each element and sum up its number of characters
    n_chars_dict += strlen(elements[i]);
  }
  // last NUL
  n_chars_dict += 1;
  // 4. allocate dict and assign above "elements"
  ALLOCATE(*dict, sizeof(char)*n_chars_dict);
  for(size_t i = 0, offset = 0; i < n_elements_dict; i++){
    const size_t n_chars_element = strlen(elements[i]);
    memcpy((*dict)+offset, elements[i], sizeof(char)*n_chars_element);
    offset += n_chars_element;
  }
  (*dict)[n_chars_dict-1] = NUL;
  // clean-up all working memories
  DEALLOCATE(descr_value);
  DEALLOCATE(fortran_order_value);
  DEALLOCATE(shape_value);
  DEALLOCATE(elements);
  // as the length of "dict", use length WITHOUT NUL,
  // i.e. strlen(*dict)
  *n_dict = strlen(*dict);
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s dict created: %s (size: %zu)\n", LOG_PREFIX, *dict, *n_dict);
#endif
  return 0;
}

static int create_padding(uint8_t **padding, size_t *n_padding, uint8_t *major_ver, const size_t n_dict){
  /*
   * The following relation holds for the header size
   *   size_header =
   *     + sizeof(magic string)      (= 6            bytes)
   *     + sizeof(major_version)     (= 1            byte )
   *     + sizeof(minor_version)     (= 1            byte )
   *     + sizeof(header_len)        (= 2 or 4       bytes)
   *     + sizeof(char)*strlen(dict) (= strlen(dict) bytes)
   *     + sizeof(uint8_t)*n_padding (= n_padding    bytes)
   *   is divisible by 64
   * Definitely this is not generally true, and we need some paddings
   *   consisting of some (0 or more) spaces ' ' and one newline '\n',
   *   whose length (number of elements) is returned
   */
  // size of each element
  size_t size_magic_string  = sizeof(char)*strlen(MAGIC_STRING);
  size_t size_major_version = sizeof(uint8_t);
  size_t size_minor_version = sizeof(uint8_t);
  size_t size_dict          = sizeof(char)*n_dict;
  // large portion of the header is occupied by dict
  // so check dict size, and if it is larger than USHRT_MAX-64,
  // use major_version = 2
  size_t size_header_len;
  if(size_dict > USHRT_MAX-64){
    *major_ver = 2;
    size_header_len = sizeof(uint8_t)*4;
  }else{
    *major_ver = 1;
    size_header_len = sizeof(uint8_t)*2;
  }
  // size of all data except padding
  size_t size_except_padding =
    +size_magic_string
    +size_major_version
    +size_minor_version
    +size_header_len
    +size_dict;
  // decide total size of the header, which should be 64 x N
  // increase total size by 64 until becoming larger than size_except_padding
  // NOTE: size_padding == 0 is NOT allowed since '\n' is necessary at the end
  //   thus the condition to continue loop is "<=", not "<"
  size_t size_header = 0;
  while(size_header <= size_except_padding){
    size_header += 64;
  }
  size_t size_padding = size_header-size_except_padding;
  *n_padding = size_padding/sizeof(uint8_t);
  ALLOCATE(*padding, sizeof(uint8_t)*(*n_padding));
  // many ' 's: 0x20
  memset(*padding, 0x20, sizeof(uint8_t)*(*n_padding-1));
  // last '\n': 0x0a
  (*padding)[*n_padding-1] = 0x0a;
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s padding created: 0x", LOG_PREFIX);
  for(size_t i = 0; i < *n_padding; i++){
    printf("%02x", (*padding)[i]);
  }
  printf(" (size: %zu)\n", *n_padding);
#endif
  return 0;
}

static int create_header_len(uint8_t **header_len, size_t *n_header_len, const uint8_t major_ver, const size_t n_dict, const size_t n_padding){
  /*
   * In short, HEADER_LEN = n_dict + n_padding,
   * which should be written as a little-endian form (irrespective to the architecture)
   */
  // For safety, reject too large dict size
  // Here "too large" means header size (not data size) larger than approx. 20GB,
  //   which would not happen normally
  // we do not have to check n_padding, which is always smaller than 64 by definition
  if(n_dict >= UINT_MAX/2){
    printf("error: dictionary size (%zu bytes) is larger than %u bytes\n", n_dict, UINT_MAX/2);
    printf("this would not happen normally\n");
    printf("enable logging (compile with -DLOGGING_SIMPLE_NPYIO) to check what is happening\n");
    exit(EXIT_FAILURE);
  }
  if(major_ver == 2){
    // major version 2, use uint32_t to store header_len
    uint32_t header_len_uint32_t = (uint32_t)n_dict+(uint32_t)n_padding;
    *n_header_len = sizeof(uint32_t)/sizeof(uint8_t);
    ALLOCATE(*header_len, sizeof(uint8_t)*(*n_header_len));
    memcpy(*header_len, &header_len_uint32_t, *n_header_len);
#if defined(LOGGING_SIMPLE_NPYIO)
    printf("%s header_len (should be the sum of dict size and padding size): %u\n", LOG_PREFIX, header_len_uint32_t);
#endif
  }else{
    // major version 1, use uint16_t to store header_len
    uint16_t header_len_uint16_t = (uint16_t)n_dict+(uint16_t)n_padding;
    *n_header_len = sizeof(uint16_t)/sizeof(uint8_t);
    ALLOCATE(*header_len, sizeof(uint8_t)*(*n_header_len));
    memcpy(*header_len, &header_len_uint16_t, *n_header_len);
#if defined(LOGGING_SIMPLE_NPYIO)
    printf("%s header_len (should be the sum of dict size and padding size): %hu\n", LOG_PREFIX, header_len_uint16_t);
#endif
  }
  // if the architecture is big-endian
  if(is_big_endian()){
    // convert HEADER_LEN to little endian
    convert_endian(header_len, sizeof(*header_len));
#if defined(LOGGING_SIMPLE_NPYIO)
    printf("%s endian converted\n", LOG_PREFIX);
#endif
  }
  return 0;
}

size_t simple_npyio_w_header(const size_t ndim, const size_t *shape, const char dtype[], const bool is_fortran_order, FILE *fp){
  /*
   * From input information (ndim, shape, dtype, is_fortran_order),
   *   create and output "header", which is enough and sufficient information consists of a *.npy file
   * NPY file format is defined here:
   *   https://numpy.org/devdocs/reference/generated/numpy.lib.format.html#format-version-1-0
   *
   * The size of the "header", defined as "header_size",
   *   is returned, which could be useful by the user
   *
   * Header of a npy file contains these 6 data (n_datasets):
   *       ---NAME--- --TYPE-- -# OF ELEMENTS- -------SIZE-------
   * No. 0 magic_str  char     6               6 bytes
   * No. 1 major_ver  uint8_t  1               1 byte
   * No. 2 minor_ver  uint8_t  1               1 byte
   * No. 3 header_len uint8_t  n_header_len    n_header_len bytes
   * No. 4 dict       char     n_dict          n_dict bytes
   * No. 5 padding    uint8_t  n_padding       n_padding bytes
   *
   * See below and corresponding function for details of each member
   */
  /* prepare all datasets (from No. 0 to No. 5) */
  // No. 0: magic_string
  const char magic_str[] = {MAGIC_STRING};
  size_t n_magic_str = strlen(magic_str);
  // No. 2: minor_version, always 0
  const uint8_t minor_ver = 0;
  // No. 4: dictionary (and its size)
  char *dict = NULL;
  size_t n_dict;
  create_dict(&dict, &n_dict, ndim, shape, dtype, is_fortran_order);
  // No. 1: major_version
  // and
  // No. 5: padding (and its size)
  uint8_t major_ver;
  uint8_t *padding = NULL;
  size_t n_padding;
  create_padding(&padding, &n_padding, &major_ver, n_dict);
  // No. 3: header_len
  uint8_t *header_len = NULL;
  size_t n_header_len;
  create_header_len(&header_len, &n_header_len, major_ver, n_dict, n_padding);
  /* dump all information to a buffer "header" and compute total size "header_size" */
  uint8_t *header = NULL;
  size_t header_size;
  {
    const uint8_t n_datasets = 6;
    size_t *sizes   = NULL;
    size_t *offsets = NULL;
    ALLOCATE(sizes,   sizeof(size_t)*n_datasets);
    ALLOCATE(offsets, sizeof(size_t)*n_datasets);
    sizes[0] = sizeof(   char)*n_magic_str;
    sizes[1] = sizeof(uint8_t);
    sizes[2] = sizeof(uint8_t);
    sizes[3] = sizeof(uint8_t)*n_header_len;
    sizes[4] = sizeof(   char)*n_dict;
    sizes[5] = sizeof(uint8_t)*n_padding;
    // total size
    header_size = 0;
    for(uint8_t i = 0; i < n_datasets; i++){
      header_size += sizes[i];
    }
    // offsets
    offsets[0] = 0;
    for(uint8_t i = 1; i < n_datasets; i++){
      offsets[i] = offsets[i-1]+sizes[i-1];
    }
    // allocate
    ALLOCATE(header, header_size);
    // write all information to a buffer "header"
    memcpy(header+offsets[0], magic_str,  sizes[0]);
    memcpy(header+offsets[1], &major_ver, sizes[1]);
    memcpy(header+offsets[2], &minor_ver, sizes[2]);
    memcpy(header+offsets[3], header_len, sizes[3]);
    memcpy(header+offsets[4], dict,       sizes[4]);
    memcpy(header+offsets[5], padding,    sizes[5]);
    // clean-up buffers
    DEALLOCATE(sizes);
    DEALLOCATE(offsets);
  }
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s header_size: %zu\n", LOG_PREFIX, header_size);
#endif
  /* write to the given file stream */
  fwrite(header, sizeof(uint8_t), header_size, fp);
  // clean-up all buffers
  DEALLOCATE(dict);
  DEALLOCATE(padding);
  DEALLOCATE(header_len);
  DEALLOCATE(header);
  return header_size;
}

#undef MAGIC_STRING
#undef NUL
#undef ALLOCATE
#undef DEALLOCATE

#if defined(LOGGING_SIMPLE_NPYIO)
#undef LOG_PREFIX
#endif

