namespace sanya {

Heap &Heap::Get() {
    if (!default_s)
        default_s = new Heap(1024 * 1024 * 2);
    return *default_s;
}

RawHeapObject *Heap::Alloc(size_t size) {
    // Align.
    size = (size + kAligner) & (~kAligner);
    size_t usage_after_alloc = size + usage_;
    if (usage_after_alloc > size_) {
        // Mark and copy is triggered.
        TriggerCollection();

        // Recheck size.
        usage_after_alloc = size + usage_;
        if (usage_after_alloc > size_) {
            FATAL_ERROR("out of space");
        }
    }

    // Allocate this pointer.
    RawHeapObject *ptr = (RawHeapObject *)(from_space_ + usage_);
    usage_ = usage_after_alloc;

    // GC-related things are in operator new.
    return ptr;
}

RawObject *Heap::MarkAndCopy(RawObject *ro) {
    // Note `heap` here means myself, the heap object allocator, not
    // the program heap. That is, fixed heap allocated objects will return
    // false.
    // E.g., fixnum object.
    if (!IsHeapAllocated(ro)) return ro;

    // If is already copied.
    if (IsForwardPointer(ro)) return GetForwardPointer((RawHeapObject *)ro);

    // Prepare for the copy.
    RawHeapObject *rho = (RawHeapObject *)ro;
    size_t object_size = GetRawObjectSize(rho);
    RawHeapObject *destination = (RawHeapObject *)(to_space_ + copy_usage_);
    copy_usage_ += object_size;

    // Copy and setting up forward pointer.
    memcpy(destination, rho, object_size);
    SetForwardPointer(rho, destination);
    destination->self_ = (RawHeapObject *)(destination->self_);

    // Update internal pointers
    // XXX: eliminate recursive calls!
    destination->UpdateInteriorPointers(*this);

    return destination;
}

bool Heap::IsHeapAllocated(RawObject *ro) {
    return ro->heap_allocated();
}

bool Heap::IsForwardPointer(RawObject *ro) {
    return ro != ((RawHeapObject *)ro)->self_;
}

RawHeapObject *Heap::GetForwardPointer(RawHeapObject *ro) {
    return ro->self_;
}

void Heap::SetForwardPointer(RawHeapObject *from_address,
                             RawHeapObject *to_address) {
    from_address->self_ = to_address;
}

size_t Heap::GetRawObjectSize(RawObject *ro) {
    return ((RawHeapObject *)ro)->object_size_;
}


RootSet &RootSet::Get() {
    if (!default_s)
        default_s = new RootSet();
    return *default_s;
}

void RootSet::Put(Handle *o) {
    o->next_root_ = head_->next_root_;
    o->prev_root_ = head_;
    head_->next_root_->prev_root_ = o;
    head_->next_root_ = o;
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
