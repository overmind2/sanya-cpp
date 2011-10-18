
#include "objectmodel.hpp"

namespace sanya {

// Some virtual function implementations.
void RawObject::printRepr(FILE *stream) const {
    switch (getType()) {
        case kNilType:
            fprintf(stream, "()");
            break;
        case kFixnumType:
            fprintf(stream, "%ld", ((RawFixnum *)this)->unwrap());
            break;
        default:
            printReprV(stream);
    }
}

void RawPair::printReprV(FILE *stream) const {
    fprintf(stream, "(");
    car_->printRepr(stream);

    RawObject *it;
    for (it = cdr_; it->isPair(); it = ((RawPair *)it)->cdr_) {
        fprintf(stream, " ");
        ((RawPair *)it)->car_->printRepr(stream);
    }

    if (!it->isNil()) {
        fprintf(stream, " . ");
        it->printRepr(stream);
    }

    fprintf(stream, ")");
}

void RawPair::updateInteriorPointers(Heap &heap) {
    car_ = heap.markAndCopy(car_);
    cdr_ = heap.markAndCopy(cdr_);
}

void RawSymbol::printReprV(FILE *stream) const {
    fprintf(stream, "%s", sval_);
}

void RawVector::printReprV(FILE *stream) const {
    fprintf(stream, "#(");
    if (len_ == 0) {
        fprintf(stream, ")");
        return;
    }

    ref(0)->printRepr(stream);
    for (size_t i = 1; i < len_; ++i) {
        fprintf(stream, " ");
        ref(i)->printRepr(stream);
    }
    fprintf(stream, ")");
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
