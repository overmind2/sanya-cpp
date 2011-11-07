#include "sanya.hpp"
#include "objectmodel.hpp"
#include "handle.hpp"
#include "objectmodel-inl.hpp"
#include "handle-inl.hpp"
#include "heap-inl.hpp"

extern "C" {

sanya::RawObject *sparse_do_string(const char *s);
sanya::RawObject *sparse_do_file(FILE *fp);

}

