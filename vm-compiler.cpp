#include "vm-compiler.hpp"
#include "inlines.hpp"

namespace sanya {

namespace vm_compiler {


Walker::Walker(Walker *parent)
    : parent_(parent),
      insn_list_(NULL) {

    insn_list_ = RawGrowableVector::Wrap();
}

Value Walker::visit(const Handle& expr,
                    const VisitFlag flag,
                    const intptr_t return_to) {
    switch (expr.AsObject().object_type()) {
        case RawObject::kFixnumType:
        case RawObject::kNilType:
            FATAL_ERROR("illegal empty combination");
        case RawObject::kBooleanType:
        case RawObject::kTagType:
        case RawObject::kSymbolType:
        case RawObject::kPairType:
        case RawObject::kVectorType:
        case RawObject::kDictType:
        default:
            FATAL_ERROR("not reached");
    }
}

}  // namespace vm_compiler

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

