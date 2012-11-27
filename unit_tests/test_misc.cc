#include <stdint.h>

#include <algorithm>
#include <functional>

#include <gtest/gtest.h>
#include <unit_tests/test_main.hpp>
#include <jellyfish/misc.hpp>

namespace {
using testing::Types;

template <typename T>
class FloorLog2Test : public ::testing::Test {
public:
  T x;

  FloorLog2Test() : x(1) {}
};

typedef Types<uint32_t, uint64_t> Implementations;
TYPED_TEST_CASE(FloorLog2Test, Implementations);

TYPED_TEST(FloorLog2Test, FloorLog2) {
  unsigned int i = 0;

  for(i = 0; i < 8 * sizeof(this->x); i++) {
    ASSERT_EQ(i, jellyfish::floorLog2(this->x << i));
    ASSERT_EQ(i, jellyfish::floorLog2((this->x << i) | ((this->x << i) - 1)));
  }
}

TEST(Random, Bits) {
  uint64_t m = 0;
  uint64_t m2 = 0;
  int not_zero = 0;
  for(int i = 0; i < 1024; ++i) {
    m        += jellyfish::random_bits(41);
    m2       += random_bits(41);
    not_zero += random_bits() > 0;
  }
  EXPECT_LT((uint64_t)1 << 49, m); // Should be false with very low probability
  EXPECT_LT((uint64_t)1 << 49, m2); // Should be false with very low probability
  EXPECT_LT(512, not_zero);
}

TEST(BinarySearchFirst, Int) {
  static int size = 1024;

  for(int i = 0; i < size; ++i)
    EXPECT_EQ(i, *binary_search_first_false(pointer_integer<int>(0), pointer_integer<int>(size),
                                            std::bind2nd(std::less<int>(), i)));
}

TEST(Slices, NonOverlapAll) {
  for(int iteration = 0; iteration < 100; ++iteration) {
    unsigned int size      = random_bits(20);
    unsigned int nb_slices = random_bits(4) + 1;
    SCOPED_TRACE(::testing::Message() << "iteration:" << iteration
                 << " size:" << size << " nb_slices:" << nb_slices);

    unsigned int total = 0;
    unsigned int prev  = 0;
    for(unsigned int i = 0; i < nb_slices; ++i) {
      SCOPED_TRACE(::testing::Message() << "i:" << i);
      std::pair<unsigned int, unsigned int> b = jellyfish::slice(i, nb_slices, size);
      ASSERT_EQ(prev, b.first);
      ASSERT_GT(b.second, b.first);
      total += b.second - b.first;
      prev   = b.second;
    }
    ASSERT_EQ(size, total);
  }
}

TEST(Shifts, LeftRight) {
  for(uint64_t s = 0, m = 1; s < 65; ++s, m *= 2) {
    uint64_t x = random_bits(64);
    SCOPED_TRACE(::testing::Message() << "s:" << s << " m:" << m
                 << " x:" << x);
    EXPECT_EQ(m ? x / m : 0, jellyfish::rshift(x, s));
    EXPECT_EQ(x * m, jellyfish::lshift(x, s));
  }
}

TEST(BitMask, Create) {
  for(uint64_t s = 0, m = 1; s < 65; ++s, m *= 2) {
    EXPECT_EQ(m, jellyfish::bitmask<uint64_t>(s) + 1);
  }
}
}
