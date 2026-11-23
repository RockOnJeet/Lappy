// This is a simple unit test using the Unity framework for C++.

#include "unity.h"

void setUp(void) {
  // Runs before each test
  // Use to intitialize external state (the one which doesn't reset
  // automatically)
}

void tearDown(void) {
  // Runs after each test
  // Use to cleanup external state in case test fails and leaves artifacts
}

void test_addition(void) {
  TEST_ASSERT_EQUAL(4, 2 + 2);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_addition);

  return UNITY_END();
}