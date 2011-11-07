#ifndef OBJECTMODEL_HPP
#define OBJECTMODEL_HPP

#include <new>
#include <cstring>
#include "sanya.hpp"
#include "heap.hpp"
#include "handle.hpp"

/**
 * @file objectmodel.hpp
 * @brief Scheme objects and implementations.
 *
 * RawObject and subclasses are those that are allocated on the heap and
 * may be collected or moved from place to place by the garbage collector
 * (see heap.hpp).
 *
 * Handle and friends are interfaces that contains a raw pointer. They
 * are allocated on the runtime stack maintained by the compiler.
 */

namespace sanya {

class RawObject {
    friend class Heap;
public:
    static const uintptr_t kNonHeapTypeShift = 4;
    static const uintptr_t kNonHeapTypeMask = (1 << kNonHeapTypeShift) - 1;

    enum ObjectType {
        kFixnumType     = 1,
        kNilType        = 2,
        kBooleanType    = 3,
        kTagType        = 4,
        kLastNonHeapType = kNonHeapTypeMask,  // Last non-heap-object
        kSymbolType = kLastNonHeapType + 1,
        kPairType,
        kVectorType
    };

    virtual ~RawObject() { }

    // Should have static alloc/init methods.
    inline void *operator new(size_t size);
    inline void operator delete(void *ptr);

    // Since `this` may be NULL, we need to add additional check.
    inline bool heap_allocated() const;

    // Here `this` will never be NULL.
    inline ObjectType object_type() const;

    inline void Write(FILE *stream) const;
    inline intptr_t Hash() const;
    inline bool IsTrue() const;

    inline bool IsNil() const;
    inline bool IsBoolean() const;
    inline bool IsFixnum() const;
    inline bool IsPair() const;
    inline bool IsSymbol() const;
    inline bool IsVector() const;

    virtual void Write_V(FILE *stream) const = 0;
    virtual intptr_t Hash_V() const = 0;

protected:
    // Not constructed directly.
    RawObject() { }

    // Let the collector mark and copy interior pointers,
    // then update interior pointer fields.
    // This is what GC does not know (if we want to write cleaner code)
    // but is it possible that inlining it will greatly affect the performance?
    virtual void UpdateInteriorPointers(Heap &heap) = 0;

    // Those may not present in non-heap objects
    uint32_t object_size_;
    ObjectType object_type_;
    RawHeapObject *self_;

private:
    // Arrayish new and deletes are disabled.
    void *operator new[](size_t size);
    void operator delete[](void *ptr);
};

class RawFixnum : public RawObject {
public:
    static inline RawFixnum *Wrap(intptr_t int_val);
    inline intptr_t Unwrap() const;
};

class RawNil : public RawObject {
public:
    static inline RawNil *Wrap();
};

class RawBoolean : public RawObject {
public:
    static inline RawBoolean *Wrap(bool bool_val);
    inline intptr_t Unwrap() const;
};

/**
 * @brief The baseclass for any raw object that is allocated on the
 * garbage-collected heap.
 */
class RawHeapObject : public RawObject {
    friend class Heap;
};

class RawPair : public RawHeapObject {
public:
    inline static RawPair *Wrap(const Handle &car_h, const Handle &cdr_h);

    inline RawObject *car() const;
    inline RawObject *cdr() const;

    inline void set_car(RawObject *new_car);
    inline void set_cdr(RawObject *new_cdr);

    virtual void Write_V(FILE *stream) const;
    intptr_t Hash_V() const;

protected:
    virtual void UpdateInteriorPointers(Heap &heap);

private:
    inline RawPair();

    RawObject *car_;
    RawObject *cdr_;
};

class RawSymbol : public RawHeapObject {
public:
    inline static RawSymbol *Wrap(const char *str_val);
    inline const char *Unwrap() const;
    inline size_t length() const;

    static inline intptr_t StringHash(const char *s, size_t len);
    virtual void Write_V(FILE *stream) const;
    virtual intptr_t Hash_V() const;

protected:
    // Default ctor will clear out the values.
    inline RawSymbol(const char *s, size_t len);

    // Dummy
    virtual void UpdateInteriorPointers(Heap &heap) { }

private:
    size_t len_;
    intptr_t hash_;
    char sval_[0];
};

class RawVector : public RawHeapObject {
public:
    static inline RawVector *Wrap(size_t length, const Handle &fill);

    inline RawObject *&At(size_t index);
    inline RawObject *const&At(size_t index) const;
    inline size_t length() const;

    virtual void Write_V(FILE *stream) const;
    virtual intptr_t Hash_V() const;

protected:
    // Contains a loop so no inlining
    RawVector(size_t length, const Handle &fill);

    virtual void UpdateInteriorPointers(Heap &heap);

private:
    size_t length_;
    RawObject *data_[0];
};

class RawDict : public RawHeapObject {
public:
    static const size_t kInitLength = 8;
    enum LookupFlag {
        kLookupDefault,
        kCreateOnAbsent,
        kDeleteOnFound
    };

    static RawDict *Wrap() {
        return new RawDict();
    }

    virtual intptr_t Hash_V() const;
    virtual void Write_V(FILE *stream) const;

    /**
     * @brief Lookup a given cstring inside the hashtable. Return an
     * entry of this string, or NULL if not found.
     */
    RawPair *LookupSymbol(const Handle &key, LookupFlag flag);

protected:
    RawDict();

    virtual void UpdateInteriorPointers(Heap &heap);

private:
    size_t used_;
    size_t mask_;
    RawVector *vec_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* OBJECTMODEL_HPP */
