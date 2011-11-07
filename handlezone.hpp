#include <vector>
#include "handle.hpp"
#include "objectmodel.hpp"
#include "heap.hpp"
#include "handle-inl.hpp"
#include "heap-inl.hpp"
#include "objectmodel-inl.hpp"

namespace sanya {

/**
 * @class HandleZone
 * @brief Handle manager when stack-allocated handles are not applicable.
 */
class HandleZone {
public:
    HandleZone();
    ~HandleZone();
    Handle &Alloc(RawObject *);

private:
    std::vector<Handle> handles_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:
