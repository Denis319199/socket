// No copyright

#ifndef FIBER_STACKPOOL_HPP_
#define FIBER_STACKPOOL_HPP_

#include <vector>

class StackPool {
 public:
  static StackPool &GetStackPool();

  StackPool(const StackPool &) = delete;
  StackPool &operator=(const StackPool &) = delete;

  void *Allocate();

  void Deallocate(void *stack_ptr);

 private:
  StackPool();

  ~StackPool();

  struct StackPoolEntry {
    void *stack;
    bool is_busy;
  };

  std::vector<StackPoolEntry> stack_pool_;

  static StackPool global_stack_pool_instance_;
  static constexpr std::size_t kStackSize{1'048'576};
};

#endif  // FIBER_STACKPOOL_HPP_
