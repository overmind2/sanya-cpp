#ifndef OBJSPACE_HPP
#define OBJSPACE_HPP
/**
 * @file objspace.hpp
 * @brief Provides interface for accessing objects from handles.
 */

#include "objectmodel.hpp"

namespace sanya {

class ObjSpace {
public:
    inline static ObjSpace& Get();

    inline RawSymbol *InternSymbol(const Handle &symbol);
    inline RawSymbol *InternSymbol(const char *s);

protected:
    static ObjSpace *inst_s;

    inline ObjSpace();
    Handle symbol_table_;
};

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* OBJSPACE_HPP */
