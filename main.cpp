#include <iostream>
#include <string>
#include "deque.h"
#include "small_tests.h"
using namespace std;
template<typename T>
bool equal(const Deque<T>& d1, const Deque<T>& d2) {
  string s1;
  string s2;
  return d1.to_string2() == d2.to_string2();
}
int main() {
  test6();
  // Deque<int> d;
}
