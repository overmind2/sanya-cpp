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

