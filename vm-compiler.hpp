#ifndef VM_COMPILER_HPP
#define VM_COMPILER_HPP
#include "objectmodel.hpp"
#include "handle.hpp"

namespace sanya {

namespace vm_compiler {

/**
 * @class Value
 * @brief Compile-time value representation.
 */
class Value {
public:
    enum Type {
        kFrameSlot,
        kCell,
        kGlobal,
        kConst
    };

    Value(Type tp)
        : type_(tp),
          value_(NULL) { }

    bool IsFrameSlot() {
        return type_ == kFrameSlot;
    }

    bool IsCell() {
        return type_ == kCell;
    }

    bool IsGlobal() {
        return type_ == kGlobal;
    }

    bool IsConst() {
        return type_ == kConst;
    }

    intptr_t index() {
        return index_;
    }

    void set_index(intptr_t new_index) {
        index_ = new_index;
    }

    Handle& value() {
        return value_;
    }

    void set_value(const Handle& new_value) {
        value_ = new_value;
    }

protected:
    Type type_;
    Handle value_;    // for const
    intptr_t index_;  // for frame, cell and global
};

/**
 * @class Walker
 * @brief Visits expressions and calculate the result
 */
class Walker {
public:
    enum VisitFlag {
        kNormalVisit = 0,
        kTailVisit = 1
    };
    const static intptr_t kAnyFrameSlot = -1;

    Walker(Walker *parent = NULL);
    Value visit(const Handle& expr,
                const VisitFlag flag = kNormalVisit,
                const intptr_t return_to = kAnyFrameSlot);

protected:
    Walker *parent_;
    Handle insn_list_;
    Handle const_list_;
    Handle global_vars_;
};

}  // namespace vm_compiler

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:

#endif /* VM_COMPILER_HPP */
