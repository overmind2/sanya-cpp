
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "heap.hpp"
#include "objectmodel.hpp"
#include "inlines.hpp"

namespace sanya {

Heap *Heap::default_s = NULL;

Heap::Heap(size_t size)
    : size_(size),
      usage_(0),
      from_space_(new char[size]),
      copy_usage_(0),
      to_space_(new char[size]) {

    // Not quite sure why valgrind says error....
    memset(from_space_, 0, size);
    memset(to_space_, 0, size_);
}

Heap::~Heap() {
    delete[] from_space_;
    from_space_ = NULL;
    delete[] to_space_;
    to_space_ = NULL;
    size_ = 0;
    usage_ = 0;
}

void Heap::TriggerCollection() {
    Handle *dummy = RootSet::Get().head_;
    Handle *it;

    // Iterate through the root set, do mark-and-copy, and update
    // the pointer fields in the root sets.
    for (it = dummy->next_root_; it != dummy; it = it->next_root_) {
        it->raw_ = MarkAndCopy(it->raw_);
    }

    // Optional: call destructors for objects.
    printf(":heap-collect %ld => %ld\n", usage_, copy_usage_);

    // Flip over and clean up
    std::swap(usage_, copy_usage_);
    copy_usage_ = 0;
    std::swap(from_space_, to_space_);
    std::fill(to_space_, to_space_ + size_, 0);
}

// Private implementation of a dummy head.
class DummyObjectHead : public Handle {
public:
    DummyObjectHead() {
        next_root_ = this;
        prev_root_ = this;
    }
};

static DummyObjectHead dummy_head_s = DummyObjectHead();

RootSet *RootSet::default_s = NULL;

RootSet::RootSet()
    : head_(&dummy_head_s) { }

RootSet::~RootSet() {
}


}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
