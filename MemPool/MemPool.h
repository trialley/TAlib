#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <utility>
#include "../Mutex/MutexLock.h"

#define BlockSize 4096

struct Slot {
  Slot* next;
};

class MemPool {
 private:
  int slot_size;

  Slot* currentBlock;
  Slot* currentSlot;
  Slot* lastSlot;
  Slot* freeSlot;

  MutexLock m_freeSlot;
  MutexLock m_other;
  size_t padPointer(char* p, size_t align);
  Slot* allocateBlock();
  Slot* nofree_solve();

 public:
  MemPool();
  ~MemPool();
  void init(int size);
  Slot* allocate();
  void deallocate(Slot* p);
};

void* operator new(size_t);
void operator delete(void* p, size_t size);

void* use_memory(size_t size);
void free_memory(size_t size, void* p);
MemPool& get_MemPool(int id);

template <class T, class... Args>
T* newElement(Args&&... args) {
  T* p;
  if (p = reinterpret_cast<T*>(use_memory(sizeof(T))))
    new (p) T(std::forward<Args>(args)...);
  return p;
}

template <class T>
void deleteElement(T* p) {
  if (p)
    p->~T();
  free_memory(sizeof(T), reinterpret_cast<void*>(p));
}
