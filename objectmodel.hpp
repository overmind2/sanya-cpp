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
        kFixnumType = 1,
        kNilType = 2,
        kTagType = 3,
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

    inline bool IsNil() const;
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

/**
 * @brief The baseclass for any raw object that is allocated on the
 * garbage-collected heap.
 */
class RawHeapObject : public RawObject {
    friend class Heap;
};

class RawPair : public RawHeapObject {
public:
    static RawPair *Wrap(const Handle &car_h, const Handle &cdr_h) {
        RawPair *self = new RawPair();
        self->car_ = car_h.Raw();
        self->cdr_ = cdr_h.Raw();
        return self;
    }
    RawObject *car() const {
        return car_;
    }
    RawObject *cdr() const {
        return cdr_;
    }
    virtual void Write_V(FILE *stream) const;
    intptr_t Hash_V() const;

protected:
    virtual void UpdateInteriorPointers(Heap &heap);

private:
    RawPair() { }

    RawObject *car_;
    RawObject *cdr_;
};

class RawSymbol : public RawHeapObject {
public:
    static RawSymbol *Wrap(const char *str_val) {
        size_t len = strlen(str_val);
        void *addr = RawObject::operator new(sizeof(RawSymbol) + len + 1);
        return (RawSymbol *)::new (addr) RawSymbol(str_val, len);
    }
    const char *Unwrap() const {
        return sval_;
    }
    size_t length() const {
        return len_;
    }
    virtual void Write_V(FILE *stream) const;

    static intptr_t StringHash(const char *s, size_t len) {
        intptr_t iter = len;
        intptr_t x;
        x = *s << 7;
        while (--iter >= 0)
            x = (1000003*x) ^ *s++;
        x ^= len;
        if (x == -1)
            x = -2;
        return x;
    }

    virtual intptr_t Hash_V() const;

protected:
    // Default ctor will clear out the values.
    RawSymbol(const char *s, size_t len) {
        object_type_ = kSymbolType;
        memcpy(sval_, s, len + 1);
        this->len_ = len;
        this->hash_ = StringHash(s, len);
    }

    // Dummy
    virtual void UpdateInteriorPointers(Heap &heap) { }

private:
    size_t len_;
    intptr_t hash_;
    char sval_[0];
};

class RawVector : public RawHeapObject {
public:
    static RawVector *Wrap(size_t length, const Handle &fill) {
        void *addr = RawObject::operator new(sizeof(RawVector) +
                length * sizeof(RawObject *));
        return (RawVector *)::new (addr) RawVector(length, fill);
    }

    RawObject *&At(size_t index) {
        if (index < 0 || index >= length_) {
            // Since we are doing safe operations, this is checked.
            FATAL_ERROR("vector index out of bound");
        }
        else {
            return data_[index];
        }
    }

    RawObject *const&At(size_t index) const {
        return ((RawVector *)this)->At(index);
    }

    size_t length() const {
        return length_;
    }

    virtual void Write_V(FILE *stream) const;
    virtual intptr_t Hash_V() const;

protected:
    RawVector(size_t length, const Handle &fill) {
        object_type_ = kVectorType;
        this->length_ = length;
        for (size_t i = 0; i < length; ++i) {
            data_[i] = fill.Raw();
        }
    }

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

    RawPair *LookupCString(const char *key, size_t len, intptr_t hash,
                           LookupFlag flag) {
        size_t bucket = hash & mask_;
        Handle self = this;
        Handle vec = self.AsDict().vec_;
        Handle head = vec.AsVector().At(bucket);
        Handle dummy_head = RawPair::Wrap(RawNil::Wrap(), head.Raw());
        Handle iter = NULL;
        Handle entry = NULL;

        while (!dummy_head.AsObject().IsNil()) {
            iter = dummy_head.AsPair().cdr();
            entry = iter.AsPair().car();
            Handle entry_key = entry.AsPair().car();
            if (entry_key.AsSymbol().Hash() == hash) {
                size_t slen = entry_key.AsSymbol().length();
                const char *sval = entry_key.AsSymbol().Unwrap();
                if (slen != len) {
                    continue;
                }
                else {
                    if (memcmp(key, sval, len) != 0) {
                        continue;
                    }
                    else {
                        // Finally this symbol is found.
                        goto found;
                    }
                }
            }
            dummy_head = iter;
        }
        // Key is absent.
        if (flag & kCreateOnAbsent) {
        }
found:
        return NULL;
    }

    RawPair *LookupHashable(const Handle &key, LookupFlag flag) {
        intptr_t hash = key.AsObject().Hash();
        return NULL;
    }

protected:
    RawDict()
        : used_(0),
          mask_(kInitLength - 1),
          vec_(NULL) {
        Handle self = this;
        self.AsDict().vec_ = RawVector::Wrap(kInitLength, RawNil::Wrap());
    }

    virtual void UpdateInteriorPointers(Heap &heap);

private:
    size_t used_;
    size_t mask_;
    RawVector *vec_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* OBJECTMODEL_HPP */
