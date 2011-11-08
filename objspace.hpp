#ifndef OBJSPACE_HPP
#define OBJSPACE_HPP
/**
 * @file objspace.hpp
 * @brief Provides interface for accessing objects from handles.
 */

#include "objectmodel.hpp"
#include "objectmodel-inl.hpp"

namespace sanya {

class ObjSpace {
public:
    inline ObjSpace();

    inline RawSymbol *InternSymbol(const Handle &symbol);
    inline RawSymbol *InternSymbol(const char *s);

protected:
    Handle symbol_table_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* OBJSPACE_HPP */
