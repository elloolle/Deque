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
  Deque<int> a(10,10);
  auto b = a;
  for (int i = 10; i < 1000; ++i) {
    Deque<string> c(i,"294350");
    auto l = c;
    if(!(equal(c,l))) {
      cout<<c.to_string2()<<' ';
      cout<<l.to_string2();
      break;
    }
  }
}
