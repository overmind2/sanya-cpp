namespace sanya {

inline Handle::~Handle() {
    if (prev_root_ || next_root_) {
        UnlinkFromRootSet();
    }
}

/** @brief Construct from newly-allocated raw object */
inline Handle::Handle(RawObject *ro) {
    Empty();
    LinkToRootSet(ro);
}

/** @brief Copy constructor from another object */
inline Handle::Handle(const Handle &o) {
    Empty();
    LinkToRootSet(o.raw_);
}

/** @brief Assignment mutator */
inline Handle &Handle::operator=(const Handle &o) {
    LinkToRootSet(o.raw_);
    return *this;
}

/** @brief Assignment constructor */
inline Handle &Handle::operator=(RawObject *ro) {
    LinkToRootSet(ro);
    return *this;
}

/** @brief Raw pointer accessor */
inline RawObject *Handle::Raw() const {
    return raw_;
}

RawObject &Handle::AsObject() const {
    return *raw_;
}

RawPair &Handle::AsPair() const {
    return *(RawPair *)raw_;
}

RawFixnum &Handle::AsFixnum() const {
    return *(RawFixnum *)raw_;
}

RawSymbol &Handle::AsSymbol() const {
    return *(RawSymbol *)raw_;
}

RawVector &Handle::AsVector() const {
    return *(RawVector *)raw_;
}

RawDict &Handle::AsDict() const {
    return *(RawDict *)raw_;
}

inline Handle::Handle() {
    Empty();
}

inline void Handle::Empty() {
    raw_ = NULL;
    prev_root_ = NULL;
    next_root_ = NULL;
}

inline void Handle::LinkToRootSet(RawObject *ro) {
    if (ro->heap_allocated()) {
        if (!next_root_) {
            // The first time we got a heap object
            RootSet::Get().Put(this);
        }
        else {
            // So we already have a heap object
        }
    }
    else if (raw_->heap_allocated()) {
        // Switching from heap object to non-heap object
        // release the root set.
        UnlinkFromRootSet();
    }
    else {
        // Neither of the pointers are heap-allocated -- ignore.
    }
    raw_ = ro;
}

inline void Handle::UnlinkFromRootSet() {
    prev_root_->next_root_ = next_root_;
    next_root_->prev_root_ = prev_root_;
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
