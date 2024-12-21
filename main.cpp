#include <iostream>
#include <string>
#include <vector>
#include "deque.h"
// #include "tests.h"


using namespace std;
template<typename T>
bool equal(const Deque<T>& d1, const Deque<T>& d2) {
  string s1;
  string s2;
  return d1.to_string2() == d2.to_string2();
}
struct S {
  int y = 0;
  S& operator+=(int x) {
    y+=x;
  }
};
int main() {
  // test6();
  Deque<S> d;
  // vector<int>::iterator it1 = 1+it;
  auto a = d.rbegin();
  a.operator->()->y;
  a->y;
}
