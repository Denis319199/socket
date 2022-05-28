// No copyright

#include <fiber/StackPool.hpp>

StackPool StackPool::global_stack_pool_instance_{};

StackPool &StackPool::GetStackPool() { return global_stack_pool_instance_; }

void *StackPool::Allocate() {
  for (auto &entry : stack_pool_) {
    if (!entry.is_busy) {
      entry.is_busy = true;
      return static_cast<std::byte *>(entry.stack) + kStackSize;
    }
  }

  auto new_stack{::operator new(kStackSize)};
  stack_pool_.push_back({new_stack, true});
  return static_cast<std::byte *>(new_stack) + kStackSize;
}

void StackPool::Deallocate(void *stack_ptr) {
  for (auto &entry : stack_pool_) {
    auto cur_stack{entry.stack};
    if (cur_stack <= stack_ptr
        && stack_ptr <= static_cast<std::byte *>(cur_stack) + kStackSize) {
      entry.is_busy = false;
      break;
    }
  }
}

StackPool::StackPool() : stack_pool_{} {}

StackPool::~StackPool() {
  for (auto entry : stack_pool_) { ::operator delete(entry.stack); }
}
