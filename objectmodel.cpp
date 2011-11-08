
#include <algorithm>
#include "objectmodel.hpp"
#include "objectmodel-inl.hpp"
#include "heap-inl.hpp"
#include "handle-inl.hpp"

namespace sanya {

// Some virtual function implementations.

void RawPair::Write_V(FILE *stream) const {
    fprintf(stream, "(");
    car_->Write(stream);

    RawObject *it;
    for (it = cdr_; it->IsPair(); it = ((RawPair *)it)->cdr_) {
        fprintf(stream, " ");
        ((RawPair *)it)->car_->Write(stream);
    }

    if (!it->IsNil()) {
        fprintf(stream, " . ");
        it->Write(stream);
    }

    fprintf(stream, ")");
}

intptr_t RawPair::Hash_V() const {
    FATAL_ERROR("mutable hash");
}

void RawPair::UpdateInteriorPointers(Heap &heap) {
    car_ = heap.MarkAndCopy(car_);
    cdr_ = heap.MarkAndCopy(cdr_);
}

void RawSymbol::Write_V(FILE *stream) const {
    fprintf(stream, "%s", sval_);
}

intptr_t RawSymbol::Hash_V() const {
    return hash_;
}

RawVector::RawVector(size_t length, const Handle &fill) {
    object_type_ = kVectorType;
    this->length_ = length;
    std::fill(data_, data_ + length, fill.raw());
}

void RawVector::Write_V(FILE *stream) const {
    fprintf(stream, "#(");
    if (length_ == 0) {
        fprintf(stream, ")");
        return;
    }

    At(0)->Write(stream);
    for (size_t i = 1; i < length_; ++i) {
        fprintf(stream, " ");
        At(i)->Write(stream);
    }
    fprintf(stream, ")");
}

intptr_t RawVector::Hash_V() const {
    FATAL_ERROR("mutable hash");
}

void RawVector::UpdateInteriorPointers(Heap &heap) {
    for (size_t i = 0; i < length_; ++i) {
        data_[i] = heap.MarkAndCopy(data_[i]);
    }
}

RawPair *RawDict::LookupSymbol(const Handle& symbol, LookupFlag flag) {
    size_t bucket = symbol.AsSymbol().Hash() & mask_;
    Handle self = this;
    Handle vec = self.AsDict().vec_;
    Handle head = vec.AsVector().At(bucket);
    Handle dummy_head = RawPair::Wrap(RawNil::Wrap(), head.raw());
    Handle iter = NULL;
    Handle entry = NULL;
    Handle entry_key = NULL;

    while (!dummy_head.AsPair().cdr()->IsNil()) {
        iter = dummy_head.AsPair().cdr();
        entry = iter.AsPair().car();
        entry_key = entry.AsPair().car();
        if (RawSymbol::SymbolEq(&entry_key.AsSymbol(), &symbol.AsSymbol())) {
            goto found;
        }
        dummy_head = iter;
    }
    // Key is absent since dummy_head.cdr is nil
    if (flag & kCreateOnAbsent) {
        entry = RawPair::Wrap(symbol, RawNil::Wrap());
        iter = RawPair::Wrap(entry.raw(), RawNil::Wrap());
        dummy_head.AsPair().set_cdr(iter.raw());
        return &entry.AsPair();
    }
    else {
        return NULL;
    }
found:
    if (flag & kDeleteOnFound) {
        dummy_head.AsPair().set_cdr(iter.AsPair().cdr());
    }
    return &entry.AsPair();
}

RawDict::RawDict()
    : used_(0),
      mask_(kInitLength - 1),
      vec_(NULL) {
    Handle self = this;
    self.AsDict().vec_ = RawVector::Wrap(kInitLength, RawNil::Wrap());
}

void RawDict::Write_V(FILE *stream) const {
}


intptr_t RawDict::Hash_V() const {
    FATAL_ERROR("mutable hash");
}

void RawDict::UpdateInteriorPointers(Heap &heap) {
    FATAL_ERROR("not implemented");
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
