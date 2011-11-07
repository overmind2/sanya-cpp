#include "handlezone.hpp"

namespace sanya {

HandleZone::HandleZone()
    : handles_(std::vector<Handle>()) { }

HandleZone::~HandleZone() { }

Handle &HandleZone::Alloc(RawObject *ro) {
    handles_.push_back(ro);
    return handles_.back();
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
