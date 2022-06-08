#include <CUnit/CUnitCI.h>
#include "simple_npyio.c"


// normal use, input string
static void test_create_descr_value_case0(void){
  // answers
  const int retval_ans = 0;
  const char value_ans[] = {"'<f8'"};
  // args
  char *value = NULL;
  const char dtype[] = {"'<f8'"};
  int retval = create_descr_value(&value, dtype);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(value_ans) == strlen(value));
  for(size_t i = 0; i < strlen(value_ans) && i < strlen(value); i++){
    CU_ASSERT(value_ans[i] == value[i]);
  }
}

// abnormal use, input NULL
static void test_create_descr_value_case1(void){
  // answers
  const int retval_ans = -1;
  // args
  char *value = NULL;
  char *dtype = NULL;
  int retval = create_descr_value(&value, dtype);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, input empty string
static void test_create_descr_value_case2(void){
  // answers
  const int retval_ans = -1;
  // args
  char *value = NULL;
  const char dtype[] = {""};
  int retval = create_descr_value(&value, dtype);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, input NUL
static void test_create_descr_value_case3(void){
  // answers
  const int retval_ans = -1;
  // args
  char *value = NULL;
  const char dtype[] = {"\x00"};
  int retval = create_descr_value(&value, dtype);
  CU_ASSERT(retval_ans == retval);
}

// normal use, give true
static void test_create_fortran_order_value_case0(void){
  // answers
  const int retval_ans = 0;
  const char value_ans[] = {"True"};
  // args
  char *value = NULL;
  const bool is_fortran_order = true;
  int retval = create_fortran_order_value(&value, is_fortran_order);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(value_ans) == strlen(value));
  for(size_t i = 0; i < strlen(value_ans) && i < strlen(value); i++){
    CU_ASSERT(value_ans[i] == value[i]);
  }
}

// normal use, give false
static void test_create_fortran_order_value_case1(void){
  // answers
  const int retval_ans = 0;
  const char value_ans[] = {"False"};
  // args
  char *value = NULL;
  const bool is_fortran_order = false;
  int retval = create_fortran_order_value(&value, is_fortran_order);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(value_ans) == strlen(value));
  for(size_t i = 0; i < strlen(value_ans) && i < strlen(value); i++){
    CU_ASSERT(value_ans[i] == value[i]);
  }
}

// normal use, scalar input (ndim = 0)
static void test_create_shape_value_case0(void){
  // answers
  const int retval_ans = 0;
  const char value_ans[] = {"()"};
  // args
  char *value = NULL;
  const size_t ndim = 0;
  size_t *shape = NULL;
  int retval = create_shape_value(&value, ndim, shape);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(value_ans) == strlen(value));
  for(size_t i = 0; i < strlen(value_ans) && strlen(value); i++){
    CU_ASSERT(value_ans[i] == value[i]);
  }
}

// normal use, vector input (ndim = 1) with single digit
static void test_create_shape_value_case1(void){
  // answers
  const int retval_ans = 0;
  const char value_ans[] = {"(5,)"};
  // args
  char *value = NULL;
  const size_t ndim = 1;
  const size_t shape[] = {5};
  int retval = create_shape_value(&value, ndim, shape);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(value_ans) == strlen(value));
  for(size_t i = 0; i < strlen(value_ans) && strlen(value); i++){
    CU_ASSERT(value_ans[i] == value[i]);
  }
}

// normal use, vector input (ndim = 1) with multiple digits
static void test_create_shape_value_case2(void){
  // answers
  const int retval_ans = 0;
  const char value_ans[] = {"(314,)"};
  // args
  char *value = NULL;
  const size_t ndim = 1;
  const size_t shape[] = {314};
  int retval = create_shape_value(&value, ndim, shape);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(value_ans) == strlen(value));
  for(size_t i = 0; i < strlen(value_ans) && strlen(value); i++){
    CU_ASSERT(value_ans[i] == value[i]);
  }
}

// normal use, array input (ndim = 2)
static void test_create_shape_value_case3(void){
  // answers
  const int retval_ans = 0;
  const char value_ans[] = {"(314,15926,)"};
  // args
  char *value = NULL;
  const size_t ndim = 2;
  const size_t shape[] = {314, 15926};
  int retval = create_shape_value(&value, ndim, shape);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(value_ans) == strlen(value));
  for(size_t i = 0; i < strlen(value_ans) && strlen(value); i++){
    CU_ASSERT(value_ans[i] == value[i]);
  }
}

// abnormal use, 0-size array input
static void test_create_shape_value_case4(void){
  // answers
  const int retval_ans = -1;
  // args
  char *value = NULL;
  const size_t ndim = 3;
  const size_t shape[] = {314, 0, 15926};
  int retval = create_shape_value(&value, ndim, shape);
  CU_ASSERT(retval_ans == retval);
}

// normal use, dict size is smaller than 64
static void test_create_padding_case0(void){
  // answers
  const int retval_ans = 0;
  const size_t n_padding_ans = 2;
  const uint8_t major_ver_ans = 1;
  // args
  uint8_t *padding = NULL;
  size_t n_padding;
  uint8_t major_ver;
  const size_t n_dict = 52;
  int retval = create_padding(&padding, &n_padding, &major_ver, n_dict);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(n_padding_ans == n_padding);
  CU_ASSERT(major_ver_ans == major_ver);
  for(size_t i = 0; i < n_padding; i++){
    uint8_t padding_ans = i == n_padding-1 ? 0x0a : 0x20;
    CU_ASSERT(padding_ans == padding[i]);
  }
}

// normal use, dict size is smaller than 128
static void test_create_padding_case1(void){
  // answers
  const int retval_ans = 0;
  const size_t n_padding_ans = 8;
  const uint8_t major_ver_ans = 1;
  // args
  uint8_t *padding = NULL;
  size_t n_padding;
  uint8_t major_ver;
  const size_t n_dict = 110;
  int retval = create_padding(&padding, &n_padding, &major_ver, n_dict);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(n_padding_ans == n_padding);
  CU_ASSERT(major_ver_ans == major_ver);
  for(size_t i = 0; i < n_padding; i++){
    uint8_t padding_ans = i == n_padding-1 ? 0x0a : 0x20;
    CU_ASSERT(padding_ans == padding[i]);
  }
}

// normal use, dict size is smaller than USHRT_MAX
static void test_create_padding_case2(void){
  // answers
  const int retval_ans = 0;
  const size_t n_padding_ans = 53;
  const uint8_t major_ver_ans = 2;
  // args
  uint8_t *padding = NULL;
  size_t n_padding;
  uint8_t major_ver;
  const size_t n_dict = USHRT_MAX;
  int retval = create_padding(&padding, &n_padding, &major_ver, n_dict);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(n_padding_ans == n_padding);
  CU_ASSERT(major_ver_ans == major_ver);
  for(size_t i = 0; i < n_padding; i++){
    uint8_t padding_ans = i == n_padding-1 ? 0x0a : 0x20;
    CU_ASSERT(padding_ans == padding[i]);
  }
}

// abnormal use, dict size is larger than UINT_MAX
static void test_create_padding_case3(void){
  // answers
  const int retval_ans = -1;
  // args
  uint8_t *padding = NULL;
  size_t n_padding;
  uint8_t major_ver;
  const size_t n_dict = UINT_MAX;
  int retval = create_padding(&padding, &n_padding, &major_ver, n_dict);
  CU_ASSERT(retval_ans == retval);
}

// normal use, version 1
static void test_create_header_len_case0(void){
  // answers
  const int retval_ans = 0;
  uint16_t header_len_ans = 54;
  uint8_t n_header_len_ans = 2;
  // args
  uint8_t *header_len = NULL;
  size_t n_header_len;
  const uint8_t major_ver = 1;
  const size_t n_dict = 52;
  const size_t n_padding = 2;
  int retval = create_header_len(&header_len, &n_header_len, major_ver, n_dict, n_padding);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(((uint8_t *)&header_len_ans)[0] == header_len[0]);
  CU_ASSERT(((uint8_t *)&header_len_ans)[1] == header_len[1]);
  CU_ASSERT(n_header_len_ans == n_header_len);
}

// normal use, version 2
static void test_create_header_len_case1(void){
  // answers
  const int retval_ans = 0;
  uint32_t header_len_ans = 2*USHRT_MAX;
  uint8_t n_header_len_ans = 4;
  // args
  uint8_t *header_len = NULL;
  size_t n_header_len;
  const uint8_t major_ver = 2;
  const size_t n_dict = USHRT_MAX;
  const size_t n_padding = USHRT_MAX;
  int retval = create_header_len(&header_len, &n_header_len, major_ver, n_dict, n_padding);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(((uint8_t *)&header_len_ans)[0] == header_len[0]);
  CU_ASSERT(((uint8_t *)&header_len_ans)[1] == header_len[1]);
  CU_ASSERT(((uint8_t *)&header_len_ans)[2] == header_len[2]);
  CU_ASSERT(((uint8_t *)&header_len_ans)[3] == header_len[3]);
  CU_ASSERT(n_header_len_ans == n_header_len);
}

// abnormal use, too large n_dict
static void test_create_header_len_case2(void){
  // answers
  const int retval_ans = -1;
  // args
  uint8_t *header_len = NULL;
  size_t n_header_len;
  const uint8_t major_ver = 2;
  const size_t n_dict = UINT_MAX;
  const size_t n_padding = 0;
  int retval = create_header_len(&header_len, &n_header_len, major_ver, n_dict, n_padding);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, too large n_dict
static void test_create_header_len_case3(void){
  // answers
  const int retval_ans = -1;
  // args
  uint8_t *header_len = NULL;
  size_t n_header_len;
  const uint8_t major_ver = 2;
  const size_t n_dict = 0;
  const size_t n_padding = UINT_MAX;
  int retval = create_header_len(&header_len, &n_header_len, major_ver, n_dict, n_padding);
  CU_ASSERT(retval_ans == retval);
}

CUNIT_CI_RUN(
    "writer",
    CUNIT_CI_TEST(test_create_descr_value_case0),
    CUNIT_CI_TEST(test_create_descr_value_case1),
    CUNIT_CI_TEST(test_create_descr_value_case2),
    CUNIT_CI_TEST(test_create_descr_value_case3),
    CUNIT_CI_TEST(test_create_fortran_order_value_case0),
    CUNIT_CI_TEST(test_create_fortran_order_value_case1),
    CUNIT_CI_TEST(test_create_shape_value_case0),
    CUNIT_CI_TEST(test_create_shape_value_case1),
    CUNIT_CI_TEST(test_create_shape_value_case2),
    CUNIT_CI_TEST(test_create_shape_value_case3),
    CUNIT_CI_TEST(test_create_shape_value_case4),
    CUNIT_CI_TEST(test_create_padding_case0),
    CUNIT_CI_TEST(test_create_padding_case1),
    CUNIT_CI_TEST(test_create_padding_case2),
    CUNIT_CI_TEST(test_create_padding_case3),
    CUNIT_CI_TEST(test_create_header_len_case0),
    CUNIT_CI_TEST(test_create_header_len_case1),
    CUNIT_CI_TEST(test_create_header_len_case2),
    CUNIT_CI_TEST(test_create_header_len_case3)
);

