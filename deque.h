#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <exception>
#include <ranges>
#include <string>

template<typename T>
struct Deque {
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
  int half_size()const {
    return (backets_size-1)/2;
  }
  // этот метод возвращает ссылку на первый backet
  T*& firstBacketRef() {
    return backets[firstBacket];
  }
  T*& lastBacketRef() {
    return backets[lastBacket];
  }
  T *const& firstBacketRef()const {
    return backets[firstBacket];
  }
  T *const& lastBacketRef()const {
    return backets[lastBacket];
  }

  static void DeleteFullBacket(T*& backet) {
    for (size_t i = 0; i < kBacketSize; ++i) {
      backet[i].~T();
    }
    delete[] reinterpret_cast<char*>(backet);
  }
  static void DeleteEmptyBacket(T*& backet) {
    delete[] reinterpret_cast<char*>(backet);
  }
  void DeleteBackets() {
    for (int i = firstBacket+1; i <= lastBacket-1; ++i) {
      DeleteFullBacket(backets[i]);
    }
    for (int i = -half_size(); i < firstBacket; ++i) {
      DeleteEmptyBacket(backets[i]);
    }
    for (int i = lastBacket+1; i <= half_size(); ++i) {
      DeleteEmptyBacket(backets[i]);
    }
  }
  static void DeleteLastBacket(T*& backet,size_t firstElement, size_t lastElement) {
    for (size_t i = firstElement; i <= lastElement; ++i) {
      backet[i].~T();
    }
    delete[] reinterpret_cast<char*>(backet);
  }
  void DeleteAllBackets() {
    DeleteBackets();
    DeleteLastBacket(firstBacketRef(),firstElement,kBacketSize-1);
    if(firstBacket==lastBacket) {
      return;
    }
    DeleteLastBacket(lastBacketRef(),0,lastElement);
  }

  // скопировать все элементы в диапазоне [firstElement,lastElement]  из backet1 в backet2
  static void copyLastBackets(T *const & backet1 ,T*& backet2, size_t firstElement, size_t lastElement) {
    for (size_t i = firstElement; i <= lastElement; ++i) {
      new (backet2+i) T(backet1[i]);
    }
  }
  void copyLastBackets(const Deque& d) {
    if(firstBacket == lastBacket) {
      copyLastBackets(d.lastBacketRef(), lastBacketRef(),firstElement, lastElement);
      return;
    }
    copyLastBackets(d.firstBacketRef(),firstBacketRef(),firstElement, kBacketSize-1);
    copyLastBackets(d.lastBacketRef(), lastBacketRef(),0, lastElement);
  }
  // скопировать все бакеты кроме последнего и первого из 1 в 2
  void copyBackets(const Deque& d) {
    if(d.backets_size == 0) {
      return;
    }
    for (int i = d.firstBacket+1; i < d.lastBacket; ++i) {
      for (size_t j = 0; j < kBacketSize; ++j) {
        new (backets[i]+j) T(d.backets[i][j]);
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
    if(new_backet_size%2==0) {
      throw std::length_error("wrong new_backet_size");
    }
    backets = new T*[new_backet_size];
    for (size_t i = 0; i < new_backet_size; ++i) {
      backets[i] = reinterpret_cast<T*>(new char[kBacketSize*sizeof(T)]);
    }
    backets_size = new_backet_size;
    backets+=half_size();
  }

  Deque():
  backets_size(0),
  sz(0),
  firstBacket(0),
  lastBacket(0)
  {}
  Deque(Deque&&) = default;
  Deque(int n,const T& value):
        sz(n)
      , firstElement(0)
      , lastElement((n-1)%kBacketSize)
  {
    backets_size = 2*std::ceil(static_cast<double>(n) / kBacketSize);
    if(backets_size%2==0) {
      ++backets_size;
    }
    if(backets_size == 1) {
      backets_size+=2;
    }
    allocate_memory(backets_size);
    firstBacket = 0;
    lastBacket = (n-1)/kBacketSize;

    int filled_count = 0;
    for (int i = firstBacket; i <= lastBacket; ++i) {
      for (size_t j = 0; j < kBacketSize; ++j){
        if(filled_count==n) {
          return;
        }
        new (backets[i]+j) T(value);
        ++filled_count;
      }
    }
  }
  Deque(int n):Deque(n,T()){}
  Deque(const Deque& d)
    : sz(d.sz),
      backets_size(d.backets_size),
      firstBacket(d.firstBacket),
      lastBacket(d.lastBacket),
      firstElement(d.firstElement),
      lastElement(d.lastElement)
  {
    if(sz == 0) {
      return;
    }
    allocate_memory(d.backets_size);
    copyAllBackets(d);
  }
  Deque& operator=(const Deque& d) {
    if(&d == this) {
      return *this;
    }
    Deque copy = d;
    swap(copy);
    return *this;
  }

  ~Deque() {
    if(sz == 0) {
      return;
    }
    DeleteAllBackets();
    T** copy_backets = backets-half_size();
    delete[] copy_backets;
  }

  void swap(Deque& d) {
    std::swap(backets,d.backets);
    std::swap(backets_size,d.backets_size);
    std::swap(sz,d.sz);
    std::swap(firstElement,d.firstElement);
    std::swap(lastElement,d.lastElement);
    std::swap(firstBacket,d.firstBacket);
    std::swap(lastBacket,d.lastBacket);
  }
  size_t size()const {
    return sz;
  }
  T& operator[](size_t i) {
    size_t pos = firstElement+i%kBacketSize;
    int backet = firstBacket+i/kBacketSize+pos/kBacketSize;
    pos%=kBacketSize;
    return backets[backet][pos];
  }
  const T& operator[](size_t i)const {
    size_t pos = firstElement+i%kBacketSize;
    int backet = firstBacket+i/kBacketSize+pos/kBacketSize;
    pos%=kBacketSize;
    return backets[backet][pos];
  }
  T& at(size_t i) {
    size_t pos = firstElement+i%kBacketSize;
    int backet = firstBacket+i/kBacketSize+pos/kBacketSize;
    pos%=kBacketSize;
    if(backet>lastBacket) {
      throw std::out_of_range("Invalid index");
    }
    if(backet == lastBacket && pos>lastElement) {
      throw std::out_of_range("Invalid index");
    }
    if(backet == firstBacket && pos<firstElement) {
      throw std::out_of_range("Invalid index");
    }
    return backets[backet][pos];
  }
  const T& at(size_t i)const {
    size_t pos = firstElement+i%kBacketSize;
    int backet = firstBacket+i/kBacketSize+pos/kBacketSize;
    pos%=kBacketSize;
    if(backet>lastBacket) {
      throw std::out_of_range("Invalid index");
    }
    if(backet == lastBacket && pos>lastElement) {
      throw std::out_of_range("Invalid index");
    }
    if(backet == firstBacket && pos<firstElement) {
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
    if(lastElement == 0) {
      --lastBacket;
      lastElement = kBacketSize-1;
    }
    else {
      --lastElement;
    }
  }
  void pop_front() {
    backets[firstBacket][firstElement].~T();
    --sz;
    if(firstElement == kBacketSize-1) {
      ++firstBacket;
      firstElement = 0;
    }
    else {
      ++firstElement;
    }
  }
  void push_back(const T& value) {
    if(lastBacket == half_size() && lastElement == kBacketSize-1) {
      reserve(backets_size*2+1);
    }
    if(sz == 0) {
      allocate_memory(kBeginSize);
      new (backets[0]) T(value);
      firstElement = 0;
      lastElement = 0;
      sz = 1;
      return;
    }
    // теперь памяти точно хватит на новый элемент
    ++sz;
    if(lastElement == kBacketSize-1) {
      ++lastBacket;
      lastElement=0;
    }
    else {
      ++lastElement;
    }
    new (backets[lastBacket]+lastElement) T(value);
  }
  void push_front(const T& value) {
    if(firstBacket == -half_size() && firstElement == 0) {
      reserve(backets_size*2+1);
    }
    if(sz == 0) {
      allocate_memory(kBeginSize);
      new (backets[0]) T(value);
      firstElement = 0;
      lastElement = 0;
      sz = 1;
      return;
    }
    ++sz;
    if(firstElement == 0) {
      --firstBacket;
      firstElement = kBacketSize-1;
    }
    else {
      --firstElement;
    }
    new (backets[firstBacket]+firstElement) T(value);
  }

  void Print()const {
    for (int i = 0; i < size(); ++i) {
      std::cerr<<(*this)[i]<<" ";
    }
    std::cerr<<'\n';
  }
  std::string to_string()const {
    std::string s;
    for (int i = 0; i < size(); ++i) {
      s+=(std::to_string(operator[](i)));
    }
    return s;
  }
  std::string to_string2()const {
    std::string s;
    for (int i = 0; i < size(); ++i) {
      s+=(operator[](i));
    }
    return s;
  }
};
