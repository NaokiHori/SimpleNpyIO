#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "simple_npyio.h"

// check 1 byte is 8 bits
#if CHAR_BIT != 8
#error "CHAR_BIT is not 8"
#endif

// all npy files should start from this magic string
#define MAGIC_STRING "\x93NUMPY"

// end-of-string
#define NUL '\x00'

// error code of find_pattern
#define ERR_FIND_PATTERN SIZE_MAX

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
#define LOG_PREFIX "[SIMPLE_NPYIO reader]"
#endif

// https://github.com/NaokiHori/CSnippets/blob/master/src/check_endian.c
static bool is_big_endian(void){
  const uint16_t val = 1 << 8;
  return (bool)(((uint8_t *)(&val))[0]);
}

// https://github.com/NaokiHori/CSnippets/blob/master/src/convert_endian.c
static int convert_endian(void *val, const size_t size){
  size_t n_bytes = size/sizeof(uint8_t);
  uint8_t *buf = NULL;
  ALLOCATE(buf, sizeof(uint8_t)*n_bytes);
  for(size_t i = 0; i < n_bytes; i++){
    buf[i] = ((uint8_t *)val)[n_bytes-i-1];
  }
  memcpy(val, buf, size);
  DEALLOCATE(buf);
  return 0;
}

static size_t find_pattern(const void *p0, const size_t size_p0, const void *p1, const size_t size_p1, const size_t offset){
  /*
   * try to find a pattern "p1" in "p0"
   *   and return its location IN BYTES
   * ERR_FIND_PATTERN is returned instead
   *   if the pattern is not found
   * note that sizes of "p0" and "p1" are
   *   in BYTES, NOT number of elements
   * thus it is necessary to divide by the
   *   sizeof original datatype
   *   after the result is obtained
   */
  size_t result = ERR_FIND_PATTERN;
  // e.g., size_p0 = 10, size_p1 = 3, offset = 2
  //     0 1 2 3 4 5 6 7 8 9
  // p0: a b c d e f g h i j
  // p1:     x y z
  //           x y z
  //             x y z
  //               x y z
  //                 x y z
  //                   x y z
  //         ^         ^
  //        imin      imax
  size_t imin = offset;
  size_t imax = size_p0-size_p1+1;
  for(size_t i = imin; i < imax; i++){
    if(memcmp(p0+i, p1, size_p1) == 0){
      result = i;
      break;
    }
  }
  return result;
}

static size_t load_magic_str(FILE *fp){
  /*
   * all npy file should start with \x93NUMPY,
   *   which is checked here
   */
  const char magic_str[] = {MAGIC_STRING};
  size_t bufsize = strlen(magic_str);
  uint8_t *buf = NULL;
  // allocate buffer and load from file
  // (file pointer is moved forward as well)
  ALLOCATE(buf, sizeof(uint8_t)*bufsize);
  fread(buf, sizeof(uint8_t), bufsize, fp);
  // pattern match
  size_t offset = 0;
  size_t location = find_pattern((void *)buf, bufsize, (void *)magic_str, bufsize, offset);
  // if not matched, this file is not npy file
  if(location == ERR_FIND_PATTERN){
    printf("NPY file should start with 0x");
    for(uint8_t i = 0; i < bufsize; i++){
      printf("%02x", (uint8_t)(magic_str[i]));
    }
    printf(", not 0x");
    for(uint8_t i = 0; i < bufsize; i++){
      printf("%02x", buf[i]);
    }
    printf("\n");
    exit(EXIT_FAILURE);
  }
  DEALLOCATE(buf);
  return bufsize;
}

static size_t load_versions(FILE *fp, uint8_t *major_ver, uint8_t *minor_ver){
  /*
   * check version of the file
   * for now 1.0, 2.0 are considered,
   *    and others are rejected
   */
  const size_t bufsize_major_ver = 1;
  const size_t bufsize_minor_ver = 1;
  // allocate buffer and load from file
  // (file pointer is moved forward as well)
  fread(major_ver, sizeof(uint8_t), bufsize_major_ver, fp);
  fread(minor_ver, sizeof(uint8_t), bufsize_minor_ver, fp);
  if(*major_ver != 1 && *major_ver != 2){
    printf("major version should be 1 or 2, %u not allowed\n", *major_ver);
    exit(EXIT_FAILURE);
  }
  if(*minor_ver != 0){
    printf("minor version should be 0, %u not allowed\n", *minor_ver);
    exit(EXIT_FAILURE);
  }
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s major version: %u\n", LOG_PREFIX, *major_ver);
  printf("%s minor version: %u\n", LOG_PREFIX, *minor_ver);
#endif
  return bufsize_major_ver + bufsize_minor_ver;
}

static size_t load_header_len(FILE *fp, size_t *header_len, size_t major_ver){
  /*
   * check header size of the npy file
   * in particular HEADER_LEN = len(dict) + len(padding)
   *   is loaded
   * memory size of this variable depends on the major version
   *   of the npy file, 2 bytes for major_ver = 1,
   *   while 4 bytes for major_ver = 2
   */
  size_t bufsize;
  if(major_ver == 1){
    // usually 2
    bufsize = sizeof(uint16_t)/sizeof(uint8_t);
  }else{
    // usually 4
    bufsize = sizeof(uint32_t)/sizeof(uint8_t);
  }
  // allocate buffer and
  //   load corresponding memory size from file
  uint8_t *buf = NULL;
  ALLOCATE(buf, sizeof(uint8_t)*bufsize);
  fread(buf, sizeof(uint8_t), bufsize, fp);
  // convert little-endian value to big endian
  //   if the architecture is big-endian-based
  if(is_big_endian()){
    convert_endian(buf, bufsize);
  }
  // interpret buffer (array of uint8_t)
  //   as a value of corresponding datatype
  if(major_ver == 1){
    // 2 bytes
    *header_len = *((uint16_t *)buf);
  }else{
    // 4 bytes
    *header_len = *((uint32_t *)buf);
  }
  DEALLOCATE(buf);
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s header_len: %zu\n", LOG_PREFIX, *header_len);
#endif
  return bufsize;
}

static size_t load_dict_and_padding(FILE *fp, uint8_t **dict_and_padding, size_t header_len){
  /*
   * load dictionary and padding
   * loading padding is also necessary (or at least one of the easiest ways)
   *   to move file pointer "fp" forward
   */
  ALLOCATE(*dict_and_padding, sizeof(uint8_t)*header_len);
  fread(*dict_and_padding, sizeof(uint8_t), header_len, fp);
  return header_len;
}

static int extract_dict(char **dict, uint8_t *dict_and_padding, size_t header_len){
  /*
   * extract dictionary from input buffer, which contains
   *   dictionary and padding
   * also unnecessary spaces are removed from the original array
   *   to simplify the following procedures
   * note that spaces inside quotations (e.g. dictionary key might contain spaces)
   *   should NOT be eliminated, which are "necessary spaces" so to say
   */
  // look for "{" and "}" to see the range of dict
  size_t s;
  {
    const char pattern[] = {"{"};
    size_t offset = 0;
    size_t s_in_bytes = find_pattern((void *)(dict_and_padding), header_len, (void *)pattern, sizeof(char)*strlen(pattern), offset);
    if(s_in_bytes == ERR_FIND_PATTERN){
      printf("pattern (%s) not found in %s\n", pattern, dict_and_padding);
      exit(EXIT_FAILURE);
    }
    s = s_in_bytes/sizeof(uint8_t);
  }
  size_t e;
  {
    const char pattern[] = {"}"};
    size_t offset = 0;
    size_t e_in_bytes = find_pattern((void *)(dict_and_padding), header_len, (void *)pattern, sizeof(char)*strlen(pattern), offset);
    if(e_in_bytes == ERR_FIND_PATTERN){
      printf("pattern (%s) not found in %s\n", pattern, dict_and_padding);
      exit(EXIT_FAILURE);
    }
    e = e_in_bytes/sizeof(uint8_t);
  }
  // flag dict_and_padding to decide
  //   which part should be / should not be extracted
  // "meaningless spaces" (spaces outside quotations) are de-flagged
  size_t n_elements_dict = 0;
  bool *is_dict = NULL;
  {
    ALLOCATE(is_dict, sizeof(bool)*(e-s+1));
    // TODO: consider double quotations
    bool is_inside_s_quotations = false;
    for(size_t i = s; i <= e; i++){
      uint8_t c = dict_and_padding[i];
      // check whether we are in a pair of quotations
      if(c == (uint8_t)('\'')){
        is_inside_s_quotations = !is_inside_s_quotations;
      }
      if(c != (uint8_t)(' ')){
        // if "c" is not space, the information is meaningful
        //   as a part of the dictionary
        n_elements_dict++;
        is_dict[i-s] = true;
      }else{
        if(is_inside_s_quotations){
          // even if "c" is a space, it is a meaningful information
          //   since it is inside a pair of quotations (e.g., key includes space)
          n_elements_dict++;
          is_dict[i-s] = true;
        }else{
          // "c" is a space and outside pair of parentheses,
          //   indicating this space is meaningless
          is_dict[i-s] = false;
        }
      }
    }
  }
  // copy flagged part to dict
  ALLOCATE(*dict, sizeof(char)*(n_elements_dict+1)); // + NUL
  for(size_t i = s, j = 0; i <= e; i++){
    if(is_dict[i-s]){
      (*dict)[j] = dict_and_padding[i];
      j++;
    }
  }
  DEALLOCATE(is_dict);
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s dict extracted: %s\n", LOG_PREFIX, *dict);
#endif
  return 0;
}

static int find_dict_value(const char key[], char **val, const char *dict){
  // find key locations
  size_t key_index_s = 0;
  {
    size_t offset = 0;
    size_t s_in_bytes = find_pattern((void *)dict, sizeof(char)*strlen(dict), (void *)key, sizeof(char)*strlen(key), offset);
    if(s_in_bytes == ERR_FIND_PATTERN){
      printf("pattern (%s) not found in %s\n", key, dict);
      exit(EXIT_FAILURE);
    }
    key_index_s = s_in_bytes/sizeof(char);
  }
  size_t key_index_e = key_index_s + strlen(key) - 1;
  // detect value location, start / end indices
  bool is_inside_parentheses = false;
  size_t val_index_s = key_index_e + 2; // end of the key + ":", assuming no spaces
  size_t val_index_e;
  // parse dict, based on a fact that python dict is delimited by ","
  // we cannot find the last ","
  //   if the pair of key / value locate at the last of the given dict,
  //   the last index is the end of the value in that case
  // note that "," can exist inside value,
  //   so it is essential to consider
  //   "," only outside pair of parentheses are delimiters
  for(val_index_e = val_index_s; val_index_e < strlen(dict); val_index_e++){
    char c = dict[val_index_e];
    // check (
    if(c == '('){
      if(!is_inside_parentheses){
        is_inside_parentheses = true;
      }else{
        printf("find '(' even though I have already found it\n");
        printf("this might happen for nested arrays, which are not supported for now\n");
        exit(EXIT_FAILURE);
      }
    }
    // check )
    if(c == ')'){
      if(!is_inside_parentheses){
        printf("find ')' even though I have not found corresponding '('\n");
        exit(EXIT_FAILURE);
      }else{
        is_inside_parentheses = false;
      }
    }
    // look for ',' (delimiter of python dict), only when we are outside parentheses
    // also, if "}" is found, now we are at the end of dict
    if(!is_inside_parentheses && (c == ',' || c == '}') ){
      // end should be just before found ','
      val_index_e -= 1;
      break;
    }
  }
  size_t n_elements_val = val_index_e - val_index_s + 1;
  ALLOCATE(*val, sizeof(char)*(n_elements_val+1)); // + NUL
  memcpy(*val, dict+val_index_s, sizeof(char)*n_elements_val);
  return 0;
}

static int extract_shape(size_t *ndim, size_t **shape, const char *dict){
  // find a key 'shape' and extract its value
  char *val = NULL;
  find_dict_value("'shape'", &val, dict);
  // 1. compute memory size (i.e. ndim) to store sizes
  {
    char *str = NULL;
    // copy value to a buffer after removing parentheses
    ALLOCATE(str, sizeof(char)*(strlen(val)-2+1));
    memcpy(str, val+1, strlen(val)-2);
    // parse value to know ndim (e.g. 4,3,5, -> ndim = 3)
    size_t n_str = strlen(str);
    *ndim = 0;
    // maximum loop size should be smaller than n_str
    for(size_t i = 0; i < n_str; i++){
      const char sep[] = {","};
      char *buf = i == 0 ? strtok(str, sep) : strtok(NULL, sep);
      if(buf == NULL){
        break;
      }else{
        (*ndim)++;
      }
    }
    DEALLOCATE(str);
  }
  // 2. assign shape
  ALLOCATE(*shape, sizeof(size_t)*(*ndim));
  {
    char *str = NULL;
    // copy value to a buffer after removing parentheses
    ALLOCATE(str, sizeof(char)*(strlen(val)-2+1));
    memcpy(str, val+1, strlen(val)-2);
    // parse value to know shape (e.g. 4,3,5, -> 4 and 3 and 5),
    //   which are assigned to "shape"
    size_t n_str = strlen(str);
    // maximum loop size should be smaller than n_str
    for(size_t i = 0, j = 0; i < n_str; i++){
      const char sep[] = {","};
      char *buf = i == 0 ? strtok(str, sep) : strtok(NULL, sep);
      if(buf == NULL){
        break;
      }else{
        (*shape)[j] = atoi(buf);
        j++;
      }
    }
    DEALLOCATE(str);
  }
  DEALLOCATE(val);
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s ndim: %zu\n", LOG_PREFIX, *ndim);
  for(size_t i = 0; i < *ndim; i++){
    printf("%s shape[%3zu]: %7zu\n", LOG_PREFIX, i, (*shape)[i]);
  }
#endif
  return 0;
}

static int extract_dtype(char **dtype, const char *dict){
  // find a key 'descr' and extract its value
  char *val = NULL;
  find_dict_value("'descr'", &val, dict);
  // check quotations in which datatype is stored
  size_t s;
  {
    const char pattern[] = {"'"};
    size_t offset = 0;
    size_t s_in_bytes = find_pattern((void *)val, sizeof(char)*strlen(val), (void *)pattern, sizeof(char)*strlen(pattern), offset);
    if(s_in_bytes == ERR_FIND_PATTERN){
      printf("pattern (%s) not found in %s\n", pattern, val);
      exit(EXIT_FAILURE);
    }
    s = s_in_bytes/sizeof(char);
  }
  size_t e;
  {
    const char pattern[] = {"'"};
    size_t offset = s + 1;
    size_t e_in_bytes = find_pattern((void *)val, sizeof(char)*strlen(val), (void *)pattern, sizeof(char)*strlen(pattern), offset);
    if(e_in_bytes == ERR_FIND_PATTERN){
      printf("pattern (%s) not found in %s\n", pattern, val);
      exit(EXIT_FAILURE);
    }
    e = e_in_bytes/sizeof(char);
  }
  // now we know memory size to be allocated
  // although quotations are used to identify the location,
  //   they are not needed as a value
  // thus we do not copy them
  size_t n = (e-1)-(s+1)+1;
  ALLOCATE(*dtype, sizeof(char)*(n+1)); // + NUL
  memcpy(*dtype, val+(s+1), sizeof(char)*n);
  (*dtype)[n] = NUL;
  DEALLOCATE(val);
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s dtype: %s\n", LOG_PREFIX, *dtype);
#endif
  return 0;
}

static int extract_is_fortran_order(bool *is_fortran_order, const char *dict){
  // find a key 'fortran_order' and extract its value
  char *val = NULL;
  find_dict_value("'fortran_order'", &val, dict);
  const char pattern[] = {"True"};
  size_t offset = 0;
  size_t location = find_pattern((void *)val, sizeof(char)*strlen(val), (void *)pattern, sizeof(char)*strlen(pattern), offset);
  if(location == ERR_FIND_PATTERN){
    *is_fortran_order = false;
  }else{
    *is_fortran_order = true;
  }
  DEALLOCATE(val);
#if defined(LOGGING_SIMPLE_NPYIO)
  printf("%s is_fortran_order: %u\n", LOG_PREFIX, *is_fortran_order);
#endif
  return 0;
}

size_t simple_npyio_r_header(size_t *ndim, size_t **shape, char **dtype, bool *is_fortran_order, FILE *fp){
  /* step 1: load header from file and move file pointer forward */
  // versions
  uint8_t major_ver, minor_ver;
  // header_len: lengths of dict and padding
  // header_size: matadata (magic string, versions) + header_len
  size_t header_len, header_size;
  // dict + padding
  uint8_t *dict_and_padding = NULL;
  // load header to get / sanitise input and move file pointer forward
  // also the total header size "header_size" is calculated by summing up datasize loaded
  header_size = 0;
  header_size += load_magic_str(fp);
  header_size += load_versions(fp, &major_ver, &minor_ver);
  header_size += load_header_len(fp, &header_len, major_ver);
  header_size += load_dict_and_padding(fp, &dict_and_padding, header_len);
  /* step 2: extract dictionary */
  // extract dict from dict + padding
  // also non-crutial spaces (spaces outside quotations) are eliminated
  char *dict = NULL;
  extract_dict(&dict, dict_and_padding, header_len);
  DEALLOCATE(dict_and_padding);
  /* step 3: extract information which are needed to reconstruct array */
  /* in particular, shape, datatype, and memory order of the array */
  extract_shape(ndim, shape, dict);
  extract_dtype(dtype, dict);
  extract_is_fortran_order(is_fortran_order, dict);
  // clean-up buffer
  DEALLOCATE(dict);
  return header_size;
}

#undef MAGIC_STRING
#undef NUL
#undef ALLOCATE
#undef DEALLOCATE

