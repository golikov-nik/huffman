//
// Created by nigo on 01/06/19.
//

#include <gtest/gtest.h>
#include "huffman.h"
#include <string>
#include <sstream>
#include <algorithm>

using std::string;
using std::stringstream;

namespace {
  void test_string(string const& s) {
    stringstream in(s);
    stringstream encoded;
    huffman::encode(in, encoded);
    stringstream decoded;
    huffman::decode(encoded, decoded);
    ASSERT_EQ(decoded.str(), s);
  }

  void assert_fail_decode(string const& s) {
    stringstream in(s);
    stringstream out;
    ASSERT_ANY_THROW(huffman::decode(in, out));
  }

  void test_and_assert_fail(string const& s) {
    test_string(s);
    assert_fail_decode(s);
  }

  char random_char() {
    return rand() % 256;
  }

  string random_string(size_t n) {
    string result(n, '\0');
    std::generate(result.begin(), result.end(), random_char);
    return result;
  }
}

TEST(correctness, small_strings) {
  test_and_assert_fail("a");
  test_and_assert_fail("b");
  test_and_assert_fail("abacaba");
  test_and_assert_fail("1afa1114jk1e");
  test_and_assert_fail(string(10, '\0'));
}

TEST(correctness, big) {
  test_and_assert_fail(string(100000, 'a'));
}

TEST(correctness, empty) {
  test_and_assert_fail("");
}

TEST(correctness, random) {
  for (size_t n = 1; n != 1000000000; n *= 10) {
    test_and_assert_fail(random_string(n));
  }
  for (unsigned iters = 0; iters < 10; iters++) {
    std::cerr << iters << "\n";
    test_and_assert_fail(random_string(rand() & ((1u << 16u) - 1)));
  }
}
