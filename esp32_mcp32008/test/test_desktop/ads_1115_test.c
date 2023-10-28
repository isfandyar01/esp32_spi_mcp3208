#include <unity.h>

void setUp(void) {} /* Is run before every test, put unit init calls here. */
void tearDown(void) {
} /* Is run after every test, put unit clean-up calls here. */

/**
 * @brief adds two ints
 *
 * @param x
 * @param y
 * @return int
 */
int add(int x, int y) { return x + y; }

void test_add_two_ints(void) {
  int sum = add(1, 4);
  TEST_ASSERT_EQUAL_INT32(6, sum);
}

void RUN_UNITY_TESTS() {
  UNITY_BEGIN();
  RUN_TEST(test_add_two_ints);
  UNITY_END();
}

int main(int argc, char **argv) {
  RUN_UNITY_TESTS();
  return 0;
}