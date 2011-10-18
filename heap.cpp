
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>
#include "heap.hpp"
#include "objectmodel.hpp"

namespace sanya {

static Heap heap_s = Heap(1024 * 1024 * 2);

Heap::Heap(size_t size)
    : size_(size),
      usage_(0),
      fromSpace_(new char[size]),
      copyUsage_(0),
      toSpace_(new char[size]) {

    // Not quite sure why valgrind says error....
    memset(fromSpace_, 0, size);
    memset(toSpace_, 0, size_);
}

Heap::~Heap() {
    delete[] fromSpace_;
    fromSpace_ = 0;
    delete[] toSpace_;
    toSpace_ = 0;
    size_ = 0;
    usage_ = 0;
}

Heap &Heap::get() {
    return heap_s;
}

void Heap::triggerCollection() {
    Object *dummy = RootSet::get().head_;
    Object *it;

    // Iterate through the root set, do mark-and-copy, and update
    // the pointer fields in the root sets.
    for (it = dummy->nextRoot_; it != dummy; it = it->nextRoot_) {
        it->raw_ = markAndCopy(it->raw_);
    }

    // Optional: call destructors for objects.

    // Switch two spaces
    std::swap(usage_, copyUsage_);
    std::swap(fromSpace_, toSpace_);
    memset(toSpace_, 0, size_);
}

RawObject *Heap::markAndCopy(RawObject *ro) {
    // Note `heap` here means myself, the heap object allocator, not
    // the program heap. That is, fixed heap allocated objects will return
    // false.
    // E.g., fixnum object.
    if (!isHeapAllocated(ro)) return ro;

    // If is already copied.
    if (isForwardPointer(ro)) return getForwardPointer((RawHeapObject *)ro);

    // Prepare for the copy.
    RawHeapObject *rho = (RawHeapObject *)ro;
    size_t objectSize = getRawObjectSize(rho);
    RawHeapObject *destination = (RawHeapObject *)(toSpace_ + copyUsage_);
    copyUsage_ += objectSize;

    // Copy and setting up forward pointer.
    memcpy(destination, rho, objectSize);
    setForwardPointer(rho, destination);
    destination->self_ = (RawHeapObject *)markAndCopy(destination->self_);

    // Update internal pointers
    destination->updateInteriorPointers(*this);

    return destination;
}

bool Heap::isHeapAllocated(RawObject *ro) {
    return ro->isHeapAllocated();
}

bool Heap::isForwardPointer(RawObject *ro) {
    return ro != ((RawHeapObject *)ro)->self_;
}

RawHeapObject *Heap::getForwardPointer(RawHeapObject *ro) {
    return ro->self_;
}

void Heap::setForwardPointer(RawHeapObject *fromAddress,
                             RawHeapObject *toAddress) {
    fromAddress->self_ = toAddress;
}

size_t Heap::getRawObjectSize(RawObject *ro) {
    return ((RawHeapObject *)ro)->objectSize_;
}

// Private implementation of a dummy head.
class DummyObjectHead : public Object {
public:
    DummyObjectHead() {
        nextRoot_ = this;
        prevRoot_ = this;
    }
};

static DummyObjectHead dummyHead_s = DummyObjectHead();
static RootSet rootSet_s = RootSet();

RootSet::RootSet()
    : head_(&dummyHead_s) { }

RootSet::~RootSet() {
}

RootSet &RootSet::get() {
    return rootSet_s;
}

void RootSet::put(Object *o) {
    o->nextRoot_ = head_->nextRoot_;
    o->prevRoot_ = head_;
    head_->nextRoot_->prevRoot_ = o;
    head_->nextRoot_ = o;
}


}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
