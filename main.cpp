
#include <cstdio>
#include "heap.hpp"
#include "objectmodel.hpp"
#include "heap-inl.hpp"
#include "objectmodel-inl.hpp"
#include "handle-inl.hpp"

using namespace sanya;

int main(int argc, const char *argv[])
{
    Handle symb = RawSymbol::Wrap("hehehe");
    Handle nil = RawNil::Wrap();
    Handle pair = RawPair::Wrap(symb, nil);
    Handle vec = RawVector::Wrap(10, pair);

    printf("symb = %p, \"%s\"\n", symb.Raw(), symb.AsSymbol().Unwrap());
    pair.Raw()->Write(stdout);
    vec.Raw()->Write(stdout);

    Heap::Get().TriggerCollection();

    pair.Raw()->Write(stdout);
    printf("symb = %p, \"%s\"\n", symb.Raw(), symb.AsSymbol().Unwrap());
    vec.Raw()->Write(stdout);

    return 0;
}

// vim: set ts=4 sw=4 sts=4:
