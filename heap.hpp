#ifndef HEAP_HPP
#define HEAP_HPP

#include <inttypes.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <utility>

namespace sanya {

// Forwarded from objectmodel.hpp
class Object;
class RawObject;
class RawHeapObject;

class Heap {
public:
    Heap(size_t size);
    ~Heap();
    static Heap &get();

    /**
     * @brief Allocate a chunk of memory on the heap.
     * If we are using stop-the-world GC, this may trigger a full marking
     * followed by a full copying. Or if a incremental GC is used, this may
     * trigger a tiny marking or a tiny copying.
     *
     * Now we are using naive stop-the-world GC.
     */
    RawHeapObject *alloc(size_t size) {
        // Align.
        size = (size + 15) & (~15);
        size_t usageAfterAlloc = size + usage_;
        if (usageAfterAlloc > size_) {
            // Mark and copy is triggered.
            triggerCollection();

            // Recheck size.
            usageAfterAlloc = size + usage_;
            if (usageAfterAlloc > size_) {
                // Heap ran out of space.
                fprintf(stderr, "ERROR: Heap::alloc(%ld) -- out of space\n",
                        size);
                exit(1);
            }
        }

        // Allocate this pointer.
        RawHeapObject *ptr = (RawHeapObject *)(fromSpace_ + usage_);
        usage_ = usageAfterAlloc;

        // GC-related things are in operator new.
        return ptr;
    }

    void dealloc(RawHeapObject *ro) { }

    /** 
     * @brief If this object is not allocated on the heap, return itself.
     * If this object was copied, return its forwarding pointer.
     * Otherwise, copy this object and return its forwarding pointer.
     */
    RawObject *markAndCopy(RawObject *ro);

    void triggerCollection();

protected:

    // False for fixnum.
    bool isHeapAllocated(RawObject *ro);

    // When ro is already copied.
    bool isForwardPointer(RawObject *ro);
    RawHeapObject *getForwardPointer(RawHeapObject *ro);
    void setForwardPointer(RawHeapObject *fromAddress,
                           RawHeapObject *toAddress);

    size_t getRawObjectSize(RawObject *ro);

private:
    size_t size_;
    size_t usage_;
    char *fromSpace_;

    size_t copyUsage_;
    char *toSpace_;
};

class RootSet {
    friend class Heap;
public:
    RootSet();
    ~RootSet();
    static RootSet &get();

    void put(Object *o);

private:
    Object *head_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* HEAP_HPP */
