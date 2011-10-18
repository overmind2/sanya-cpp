
#include <cstdio>
#include "heap.hpp"
#include "objectmodel.hpp"

using namespace sanya;

int main(int argc, const char *argv[])
{
    Object symb = RawSymbol::wrap("hehehe");
    Object nil = RawNil::wrap();
    Object pair = RawPair::wrap(symb, nil);
    Object vec = RawVector::wrap(10, pair);

    printf("symb = %p, \"%s\"\n", symb.raw(), symb.asSymbol()->unwrap());
    pair.raw()->printRepr(stdout);
    vec.raw()->printRepr(stdout);

    Heap::get().triggerCollection();

    pair.raw()->printRepr(stdout);
    printf("symb = %p, \"%s\"\n", symb.raw(), symb.asSymbol()->unwrap());
    vec.raw()->printRepr(stdout);

    return 0;
}

// vim: set ts=4 sw=4 sts=4:
