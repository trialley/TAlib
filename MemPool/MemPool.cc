
#include "MemPool.h"

MemPool::MemPool() {
}

void MemPool::init(int size) {
  slot_size = size;
  currentBlock = currentSlot = lastSlot = freeSlot = NULL;
}

MemPool::~MemPool() {
  Slot* curr = currentBlock;
  while (curr) {
    Slot* prev = curr->next;
    free(reinterpret_cast<void*>(curr));
    //operator delete(reinterpret_cast<void *>(curr));
    curr = prev;
  }
}

inline size_t MemPool::padPointer(char* p, size_t align) {
  uintptr_t result = reinterpret_cast<uintptr_t>(p);
  //printf("align=%lu\n",align);
  return ((align - result) % align);
}

Slot* MemPool::allocateBlock() {
  char* newBlock = NULL;
  while (!(newBlock = reinterpret_cast<char*>(malloc(BlockSize))))
    ;
  char* body = newBlock + sizeof(Slot);
  //printf("slot_size=%d\n",slot_size);
  size_t bodyPadding = padPointer(body, static_cast<size_t>(slot_size));
  //data_pointer newBlock=reinterpret_cast<data_pointer>(operator new(BlockSize));
  Slot* useSlot;
  {
    MutexLockGuard lock(m_other);
    reinterpret_cast<Slot*>(newBlock)->next = currentBlock;
    currentBlock = reinterpret_cast<Slot*>(newBlock);
    //char* body=newBlock+sizeof(Slot);
    //size_t bodyPadding=padPointer(body,slot_size);
    currentSlot = reinterpret_cast<Slot*>(body + bodyPadding);
    lastSlot = reinterpret_cast<Slot*>(newBlock + BlockSize - slot_size + 1);
    useSlot = currentSlot;
    currentSlot += (slot_size >> 3);
  }
  return useSlot;
}

Slot* MemPool::nofree_solve() {
  if (currentSlot >= lastSlot)
    return allocateBlock();
  Slot* useSlot;
  {
    MutexLockGuard lock(m_other);
    useSlot = currentSlot;
    currentSlot += (slot_size >> 3);
  }
  return useSlot;
}

Slot* MemPool::allocate() {
  if (freeSlot) {
    {
      MutexLockGuard lock(m_freeSlot);
      if (freeSlot) {
        Slot* result = freeSlot;
        freeSlot = freeSlot->next;
        return result;
      }
    }
  }
  return nofree_solve();
}

inline void MemPool::deallocate(Slot* p) {
  if (p) {
    MutexLockGuard lock(m_freeSlot);
    p->next = freeSlot;
    freeSlot = p;
  }
}

void* use_memory(size_t size) {
  if (!size)
    return nullptr;
  if (size > 512)
    return malloc(size);
  return reinterpret_cast<void*>(get_MemPool(((size + 7) >> 3) - 1).allocate());
}

void free_memory(size_t size, void* p) {
  if (!p)
    return;
  if (size > 512) {
    free(p);
    return;
  }
  get_MemPool(((size + 7) >> 3) - 1).deallocate(reinterpret_cast<Slot*>(p));
}

// void operator delete(void* p, size_t size) {
//   free_memory(size, p);
// }
// void* operator new(size_t size) {
//   use_memory(size);
// }

static int __attribute__((constructor)) init_MemPool() {
  for (int i = 0; i < 64; ++i)
    get_MemPool(i).init((i + 1) << 3);
}

MemPool& get_MemPool(int id) {
  static MemPool MemPool[64];
  return MemPool[id];
}
