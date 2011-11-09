#ifndef PARSE_API_H
#define PARSE_API_H
#include "sanya.hpp"
#include "objectmodel.hpp"
#include "handle.hpp"
#include "inlines.hpp"

extern "C" {

sanya::RawObject *sparse_do_string(const char *s);
sanya::RawObject *sparse_do_file(FILE *fp);

}


#endif /* PARSE_API_H */
