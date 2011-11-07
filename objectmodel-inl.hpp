namespace sanya {

void *RawObject::operator new(size_t size) {
    void *ptr = Heap::Get().Alloc(size);
    RawObject *o = (RawObject *)ptr;

    o->object_size_ = size;
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
        default:
            Write_V(stream);
    }
}

bool RawObject::IsNil() const {
    return object_type() == kNilType;
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

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
