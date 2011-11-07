
#include "objectmodel.hpp"
#include "heap-inl.hpp"
#include "objectmodel-inl.hpp"
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
