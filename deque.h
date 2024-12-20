#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <exception>
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

  static void DeleteLastBacket(T*& backet, size_t lastElement) {
    for (size_t i = 0; i <= lastElement; ++i) {
      backet[i].~T();
    }
    delete[] reinterpret_cast<char*>(backet);
  }

  void DeleteAllBackets() {
    DeleteBackets();
    DeleteLastBacket(firstBacketRef(),firstElement);
    if(firstBacket==lastBacket) {
      return;
    }
    DeleteLastBacket(lastBacketRef(),lastElement);
  }

  // создать последний бакет и скопировать все не сырые элементы из backet1 в backet2
  static void copyLastBackets(T *const & backet1 ,T*& backet2, size_t firstElement, size_t lastElement) {
    backet2 = reinterpret_cast<T*>(new char[kBacketSize*sizeof(T)]);
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
  // скопировать все бакеты кроме последнего и первого из 1 в 2, причем известно, что их размеры равны
  // и firstBacket, lastBacket тоже равны
  void copyBackets(const Deque& d) {
    if(d.backets_size == 0) {
      return;
    }
    for (int i = firstBacket+1; i < lastBacket; ++i) {
      backets[i] = reinterpret_cast<T*>(new char[kBacketSize*sizeof(T)]);
      for (size_t j = 0; j < kBacketSize; ++j) {
        new (backets[i]+j) T(d.backets[i][j]);
      }
    }
    for (int i = -half_size(); i < firstBacket; ++i) {
      backets[i] = reinterpret_cast<T*>(new char[kBacketSize*sizeof(T)]);
    }
    for (int i = lastBacket+1; i <= half_size(); ++i) {
      backets[i] = reinterpret_cast<T*>(new char[kBacketSize*sizeof(T)]);
    }
  }
  Deque():sz(0){}
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
    backets = new T*[backets_size];
    int half_size = (backets_size-1)/2;
    backets+=half_size;
    firstBacket = 0;
    lastBacket = (n-1)/kBacketSize;
    for (int i = 0; i < backets_size; ++i) {
      backets[i-half_size] = reinterpret_cast<T*>(new char[kBacketSize*sizeof(T)]);
    }
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
    backets = new T*[backets_size];
    backets+=half_size();
    // копируем нужное количество T во всех бакетах кроме последнего
    copyBackets(d);
    // копируем нужное количество T в первом и последнем backet
    copyLastBackets(d);
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
  Deque& operator=(const Deque& d) {
    if(&d == this) {
      return *this;
    }
    Deque copy = d;
    swap(copy);
    return *this;
  }
  size_t size()const {
    return sz;
  }
  T& operator[](size_t i) {
    return backets[firstBacket+i/kBacketSize][i%kBacketSize];
  }
  const T& operator[](size_t i)const {
    return backets[firstBacket+i/kBacketSize][i%kBacketSize];
  }
  T& at(size_t i) {
    int pos_of_backet = firstBacket+static_cast<int>(i/kBacketSize);
    if(pos_of_backet>lastBacket) {
      throw std::out_of_range("Invalid index");
    }
    if(pos_of_backet == lastBacket && i%kBacketSize>lastElement) {
      throw std::out_of_range("Invalid index");
    }
    if(pos_of_backet == firstBacket && i%kBacketSize<firstElement) {
      throw std::out_of_range("Invalid index");
    }
    return backets[firstBacket+i/kBacketSize][i%kBacketSize];
  }
  const T& at(size_t i)const {
    int pos_of_backet = firstBacket+static_cast<int>(i/kBacketSize);
    if(pos_of_backet>half_size()) {
      throw std::out_of_range("Invalid index");
    }
    if(pos_of_backet == half_size() && i%kBacketSize>lastElement) {
      throw std::out_of_range("Invalid index");
    }
    if(pos_of_backet == -half_size() && i%kBacketSize<firstElement) {
      throw std::out_of_range("Invalid index");
    }
    return backets[firstBacket+i/kBacketSize][i%kBacketSize];
  }
  ~Deque() {
    if(sz == 0) {
      return;
    }
    DeleteAllBackets();
    T** copy_backets = backets-(backets_size-1)/2;
    delete[] copy_backets;
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
