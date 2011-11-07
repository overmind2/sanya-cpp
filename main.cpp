#include <cstdio>
#include <string>
#include <iostream>
#include "heap.hpp"
#include "objectmodel.hpp"
#include "heap-inl.hpp"
#include "objectmodel-inl.hpp"
#include "handle-inl.hpp"
#include "sparse/parse_api.h"

using namespace sanya;

int main(int argc, const char *argv[])
{
    Handle expr = RawNil::Wrap();

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        expr = sparse_do_string(line.c_str());
        //Heap::Get().TriggerCollection();
        expr.raw()->Write(stdout);
    }

    return 0;
}

// vim: set ts=4 sw=4 sts=4:
