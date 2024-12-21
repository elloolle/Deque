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
namespace TestsByUnrealf1 {
struct Fragile {
  Fragile(int durability, int data)
    : durability(durability),
      data(data) {}
  ~Fragile() = default;

  // for std::swap
  Fragile(Fragile&& other)
    : Fragile() {
    *this = other;
  }

  Fragile(const Fragile& other)
    : Fragile() {
    *this = other;
  }

  Fragile& operator=(const Fragile& other) {
    durability = other.durability - 1;
    data = other.data;
    if (durability <= 0) { throw 2; }
    return *this;
  }

  int durability;
  int data;

private:
  Fragile() {}
};

struct Explosive {
  struct Safeguard {};

  inline static bool exploded = false;

  Explosive()
    : should_explode(true) {
    throw 1;
  }

  Explosive(Safeguard)
    : should_explode(false) {}

  Explosive(const Explosive&)
    : should_explode(true) {
    throw 2;
  }

  // TODO: is this ok..?
  Explosive& operator=(const Explosive&) { return *this; }

  ~Explosive() { exploded |= should_explode; }

private:
  const bool should_explode;
};

struct DefaultConstructible {
  DefaultConstructible() { data = default_data; }

  int data = default_data;
  inline static const int default_data = 117;
};

struct NotDefaultConstructible {
  NotDefaultConstructible() = delete;
  NotDefaultConstructible(int input)
    : data(input) {}
  int data;

  auto operator<=>(const NotDefaultConstructible&) const = default;
};

struct CountedException : public std::exception {};

template <int when_throw>
struct Counted {
  inline static int counter = 0;

  Counted() {
    ++counter;
    if (counter == when_throw) {
      --counter;
      throw CountedException();
    }
  }

  Counted(const Counted&)
    : Counted() {}

  ~Counted() { --counter; }
};

template <typename iter, typename T>
struct CheckIter {
  using traits = std::iterator_traits<iter>;

  static_assert(std::is_same_v<std::remove_cv_t<typename traits::value_type>, std::remove_cv_t<T>>);
  static_assert(std::is_same_v<typename traits::pointer, T*>);
  static_assert(std::is_same_v<typename traits::reference, T&>);
  static_assert(
    std::is_same_v<typename traits::iterator_category, std::random_access_iterator_tag>);

  static_assert(std::is_same_v<decltype(std::declval<iter>()++), iter>);
  static_assert(std::is_same_v<decltype(++std::declval<iter>()), iter&>);
  static_assert(std::is_same_v<decltype(std::declval<iter>() + 5), iter>);
  static_assert(std::is_same_v<decltype(std::declval<iter>() += 5), iter&>);

  static_assert(std::is_same_v<decltype(std::declval<iter>() - std::declval<iter>()),
                               typename traits::difference_type>);
  static_assert(std::is_same_v<decltype(*std::declval<iter>()), T&>);

  static_assert(std::is_same_v<decltype(std::declval<iter>() < std::declval<iter>()), bool>);
  static_assert(std::is_same_v<decltype(std::declval<iter>() <= std::declval<iter>()), bool>);
  static_assert(std::is_same_v<decltype(std::declval<iter>() > std::declval<iter>()), bool>);
  static_assert(std::is_same_v<decltype(std::declval<iter>() >= std::declval<iter>()), bool>);
  static_assert(std::is_same_v<decltype(std::declval<iter>() == std::declval<iter>()), bool>);
  static_assert(std::is_same_v<decltype(std::declval<iter>() != std::declval<iter>()), bool>);
};

void testDefault() {
  Deque<int> defaulted;
  assert((defaulted.size() == 0));
  Deque<NotDefaultConstructible> without_default;
  assert((without_default.size() == 0));
}

void testCopy() {
  Deque<NotDefaultConstructible> without_default;
  Deque<NotDefaultConstructible> copy = without_default;
  assert((copy.size() == 0));
}

void testWithSize() {
  int size = 17;
  int value = 14;
  Deque<int> simple(size);
  assert((simple.size() == size_t(size)) &&
         std::all_of(simple.begin(), simple.end(), [](int item) { return item == 0; }));
  Deque<NotDefaultConstructible> less_simple(size, value);
  assert((less_simple.size() == size_t(size)) &&
         std::all_of(less_simple.begin(), less_simple.end(),
                     [&](const auto& item) { return item.data == value; }));
  Deque<DefaultConstructible> default_constructor(size);
  assert(std::all_of(default_constructor.begin(), default_constructor.end(), [](const auto& item) {
    return item.data == DefaultConstructible::default_data;
  }));
}

void testAssignment() {
  Deque<int> first(10, 10);
  Deque<int> second(9, 9);
  first = second;
  assert((first.size() == second.size()) && (first.size() == 9) &&
         std::equal(first.begin(), first.end(), second.begin()));
}

void testStaticAsserts() {
  using T1 = int;
  using T2 = NotDefaultConstructible;

  static_assert(std::is_default_constructible_v<Deque<T1>>, "should have default constructor");
  static_assert(std::is_default_constructible_v<Deque<T2>>, "should have default constructor");
  static_assert(std::is_copy_constructible_v<Deque<T1>>, "should have copy constructor");
  static_assert(std::is_copy_constructible_v<Deque<T2>>, "should have copy constructor");
  // static_assert(std::is_constructible_v<Deque<T1>, int>, "should have constructor from int");
  // static_assert(std::is_constructible_v<Deque<T2>, int>, "should have constructor from int");
  static_assert(std::is_constructible_v<Deque<T1>, int, const T1&>,
                "should have constructor from int and const T&");
  static_assert(std::is_constructible_v<Deque<T2>, int, const T2&>,
                "should have constructor from int and const T&");

  static_assert(std::is_copy_assignable_v<Deque<T1>>, "should have assignment operator");
  static_assert(std::is_copy_assignable_v<Deque<T2>>, "should have assignment operator");
}

void testOperatorSubscript() {
  Deque<size_t> defaulted(1300, 43);
  assert((defaulted[0] == defaulted[1280]) && (defaulted[0] == 43));
  assert((defaulted.at(0) == defaulted[1280]) && (defaulted.at(0) == 43));
  int caught = 0;

  try {
    defaulted.at(size_t(-1));
  } catch (std::out_of_range& e) { ++caught; }

  try {
    defaulted.at(1300);
  } catch (std::out_of_range& e) { ++caught; }

  assert(caught == 2);
}

void testStaticAssertsAccess() {
  Deque<size_t> defaulted;
  const Deque<size_t> constant;
  static_assert(std::is_same_v<decltype(defaulted[0]), size_t&>);
  static_assert(std::is_same_v<decltype(defaulted.at(0)), size_t&>);
  static_assert(std::is_same_v<decltype(constant[0]), const size_t&>);
  static_assert(std::is_same_v<decltype(constant.at(0)), const size_t&>);

  // static_assert(noexcept(defaulted[0]), "operator[] should not throw");
  static_assert(!noexcept(defaulted.at(0)), "at() can throw");
}


void testStaticAssertsIterators() {
  CheckIter<Deque<int>::iterator, int> iter;
  std::ignore = iter;
  CheckIter<decltype(std::declval<Deque<int>>().rbegin()), int> reverse_iter;
  std::ignore = reverse_iter;
  CheckIter<decltype(std::declval<Deque<int>>().cbegin()), const int> const_iter;
  std::ignore = const_iter;

  static_assert(std::is_convertible_v<decltype(std::declval<Deque<int>>().begin()),
                                      decltype(std::declval<Deque<int>>().cbegin())>,
                "should be able to construct const iterator from non const iterator");
  static_assert(!std::is_convertible_v<decltype(std::declval<Deque<int>>().cbegin()),
                                       decltype(std::declval<Deque<int>>().begin())>,
                "should NOT be able to construct iterator from const iterator");
}

void testIteratorsArithmetic() {
  Deque<int> empty;
  assert((empty.end() - empty.begin()) == 0);
  assert((empty.begin() + 0 == empty.end()) && (empty.end() - 0 == empty.begin()));

  Deque<int> one(1);
  auto iter2 = one.end();
  assert(((--iter2) == one.begin()));

  assert((empty.rend() - empty.rbegin()) == 0);
  assert((empty.rbegin() + 0 == empty.rend()) && (empty.rend() - 0 == empty.rbegin()));
  auto r_iter = empty.rbegin();
  assert((r_iter++ == empty.rbegin()));

  assert((empty.cend() - empty.cbegin()) == 0);
  assert((empty.cbegin() + 0 == empty.cend()) && (empty.cend() - 0 == empty.cbegin()));
  auto c_iter = empty.cbegin();
  assert((c_iter++ == empty.cbegin()));

  Deque<int> d(1000, 3);
  assert(size_t((d.end() - d.begin())) == d.size());
  assert((d.begin() + d.size() == d.end()) && (d.end() - d.size() == d.begin()));
}

void testIteratorsComparison() {
  Deque<int> d(1000, 3);

  assert(d.end() > d.begin());
  assert(d.cend() > d.cbegin());
  assert(d.rend() > d.rbegin());
}

void testIteratorsAlgorithms() {
  Deque<int> d(1000, 3);

  std::iota(d.begin(), d.end(), 13);
  std::mt19937 g(31415);
  std::shuffle(d.begin(), d.end(), g);
  std::sort(d.rbegin(), d.rbegin() + 500);
  std::reverse(d.begin(), d.end());
  auto sorted_border = std::is_sorted_until(d.begin(), d.end());
  // std::copy(d.begin(), d.end(), std::ostream_iterator<int>(std::cout, " "));
  // std::cout << std::endl;
  assert(sorted_border - d.begin() == 500);
}

void testPushAndPop() {
  Deque<NotDefaultConstructible> d(10000, {1});
  auto start_size = d.size();

  auto middle = &(*(d.begin() + start_size / 2));  // 5000
  auto& middle_element = *middle;
  auto begin = &(*d.begin());
  auto end = &(*d.rbegin());

  auto middle2 = &(*((d.begin() + start_size / 2) + 2000));  // 7000

  // remove 400 elements
  for (size_t i = 0; i < 400; ++i) {
    d.pop_back();
  }

  // begin and middle pointers are still valid
  assert(begin->data == 1);
  assert(middle->data == 1);
  assert(middle_element.data == 1);
  assert(middle2->data == 1);

  end = &*d.rbegin();

  // 800 elemets removed in total
  for (size_t i = 0; i < 400; ++i) {
    d.pop_front();
  }

  // and and middle iterators are still valid
  assert(end->data == 1);
  assert(middle->data == 1);
  assert(middle_element.data == 1);
  assert(middle2->data == 1);

  // removed 9980 items in total
  for (size_t i = 0; i < 4590; ++i) {
    d.pop_front();
    d.pop_back();
  }

  assert(d.size() == 20);
  assert(middle_element.data == 1);
  assert(middle->data == 1 && middle->data == 1);
  assert(std::all_of(d.begin(), d.end(), [](const auto& item) { return item.data == 1; }));

  begin = &*d.begin();
  end = &*d.rbegin();

  for (size_t i = 0; i < 5500; ++i) {
    d.push_back({2});
    d.push_front({2});
  }

  assert((*begin).data == 1);
  assert((*end).data == 1);
  assert(d.begin()->data == 2);
  assert(d.size() == 5500 * 2 + 20);
  assert(std::count(d.begin(), d.end(), NotDefaultConstructible{1}) == 20);
  assert(std::count(d.begin(), d.end(), NotDefaultConstructible{2}) == 11000);
}

void testInsertAndErase() {
  Deque<NotDefaultConstructible> d(10000, {1});
  auto start_size = d.size();

  d.insert(d.begin() + start_size / 2, NotDefaultConstructible{2});
  assert(d.size() == start_size + 1);
  d.erase(d.begin() + start_size / 2 - 1);
  assert(d.size() == start_size);

  assert(size_t(std::count(d.begin(), d.end(), NotDefaultConstructible{1})) == start_size - 1);
  assert(std::count(d.begin(), d.end(), NotDefaultConstructible{2}) == 1);

  Deque<NotDefaultConstructible> copy;
  for (const auto& item : d) {
    copy.insert(copy.end(), item);
  }
  // std::copy(d.cbegin(), d.cend(), std::inserter(copy, copy.begin()));

  assert(d.size() == copy.size());
  assert(std::equal(d.begin(), d.end(), copy.begin()));
}

void testExceptions() {
  try {
    Deque<Counted<17>> d(100);
  } catch (CountedException& e) { assert(Counted<17>::counter == 0); } catch (...) {
    // should have caught same exception as thrown by Counted
    assert(false);
  }

  try {
    Deque<Explosive> d(100);
  } catch (...) {}

  try {
    Deque<Explosive> d;
  } catch (...) {
    // no objects should have been created
    assert(false);
  }
  assert(Explosive::exploded == false);

  try {
    Deque<Explosive> d;
    auto safe = Explosive(Explosive::Safeguard{});
    d.push_back(safe);
  } catch (...) {}

  // Destructor should not be called for an object
  // with no finihshed constructor
  // the only destructor called - safe explosive with the safeguard
  assert(Explosive::exploded == false);
}
}  // namespace TestsByUnrealf1