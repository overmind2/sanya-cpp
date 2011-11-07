#ifndef HEAP_HPP
#define HEAP_HPP

#include <inttypes.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <utility>

#include "sanya.hpp"

namespace sanya {

// Forwarded from objectmodel.hpp
class Handle;
class RawObject;
class RawHeapObject;

/**
 * @class Heap
 * @brief An memory manager that acts as a partial object space which
 * knows every objects and their handles (through RootSet).
 */
class Heap {
public:
    const static int kAlignment = 16;  // 16-bytes
    const static int kAligner = kAlignment - 1;

    Heap(size_t size);
    ~Heap();

    /** @brief Get the singleton heap */
    inline static Heap &Get();

    /**
     * @brief Allocate a chunk of memory on the heap.
     * If we are using stop-the-world GC, this may trigger a full marking
     * followed by a full copying. Or if a incremental GC is used, this may
     * trigger a tiny marking or a tiny copying.
     *
     * We really want to inline this.
     */
    inline RawHeapObject *Alloc(size_t size);

    // Not used
    void Dealloc(RawHeapObject *rho) { }

    /** 
     * @brief If this object is not allocated on the heap, return itself.
     * If this object was copied, return its forwarding pointer.
     * Otherwise, copy this object and return its forwarding pointer.
     */
    inline RawObject *MarkAndCopy(RawObject *ro);

    /**
     * @brief Start stop-the-world copy collection. Don't try to inline this.
     */
    void TriggerCollection();

protected:

    // False for fixnum.
    inline bool IsHeapAllocated(RawObject *ro);

    // When ro is already copied.
    inline bool IsForwardPointer(RawObject *ro);
    inline RawHeapObject *GetForwardPointer(RawHeapObject *ro);
    inline void SetForwardPointer(RawHeapObject *from_address,
                                  RawHeapObject *to_address);

    inline size_t GetRawObjectSize(RawObject *ro);

private:
    static Heap *default_s;

    size_t size_;
    size_t usage_;
    char *from_space_;

    size_t copy_usage_;
    char *to_space_;
};

class RootSet {
    friend class Heap;
public:
    RootSet();
    ~RootSet();
    static inline RootSet &Get();

    inline void Put(Handle *o);

private:
    static RootSet *default_s;

    Handle *head_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* HEAP_HPP */
