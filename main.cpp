#include <cstdio>
#include <string>
#include <iostream>
#include "heap.hpp"
#include "objectmodel.hpp"
#include "sparse/parse_api.h"
#include "inlines.hpp"

using namespace sanya;

int main(int argc, const char *argv[])
{
    Handle expr = RawNil::Wrap();

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        expr = sparse_do_string(line.c_str());
        if (!expr.raw()) {
            break;
        }
        //Heap::Get().TriggerCollection();
        expr.raw()->Write(stdout);
    }

    return 0;
}

// vim: set ts=4 sw=4 sts=4:
