#include "handlezone.hpp"

namespace sanya {

HandleZone::HandleZone()
    : handles_(std::vector<Handle *>()) { }

HandleZone::~HandleZone() {
    for (size_t i = 0; i < handles_.size(); ++i) {
        delete handles_[i];
    }
}

Handle &HandleZone::Alloc(RawObject *ro) {
    handles_.push_back(new Handle(ro));
    return *(handles_.back());
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

