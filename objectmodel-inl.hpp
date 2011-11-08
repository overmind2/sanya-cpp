#ifndef OBJECTMODEL_INL_HPP
#define OBJECTMODEL_INL_HPP
#include <algorithm>

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
    return object_type() == kNilType;
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

inline bool RawSymbol::interned() const {
    return interned_;
}

inline void RawSymbol::set_interned(bool p) {
    interned_ = p;
}

intptr_t RawSymbol::StringHash(const char *s, size_t len) {
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
        size_t lhs_len = lhs->length();
        return std::equal(lhs_str, lhs_str + lhs_len, rhs->Unwrap());
    }
}

RawSymbol::RawSymbol(const char *s, size_t len) {
    object_type_ = kSymbolType;
    std::copy(s, s + len + 1, sval_);
    this->len_ = len;
    this->hash_ = StringHash(s, len);
}

RawVector *RawVector::Wrap(size_t length, const Handle &fill) {
    void *addr = RawObject::operator new(sizeof(RawVector) +
            length * sizeof(RawObject *));
    return (RawVector *)::new (addr) RawVector(length, fill);
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

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:


#endif /* OBJECTMODEL_INL_HPP */

