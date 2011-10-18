#ifndef OBJECTMODEL_HPP
#define OBJECTMODEL_HPP

#include <new>
#include <cstring>
#include "heap.hpp"

/**
 * @file objectmodel.hpp
 * @brief Scheme objects and implementations.
 *
 * RawObject and subclasses are those that are allocated on the heap and
 * may be collected or moved from place to place by the garbage collector
 * (see heap.hpp).
 *
 * Object and friends are interfaces that contains a raw pointer. They
 * are allocated on the runtime stack maintained by the compiler.
 */

namespace sanya {

class RawObject {
    friend class Heap;
public:
    static const uintptr_t nonHeapTypeShift = 4;
    static const uintptr_t nonHeapTypeMask = (1 << nonHeapTypeShift) - 1;

    enum ObjectType {
        kFixnumType = 1,
        kNilType = 2,
         kLastNonHeapType = nonHeapTypeMask,
        kSymbolType = kLastNonHeapType + 1,
        kPairType,
        kVectorType
    };

    virtual ~RawObject() { }

    // Should have static alloc/init methods.
    void *operator new(size_t size) {
        void *ptr = Heap::get().alloc(size);
        RawObject *o = (RawObject *)ptr;

        o->objectSize_ = size;
        o->self_ = (RawHeapObject *)o;
        return ptr;
    }
    void operator delete(void *ptr) {
        Heap::get().dealloc((RawHeapObject *)ptr);
    }
    bool isHeapAllocated() const {
        return !(((uintptr_t)this) & nonHeapTypeMask) && this;
    }
    // this Maybe NULL.
    ObjectType getType() const {
        uintptr_t word = (uintptr_t)this;
        uintptr_t type;
        if ((type = word & nonHeapTypeMask)) {
            return (ObjectType)type;
        }
        return this->objectType_;
    }

    bool isNil() const {
        return getType() == RawObject::kNilType;
    }

    bool isFixnum() const {
        return getType() == RawObject::kFixnumType;
    }

    bool isPair() const {
        return getType() == RawObject::kPairType;
    }

    bool isSymbol() const {
        return getType() == RawObject::kSymbolType;
    }

    bool isVector() const {
        return getType() == RawObject::kVectorType;
    }

    void printRepr(FILE *stream) const;
    virtual void printReprV(FILE *stream) const = 0;

protected:
    // Not constructed directly.
    RawObject() { }

    // Let the collector mark and copy interior pointers,
    // then update interior pointer fields.
    // This is what GC does not know (if we want to write cleaner code)
    // but is it possible that inlining it will greatly affect the performance?
    virtual void updateInteriorPointers(Heap &heap) = 0;

    // Those may not present in non-heap objects
    uint32_t objectSize_;
    ObjectType objectType_;
    RawHeapObject *self_;

private:
    // Arrayish new and delete are disabled.
    void *operator new[](size_t size);
    void operator delete[](void *ptr);
};

class RawFixnum : public RawObject {
public:
    static RawFixnum *wrap(long ival) {
        return (RawFixnum *)((ival << nonHeapTypeShift) | kFixnumType);
    }
    long unwrap() const {
        return ((intptr_t)this) >> nonHeapTypeShift;
    }
};

class RawNil : public RawObject {
public:
    static RawNil *wrap() {
        return (RawNil *)kNilType;
    }
};

/**
 * @brief The baseclass for any raw object that is allocated on the
 * garbage-collected heap.
 */
class RawHeapObject : public RawObject {
    friend class Heap;
public:
    ObjectType getType() const {
        return objectType_;
    }
};

// Forwarding for Object.
class RawPair;
class RawSymbol;
class RawVector;
class RawHash;

class Object {
    friend class Heap;
    friend class RootSet;
    friend class DummyObjectHead;
public:
    ~Object() {
        if (prevRoot_ || nextRoot_) {
            unlinkFromRootSet();
        }
    }

    /** @brief Construct from newly-allocated raw object */
    Object(RawObject *ro) {
        empty();
        linkToRootSet(ro);
    }

    /** @brief Copy constructor from another object */
    Object(const Object &o) {
        empty();
        linkToRootSet(o.raw_);
    }

    /** @brief Assignment mutator */
    Object &operator=(const Object &o) {
        linkToRootSet(o.raw_);
        return *this;
    }

    /** @brief Assignment constructor */
    Object &operator=(RawObject *ro) {
        linkToRootSet(ro);
        return *this;
    }

    /** @brief Raw pointer accessor */
    RawObject *raw() const {
        return raw_;
    }
    RawHeapObject *asRHO() const {
        return (RawHeapObject *)raw_;
    }
    RawPair *asPair() const {
        return (RawPair *)raw_;
    }
    RawFixnum *asFixnum() const {
        return (RawFixnum *)raw_;
    }
    RawSymbol *asSymbol() const {
        return (RawSymbol *)raw_;
    }

private:
    Object() { }

    void empty() {
        raw_ = NULL;
        prevRoot_ = NULL;
        nextRoot_ = NULL;
    }

    void linkToRootSet(RawObject *ro) {
        if (ro->isHeapAllocated()) {
            if (!nextRoot_) {
                // The first time we got a heap object
                RootSet::get().put(this);
            }
            else {
                // So we already have a heap object
            }
        }
        else if (raw_->isHeapAllocated()) {
            // Switching from heap object to non-heap object
            // release the root set.
            unlinkFromRootSet();
        }
        else {
            // Neither of the pointers are heap-allocated -- ignore.
        }
        raw_ = ro;
    }

    void unlinkFromRootSet() {
        prevRoot_->nextRoot_ = nextRoot_;
        nextRoot_->prevRoot_ = prevRoot_;
    }

    /** @brief The raw object pointer, may be changed during GC */
    RawObject *raw_;

    /** @brief Previous object in the GC root */
    Object *prevRoot_;

    /** @brief Next object in the GC root */
    Object *nextRoot_;
};

class RawPair : public RawHeapObject {
public:
    static RawPair *wrap(const Object &car, const Object &cdr) {
        RawPair *self = new RawPair();
        self->car_ = car.raw();
        self->cdr_ = cdr.raw();
        return self;
    }
    RawObject *car() const {
        return car_;
    }
    RawObject *cdr() const {
        return cdr_;
    }
    virtual void printReprV(FILE *stream) const;

protected:
    virtual void updateInteriorPointers(Heap &heap);

private:
    RawPair() { }

    RawObject *car_;
    RawObject *cdr_;
};

class RawSymbol : public RawHeapObject {
public:
    static RawSymbol *wrap(const char *s) {
        size_t len = strlen(s);
        void *addr = RawObject::operator new(sizeof(RawSymbol) + len + 1);
        return (RawSymbol *)::new (addr) RawSymbol(s, len);
    }
    const char *unwrap() const {
        return sval_;
    }
    virtual void printReprV(FILE *stream) const;

protected:
    // Default ctor will clear out the values.
    RawSymbol(const char *s, size_t len) {
        objectType_ = kSymbolType;
        memcpy(sval_, s, len + 1);
        this->len = len;
    }
    virtual void updateInteriorPointers(Heap &heap) { }

private:
    size_t len;
    char sval_[0];
};

class RawVector : public RawHeapObject {
public:
    static RawVector *wrap(size_t length, const Object &fill) {
        void *addr = RawObject::operator new(sizeof(RawVector) +
                length * sizeof(RawObject *));
        return (RawVector *)::new (addr) RawVector(length, fill);
    }

    RawObject *&ref(size_t index) {
        if (index >= len_) {
            // We are checked.
        }
        else {
            return data_[index];
        }
    }

    RawObject *const&ref(size_t index) const {
        if (index >= len_) {
            // We are checked.
        }
        else {
            return data_[index];
        }
    }

    size_t length() const {
        return len_;
    }

    virtual void printReprV(FILE *stream) const;

protected:
    RawVector(size_t length, const Object &fill) {
        objectType_ = kVectorType;
        this->len_ = length;
        for (size_t i = 0; i < length; ++i) {
            data_[i] = fill.raw();
        }
    }

    virtual void updateInteriorPointers(Heap &heap) {
        for (size_t i = 0; i < len_; ++i) {
            data_[i] = heap.markAndCopy(data_[i]);
        }
    }

private:
    size_t len_;
    RawObject *data_[0];
};

class RawHash : public RawHeapObject {
public:
    static RawHash *wrap() {
        return new RawHash();
    }

    virtual void printReprV(FILE *stream) const;

protected:
    RawHash() {
    }

    virtual void updateInteriorPointers(Heap &heap) {
    }

private:
    size_t size_;
    size_t mask_;
    RawVector *vec_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* OBJECTMODEL_HPP */
