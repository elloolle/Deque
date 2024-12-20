#include <algorithm>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "deque.h"

void test1() {
  Deque<int> d(10, 3);

  d[3] = 5;

  d[7] = 8;

  d[9] = 10;

  std::string s = "33353338310";
  std::string ss;
  Deque<int> dd;
  d.Print();
  {
    Deque<int> d2 = d;
    d2.Print();
    dd = d2;
    dd.Print();
  }

  d[1] = 2;

  d.at(2) = 1;

  try {
    d.at(10) = 0;
    assert(false);
  } catch (std::out_of_range&) {}

  const Deque<int>& ddd = dd;
  for (size_t i = 0; i < d.size(); ++i) {
    ss += std::to_string(ddd[i]);
  }

  assert(s == ss);
}
