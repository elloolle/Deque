#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <exception>
#include <ranges>
#include <string>
#include <iterator>
#include <strings.h>
#include <compare>
template<typename T>
struct Deque {
  template<bool is_const>
  struct universe_iterator;
  T** backets;
  // backets_size - всегда нечетно. Один центральный бакет и одинаковое кол-во
  // бакетов вправо и влево
  size_t backets_size;
  size_t sz;
  // индекс первого элемента в первом бакете
  size_t firstElement;
  size_t lastElement;
  int firstBacket;
  int lastBacket;
  static constexpr size_t kBacketSize = 32;
  static constexpr size_t kBeginSize = 3;
  using iterator = universe_iterator<false>;
  using const_iterator = universe_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  int half_size() const {
    return (backets_size - 1) / 2;
  }
  // этот метод возвращает ссылку на первый backet
  T*& firstBacketRef() {
    return backets[firstBacket];
  }
  T*& lastBacketRef() {
    return backets[lastBacket];
  }
  T* const& firstBacketRef() const {
    return backets[firstBacket];
  }
  T* const& lastBacketRef() const {
    return backets[lastBacket];
  }

  static void DeleteFullBacket(T*& backet) {
    for (size_t i = 0; i < kBacketSize; ++i) {
      backet[i].~T();
    }
    delete[] reinterpret_cast<char *>(backet);
  }
  static void DeleteEmptyBacket(T*& backet) {
    delete[] reinterpret_cast<char *>(backet);
  }
  void DeleteBackets() {
    for (int i = firstBacket + 1; i <= lastBacket - 1; ++i) {
      DeleteFullBacket(backets[i]);
    }
    for (int i = -half_size(); i < firstBacket; ++i) {
      DeleteEmptyBacket(backets[i]);
    }
    for (int i = lastBacket + 1; i <= half_size(); ++i) {
      DeleteEmptyBacket(backets[i]);
    }
  }
  static void DeleteLastBacket(T*& backet,
                               size_t firstElement,
                               size_t lastElement) {
    for (size_t i = firstElement; i <= lastElement; ++i) {
      backet[i].~T();
    }
    delete[] reinterpret_cast<char *>(backet);
  }
  void DeleteAllBackets() {
    DeleteBackets();
    DeleteLastBacket(firstBacketRef(), firstElement, kBacketSize - 1);
    if (firstBacket == lastBacket) {
      return;
    }
    DeleteLastBacket(lastBacketRef(), 0, lastElement);
  }

  // скопировать все элементы в диапазоне [firstElement,lastElement]  из backet1 в backet2
  static void copyLastBackets(T* const & backet1,
                              T*& backet2,
                              size_t firstElement,
                              size_t lastElement) {
    for (size_t i = firstElement; i <= lastElement; ++i) {
      new(backet2 + i) T(backet1[i]);
    }
  }
  void copyLastBackets(const Deque& d) {
    if (firstBacket == lastBacket) {
      copyLastBackets(d.lastBacketRef(),
                      lastBacketRef(),
                      firstElement,
                      lastElement);
      return;
    }
    copyLastBackets(d.firstBacketRef(),
                    firstBacketRef(),
                    firstElement,
                    kBacketSize - 1);
    copyLastBackets(d.lastBacketRef(), lastBacketRef(), 0, lastElement);
  }
  // скопировать все бакеты кроме последнего и первого из 1 в 2
  void copyBackets(const Deque& d) {
    if (d.backets_size == 0) {
      return;
    }
    for (int i = d.firstBacket + 1; i < d.lastBacket; ++i) {
      for (size_t j = 0; j < kBacketSize; ++j) {
        new(backets[i] + j) T(d.backets[i][j]);
      }
    }
  }
  void copyAllBackets(const Deque& d) {
    // копируем все T во всех бакетах кроме первого и последнего
    copyBackets(d);
    // копируем нужное количество T в первом и последнем backet
    copyLastBackets(d);
  }
  // считаем, что объект "пустой"
  void allocate_memory(size_t new_backet_size) {
    if (new_backet_size % 2 == 0) {
      throw std::length_error("wrong new_backet_size");
    }
    backets = new T *[new_backet_size];
    for (size_t i = 0; i < new_backet_size; ++i) {
      backets[i] = reinterpret_cast<T *>(new char[kBacketSize * sizeof(T)]);
    }
    backets_size = new_backet_size;
    backets += half_size();
  }

  Deque(): backets_size(0),
           sz(0),
           firstBacket(0),
           lastBacket(0) {
  }
  Deque(Deque&&) = default;
  Deque(int n, const T& value): sz(n)
                                , firstElement(0)
                                , lastElement((n - 1) % kBacketSize) {
    backets_size = 2 * std::ceil(static_cast<double>(n) / kBacketSize);
    if (backets_size % 2 == 0) {
      ++backets_size;
    }
    if (backets_size == 1) {
      backets_size += 2;
    }
    allocate_memory(backets_size);
    firstBacket = 0;
    lastBacket = (n - 1) / kBacketSize;

    int filled_count = 0;
    for (int i = firstBacket; i <= lastBacket; ++i) {
      for (size_t j = 0; j < kBacketSize; ++j) {
        if (filled_count == n) {
          return;
        }
        new(backets[i] + j) T(value);
        ++filled_count;
      }
    }
  }
  Deque(int n): Deque(n, T()) {
  }
  Deque(const Deque& d)
    : sz(d.sz),
      backets_size(d.backets_size),
      firstBacket(d.firstBacket),
      lastBacket(d.lastBacket),
      firstElement(d.firstElement),
      lastElement(d.lastElement) {
    if (sz == 0) {
      return;
    }
    allocate_memory(d.backets_size);
    copyAllBackets(d);
  }
  Deque& operator=(const Deque& d) {
    if (&d == this) {
      return *this;
    }
    Deque copy = d;
    swap(copy);
    return *this;
  }

  ~Deque() {
    if (sz == 0) {
      return;
    }
    DeleteAllBackets();
    T** copy_backets = backets - half_size();
    delete[] copy_backets;
  }

  void swap(Deque& d) {
    std::swap(backets, d.backets);
    std::swap(backets_size, d.backets_size);
    std::swap(sz, d.sz);
    std::swap(firstElement, d.firstElement);
    std::swap(lastElement, d.lastElement);
    std::swap(firstBacket, d.firstBacket);
    std::swap(lastBacket, d.lastBacket);
  }
  size_t size() const {
    return sz;
  }
  T& operator[](size_t i) {
    size_t pos = firstElement + i % kBacketSize;
    int backet = firstBacket + i / kBacketSize + pos / kBacketSize;
    pos %= kBacketSize;
    return backets[backet][pos];
  }
  const T& operator[](size_t i) const {
    size_t pos = firstElement + i % kBacketSize;
    int backet = firstBacket + i / kBacketSize + pos / kBacketSize;
    pos %= kBacketSize;
    return backets[backet][pos];
  }
  T& at(size_t i) {
    size_t pos = firstElement + i % kBacketSize;
    int backet = firstBacket + i / kBacketSize + pos / kBacketSize;
    pos %= kBacketSize;
    if (backet > lastBacket) {
      throw std::out_of_range("Invalid index");
    }
    if (backet == lastBacket && pos > lastElement) {
      throw std::out_of_range("Invalid index");
    }
    if (backet == firstBacket && pos < firstElement) {
      throw std::out_of_range("Invalid index");
    }
    return backets[backet][pos];
  }
  const T& at(size_t i) const {
    size_t pos = firstElement + i % kBacketSize;
    int backet = firstBacket + i / kBacketSize + pos / kBacketSize;
    pos %= kBacketSize;
    if (backet > lastBacket) {
      throw std::out_of_range("Invalid index");
    }
    if (backet == lastBacket && pos > lastElement) {
      throw std::out_of_range("Invalid index");
    }
    if (backet == firstBacket && pos < firstElement) {
      throw std::out_of_range("Invalid index");
    }
    return backets[backet][pos];
  }
  void reserve(size_t new_backets_size) {
    Deque copy = std::move(*this);
    allocate_memory(new_backets_size);
    // allocate_memory меняет backets и backets_size,
    // a все остальные поля имеют тривиальные типы, поэтому
    // при std::move они скопируются и не поменяют значения
    copyAllBackets(copy);
  }
  void pop_back() {
    backets[lastBacket][lastElement].~T();
    --sz;
    if (lastElement == 0) {
      --lastBacket;
      lastElement = kBacketSize - 1;
    } else {
      --lastElement;
    }
  }
  void pop_front() {
    backets[firstBacket][firstElement].~T();
    --sz;
    if (firstElement == kBacketSize - 1) {
      ++firstBacket;
      firstElement = 0;
    } else {
      ++firstElement;
    }
  }
  void push_back(const T& value) {
    if (lastBacket == half_size() && lastElement == kBacketSize - 1) {
      reserve(backets_size * 2 + 1);
    }
    if (sz == 0) {
      allocate_memory(kBeginSize);
      new(backets[0]) T(value);
      firstElement = 0;
      lastElement = 0;
      sz = 1;
      return;
    }
    // теперь памяти точно хватит на новый элемент
    ++sz;
    if (lastElement == kBacketSize - 1) {
      ++lastBacket;
      lastElement = 0;
    } else {
      ++lastElement;
    }
    new(backets[lastBacket] + lastElement) T(value);
  }
  void push_front(const T& value) {
    if (firstBacket == -half_size() && firstElement == 0) {
      reserve(backets_size * 2 + 1);
    }
    if (sz == 0) {
      allocate_memory(kBeginSize);
      new(backets[0]) T(value);
      firstElement = 0;
      lastElement = 0;
      sz = 1;
      return;
    }
    ++sz;
    if (firstElement == 0) {
      --firstBacket;
      firstElement = kBacketSize - 1;
    } else {
      --firstElement;
    }
    new(backets[firstBacket] + firstElement) T(value);
  }


  template<bool is_const>
  struct universe_iterator {
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = std::conditional_t<is_const, const T&, T&>;
    using pointer = std::conditional_t<is_const,const T*, T*>;
    using iterator_category = std::random_access_iterator_tag;
    static constexpr size_t kBacketSize = Deque::kBacketSize;
    T** backet;
    // index принадлежит диапазону [0,kBacketSize-1]
    size_t index;
    universe_iterator(T** backet, size_t index):backet(backet),index(index){}
    template<bool other_const>
    universe_iterator(const universe_iterator<other_const>& it,
                     std::enable_if_t<(!is_const && !other_const) || (is_const && other_const)
                     || (is_const && !other_const)>* = nullptr)
        : backet(it.backet), index(it.index) {}
    universe_iterator& operator+=(int n) {
      index += n % kBacketSize;
      backet += n % kBacketSize + index / kBacketSize;
      index%=kBacketSize;
      return *this;
    }
    universe_iterator& operator-=(int n) {
      n = -n;
      index += n % kBacketSize;
      backet += n % kBacketSize + index / kBacketSize;
      index%=kBacketSize;
      return *this;
    }
    universe_iterator& operator++() {
      return operator+=(1);
    }
    universe_iterator operator++(int) {
      universe_iterator copy = *this;
      operator+=(1);
      return copy;
    }
    universe_iterator& operator--() {
      return operator+=(-1);
    }
    universe_iterator operator--(int) {
      universe_iterator copy = *this;
      operator+=(-1);
      return copy;
    }
    reference operator*()const {
      return *(*backet+index);
    }
    pointer operator->()const {
      return *backet+index;
    }

    difference_type operator-(const universe_iterator& it)const {
      return (backet-it.backet)*kBacketSize + index - it.index;
    }
    friend universe_iterator operator+(int n, const universe_iterator& it) {
      auto copy = it;
      copy+=n;
      return copy;
    }
    friend universe_iterator operator+(const universe_iterator& it, int n) {
      return n+it;
    }
    friend universe_iterator operator-(const universe_iterator& it, int n) {
      auto copy = it;
      copy-=n;
      return copy;
    }
    friend universe_iterator operator-(int n, const universe_iterator& it) {
      return it-n;
    }
    bool operator==(const universe_iterator& it) const {
      return (*this - it) == 0;
    }

    bool operator!=(const universe_iterator& it) const {
      return (*this - it) != 0;
    }

    bool operator<(const universe_iterator& it) const {
      return (*this - it) < 0;
    }

    bool operator<=(const universe_iterator& it) const {
      return (*this - it) <= 0;
    }

    bool operator>(const universe_iterator& it) const {
      return (*this - it) > 0;
    }

    bool operator>=(const universe_iterator& it) const {
      return (*this - it) >= 0;
    }
  };

  iterator begin() {
    return iterator{backets+firstBacket,firstElement};
  }
  const_iterator cbegin()const {
    return const_iterator{backets+firstBacket,firstElement};
  }
  const_iterator begin()const {
    return cbegin();
  }
  iterator end() {
    return iterator{backets+lastBacket,lastElement+1};
  }
  const_iterator cend()const {
    return const_iterator{backets+lastBacket,lastElement+1};
  }
  const_iterator end()const {
    return cend();
  }
  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }
  const_reverse_iterator crbegin()const {
    return reverse_iterator(cend());
  }
  const_reverse_iterator rbegin()const {
    return crbegin();
  }
  reverse_iterator rend() {
    return reverse_iterator(begin());
  }
  const_reverse_iterator crend()const {
    return reverse_iterator(begin());
  }
  const_reverse_iterator rend()const {
    return reverse_iterator(begin());
  }
  void Print() const {
    for (int i = 0; i < size(); ++i) {
      std::cerr << (*this)[i] << " ";
    }
    std::cerr << '\n';
  }
  std::string to_string() const {
    std::string s;
    for (int i = 0; i < size(); ++i) {
      s += (std::to_string(operator[](i)));
    }
    return s;
  }
  std::string to_string2() const {
    std::string s;
    for (int i = 0; i < size(); ++i) {
      s += (operator[](i));
    }
    return s;
  }
};
