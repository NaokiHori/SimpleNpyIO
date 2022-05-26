#include <CUnit/CUnitCI.h>
#include "simple_npyio.c"


// assuming little-endian environment
static void test_is_big_endian(void){
  // answer
  const bool retval_ans = false;
  bool retval = is_big_endian();
  CU_ASSERT(retval_ans == retval);
}

// normal use, 1 byte byte swapping
static void test_convert_endian_case0(void){
  // answers
  const int retval_ans = 0;
  const uint8_t val_ans = 0x16;
  // args
  uint8_t val = 0x16;
  const size_t size = 1;
  int retval = convert_endian(&val, size);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(val_ans == val);
}

// normal use, 2 byte byte swapping
static void test_convert_endian_case1(void){
  // answers
  const int retval_ans = 0;
  const uint8_t vals_ans[] = {0x46, 0x16};
  // args
  uint8_t vals[] = {0x16, 0x46};
  const size_t size = 2;
  int retval = convert_endian(vals, size);
  CU_ASSERT(retval_ans == retval);
  for(size_t i = 0; i < size; i++){
    CU_ASSERT(vals_ans[i] == vals[i]);
  }
}

// normal use, 3 byte byte swapping
static void test_convert_endian_case2(void){
  // answers
  const int retval_ans = 0;
  const uint8_t vals_ans[] = {0x66, 0x46, 0x16};
  // args
  uint8_t vals[] = {0x16, 0x46, 0x66};
  const size_t size = 3;
  int retval = convert_endian(vals, size);
  CU_ASSERT(retval_ans == retval);
  for(size_t i = 0; i < size; i++){
    CU_ASSERT(vals_ans[i] == vals[i]);
  }
}

// abnormal use, NULL pointer is given
static void test_convert_endian_case3(void){
  // answers
  const int retval_ans = -1;
  // args
  uint8_t *vals = NULL;
  const size_t size = 1;
  int retval = convert_endian(vals, size);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, 0 size
static void test_convert_endian_case4(void){
  // answers
  const int retval_ans = -1;
  // args
  uint8_t val = 0x16;
  const size_t size = 0;
  int retval = convert_endian(&val, size);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, too large size
static void test_convert_endian_case5(void){
  // answers
  const int retval_ans = -1;
  // args
  uint8_t val = 0x16;
  const size_t size = SIZE_MAX;
  int retval = convert_endian(&val, size);
  CU_ASSERT(retval_ans == retval);
}

// normal use, single character from string, found
static void test_find_pattern_case0(void){
  // answers
  const int retval_ans = 0;
  const size_t loc_ans = 7;
  // args             01234567
  size_t loc;
  const char p0[] = {"hello, world"};
  const char p1[] = {"w"};
  const size_t size_p0 = strlen(p0);
  const size_t size_p1 = strlen(p1);
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(loc_ans == loc);
}

// normal use, single character from string, not found
static void test_find_pattern_case1(void){
  // answers
  const int retval_ans = -1;
  // args
  size_t loc;
  const char p0[] = {"hello, world"};
  const char p1[] = {"v"};
  const size_t size_p0 = strlen(p0);
  const size_t size_p1 = strlen(p1);
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
}

// normal use, multi characters from string, found
static void test_find_pattern_case2(void){
  // answers
  const int retval_ans = 0;
  const size_t loc_ans = 10;
  // args
  size_t loc;
  const char p0[] = {"hello, world"};
  const char p1[] = {"ld"};
  const size_t size_p0 = strlen(p0);
  const size_t size_p1 = strlen(p1);
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(loc_ans == loc);
}

// normal use, multi characters from string, not found
static void test_find_pattern_case3(void){
  // answers
  const int retval_ans = -1;
  // args
  size_t loc;
  const char p0[] = {"hello, world"};
  const char p1[] = {"le"};
  const size_t size_p0 = strlen(p0);
  const size_t size_p1 = strlen(p1);
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
}

// normal use, identical strings
static void test_find_pattern_case4(void){
  // answers
  const int retval_ans = 0;
  const size_t loc_ans = 0;
  // args
  size_t loc;
  const char p0[] = {"hello, world"};
  const char p1[] = {"hello, world"};
  const size_t size_p0 = strlen(p0);
  const size_t size_p1 = strlen(p1);
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(loc_ans == loc);
}

// normal use, uint8_t arrays
static void test_find_pattern_case5(void){
  // answers
  const int retval_ans = 0;
  const size_t loc_ans = 6;
  // args
  size_t loc;
  const uint8_t p0[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
  const uint8_t p1[] = {2, 6, 5};
  const size_t size_p0 = sizeof(p0);
  const size_t size_p1 = sizeof(p1);
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
  CU_ASSERT(loc_ans == loc);
}

// abnormal use, pattern 0 NULL input
static void test_find_pattern_case6(void){
  // answers
  const int retval_ans = -1;
  // args
  size_t loc;
  const char *p0 = NULL;
  const char *p1 = {"word"};
  const size_t size_p0 = 0;
  const size_t size_p1 = strlen(p1);
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, pattern 1 NULL input
static void test_find_pattern_case7(void){
  // answers
  const int retval_ans = -1;
  // args
  size_t loc;
  const uint8_t p0[] = {3, 1, 4};
  const uint8_t *p1 = NULL;
  const size_t size_p0 = sizeof(p0);
  const size_t size_p1 = 0;
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
}

// abnormal use, pattern 0 is shorter than pattern 1
static void test_find_pattern_case8(void){
  // answers
  const int retval_ans = -1;
  // args
  size_t loc;
  const char p0[] = {"word"};
  const char p1[] = {"hello, world"};
  const size_t size_p0 = strlen(p0);
  const size_t size_p1 = strlen(p1);
  int retval = find_pattern(&loc, p0, size_p0, p1, size_p1);
  CU_ASSERT(retval_ans == retval);
}

CUNIT_CI_RUN(
    "auxiliary",
    CUNIT_CI_TEST(test_is_big_endian),
    CUNIT_CI_TEST(test_convert_endian_case0),
    CUNIT_CI_TEST(test_convert_endian_case1),
    CUNIT_CI_TEST(test_convert_endian_case2),
    CUNIT_CI_TEST(test_convert_endian_case3),
    CUNIT_CI_TEST(test_convert_endian_case4),
    CUNIT_CI_TEST(test_convert_endian_case5),
    CUNIT_CI_TEST(test_find_pattern_case0),
    CUNIT_CI_TEST(test_find_pattern_case1),
    CUNIT_CI_TEST(test_find_pattern_case2),
    CUNIT_CI_TEST(test_find_pattern_case3),
    CUNIT_CI_TEST(test_find_pattern_case4),
    CUNIT_CI_TEST(test_find_pattern_case5),
    CUNIT_CI_TEST(test_find_pattern_case6),
    CUNIT_CI_TEST(test_find_pattern_case7),
    CUNIT_CI_TEST(test_find_pattern_case8)
);

