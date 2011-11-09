#ifndef OBJECTMODEL_INL_HPP
#define OBJECTMODEL_INL_HPP
#include <algorithm>
#include "sanya.hpp"

namespace sanya {

void *RawObject::operator new(size_t size) {
    void *ptr = Heap::Get().Alloc(size);
    RawObject *o = (RawObject *)ptr;

    // Size is set in the heap since alignment may occur.
    o->self_ = (RawHeapObject *)o;
    return ptr;
}

void RawObject::operator delete(void *ptr) {
    Heap::Get().Dealloc((RawHeapObject *)ptr);
}

bool RawObject::heap_allocated() const {
    return !(((uintptr_t)this) & kNonHeapTypeMask) && this;
}

RawObject::ObjectType RawObject::object_type() const {
    uintptr_t word = (uintptr_t)this;
    uintptr_t type;
    if ((type = word & kNonHeapTypeMask)) {
        // Is non-heap-allocated object
        return (ObjectType)type;
    }
    else {
        // Is heap-allocated object
        return this->object_type_;
    }
}

bool RawObject::IsTrue() const {
    return this != RawBoolean::Wrap(false);
}

intptr_t RawObject::Hash() const {
    if (heap_allocated())
        return Hash_V();
    else
        return (intptr_t)this;
}

void RawObject::Write(FILE *stream) const {
    switch (object_type()) {
        case kNilType:
            fprintf(stream, "()");
            break;
        case kFixnumType:
            fprintf(stream, "%ld", ((RawFixnum *)this)->Unwrap());
            break;
        case kBooleanType:
            fprintf(stream, "%s", ((RawBoolean *)this)->Unwrap() ? "#t" : "#f");
            break;
        default:
            Write_V(stream);
    }
}

bool RawObject::IsNil() const {
    return this == RawNil::Wrap();
}

bool RawObject::IsBoolean() const {
    return object_type() == kBooleanType;
}

bool RawObject::IsFixnum() const {
    return object_type() == kFixnumType;
}

bool RawObject::IsPair() const {
    return object_type() == kPairType;
}

bool RawObject::IsSymbol() const {
    return object_type() == kSymbolType;
}

bool RawObject::IsVector() const {
    return object_type() == kVectorType;
}

bool RawObject::IsGrowableVector() const {
    return object_type() == kGrowableVectorType;
}

bool RawObject::IsDict() const {
    return object_type() == kDictType;
}

RawFixnum *RawFixnum::Wrap(intptr_t int_val) {
    // XXX: overflow check
    return (RawFixnum *)((int_val << kNonHeapTypeShift) | kFixnumType);
}

intptr_t RawFixnum::Unwrap() const {
    return ((intptr_t)this) >> kNonHeapTypeShift;
}

RawNil *RawNil::Wrap() {
    return (RawNil *)kNilType;
}

RawBoolean *RawBoolean::Wrap(bool bool_val) {
    return (RawBoolean *)((bool_val << kNonHeapTypeShift) | kBooleanType);
}

intptr_t RawBoolean::Unwrap() const {
    return ((intptr_t)this) >> kNonHeapTypeShift;
}

RawPair::RawPair() {
    object_type_ = kPairType;
}

RawPair *RawPair::Wrap(const Handle &car_h, const Handle &cdr_h) {
    RawPair *self = new RawPair();
    self->car_ = car_h.raw();
    self->cdr_ = cdr_h.raw();
    return self;
}

RawObject *RawPair::car() const {
    return car_;
}

RawObject *RawPair::cdr() const {
    return cdr_;
}

void RawPair::set_car(RawObject *new_car) {
    car_ = new_car;
}

void RawPair::set_cdr(RawObject *new_cdr) {
    cdr_ = new_cdr;
}

RawSymbol::RawSymbol(const char *s, size_t len) {
    object_type_ = kSymbolType;
    std::copy(s, s + len + 1, sval_);
    this->len_ = len;
    this->hash_ = StringHash(s, len);
    this->interned_ = false;
}

RawSymbol *RawSymbol::Wrap(const char *str_val) {
    size_t len = strlen(str_val);
    void *addr = RawObject::operator new(sizeof(RawSymbol) + len + 1);
    return (RawSymbol *)::new (addr) RawSymbol(str_val, len);
}

const char *RawSymbol::Unwrap() const {
    return sval_;
}

size_t RawSymbol::length() const {
    return len_;
}

bool RawSymbol::interned() const {
    return interned_;
}

void RawSymbol::set_interned(bool p) {
    interned_ = p;
}

// Copied from Java's hash algorithm
intptr_t RawSymbol::StringHash(const char *s, size_t len) {
    uintptr_t hash = 0;
    while (len-- > 0) {
        hash = 31*hash + (unsigned) *s;
        s++;
    }
    return hash;
}

bool RawSymbol::SymbolEq(RawSymbol *lhs, RawSymbol *rhs) {
    if (lhs == rhs) {
        // Most of the case
        return true;
    }
    else if (lhs->interned() && rhs->interned()) {
        // Most of the case
        return false;
    }
    else {
        // Either of them are interned. This happens during interning.
        // (it's never the case that both of them are not interned)
        if (lhs->hash_ != rhs->hash_) {
            return false;
        }
        else if (lhs->length() != rhs->length()) {
            return false;
        }
        const char *lhs_str = lhs->Unwrap();
        const char *rhs_str = rhs->Unwrap();
        printf("comparing %s with %s\n", lhs_str, rhs_str);
        size_t lhs_len = lhs->length();
        return std::equal(lhs_str, lhs_str + lhs_len, rhs->Unwrap());
    }
}

RawVector *RawVector::Wrap(size_t length, const Handle &fill) {
    void *addr = RawObject::operator new(sizeof(RawVector) +
            length * sizeof(RawObject *));
    return (RawVector *)::new (addr) RawVector(length, fill);
}

RawVector *RawVector::Wrap(RawVector *copy_from, size_t copy_howmany,
                           size_t length, const Handle &fill) {
    void *addr = RawObject::operator new(sizeof(RawVector) +
            length * sizeof(RawObject *));
    return (RawVector *)::new (addr) RawVector(copy_from, copy_howmany,
                                               length, fill);
}

RawObject *& RawVector::At(size_t index) {
    if (index < 0 || index >= length_) {
        // Since we are doing safe operations, this is checked.
        FATAL_ERROR("vector index out of bound");
    }
    else {
        return data_[index];
    }
}

RawObject *const& RawVector::At(size_t index) const {
    return ((RawVector *)this)->At(index);
}

size_t RawVector::length() const {
    return length_;
}

RawGrowableVector::RawGrowableVector()
    : usage_(0) {
    Handle self = this;
    data_ = RawVector::Wrap(kInitSize, RawNil::Wrap());
}

RawGrowableVector *RawGrowableVector::Wrap() {
    return new RawGrowableVector();
}

RawObject *& RawGrowableVector::At(intptr_t index) {
    if (index < 0) {
        index += usage_;
        if (index < 0) {
            index = 0;
        }
    }
    return data_->At(index);
}

RawObject *const& RawGrowableVector::At(intptr_t index) const {
    return ((RawGrowableVector *)this)->At(index);
}

size_t RawGrowableVector::length() const {
    return usage_;
}

RawObject *RawGrowableVector::Pop() {
    Handle retval = At(-1);
    At(-1) = RawNil::Wrap();
    DecreaseUsage();
    return retval.raw();
}

void RawGrowableVector::Append(const Handle &o) {
    IncreaseUsage();
    At(-1) = o.raw();
}

void RawGrowableVector::DecreaseUsage() {
    usage_ -= 1;
    if (usage_ > (kInitSize << 2) && usage_ < (data_->length() >> 2)) {
        size_t new_size = data_->length() >> 1;
        Resize(new_size);
    }
}

void RawGrowableVector::IncreaseUsage() {
    usage_ += 1;
    if (usage_ > data_->length()) {
        size_t new_size = data_->length() << 1;
        Resize(new_size);
    }
}

RawDict::RawDict()
    : used_(0),
      size_(kPrimes[0]),
      vec_(NULL) {
    Handle self = this;
    object_type_ = kDictType;
    self.AsDict().vec_ = RawVector::Wrap(size_, RawNil::Wrap());
}

void RawDict::IncreaseUsage() {
    ++used_;
    // More than 150%
    if (used_ > (size_ + (size_ >> 1))) {
        size_t next_size;
        for (size_t i = 0; i < kNumberPrimes; ++i) {
            next_size = kPrimes[i];
            if (next_size > size_) {
                Resize(next_size);
                return;
            }
        }
        FATAL_ERROR("dict too large");
    }
}

void RawDict::DecreaseUsage() {
    --used_;
    // Less than 25% but more than kInitLength
    if (used_ < (size_ >> 2) && size_ > kPrimes[0]) {
        size_t next_size;
        for (size_t i = 0; i < kNumberPrimes; ++i) {
            next_size = kPrimes[i];
            if (next_size > size_) {
                Resize(kPrimes[i - 1]);
                return;
            }
        }
    }
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:


#endif /* OBJECTMODEL_INL_HPP */

