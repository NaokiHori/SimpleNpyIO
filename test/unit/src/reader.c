#include <CUnit/CUnitCI.h>
#include "simple_npyio.c"


// normal use (general dict)
static void test_find_dict_value_case0(void){
  // answers
  const int retval_ans = 0;
  const char val_ans[] = {"'val 0'"};
  // args
  const char key[] = {"'key0'"};
  char *val = NULL;
  const char dict[] = {"{'key0':'val 0','key1':val1}"};
  int retval = find_dict_value(key, &val, dict);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(val_ans) == strlen(val));
  for(size_t i = 0; i < strlen(val_ans) && i < strlen(val); i++){
    CU_ASSERT(val_ans[i] == val[i]);
  }
  free(val);
}

// normal use (general dict, end contains ",")
static void test_find_dict_value_case1(void){
  // answers
  const int retval_ans = 0;
  const char val_ans[] = {"val1"};
  // args
  const char key[] = {"'key1'"};
  char *val = NULL;
  const char dict[] = {"{'key0':'val 0','key1':val1,}"};
  int retval = find_dict_value(key, &val, dict);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(val_ans) == strlen(val));
  for(size_t i = 0; i < strlen(val_ans) && i < strlen(val); i++){
    CU_ASSERT(val_ans[i] == val[i]);
  }
  free(val);
}

// abnormal use (key not found)
static void test_find_dict_value_case2(void){
  // answers
  const int retval_ans = -1;
  // args
  const char key[] = {"'key2'"};
  char *val = NULL;
  const char dict[] = {"{'key0':'val 0','key1':val1}"};
  int retval = find_dict_value(key, &val, dict);
  CU_ASSERT(retval_ans == retval);
  free(val);
}

// abnormal use (zero-length key)
static void test_find_dict_value_case3(void){
  // answers
  const int retval_ans = -1;
  // args
  const char key[] = {""};
  char *val = NULL;
  const char dict[] = {"{'key0':'val 0','key1':val1}"};
  int retval = find_dict_value(key, &val, dict);
  CU_ASSERT(retval_ans == retval);
  free(val);
}

// abnormal use (zero-length dict)
static void test_find_dict_value_case4(void){
  // answers
  const int retval_ans = -1;
  // args
  const char key[] = {"'key0'"};
  char *val = NULL;
  const char dict[] = {""};
  int retval = find_dict_value(key, &val, dict);
  CU_ASSERT(retval_ans == retval);
  free(val);
}

// normal use
static void test_extract_shape_case0(void){
  // answers
  const int retval_ans = 0;
  const size_t ndim_ans = 0;
  const size_t *shape_ans = NULL;
  // args
  size_t ndim;
  size_t *shape = NULL;
  const char val[] = {"()"};
  int retval = extract_shape(&ndim, &shape, val);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(ndim_ans == ndim);
  for(size_t i = 0; i < ndim; i++){
    CU_ASSERT(shape_ans[i] == shape[i]);
  }
  free(shape);
}

// normal use
static void test_extract_shape_case1(void){
  // answers
  const int retval_ans = 0;
  const size_t ndim_ans = 1;
  const size_t shape_ans[] = {3};
  // args
  size_t ndim;
  size_t *shape = NULL;
  const char val[] = {"(3)"};
  int retval = extract_shape(&ndim, &shape, val);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(ndim_ans == ndim);
  for(size_t i = 0; i < ndim; i++){
    CU_ASSERT(shape_ans[i] == shape[i]);
  }
  free(shape);
}

// normal use
static void test_extract_shape_case2(void){
  // answers
  const int retval_ans = 0;
  const size_t ndim_ans = 1;
  const size_t shape_ans[] = {3};
  // args
  size_t ndim;
  size_t *shape = NULL;
  const char val[] = {"(3,)"};
  int retval = extract_shape(&ndim, &shape, val);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(ndim_ans == ndim);
  for(size_t i = 0; i < ndim; i++){
    CU_ASSERT(shape_ans[i] == shape[i]);
  }
  free(shape);
}

// normal use
static void test_extract_shape_case3(void){
  // answers
  const int retval_ans = 0;
  const size_t ndim_ans = 2;
  const size_t shape_ans[] = {3,15};
  // args
  size_t ndim;
  size_t *shape = NULL;
  const char val[] = {"(3, 15 ,)"};
  int retval = extract_shape(&ndim, &shape, val);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(ndim_ans == ndim);
  for(size_t i = 0; i < ndim; i++){
    CU_ASSERT(shape_ans[i] == shape[i]);
  }
  free(shape);
}

// abnormal use, give NULL
static void test_extract_shape_case4(void){
  // answers
  const int retval_ans = -1;
  // args
  size_t ndim;
  size_t *shape = NULL;
  const char *val = NULL;
  int retval = extract_shape(&ndim, &shape, val);
  CU_ASSERT(retval_ans == retval);
  free(shape);
}

// normal use
static void test_extract_dtype_case0(void){
  // answers
  const int retval_ans = 0;
  const char dtype_ans[] = {"<f8"};
  // args
  char *dtype = NULL;
  const char val[] = {"<f8"};
  int retval = extract_dtype(&dtype, val);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(strlen(dtype_ans) == strlen(dtype));
  for(size_t i = 0; i < strlen(dtype_ans) && i < strlen(dtype); i++){
    CU_ASSERT(dtype_ans[i] == dtype[i]);
  }
  free(dtype);
}

// abnormal use, give NULL
static void test_extract_dtype_case1(void){
  // answers
  const int retval_ans = -1;
  // args
  char *dtype = NULL;
  const char *val = NULL;
  int retval = extract_dtype(&dtype, val);
  CU_ASSERT(retval_ans == retval);
  free(dtype);
}

// normal use
static void test_extract_is_fortran_order_case0(void){
  // answers
  const int retval_ans = 0;
  const bool is_fortran_order_ans = true;
  // args
  bool is_fortran_order;
  const char val[] = {"True"};
  int retval = extract_is_fortran_order(&is_fortran_order, val);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(is_fortran_order_ans == is_fortran_order);
}

// normal use
static void test_extract_is_fortran_order_case1(void){
  // answers
  const int retval_ans = 0;
  const bool is_fortran_order_ans = false;
  // args
  bool is_fortran_order;
  const char val[] = {"False"};
  int retval = extract_is_fortran_order(&is_fortran_order, val);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(is_fortran_order_ans == is_fortran_order);
}

// abnormal use, find both
static void test_extract_is_fortran_order_case2(void){
  // answers
  const int retval_ans = -1;
  // args
  bool is_fortran_order;
  const char val[] = {"True and False"};
  int retval = extract_is_fortran_order(&is_fortran_order, val);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, find none of them
static void test_extract_is_fortran_order_case3(void){
  // answers
  const int retval_ans = -1;
  // args
  bool is_fortran_order;
  const char val[] = {""};
  int retval = extract_is_fortran_order(&is_fortran_order, val);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, give NULL
static void test_extract_is_fortran_order_case4(void){
  // answers
  const int retval_ans = -1;
  // args
  bool is_fortran_order;
  const char *val = NULL;
  int retval = extract_is_fortran_order(&is_fortran_order, val);
  CU_ASSERT(retval_ans == retval);
}

/* only functions which do not need FILE streams are tested for now */
CUNIT_CI_RUN(
    "reader",
    CUNIT_CI_TEST(test_find_dict_value_case0),
    CUNIT_CI_TEST(test_find_dict_value_case1),
    CUNIT_CI_TEST(test_find_dict_value_case2),
    CUNIT_CI_TEST(test_find_dict_value_case3),
    CUNIT_CI_TEST(test_find_dict_value_case4),
    CUNIT_CI_TEST(test_extract_shape_case0),
    CUNIT_CI_TEST(test_extract_shape_case1),
    CUNIT_CI_TEST(test_extract_shape_case2),
    CUNIT_CI_TEST(test_extract_shape_case3),
    CUNIT_CI_TEST(test_extract_shape_case4),
    CUNIT_CI_TEST(test_extract_dtype_case0),
    CUNIT_CI_TEST(test_extract_dtype_case1),
    CUNIT_CI_TEST(test_extract_is_fortran_order_case0),
    CUNIT_CI_TEST(test_extract_is_fortran_order_case1),
    CUNIT_CI_TEST(test_extract_is_fortran_order_case2),
    CUNIT_CI_TEST(test_extract_is_fortran_order_case3),
    CUNIT_CI_TEST(test_extract_is_fortran_order_case4)
);

