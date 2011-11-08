#ifndef SANYA_API_H
#define SANYA_API_H

#include <stdlib.h>
#include "sanya.hpp"
#include "objectmodel.hpp"
#include "handle.hpp"
#include "objectmodel-inl.hpp"
#include "handle-inl.hpp"
#include "heap-inl.hpp"
#include "handlezone.hpp"

using namespace sanya;

extern HandleZone *parser_zone;

extern "C" {

inline Handle *alloc_handle(RawObject *ro) {
    return &parser_zone->Alloc(ro);
}

inline RawObject *make_nil() {
    return RawNil::Wrap();
}

inline RawObject *make_true() {
    return RawBoolean::Wrap(1);
}

inline RawObject *make_false() {
    return RawBoolean::Wrap(0);
}

inline RawObject *make_pair(RawObject *car, RawObject *cdr) {
    return RawPair::Wrap(car, cdr);
}

inline RawObject *make_fixnum(const char *s) {
    return RawFixnum::Wrap(strtol(s, NULL, 10));
}

inline RawObject *make_flonum(const char *s) {
    FATAL_ERROR("no flonum support yet");
}

inline RawObject *make_string(const char *s, int length) {
    FATAL_ERROR("no string support yet");
}

inline RawObject *make_symbol(const char *s) {
    return RawSymbol::Wrap(s);
}

inline RawObject *make_vector(RawObject *list) {
    FATAL_ERROR("not supported yet");
}

inline RawObject *make_quoted(RawObject *content) {
    FATAL_ERROR("not supported yet");
}

inline RawObject *make_quasiquoted(RawObject *content) {
    FATAL_ERROR("not supported yet");
}

inline RawObject *make_unquoted(RawObject *content) {
    FATAL_ERROR("not supported yet");
}

inline RawObject *make_splicing(RawObject *content) {
    FATAL_ERROR("not supported yet");
}

}


#endif /* SANYA_API_H */
