
#include <algorithm>
#include "objectmodel.hpp"
#include "inlines.hpp"

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

RawVector::RawVector(RawVector *copy_from, size_t copy_howmany,
                     size_t length, const Handle &fill) {
    object_type_ = kVectorType;
    this->length_ = length;
    std::copy(copy_from->data_, copy_from->data_ + copy_howmany, data_);
    std::fill(data_ + copy_howmany, data_ + length, fill.raw());
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

void RawGrowableVector::Write_V(FILE *stream) const {
    fprintf(stream, "#[");
    if (usage_ == 0) {
        fprintf(stream, "]");
        return;
    }

    At(0)->Write(stream);
    for (size_t i = 1; i < usage_; ++i) {
        fprintf(stream, " ");
        At(i)->Write(stream);
    }
    fprintf(stream, "]");
}

intptr_t RawGrowableVector::Hash_V() const {
    FATAL_ERROR("mutable hash");
}

void RawGrowableVector::Resize(size_t to_size) {
    Handle self = this;
    self.AsGrowableVector().data_
        = RawVector::Wrap(data_, usage_, to_size, RawNil::Wrap());
}

void RawGrowableVector::UpdateInteriorPointers(Heap &heap) {
    data_ = (RawVector *)heap.MarkAndCopy(data_);
}

const size_t RawDict::kPrimes[] = {
    5UL,          53UL,         97UL,         193UL,       389UL,
    769UL,        1543UL,       3079UL,       6151UL,      12289UL,
    24593UL,      49157UL,      98317UL,      196613UL,    393241UL,
    786433UL,     1572869UL,    3145739UL,    6291469UL,   12582917UL,
    25165843UL,   50331653UL,   100663319UL,  201326611UL, 402653189UL,
    805306457UL,  1610612741UL, 3221225473UL, 4294967291UL
};

RawPair *RawDict::LookupSymbol(const Handle& symbol, LookupFlag flag) {
    size_t bucket = symbol.AsSymbol().Hash() % size_;
    Handle self = this;
    Handle vec = self.AsDict().vec_;
    Handle head = vec.AsVector().At(bucket);

    if (head.raw()->IsNil()) {
        // When there is no key here (which could be common)
        if (flag & kCreateOnAbsent) {
            Handle new_entry = RawPair::Wrap(symbol, RawNil::Wrap());
            Handle lis_item = RawPair::Wrap(new_entry.raw(), RawNil::Wrap());
            vec.AsVector().At(bucket) = lis_item.raw();
            self.AsDict().IncreaseUsage();  // may enlarge and rehash
            return &new_entry.AsPair();
        }
        else {
            return NULL;
        }
    }

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
        self.AsDict().IncreaseUsage();  // may enlarge and rehash
        return &entry.AsPair();
    }
    else {
        return NULL;
    }
found:
    if (flag & kDeleteOnFound) {
        if (iter.raw() == head.raw()) {
            // Removing the first item in the list -- should modify vector.
            vec.AsVector().At(bucket) = iter.AsPair().cdr();
        }
        else {
            dummy_head.AsPair().set_cdr(iter.AsPair().cdr());
        }
        self.AsDict().DecreaseUsage();  // may shrink and rehash
    }
    return &entry.AsPair();
}

void RawDict::Resize(const size_t new_size) {
    printf("Resize from %ld to %ld\n", size_, new_size);
    Handle self = this;
    const size_t old_length = vec_->length();
    Handle old_vec = vec_;
    Handle new_vec = RawVector::Wrap(new_size, RawNil::Wrap());
    for (size_t i = 0; i < old_length; ++i) {
        Handle lis = old_vec.AsVector().At(i);
        while (!lis.raw()->IsNil()) {
            Handle item = lis.AsPair().car();
            lis = lis.AsPair().cdr();
            Handle key = item.AsPair().car();
            const intptr_t hash = key.AsSymbol().Hash();
            const size_t bucket = hash % new_size;
            printf("Rehashing from %ld to %ld\n",
                    hash % (old_length - 1), bucket);
            new_vec.AsVector().At(bucket)
                = RawPair::Wrap(item, new_vec.AsVector().At(bucket));
        }
    }
    self.AsDict().size_ = new_size;
    self.AsDict().vec_ = &new_vec.AsVector();
}

void RawDict::Write_V(FILE *stream) const {
}


intptr_t RawDict::Hash_V() const {
    FATAL_ERROR("mutable hash");
}

void RawDict::UpdateInteriorPointers(Heap &heap) {
    vec_ = (RawVector *)heap.MarkAndCopy(vec_);
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
