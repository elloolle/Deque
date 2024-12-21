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
};
int main() {
  // test6();
  Deque<int> d;
  for (int i = 0; i < 10; ++i) {
    d.push_back(i);
  }
  d.Print();
  d.insert(d.begin()+3,100);
  d.Print();
}
